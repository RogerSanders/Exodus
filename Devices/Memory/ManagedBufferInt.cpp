#include "ManagedBufferInt.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ManagedBufferInt::ManagedBufferInt(const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryWrite(L"ManagedBufferInt", ainstanceName, amoduleID), memory(0)
{}

//----------------------------------------------------------------------------------------
bool ManagedBufferInt::Construct(IHeirarchicalStorageNode& node)
{
	bool result = MemoryWrite::Construct(node);
	if(GetInterfaceSize() <= 0)
	{
		return false;
	}
	memory.Resize(GetInterfaceSize());
	memoryLocked.resize(GetInterfaceSize());
	return result;
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
void ManagedBufferInt::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory.ReadLatest((location + i) % memory.Size()));
	}
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		memory.WriteLatest((location + i) % memory.Size(), data.GetByte((dataByteSize - 1) - i));
	}
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void ManagedBufferInt::LoadState(IHeirarchicalStorageNode& node)
{
	memory.LoadState(node);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::GetState(IHeirarchicalStorageNode& node) const
{
	memory.GetState(node, GetDeviceInstanceName());
}

//----------------------------------------------------------------------------------------
//Memory locking functions
//----------------------------------------------------------------------------------------
bool ManagedBufferInt::IsMemoryLockingSupported() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{
	for(unsigned int i = 0; i < size; ++i)
	{
		memoryLocked[location + i] = state;
	}
}

//----------------------------------------------------------------------------------------
bool ManagedBufferInt::IsByteLocked(unsigned int location) const
{
	return memoryLocked[location];
}

//----------------------------------------------------------------------------------------
//Size functions
//----------------------------------------------------------------------------------------
unsigned int ManagedBufferInt::BufferSize() const
{
	return memory.Size();
}

//----------------------------------------------------------------------------------------
//Access functions
//----------------------------------------------------------------------------------------
ManagedBufferInt::DataType ManagedBufferInt::BufferRead(unsigned int address, const AccessTarget& accessTarget) const
{
	return memory.Read(address, accessTarget);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferWrite(unsigned int address, const DataType& data, const AccessTarget& accessTarget)
{
	if(!IsByteLocked(address) || (accessTarget.accessTarget == AccessTarget::ACCESSTARGET_LATEST))
	{
		memory.Write(address, data, accessTarget);
	}
}

//----------------------------------------------------------------------------------------
ManagedBufferInt::DataType ManagedBufferInt::BufferRead(unsigned int address, TimesliceType readTime) const
{
	return memory.Read(address, readTime);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferWrite(unsigned int address, TimesliceType writeTime, const DataType& data)
{
	if(!IsByteLocked(address))
	{
		memory.Write(address, writeTime, data);
	}
}

//----------------------------------------------------------------------------------------
ManagedBufferInt::DataType& ManagedBufferInt::BufferReferenceCommitted(unsigned int address)
{
	return memory.ReferenceCommitted(address);
}

//----------------------------------------------------------------------------------------
ManagedBufferInt::DataType ManagedBufferInt::BufferReadCommitted(unsigned int address) const
{
	return memory.ReadCommitted(address);
}

//----------------------------------------------------------------------------------------
ManagedBufferInt::DataType ManagedBufferInt::BufferReadCommitted(unsigned int address, TimesliceType readTime) const
{
	return memory.ReadCommitted(address, readTime);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferWriteCommitted(unsigned int address, const DataType& data)
{
	memory.WriteCommitted(address, data);
}

//----------------------------------------------------------------------------------------
ManagedBufferInt::DataType ManagedBufferInt::BufferReadLatest(unsigned int address) const
{
	return memory.ReadLatest(address);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferWriteLatest(unsigned int address, const DataType& data)
{
	memory.WriteLatest(address, data);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferGetLatestBufferCopy(std::vector<DataType>& buffer) const
{
	memory.GetLatestBufferCopy(buffer);
}

//----------------------------------------------------------------------------------------
//Time management functions
//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferInitialize()
{
	memory.Initialize();
}

//----------------------------------------------------------------------------------------
ManagedBufferInt::Timeslice ManagedBufferInt::BufferGetLatestTimeslice()
{
	return memory.GetLatestTimeslice();
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferAdvancePastTimeslice(const Timeslice& targetTimeslice)
{
	memory.AdvancePastTimeslice(targetTimeslice);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferAdvanceToTimeslice(const Timeslice& targetTimeslice)
{
	memory.AdvanceToTimeslice(targetTimeslice);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferAdvanceByTime(TimesliceType step, const Timeslice& targetTimeslice)
{
	memory.AdvanceByTime(step, targetTimeslice);
}

//----------------------------------------------------------------------------------------
bool ManagedBufferInt::BufferAdvanceByStep(const Timeslice& targetTimeslice)
{
	return memory.AdvanceByStep(targetTimeslice);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferAdvanceBySession(TimesliceType currentProgress, AdvanceSession& advanceSession, const Timeslice& targetTimeslice)
{
	memory.AdvanceBySession(currentProgress, advanceSession, targetTimeslice);
}

//----------------------------------------------------------------------------------------
ManagedBufferInt::TimesliceType ManagedBufferInt::BufferGetNextWriteTime(const Timeslice& targetTimeslice) const
{
	return memory.GetNextWriteTime(targetTimeslice);
}

//----------------------------------------------------------------------------------------
ManagedBufferInt::WriteInfo ManagedBufferInt::BufferGetWriteInfo(unsigned int index, const Timeslice& targetTimeslice)
{
	return memory.GetWriteInfo(index, targetTimeslice);
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferCommit()
{
	memory.Commit();
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferRollback()
{
	memory.Rollback();
}

//----------------------------------------------------------------------------------------
void ManagedBufferInt::BufferAddTimeslice(TimesliceType timeslice)
{
	memory.AddTimeslice(timeslice);
}
