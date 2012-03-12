#ifndef __MANAGEDBUFFERINT_H__
#define __MANAGEDBUFFERINT_H__
#include "MemoryWrite.h"
#include "Device/Device.pkg"
#include "TimedBuffers/TimedBuffers.pkg"
#include <vector>

class ManagedBufferInt :public IManagedBufferInt, public MemoryWrite
{
public: //Device functions
	//Constructors
	ManagedBufferInt(const std::wstring& ainstanceName, unsigned int amoduleID);
	virtual bool Construct(IHeirarchicalStorageNode& node);

	//Memory interface functions
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller);
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller);

	//Savestate functions
	virtual void LoadState(IHeirarchicalStorageNode& node);
	virtual void GetState(IHeirarchicalStorageNode& node) const;

	//Memory locking functions
	virtual bool IsMemoryLockingSupported() const;
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state);
	virtual bool IsByteLocked(unsigned int location) const;

public: //RandomTimeAccessBuffer functions
	//Size functions
	virtual unsigned int BufferSize() const;

	//Access functions
	virtual DataType BufferRead(unsigned int address, const AccessTarget& accessTarget) const;
	virtual void BufferWrite(unsigned int address, const DataType& data, const AccessTarget& accessTarget);
	virtual DataType BufferRead(unsigned int address, TimesliceType readTime) const;
	virtual void BufferWrite(unsigned int address, TimesliceType writeTime, const DataType& data);
	virtual DataType& BufferReferenceCommitted(unsigned int address);
	virtual DataType BufferReadCommitted(unsigned int address) const;
	virtual DataType BufferReadCommitted(unsigned int address, TimesliceType readTime) const;
	//##TODO## Consider removing this function
	virtual void BufferWriteCommitted(unsigned int address, const DataType& data);
	virtual DataType BufferReadLatest(unsigned int address) const;
	virtual void BufferWriteLatest(unsigned int address, const DataType& data);
	//##TODO## Consider removing this function
	virtual void BufferGetLatestBufferCopy(std::vector<DataType>& buffer) const;

	//Time management functions
	virtual void BufferInitialize();
	virtual Timeslice BufferGetLatestTimeslice();
	virtual void BufferAdvancePastTimeslice(const Timeslice& targetTimeslice);
	virtual void BufferAdvanceToTimeslice(const Timeslice& targetTimeslice);
	virtual void BufferAdvanceByTime(TimesliceType step, const Timeslice& targetTimeslice);
	virtual bool BufferAdvanceByStep(const Timeslice& targetTimeslice);
	virtual void BufferAdvanceBySession(TimesliceType currentProgress, AdvanceSession& advanceSession, const Timeslice& targetTimeslice);
	virtual TimesliceType BufferGetNextWriteTime(const Timeslice& targetTimeslice) const;
	virtual WriteInfo BufferGetWriteInfo(unsigned int index, const Timeslice& targetTimeslice);
	virtual void BufferCommit();
	virtual void BufferRollback();
	virtual void BufferAddTimeslice(TimesliceType timeslice);

private:
	RandomTimeAccessBuffer<DataType, TimesliceType> memory;
	std::vector<bool> memoryLocked;
};

#endif
