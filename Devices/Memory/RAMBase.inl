//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> RAMBase<T>::RAMBase(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryWrite(aimplementationName, ainstanceName, amoduleID), memoryArraySize(0), memoryArray(0), memoryLockedArray(0), initialMemoryDataSpecified(false), repeatInitialMemoryData(false), dataIsPersistent(false)
{}

//----------------------------------------------------------------------------------------
template<class T> RAMBase<T>::~RAMBase()
{
	delete memoryArray;
	delete memoryLockedArray;
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
	memoryArraySize = GetMemoryEntryCount();
	if(memoryArraySize <= 0)
	{
		return false;
	}

	//Resize the internal memory arrays based on the specified memory entry count, and
	//initialize all elements to 0.
	delete memoryArray;
	memoryArray = new T[memoryArraySize];
	memset(&memoryArray[0], 0, (memoryArraySize * memoryArrayEntryByteSize));
	delete memoryLockedArray;
	memoryLockedArray = new bool[memoryArraySize];
	memset(&memoryLockedArray[0], 0, (memoryArraySize * sizeof(bool)));

	//Read the PersistentData attribute if specified
	IHierarchicalStorageAttribute* persistentDataAttribute = node.GetAttribute(L"PersistentData");
	if(persistentDataAttribute != 0)
	{
		dataIsPersistent = persistentDataAttribute->ExtractValue<bool>();
	}

	//If initial RAM state data has been specified, attempt to load it now.
	if(node.GetBinaryDataPresent())
	{
		//Flag that initial memory data has been specified
		initialMemoryDataSpecified = true;

		//Obtain the stream for our binary data
		Stream::IStream& dataStream = node.GetBinaryDataBufferStream();
		dataStream.SetStreamPos(0);

		//Read the RepeatData attribute if specified
		IHierarchicalStorageAttribute* repeatDataAttribute = node.GetAttribute(L"RepeatData");
		if(repeatDataAttribute != 0)
		{
			repeatInitialMemoryData = repeatDataAttribute->ExtractValue<bool>();
		}

		//Read in the initial memory data
		unsigned int dataStreamByteSize = (unsigned int)dataStream.Size();
		unsigned int entriesInDataStream = (dataStreamByteSize / memoryArrayEntryByteSize);
		unsigned int initialMemoryDataSize = ((dataStreamByteSize + (memoryArrayEntryByteSize - 1)) / memoryArrayEntryByteSize);
		unsigned int entriesToRead = (memoryArraySize < entriesInDataStream)? memoryArraySize: entriesInDataStream;
		initialMemoryData.resize(initialMemoryDataSize);
		if(!dataStream.ReadDataBigEndian(&memoryArray[0], entriesToRead))
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
				initialMemoryData[entriesToRead] = ((initialMemoryData[entriesToRead] << 8) | remainingData);
			}

			//Shift the data in the entry up by the required number of bits to align the
			//partial data read with the top of the entry, and pad out the lower bits with
			//zeros.
			initialMemoryData[entriesToRead] <<= (8 * (memoryArrayEntryByteSize - bytesRemainingInDataStream));
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::Initialize()
{
	//Initialize the memory buffer
	for(unsigned int i = 0; i < memoryArraySize; ++i)
	{
		if(!memoryLockedArray[i])
		{
			T initialValue = 0;
			if(initialMemoryDataSpecified && (repeatInitialMemoryData || (i < (unsigned int)initialMemoryData.size())))
			{
				unsigned int initialMemoryDataIndex = (i % (unsigned int)initialMemoryData.size());
				initialValue = initialMemoryData[initialMemoryDataIndex];
			}
			memoryArray[i] = initialValue;
		}
	}

	//Initialize rollback state
	buffer.clear();
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
	for(MemoryAccessBuffer::const_iterator i = buffer.begin(); i != buffer.end(); ++i)
	{
		memoryArray[i->first] = i->second;
	}
	buffer.clear();
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::ExecuteCommit()
{
	buffer.clear();
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
		memoryLockedArray[location + i] = state;
	}
}

//----------------------------------------------------------------------------------------
template<class T> bool RAMBase<T>::IsAddressLocked(unsigned int location) const
{
	return memoryLockedArray[location];
}

//----------------------------------------------------------------------------------------
//Access helper functions
//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::WriteArrayValueWithLockCheckAndRollback(unsigned int arrayEntryPos, T newValue)
{
	if(!memoryLockedArray[arrayEntryPos])
	{
		buffer.insert(MemoryAccessBufferEntry(arrayEntryPos, memoryArray[arrayEntryPos]));
		memoryArray[arrayEntryPos] = newValue;
	}
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::LoadState(IHierarchicalStorageNode& node)
{
	std::vector<T> savedMemoryData(memoryArraySize, 0);
	node.ExtractBinaryData(savedMemoryData);
	unsigned int savedMemoryDataSize = (unsigned int)savedMemoryData.size();
	unsigned int entriesToLoad = (savedMemoryDataSize <= memoryArraySize)? savedMemoryDataSize: memoryArraySize;
	unsigned int entriesToFill = memoryArraySize - entriesToLoad;
	if(entriesToLoad > 0)
	{
		memcpy(&memoryArray[0], &savedMemoryData[0], (entriesToLoad * sizeof(T)));
	}
	if(entriesToFill > 0)
	{
		memset(&memoryArray[entriesToLoad], 0, (entriesToFill * sizeof(T)));
	}

	MemoryWrite::LoadState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::SaveState(IHierarchicalStorageNode& node) const
{
	node.InsertBinaryData(memoryArray, memoryArraySize, GetFullyQualifiedDeviceInstanceName(), false);

	MemoryWrite::SaveState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::LoadPersistentState(IHierarchicalStorageNode& node)
{
	if(dataIsPersistent)
	{
		std::vector<T> savedMemoryData(memoryArraySize, 0);
		node.ExtractBinaryData(savedMemoryData);
		unsigned int savedMemoryDataSize = (unsigned int)savedMemoryData.size();
		unsigned int entriesToLoad = (savedMemoryDataSize <= memoryArraySize)? savedMemoryDataSize: memoryArraySize;
		unsigned int entriesToFill = memoryArraySize - entriesToLoad;
		if(entriesToLoad > 0)
		{
			memcpy(&memoryArray[0], &savedMemoryData[0], (entriesToLoad * sizeof(T)));
		}
		if(entriesToFill > 0)
		{
			memset(&memoryArray[entriesToLoad], 0, (entriesToFill * sizeof(T)));
		}
	}

	MemoryWrite::LoadPersistentState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::SavePersistentState(IHierarchicalStorageNode& node) const
{
	if(dataIsPersistent)
	{
		node.InsertBinaryData(memoryArray, memoryArraySize, GetFullyQualifiedDeviceInstanceName(), false);
	}

	MemoryWrite::SavePersistentState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::LoadDebuggerState(IHierarchicalStorageNode& node)
{
	std::vector<bool> savedMemoryLockedData(memoryArraySize, 0);
	node.ExtractBinaryData(savedMemoryLockedData);
	unsigned int savedMemoryLockedDataSize = (unsigned int)savedMemoryLockedData.size();
	unsigned int entriesToLoad = (savedMemoryLockedDataSize <= memoryArraySize)? savedMemoryLockedDataSize: memoryArraySize;
	unsigned int entriesToFill = memoryArraySize - entriesToLoad;
	if(entriesToLoad > 0)
	{
		//Note that we need to unpack the loaded boolean vector here one element at a
		//time, as there's a very poorly thought out specialization of std::vector for
		//boolean types in the current C++ standard, which packs each entry as a single
		//bit within an integer. As a result, there's no way to take the address of the
		//first element and do a memcpy.
		for(unsigned int i = 0; i < entriesToLoad; ++i)
		{
			memoryLockedArray[i] = savedMemoryLockedData[i];
		}
	}
	if(entriesToFill > 0)
	{
		memset(&memoryLockedArray[entriesToLoad], 0, (entriesToFill * sizeof(bool)));
	}

	MemoryWrite::LoadDebuggerState(node);
}

//----------------------------------------------------------------------------------------
template<class T> void RAMBase<T>::SaveDebuggerState(IHierarchicalStorageNode& node) const
{
	node.InsertBinaryData(memoryLockedArray, memoryArraySize, GetFullyQualifiedDeviceInstanceName() + L".MemoryLockedState", false);

	MemoryWrite::SaveDebuggerState(node);
}
