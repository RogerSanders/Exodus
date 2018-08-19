#include "TimedBufferInt.h"
#include "TimedBufferTimeslice.h"

//----------------------------------------------------------------------------------------------------------------------
// Size functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int TimedBufferInt::Size() const
{
	return _memory.Size();
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::Resize(unsigned int bufferSize, bool keepLatestBufferCopy)
{
	_memory.Resize(bufferSize, keepLatestBufferCopy);
	_memoryLocked.resize(bufferSize);
}

//----------------------------------------------------------------------------------------------------------------------
// Access functions
//----------------------------------------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::Read(unsigned int address, const AccessTarget& accessTarget) const
{
	return _memory.Read(address, accessTarget);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::Write(unsigned int address, const DataType& data, const AccessTarget& accessTarget)
{
	if (!IsByteLocked(address) || (accessTarget.target == AccessTarget::TARGET_LATEST))
	{
		_memory.Write(address, data, accessTarget);
	}
}

//----------------------------------------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::Read(unsigned int address, TimesliceType readTime) const
{
	return _memory.Read(address, readTime);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::Write(unsigned int address, TimesliceType writeTime, const DataType& data)
{
	if (!IsByteLocked(address))
	{
		_memory.Write(address, writeTime, data);
	}
}

//----------------------------------------------------------------------------------------------------------------------
TimedBufferInt::DataType& TimedBufferInt::ReferenceCommitted(unsigned int address)
{
	return _memory.ReferenceCommitted(address);
}

//----------------------------------------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::ReadCommitted(unsigned int address) const
{
	return _memory.ReadCommitted(address);
}

//----------------------------------------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::ReadCommitted(unsigned int address, TimesliceType readTime) const
{
	return _memory.ReadCommitted(address, readTime);
}

//----------------------------------------------------------------------------------------------------------------------
TimedBufferInt::DataType TimedBufferInt::ReadLatest(unsigned int address) const
{
	return _memory.ReadLatest(address);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::WriteLatest(unsigned int address, const DataType& data)
{
	_memory.WriteLatest(address, data);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::GetLatestBufferCopy(DataType* buffer, unsigned int bufferSize) const
{
	_memory.GetLatestBufferCopy(buffer, bufferSize);
}

//----------------------------------------------------------------------------------------------------------------------
// Time management functions
//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::Initialize()
{
	_memory.Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
bool TimedBufferInt::DoesLatestTimesliceExist() const
{
	return _memory.DoesLatestTimesliceExist();
}

//----------------------------------------------------------------------------------------------------------------------
TimedBufferInt::Timeslice* TimedBufferInt::GetLatestTimesliceReference()
{
	return new TimedBufferTimeslice<DataType, TimesliceType>(_memory.GetLatestTimeslice());
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::FreeTimesliceReference(Timeslice* targetTimeslice)
{
	delete (TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice;
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::AdvancePastTimeslice(const Timeslice* targetTimeslice)
{
	_memory.AdvancePastTimeslice(((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::AdvanceToTimeslice(const Timeslice* targetTimeslice)
{
	_memory.AdvanceToTimeslice(((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::AdvanceByTime(TimesliceType step, const Timeslice* targetTimeslice)
{
	_memory.AdvanceByTime(step, ((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------------------------------------
bool TimedBufferInt::AdvanceByStep(const Timeslice* targetTimeslice)
{
	return _memory.AdvanceByStep(((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::AdvanceBySession(TimesliceType currentProgress, AdvanceSession& advanceSession, const Timeslice* targetTimeslice)
{
	_memory.AdvanceBySession(currentProgress, advanceSession, ((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------------------------------------
TimedBufferInt::TimesliceType TimedBufferInt::GetNextWriteTime(const Timeslice* targetTimeslice) const
{
	return _memory.GetNextWriteTime(((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------------------------------------
TimedBufferInt::WriteInfo TimedBufferInt::GetWriteInfo(unsigned int index, const Timeslice* targetTimeslice)
{
	return _memory.GetWriteInfo(index, ((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::Commit()
{
	_memory.Commit();
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::Rollback()
{
	_memory.Rollback();
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::AddTimeslice(TimesliceType timeslice)
{
	_memory.AddTimeslice(timeslice);
}

//----------------------------------------------------------------------------------------------------------------------
// Session management functions
//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::BeginAdvanceSession(AdvanceSession& advanceSession, const Timeslice* targetTimeslice, bool retrieveWriteInfo) const
{
	_memory.BeginAdvanceSession(advanceSession, ((TimedBufferTimeslice<DataType, TimesliceType>*)targetTimeslice)->timeslice, retrieveWriteInfo);
}

//----------------------------------------------------------------------------------------------------------------------
// Memory locking functions
//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::LockMemoryBlock(unsigned int location, unsigned int size, bool state)
{
	for (unsigned int i = 0; i < size; ++i)
	{
		_memoryLocked[location + i] = state;
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool TimedBufferInt::IsByteLocked(unsigned int location) const
{
	return _memoryLocked[location];
}

//----------------------------------------------------------------------------------------------------------------------
// Savestate functions
//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::LoadState(IHierarchicalStorageNode& node)
{
	_memory.LoadState(node);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::SaveState(IHierarchicalStorageNode& node, const std::wstring& bufferName) const
{
	_memory.SaveState(node, bufferName);
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::LoadDebuggerState(IHierarchicalStorageNode& node)
{
	size_t memorySize = _memoryLocked.size();
	Stream::IStream::SizeType readCount = (node.GetBinaryDataBufferStream().Size() / (Stream::IStream::SizeType)sizeof(unsigned char));
	node.ExtractBinaryData(_memoryLocked);
	for (size_t i = readCount; i < memorySize; ++i)
	{
		_memoryLocked[i] = 0;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void TimedBufferInt::SaveDebuggerState(IHierarchicalStorageNode& node, const std::wstring& bufferName) const
{
	node.InsertBinaryData(_memoryLocked, bufferName + L".MemoryLockedState", false);
}
