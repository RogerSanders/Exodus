#include "RAM.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RAM::RAM(const std::wstring& aimplementationName, const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryWrite(aimplementationName, ainstanceName, amoduleID), initialMemoryDataSpecified(false), repeatInitialMemoryData(false), dataIsPersistent(false)
{}

//----------------------------------------------------------------------------------------
bool RAM::Construct(IHeirarchicalStorageNode& node)
{
	bool result = MemoryWrite::Construct(node);

	//Validate the specified interface size
	if(GetInterfaceSize() <= 0)
	{
		return false;
	}

	//Resize the internal memory array based on the specified interface size
	memory.resize(GetInterfaceSize());
	memoryLocked.resize(GetInterfaceSize());

	//Read the PersistentData attribute if specified
	IHeirarchicalStorageAttribute* persistentDataAttribute = node.GetAttribute(L"PersistentData");
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
		IHeirarchicalStorageAttribute* repeatDataAttribute = node.GetAttribute(L"RepeatData");
		if(repeatDataAttribute != 0)
		{
			repeatInitialMemoryData = repeatDataAttribute->ExtractValue<bool>();
		}

		//Read in the initial memory data
		unsigned int bytesInDataStream = (unsigned int)dataStream.Size();
		unsigned int memoryArraySize = GetInterfaceSize();
		unsigned int bytesToRead = (memoryArraySize < bytesInDataStream)? memoryArraySize: bytesInDataStream;
		initialMemoryData.resize(bytesInDataStream);
		if(!dataStream.ReadData(&initialMemoryData[0], bytesToRead))
		{
			return false;
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void RAM::Initialize()
{
	//Initialize the memory buffer
	for(unsigned int i = 0; i < (unsigned int)memory.size(); ++i)
	{
		if(!IsByteLocked(i))
		{
			unsigned char initialValue = 0;
			if(initialMemoryDataSpecified && (repeatInitialMemoryData || (i < (unsigned int)initialMemoryData.size())))
			{
				unsigned int initialMemoryDataIndex = (i % (unsigned int)initialMemoryData.size());
				initialValue = initialMemoryData[initialMemoryDataIndex];
			}
			memory[i] = initialValue;
		}
	}

	//Initialize rollback state
	buffer.clear();
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void RAM::ExecuteRollback()
{
	for(MemoryAccessBuffer::const_iterator i = buffer.begin(); i != buffer.end(); ++i)
	{
		memory[i->first] = i->second;
	}
	buffer.clear();
}

//----------------------------------------------------------------------------------------
void RAM::ExecuteCommit()
{
	buffer.clear();
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory[(location + i) % memory.size()]);
	}
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult RAM::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		unsigned int bytePos = (location + i) % (unsigned int)memory.size();
		if(!IsByteLocked(bytePos))
		{
			buffer.insert(MemoryAccessBufferEntry(bytePos, memory[bytePos]));
			memory[bytePos] = data.GetByte((dataByteSize - 1) - i);
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
void RAM::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory[(location + i) % memory.size()]);
	}
}

//----------------------------------------------------------------------------------------
void RAM::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		memory[(location + i) % memory.size()] = data.GetByte((dataByteSize - 1) - i);
	}
}

//----------------------------------------------------------------------------------------
//Memory locking functions
//----------------------------------------------------------------------------------------
bool RAM::IsMemoryLockingSupported() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void RAM::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{
	for(unsigned int i = 0; i < size; ++i)
	{
		memoryLocked[location + i] = state;
	}
}

//----------------------------------------------------------------------------------------
bool RAM::IsByteLocked(unsigned int location) const
{
	return memoryLocked[location];
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void RAM::LoadState(IHeirarchicalStorageNode& node)
{
	size_t memorySize = memory.size();
	node.ExtractBinaryData(memory);
	if(memory.size() < memorySize)
	{
		memory.insert(memory.end(), memorySize - memory.size(), 0);
	}

	MemoryWrite::LoadState(node);
}

//----------------------------------------------------------------------------------------
void RAM::SaveState(IHeirarchicalStorageNode& node) const
{
	node.InsertBinaryData(memory, GetFullyQualifiedDeviceInstanceName(), false);

	MemoryWrite::SaveState(node);
}

//----------------------------------------------------------------------------------------
void RAM::LoadPersistentState(IHeirarchicalStorageNode& node)
{
	if(dataIsPersistent)
	{
		size_t memorySize = memory.size();
		node.ExtractBinaryData(memory);
		if(memory.size() < memorySize)
		{
			memory.insert(memory.end(), memorySize - memory.size(), 0);
		}
	}

	MemoryWrite::LoadPersistentState(node);
}

//----------------------------------------------------------------------------------------
void RAM::SavePersistentState(IHeirarchicalStorageNode& node) const
{
	if(dataIsPersistent)
	{
		node.InsertBinaryData(memory, GetFullyQualifiedDeviceInstanceName(), false);
	}

	MemoryWrite::SavePersistentState(node);
}

//----------------------------------------------------------------------------------------
void RAM::LoadDebuggerState(IHeirarchicalStorageNode& node)
{
	size_t memorySize = memoryLocked.size();
	node.ExtractBinaryData(memoryLocked);
	if(memoryLocked.size() < memorySize)
	{
		memoryLocked.insert(memoryLocked.end(), memorySize - memoryLocked.size(), false);
	}

	MemoryWrite::LoadDebuggerState(node);
}

//----------------------------------------------------------------------------------------
void RAM::SaveDebuggerState(IHeirarchicalStorageNode& node) const
{
	node.InsertBinaryData(memoryLocked, GetFullyQualifiedDeviceInstanceName() + L".MemoryLockedState", false);

	MemoryWrite::SaveDebuggerState(node);
}
