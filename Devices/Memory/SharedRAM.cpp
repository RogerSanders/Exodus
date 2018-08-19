#include "SharedRAM.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SharedRAM::SharedRAM(const std::wstring& implementationName, const std::wstring& instanceName, unsigned int moduleID)
:MemoryWrite(implementationName, instanceName, moduleID)
{}

//----------------------------------------------------------------------------------------
bool SharedRAM::Construct(IHierarchicalStorageNode& node)
{
	bool result = MemoryWrite::Construct(node);
	if(GetMemoryEntryCount() <= 0)
	{
		return false;
	}
	_memory.resize(GetMemoryEntryCount());
	_memoryLocked.resize(GetMemoryEntryCount());
	return result;
}

//----------------------------------------------------------------------------------------
//Memory size functions
//----------------------------------------------------------------------------------------
unsigned int SharedRAM::GetMemoryEntrySizeInBytes() const
{
	return 1;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
void SharedRAM::Initialize()
{
	//Initialize the memory buffer
	_memory.assign(GetMemoryEntryCount(), 0);

	//Initialize rollback state
	_buffer.clear();
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void SharedRAM::ExecuteRollback()
{
	std::unique_lock<std::mutex> lock(_accessLock);
	for(MemoryAccessBuffer::const_iterator i = _buffer.begin(); i != _buffer.end(); ++i)
	{
		_memory[i->first] = i->second.data;
	}
	_buffer.clear();
}

//----------------------------------------------------------------------------------------
void SharedRAM::ExecuteCommit()
{
	std::unique_lock<std::mutex> lock(_accessLock);
	_buffer.clear();
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult SharedRAM::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_accessLock);

	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		MemoryAccessBuffer::iterator bufferEntryIterator = _buffer.find(location + i);
		if(bufferEntryIterator == _buffer.end())
		{
			//If the location hasn't been tagged, mark it
			_buffer.insert(MemoryAccessBufferEntry(location + i, MemoryWriteStatus(false, _memory[(location + i) % _memory.size()], caller, accessTime, accessContext)));
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
				GetSystemInterface().SetSystemRollback(GetDeviceContext(), bufferEntry->author, bufferEntry->timeslice, bufferEntry->accessContext);
			}
		}
		data.SetByteFromTopDown(i, _memory[(location + i) % _memory.size()]);
	}

	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult SharedRAM::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	std::unique_lock<std::mutex> lock(_accessLock);

	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		unsigned int bytePos = (location + i) % (unsigned int)_memory.size();
		if(!IsAddressLocked(bytePos))
		{
			MemoryAccessBuffer::iterator bufferEntryIterator = _buffer.find(location + i);
			if(bufferEntryIterator == _buffer.end())
			{
				//If the location hasn't been tagged, mark it
				_buffer.insert(MemoryAccessBufferEntry(bytePos, MemoryWriteStatus(true, _memory[bytePos], caller, accessTime, accessContext)));
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
					GetSystemInterface().SetSystemRollback(GetDeviceContext(), bufferEntry->author, bufferEntry->timeslice, bufferEntry->accessContext);
				}
			}
			_memory[bytePos] = data.GetByteFromTopDown(i);
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
		data.SetByteFromTopDown(i, _memory[(location + i) % _memory.size()]);
	}
}

//----------------------------------------------------------------------------------------
void SharedRAM::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		_memory[(location + i) % _memory.size()] = data.GetByteFromTopDown(i);
	}
}

//----------------------------------------------------------------------------------------
//Debug memory access functions
//----------------------------------------------------------------------------------------
unsigned int SharedRAM::ReadMemoryEntry(unsigned int location) const
{
	return _memory[location % _memory.size()];
}

//----------------------------------------------------------------------------------------
void SharedRAM::WriteMemoryEntry(unsigned int location, unsigned int data)
{
	_memory[location % _memory.size()] = (unsigned char)data;
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
		_memoryLocked[location + i] = state;
	}
}

//----------------------------------------------------------------------------------------
bool SharedRAM::IsAddressLocked(unsigned int location) const
{
	return _memoryLocked[location];
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void SharedRAM::LoadState(IHierarchicalStorageNode& node)
{
	size_t memorySize = _memory.size();
	Stream::IStream::SizeType readCount = (node.GetBinaryDataBufferStream().Size() / (Stream::IStream::SizeType)sizeof(unsigned char));
	node.ExtractBinaryData(_memory);
	for(size_t i = readCount; i < memorySize; ++i)
	{
		_memory[i] = 0;
	}
}

//----------------------------------------------------------------------------------------
void SharedRAM::SaveState(IHierarchicalStorageNode& node) const
{
	node.InsertBinaryData(_memory, GetFullyQualifiedDeviceInstanceName(), false);
}
