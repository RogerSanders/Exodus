#include "TimedBufferInt.h"
#include "TimedBufferTimeslice.h"

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
void TimedBufferInt::LoadState(IHeirarchicalStorageNode& node)
{
	memory.LoadState(node);
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::GetState(IHeirarchicalStorageNode& node, const std::wstring& bufferName) const
{
	memory.GetState(node, bufferName);
}

//----------------------------------------------------------------------------------------
//Memory locking functions
//----------------------------------------------------------------------------------------
void TimedBufferInt::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{
	for(unsigned int i = 0; i < size; ++i)
	{
		memoryLocked[location + i] = state;
	}
}

//----------------------------------------------------------------------------------------
bool TimedBufferInt::IsByteLocked(unsigned int location) const
{
	return memoryLocked[location];
}

//----------------------------------------------------------------------------------------
//Size functions
//----------------------------------------------------------------------------------------
unsigned int TimedBufferInt::Size() const
{
	return memory.Size();
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::Resize(unsigned int bufferSize)
{
	memory.Resize(bufferSize);
	memoryLocked.resize(bufferSize);
}

//----------------------------------------------------------------------------------------
//Access functions
//----------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::Read(unsigned int address, const AccessTarget& accessTarget) const
{
	return memory.Read(address, accessTarget);
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::Write(unsigned int address, const DataType& data, const AccessTarget& accessTarget)
{
	if(!IsByteLocked(address) || (accessTarget.target == AccessTarget::TARGET_LATEST))
	{
		memory.Write(address, data, accessTarget);
	}
}

//----------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::Read(unsigned int address, TimesliceType readTime) const
{
	return memory.Read(address, readTime);
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::Write(unsigned int address, TimesliceType writeTime, const DataType& data)
{
	if(!IsByteLocked(address))
	{
		memory.Write(address, writeTime, data);
	}
}

//----------------------------------------------------------------------------------------
TimedBufferInt::DataType& TimedBufferInt::ReferenceCommitted(unsigned int address)
{
	return memory.ReferenceCommitted(address);
}

//----------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::ReadCommitted(unsigned int address) const
{
	return memory.ReadCommitted(address);
}

//----------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::ReadCommitted(unsigned int address, TimesliceType readTime) const
{
	return memory.ReadCommitted(address, readTime);
}

//----------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::ReadLatest(unsigned int address) const
{
	return memory.ReadLatest(address);
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::WriteLatest(unsigned int address, const DataType& data)
{
	memory.WriteLatest(address, data);
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::GetLatestBufferCopy(DataType* buffer, unsigned int bufferSize) const
{
	memory.GetLatestBufferCopy(buffer, bufferSize);
}

//----------------------------------------------------------------------------------------
//Time management functions
//----------------------------------------------------------------------------------------
void TimedBufferInt::Initialize()
{
	memory.Initialize();
}

//----------------------------------------------------------------------------------------
TimedBufferInt::Timeslice* TimedBufferInt::GetLatestTimesliceReference()
{
	return new TimedBufferTimeslice<DataType, TimesliceType>(memory.GetLatestTimeslice());
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::FreeTimesliceReference(Timeslice* targetTimeslice)
{
	delete (TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice;
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::AdvancePastTimeslice(const Timeslice* targetTimeslice)
{
	memory.AdvancePastTimeslice(((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::AdvanceToTimeslice(const Timeslice* targetTimeslice)
{
	memory.AdvanceToTimeslice(((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::AdvanceByTime(TimesliceType step, const Timeslice* targetTimeslice)
{
	memory.AdvanceByTime(step, ((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------
bool TimedBufferInt::AdvanceByStep(const Timeslice* targetTimeslice)
{
	return memory.AdvanceByStep(((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::AdvanceBySession(TimesliceType currentProgress, AdvanceSession& advanceSession, const Timeslice* targetTimeslice)
{
	memory.AdvanceBySession(currentProgress, advanceSession, ((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------
TimedBufferInt::TimesliceType TimedBufferInt::GetNextWriteTime(const Timeslice* targetTimeslice) const
{
	return memory.GetNextWriteTime(((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------
TimedBufferInt::WriteInfo TimedBufferInt::GetWriteInfo(unsigned int index, const Timeslice* targetTimeslice)
{
	return memory.GetWriteInfo(index, ((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::Commit()
{
	memory.Commit();
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::Rollback()
{
	memory.Rollback();
}

//----------------------------------------------------------------------------------------
void TimedBufferInt::AddTimeslice(TimesliceType timeslice)
{
	memory.AddTimeslice(timeslice);
}
