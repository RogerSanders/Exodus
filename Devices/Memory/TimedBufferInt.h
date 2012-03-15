#ifndef __TIMEDBUFFERINTSHELL_H__
#define __TIMEDBUFFERINTSHELL_H__
#include "TimedBuffers/TimedBuffers.pkg"
#include <vector>

class TimedBufferInt :public ITimedBufferInt
{
public:
	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void GetState(IHeirarchicalStorageNode& node, const std::wstring& bufferName) const;

	//Memory locking functions
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsByteLocked(unsigned int location) const;

	//Size functions
	virtual unsigned int Size() const;
	void Resize(unsigned int bufferSize);

	//Access functions
	virtual DataType Read(unsigned int address, const AccessTarget& accessTarget) const;
	virtual void Write(unsigned int address, const DataType& data, const AccessTarget& accessTarget);
	virtual DataType Read(unsigned int address, TimesliceType readTime) const;
	virtual void Write(unsigned int address, TimesliceType writeTime, const DataType& data);
	virtual DataType& ReferenceCommitted(unsigned int address);
	virtual DataType ReadCommitted(unsigned int address) const;
	virtual DataType ReadCommitted(unsigned int address, TimesliceType readTime) const;
	virtual DataType ReadLatest(unsigned int address) const;
	virtual void WriteLatest(unsigned int address, const DataType& data);

	//Time management functions
	virtual void Initialize();
	virtual Timeslice* GetLatestTimesliceReference();
	virtual void FreeTimesliceReference(Timeslice* targetTimeslice);
	virtual void AdvancePastTimeslice(const Timeslice* targetTimeslice);
	virtual void AdvanceToTimeslice(const Timeslice* targetTimeslice);
	virtual void AdvanceByTime(TimesliceType step, const Timeslice* targetTimeslice);
	virtual bool AdvanceByStep(const Timeslice* targetTimeslice);
	virtual void AdvanceBySession(TimesliceType currentProgress, AdvanceSession& advanceSession, const Timeslice* targetTimeslice);
	virtual TimesliceType GetNextWriteTime(const Timeslice* targetTimeslice) const;
	virtual WriteInfo GetWriteInfo(unsigned int index, const Timeslice* targetTimeslice);
	virtual void Commit();
	virtual void Rollback();
	virtual void AddTimeslice(TimesliceType timeslice);

	//Session management functions
	virtual void BeginAdvanceSession(AdvanceSession& advanceSession, const Timeslice* targetTimeslice, bool retrieveWriteInfo) const;

protected:
	//Access functions
	virtual void GetLatestBufferCopy(DataType* buffer, unsigned int bufferSize) const;

private:
	RandomTimeAccessBuffer<DataType, TimesliceType> memory;
	std::vector<bool> memoryLocked;
};

#endif
