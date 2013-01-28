#include "TimedRAM.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
TimedRAM::TimedRAM(const std::wstring& ainstanceName, unsigned int amoduleID)
:MemoryWrite(L"TimedRAM", ainstanceName, amoduleID), memory(0)
{}

//----------------------------------------------------------------------------------------
bool TimedRAM::Construct(IHeirarchicalStorageNode& node)
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
//Initialization functions
//----------------------------------------------------------------------------------------
void TimedRAM::Initialize()
{
	memory.Initialize();
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void TimedRAM::ExecuteRollback()
{
	memory.Rollback();
}

//----------------------------------------------------------------------------------------
void TimedRAM::ExecuteCommit()
{
	memory.Commit();
}

//----------------------------------------------------------------------------------------
//Memory interface functions
//----------------------------------------------------------------------------------------
IBusInterface::AccessResult TimedRAM::ReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory.Read((location + i) % memory.Size(), accessTime));
	}
	return true;
}

//----------------------------------------------------------------------------------------
IBusInterface::AccessResult TimedRAM::WriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, double accessTime, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		unsigned int bytePos = (location + i) % memory.Size();
		if(!IsByteLocked(bytePos))
		{
			memory.Write(bytePos, accessTime, data.GetByte((dataByteSize - 1) - i));
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
void TimedRAM::TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory.ReadLatest((location + i) % memory.Size()));
	}
}

//----------------------------------------------------------------------------------------
void TimedRAM::TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller, unsigned int accessContext)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		memory.WriteLatest((location + i) % memory.Size(), data.GetByte((dataByteSize - 1) - i));
	}
}

//----------------------------------------------------------------------------------------
//Buffer access functions
//----------------------------------------------------------------------------------------
void TimedRAM::ReadCommitted(unsigned int location, Data& data)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory.ReadCommitted((location + i) % memory.Size()));
	}
}

//----------------------------------------------------------------------------------------
void TimedRAM::ReadCommitted(unsigned int location, double writeTime, Data& data)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory.ReadCommitted((location + i) % memory.Size(), writeTime));
	}
}

//----------------------------------------------------------------------------------------
void TimedRAM::GetLatestBufferCopy(std::vector<unsigned char>& buffer) const
{
	memory.GetLatestBufferCopy(buffer);
}

//----------------------------------------------------------------------------------------
void TimedRAM::ReadBuffer(unsigned int location, Data& data, const AccessTarget& accessTarget)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		data.SetByte((dataByteSize - 1) - i, memory.Read((location + i) % memory.Size(), accessTarget));
	}
}

//----------------------------------------------------------------------------------------
void TimedRAM::WriteBuffer(unsigned int location, const Data& data, const AccessTarget& accessTarget)
{
	unsigned int dataByteSize = data.GetByteSize();
	for(unsigned int i = 0; i < dataByteSize; ++i)
	{
		unsigned int bytePos = (location + i) % memory.Size();
		if(!IsByteLocked(bytePos) || (accessTarget.target == AccessTarget::TARGET_LATEST))
		{
			memory.Write(bytePos, data.GetByte((dataByteSize - 1) - i), accessTarget);
		}
	}
}

//----------------------------------------------------------------------------------------
//Memory locking functions
//----------------------------------------------------------------------------------------
bool TimedRAM::IsMemoryLockingSupported() const
{
	return true;
}

//----------------------------------------------------------------------------------------
void TimedRAM::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{
	for(unsigned int i = 0; i < size; ++i)
	{
		memoryLocked[location + i] = state;
	}
}

//----------------------------------------------------------------------------------------
bool TimedRAM::IsByteLocked(unsigned int location) const
{
	return memoryLocked[location];
}

//----------------------------------------------------------------------------------------
//Buffer time management functions
//----------------------------------------------------------------------------------------
TimedRAM::Timeslice TimedRAM::GetLatestTimeslice()
{
	return memory.GetLatestTimeslice();
}

//----------------------------------------------------------------------------------------
void TimedRAM::AdvancePastTimeslice(const Timeslice& targetTimeslice)
{
	memory.AdvancePastTimeslice(targetTimeslice);
}

//----------------------------------------------------------------------------------------
void TimedRAM::AdvanceToTimeslice(const Timeslice& targetTimeslice)
{
	memory.AdvanceToTimeslice(targetTimeslice);
}

//----------------------------------------------------------------------------------------
void TimedRAM::AdvanceByTime(double step, const Timeslice& targetTimeslice)
{
	memory.AdvanceByTime(step, targetTimeslice);
}

//----------------------------------------------------------------------------------------
bool TimedRAM::AdvanceByStep(const Timeslice& targetTimeslice)
{
	return memory.AdvanceByStep(targetTimeslice);
}

//----------------------------------------------------------------------------------------
double TimedRAM::GetNextWriteTime(const Timeslice& targetTimeslice)
{
	return memory.GetNextWriteTime(targetTimeslice);
}

//----------------------------------------------------------------------------------------
TimedRAM::WriteInfo TimedRAM::GetWriteInfo(unsigned int index, const Timeslice& targetTimeslice)
{
	return memory.GetWriteInfo(index, targetTimeslice);
}

//----------------------------------------------------------------------------------------
void TimedRAM::AddTimeslice(double timeslice)
{
	memory.AddTimeslice(timeslice);
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void TimedRAM::LoadState(IHeirarchicalStorageNode& node)
{
	memory.LoadState(node);
}

//----------------------------------------------------------------------------------------
void TimedRAM::SaveState(IHeirarchicalStorageNode& node) const
{
	memory.SaveState(node, GetDeviceInstanceName());
}
