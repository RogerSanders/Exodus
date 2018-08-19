//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> RAMBase<T>::RAMBase(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:MemoryWrite(implementationName, instanceName, moduleID), _memoryArraySize(0), _memoryArray(0), _memoryLockedArray(0), _initialMemoryDataSpecified(false), _repeatInitialMemoryData(false), _dataIsPersistent(false)
{}

//----------------------------------------------------------------------------------------
template<class T> RAMBase<T>::~RAMBase()
{
	delete _memoryArray;
	delete _memoryLockedArray;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
template<class T> bool RAMBase<T>::Construct(IHierarchicalStorageNode& node)
{
	bool result = MemoryWrite::Construct(node);

	//Obtain the size in bytes of a single entry in the memory array
	unsigned int memoryArrayEntryByteSize = (unsigned int)sizeof(T);

	//Validate the specified memory entry count
	_memoryArraySize = GetMemoryEntryCount();
	if(_memoryArraySize <= 0)
	{
		return false;
	}

	//Resize the internal memory arrays based on the specified memory entry count, and
	//initialize all elements to 0.
	delete _memoryArray;
	_memoryArray = new T[_memoryArraySize];
	memset(&_memoryArray[0], 0, (_memoryArraySize * memoryArrayEntryByteSize));
	delete _memoryLockedArray;
	_memoryLockedArray = new bool[_memoryArraySize];
	memset(&_memoryLockedArray[0], 0, (_memoryArraySize * sizeof(bool)));

	//Read the PersistentData attribute if specified
	IHierarchicalStorageAttribute* persistentDataAttribute = node.GetAttribute(L"PersistentData");
	if(persistentDataAttribute != 0)
	{
		_dataIsPersistent = persistentDataAttribute->ExtractValue<bool>();
	}

	//If initial RAM state data has been specified, attempt to load it now.
	if(node.GetBinaryDataPresent())
	{
		//Flag that initial memory data has been specified
		_initialMemoryDataSpecified = true;

		//Obtain the stream for our binary data
		Stream::IStream& dataStream = node.GetBinaryDataBufferStream();
		dataStream.SetStreamPos(0);

		//Read the RepeatData attribute if specified
		IHierarchicalStorageAttribute* repeatDataAttribute = node.GetAttribute(L"RepeatData");
		if(repeatDataAttribute != 0)
		{
			_repeatInitialMemoryData = repeatDataAttribute->ExtractValue<bool>();
		}

		//Read in the initial memory data
		unsigned int dataStreamByteSize = (unsigned int)dataStream.Size();
		unsigned int entriesInDataStream = (dataStreamByteSize / memoryArrayEntryByteSize);
		unsigned int initialMemoryDataSize = ((dataStreamByteSize + (memoryArrayEntryByteSize - 1)) / memoryArrayEntryByteSize);
		unsigned int entriesToRead = (_memoryArraySize < entriesInDataStream)? _memoryArraySize: entriesInDataStream;
		_initialMemoryData.resize(initialMemoryDataSize);
		if(!dataStream.ReadDataBigEndian(&_memoryArray[0], entriesToRead))
		{
			return false;
		}

		//If there is remaining data in the initial memory data stream that doesn't align
		//with a whole entry in the array, read in the remaining data into the next array
		//entry, and pad out the missing lower data with zeros.
		unsigned int bytesRemainingInDataStream = (dataStreamByteSize % memoryArrayEntryByteSize);
		if((initialMemoryDataSize > entriesInDataStream) && (bytesRemainingInDataStream > 0))
		{
			//Read in each remaining byte in the data stream into the next available array
			//entry
			for(unsigned int i = 0; i < bytesRemainingInDataStream; ++i)
			{
				unsigned char remainingData;
				if(!dataStream.ReadDataBigEndian(remainingData))
				{
					return false;
				}
				_initialMemoryData[entriesToRead] = ((_initialMemoryData[entriesToRead] << 8) | remainingData);
			}

			//Shift the data in the entry up by the required number of bits to align the
			//partial data read with the top of the entry, and pad out the lower bits with
			//zeros.
			_initialMemoryData[entriesToRead] <<= (8 * (memoryArrayEntryByteSize - bytesRemainingInDataStream));
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::Initialize()
{
	//Initialize the memory buffer
	for(unsigned int i = 0; i < _memoryArraySize; ++i)
	{
		if(!_memoryLockedArray[i])
		{
			T initialValue = 0;
			if(_initialMemoryDataSpecified && (_repeatInitialMemoryData || (i < (unsigned int)_initialMemoryData.size())))
			{
				unsigned int initialMemoryDataIndex = (i % (unsigned int)_initialMemoryData.size());
				initialValue = _initialMemoryData[initialMemoryDataIndex];
			}
			_memoryArray[i] = initialValue;
		}
	}

	//Initialize rollback state
	_buffer.clear();
}

//----------------------------------------------------------------------------------------
//Memory size functions
//----------------------------------------------------------------------------------------
template<class T> unsigned int RAMBase<T>::GetMemoryEntrySizeInBytes() const
{
	return sizeof(T);
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::ExecuteRollback()
{
	for(MemoryAccessBuffer::const_iterator i = _buffer.begin(); i != _buffer.end(); ++i)
	{
		_memoryArray[i->first] = i->second;
	}
	_buffer.clear();
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::ExecuteCommit()
{
	_buffer.clear();
}

//----------------------------------------------------------------------------------------
//Memory locking functions
//----------------------------------------------------------------------------------------
template<class T> bool RAMBase<T>::IsMemoryLockingSupported() const
{
	return true;
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{
	for(unsigned int i = 0; i < size; ++i)
	{
		_memoryLockedArray[location + i] = state;
	}
}

//----------------------------------------------------------------------------------------
template<class T> bool RAMBase<T>::IsAddressLocked(unsigned int location) const
{
	return _memoryLockedArray[location];
}

//----------------------------------------------------------------------------------------
//Access helper functions
//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::WriteArrayValueWithLockCheckAndRollback(unsigned int arrayEntryPos, T newValue)
{
	if(!_memoryLockedArray[arrayEntryPos])
	{
		_buffer.insert(MemoryAccessBufferEntry(arrayEntryPos, _memoryArray[arrayEntryPos]));
		_memoryArray[arrayEntryPos] = newValue;
	}
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::LoadState(IHierarchicalStorageNode& node)
{
	std::vector<T> savedMemoryData(_memoryArraySize, 0);
	node.ExtractBinaryData(savedMemoryData);
	unsigned int savedMemoryDataSize = (unsigned int)savedMemoryData.size();
	unsigned int entriesToLoad = (savedMemoryDataSize <= _memoryArraySize)? savedMemoryDataSize: _memoryArraySize;
	unsigned int entriesToFill = _memoryArraySize - entriesToLoad;
	if(entriesToLoad > 0)
	{
		memcpy(&_memoryArray[0], &savedMemoryData[0], (entriesToLoad * sizeof(T)));
	}
	if(entriesToFill > 0)
	{
		memset(&_memoryArray[entriesToLoad], 0, (entriesToFill * sizeof(T)));
	}

	MemoryWrite::LoadState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::SaveState(IHierarchicalStorageNode& node) const
{
	node.InsertBinaryData(_memoryArray, _memoryArraySize, GetFullyQualifiedDeviceInstanceName(), false);

	MemoryWrite::SaveState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::LoadPersistentState(IHierarchicalStorageNode& node)
{
	if(_dataIsPersistent)
	{
		std::vector<T> savedMemoryData(_memoryArraySize, 0);
		node.ExtractBinaryData(savedMemoryData);
		unsigned int savedMemoryDataSize = (unsigned int)savedMemoryData.size();
		unsigned int entriesToLoad = (savedMemoryDataSize <= _memoryArraySize)? savedMemoryDataSize: _memoryArraySize;
		unsigned int entriesToFill = _memoryArraySize - entriesToLoad;
		if(entriesToLoad > 0)
		{
			memcpy(&_memoryArray[0], &savedMemoryData[0], (entriesToLoad * sizeof(T)));
		}
		if(entriesToFill > 0)
		{
			memset(&_memoryArray[entriesToLoad], 0, (entriesToFill * sizeof(T)));
		}
	}

	MemoryWrite::LoadPersistentState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::SavePersistentState(IHierarchicalStorageNode& node) const
{
	if(_dataIsPersistent)
	{
		node.InsertBinaryData(_memoryArray, _memoryArraySize, GetFullyQualifiedDeviceInstanceName(), false);
	}

	MemoryWrite::SavePersistentState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::LoadDebuggerState(IHierarchicalStorageNode& node)
{
	std::vector<bool> savedMemoryLockedData(_memoryArraySize, 0);
	node.ExtractBinaryData(savedMemoryLockedData);
	unsigned int savedMemoryLockedDataSize = (unsigned int)savedMemoryLockedData.size();
	unsigned int entriesToLoad = (savedMemoryLockedDataSize <= _memoryArraySize)? savedMemoryLockedDataSize: _memoryArraySize;
	unsigned int entriesToFill = _memoryArraySize - entriesToLoad;
	if(entriesToLoad > 0)
	{
		//Note that we need to unpack the loaded boolean vector here one element at a
		//time, as there's a very poorly thought out specialization of std::vector for
		//boolean types in the current C++ standard, which packs each entry as a single
		//bit within an integer. As a result, there's no way to take the address of the
		//first element and do a memcpy.
		for(unsigned int i = 0; i < entriesToLoad; ++i)
		{
			_memoryLockedArray[i] = savedMemoryLockedData[i];
		}
	}
	if(entriesToFill > 0)
	{
		memset(&_memoryLockedArray[entriesToLoad], 0, (entriesToFill * sizeof(bool)));
	}

	MemoryWrite::LoadDebuggerState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::SaveDebuggerState(IHierarchicalStorageNode& node) const
{
	node.InsertBinaryData(_memoryLockedArray, _memoryArraySize, GetFullyQualifiedDeviceInstanceName() + L".MemoryLockedState", false);

	MemoryWrite::SaveDebuggerState(node);
}
