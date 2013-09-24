//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
template<class T> ROMBase<T>::ROMBase(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryRead(aimplementationName, ainstanceName, amoduleID), memoryArraySize(0), memoryArray(0)
{}

//----------------------------------------------------------------------------------------
template<class T> ROMBase<T>::~ROMBase()
{
	delete memoryArray;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
template<class T> bool ROMBase<T>::Construct(IHeirarchicalStorageNode& node)
{
	bool result = MemoryRead::Construct(node);

	//Obtain the size in bytes of a single entry in the memory array
	unsigned int memoryArrayEntryByteSize = (unsigned int)sizeof(T);

	//If embedded ROM data has been specified, attempt to load it now.
	if(node.GetBinaryDataPresent())
	{
		//Obtain the stream for our binary data
		Stream::IStream& dataStream = node.GetBinaryDataBufferStream();
		dataStream.SetStreamPos(0);

		//Validate and adjust the array entry count as necessary. If no entry count has
		//been manually specified, we set the entry count based on the size of the
		//specified embedded ROM data, if available.
		unsigned int memoryEntryCount = GetMemoryEntryCount();
		if(memoryEntryCount <= 0)
		{
			//Calculate the required size of the memory array in bytes to hold the
			//supplied data
			memoryEntryCount = (unsigned int)dataStream.Size();
			if(memoryEntryCount <= 0)
			{
				//If no memory array entry count has been specified, and the embedded ROM
				//data is empty, return false.
				return false;
			}
			else if((memoryEntryCount % memoryArrayEntryByteSize) != 0)
			{
				//If the specified ROM data doesn't give us a clean alignment with the
				//memory array entry size, we extend it to the end of the last array
				//entry. We pad out the undefined bytes in the last extended array entry
				//with zeros in this case.
				memoryEntryCount += (memoryArrayEntryByteSize - (memoryEntryCount % memoryArrayEntryByteSize));
			}

			//Convert the calculated memory entry count from the number of required bytes
			//to the number of required array entries, and set it as the current memory
			//entry count.
			memoryEntryCount /= memoryArrayEntryByteSize;
			SetMemoryEntryCount(memoryEntryCount);
		}

		//Set the memory array size based on the recorded memory entry count. Note that at
		//this point, the memory entry count has already been adjusted to a clean multiple
		//of the array entry byte size.
		memoryArraySize = memoryEntryCount;

		//Resize the internal memory array based on the calculated array size, and
		//initialize all elements to 0.
		delete memoryArray;
		memoryArray = new T[memoryArraySize];
		memset(&memoryArray[0], 0, (memoryArraySize * memoryArrayEntryByteSize));

		//Read the RepeatData attribute if specified
		bool repeatData = false;
		IHeirarchicalStorageAttribute* repeatDataAttribute = node.GetAttribute(L"RepeatData");
		if(repeatDataAttribute != 0)
		{
			repeatData = repeatDataAttribute->ExtractValue<bool>();
		}

		//Read in the ROM data
		unsigned int dataStreamByteSize = (unsigned int)dataStream.Size();
		unsigned int entriesInDataStream = (dataStreamByteSize / memoryArrayEntryByteSize);
		unsigned int entriesToRead = (memoryArraySize < entriesInDataStream)? memoryArraySize: entriesInDataStream;
		if(!dataStream.ReadDataBigEndian(&memoryArray[0], entriesToRead))
		{
			return false;
		}

		//If there is more space available in the memory array after reading in the ROM
		//data entries above, and there is remaining data in the ROM data stream that
		//doesn't align with a whole entry in the array, read in the remaining data into
		//the next array entry, and pad out the missing lower data with zeros.
		unsigned int bytesRemainingInDataStream = (dataStreamByteSize % memoryArrayEntryByteSize);
		if((memoryArraySize > entriesInDataStream) && (bytesRemainingInDataStream > 0))
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
				memoryArray[entriesToRead] = ((memoryArray[entriesToRead] << 8) | remainingData);
			}

			//Shift the data in the entry up by the required number of bits to align the
			//partial data read with the top of the entry, and pad out the lower bits with
			//zeros.
			memoryArray[entriesToRead] <<= (8 * (memoryArrayEntryByteSize - bytesRemainingInDataStream));
		}

		//If the data string has been set to repeat until the end of the memory block is
		//reached, fill out the remainder of the memory block now.
		if(repeatData)
		{
			for(unsigned int i = entriesToRead; i < memoryArraySize; ++i)
			{
				unsigned int originalDataIndex = i % entriesInDataStream;
				memoryArray[i] = memoryArray[originalDataIndex];
			}
		}
	}
	else
	{
		//If no embedded ROM data has been provided, ensure that a valid interface size
		//has been specified, and set the size of our internal memory.
		memoryArraySize = GetMemoryEntryCount();
		if(memoryArraySize <= 0)
		{
			return false;
		}
		delete memoryArray;
		memoryArray = new T[memoryArraySize];
		memset(&memoryArray[0], 0, (memoryArraySize * memoryArrayEntryByteSize));
	}

	return result;
}

//----------------------------------------------------------------------------------------
//Memory size functions
//----------------------------------------------------------------------------------------
template<class T> unsigned int ROMBase<T>::GetMemoryEntrySizeInBytes() const
{
	return sizeof(T);
}
