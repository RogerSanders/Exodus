#include "RAM.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RAM::RAM(const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryWrite(L"RAM", ainstanceName, amoduleID)
{}

//----------------------------------------------------------------------------------------
bool RAM::Construct(IHeirarchicalStorageNode& node)
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
void RAM::Initialize()
{
	//Initialize the memory buffer
	memory.assign(GetInterfaceSize(), 0);

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
	node.InsertBinaryData(memory, GetDeviceInstanceName(), false);

	MemoryWrite::SaveState(node);
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
	node.InsertBinaryData(memoryLocked, GetDeviceInstanceName() + L" - MemoryLockedState", false);

	MemoryWrite::SaveDebuggerState(node);
}
