#include "SharedRAM.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SharedRAM::SharedRAM(const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryWrite(L"SharedRAM", ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
bool SharedRAM::Construct(IHeirarchicalStorageNode& node)
{
	bool result = MemoryWrite::Construct(node);
	if(GetInterfaceSize() <= 0)
	{
		return false;
	}
	memory.resize(GetInterfaceSize());
	memoryLocked.resize(GetInterfaceSize());
	return result;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void SharedRAM::Initialize()
{
	//Initialize the memory buffer
	memory.assign(GetInterfaceSize(), 0);

	//Initialize rollback state
	buffer.clear();
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void SharedRAM::ExecuteRollback()
{
	boost::mutex::scoped_lock lock(accessLock);
	for(MemoryAccessBuffer::const_iterator i = buffer.begin(); i != buffer.end(); ++i)
	{
		memory[i->first] = i->second.data;
	}
	buffer.clear();
}

//----------------------------------------------------------------------------------------
void SharedRAM::ExecuteCommit()
{
	boost::mutex::scoped_lock lock(accessLock);
	buffer.clear();
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult SharedRAM::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(accessLock);

	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		MemoryAccessBuffer::iterator bufferEntryIterator = buffer.find(location + i);
		if(bufferEntryIterator == buffer.end())
		{
			//If the location hasn't been tagged, mark it
			buffer.insert(MemoryAccessBufferEntry(location + i, MemoryWriteStatus(false, memory[(location + i) % memory.size()], caller, accessTime, accessContext)));
		}
		else
		{
			MemoryWriteStatus* bufferEntry = &bufferEntryIterator->second;
			//If the location was tagged by a different author, mark it as shared
			bufferEntry->shared |= (bufferEntry->author != caller);
			if(bufferEntry->shared && (accessTime > bufferEntry->timeslice))
			{
				bufferEntry->timeslice = accessTime;
				bufferEntry->author = caller;
			}
			if(bufferEntry->written && bufferEntry->shared)
			{
				//If the value has been written to, and the address is shared, roll back
				GetDeviceContext()->SetSystemRollback(GetDeviceContext(), bufferEntry->author, bufferEntry->timeslice, bufferEntry->accessContext);
			}
		}
		data.SetByte((dataByteSize - 1) - i, memory[(location + i) % memory.size()]);
	}

	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult SharedRAM::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	boost::mutex::scoped_lock lock(accessLock);

	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		unsigned int bytePos = (location + i) % (unsigned int)memory.size();
		if(!IsByteLocked(bytePos))
		{
			MemoryAccessBuffer::iterator bufferEntryIterator = buffer.find(location + i);
			if(bufferEntryIterator == buffer.end())
			{
				//If the location hasn't been tagged, mark it
				buffer.insert(MemoryAccessBufferEntry(bytePos, MemoryWriteStatus(true, memory[bytePos], caller, accessTime, accessContext)));
			}
			else
			{
				MemoryWriteStatus* bufferEntry = &bufferEntryIterator->second;
				bufferEntry->written = true;
				//If the location was tagged by a different author, mark it as shared
				bufferEntry->shared |= (bufferEntry->author != caller);
				if(bufferEntry->shared && (accessTime > bufferEntry->timeslice))
				{
					bufferEntry->timeslice = accessTime;
					bufferEntry->author = caller;
				}
				//If the address is shared, roll back
				if(bufferEntry->shared)
				{
					GetDeviceContext()->SetSystemRollback(GetDeviceContext(), bufferEntry->author, bufferEntry->timeslice, bufferEntry->accessContext);
				}
			}
			memory[bytePos] = data.GetByte((dataByteSize - 1) - i);
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
void SharedRAM::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory[(location + i) % memory.size()]);
	}
}

//----------------------------------------------------------------------------------------
void SharedRAM::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
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
bool SharedRAM::IsMemoryLockingSupported() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void SharedRAM::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{
	for(unsigned int i = 0; i < size; ++i)
	{
		memoryLocked[location + i] = state;
	}
}

//----------------------------------------------------------------------------------------
bool SharedRAM::IsByteLocked(unsigned int location) const
{
	return memoryLocked[location];
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void SharedRAM::LoadState(IHeirarchicalStorageNode& node)
{
	size_t memorySize = memory.size();
	node.ExtractBinaryData(memory);
	if(memory.size() < memorySize)
	{
		memory.insert(memory.end(), memorySize - memory.size(), 0);
	}
}

//----------------------------------------------------------------------------------------
void SharedRAM::GetState(IHeirarchicalStorageNode& node) const
{
	node.InsertBinaryData(memory, GetDeviceInstanceName(), false);
}
