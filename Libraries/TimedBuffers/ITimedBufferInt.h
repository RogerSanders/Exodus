#ifndef __ITIMEDBUFFERINT_H__
#define __ITIMEDBUFFERINT_H__
#include "ITimedBufferTimeslice.h"
#include "TimedBufferWriteInfo.h"
#include "TimedBufferAccessTarget.h"
#include "TimedBufferAdvanceSession.h"

class ITimedBufferInt
{
public:
	//Typedefs
	typedef unsigned int TimesliceType;
	typedef unsigned char DataType;
	typedef ITimedBufferTimeslice<DataType, TimesliceType> Timeslice;
	typedef TimedBufferWriteInfo<DataType, TimesliceType> WriteInfo;
	typedef TimedBufferAccessTarget<DataType, TimesliceType> AccessTarget;
	typedef TimedBufferAdvanceSession<DataType, TimesliceType> AdvanceSession;

public:
	//Make sure the object can't be deleted from this base
	protected: virtual ~ITimedBufferInt() = 0 {} public:

	//Size functions
	virtual unsigned int Size() const = 0;

	//Access functions
	virtual DataType Read(unsigned int address, const AccessTarget& accessTarget) const = 0;
	//##FIX## We have the arguments the opposite way around for the two overloads of the
	//BufferWrite function
	virtual void Write(unsigned int address, const DataType& data, const AccessTarget& accessTarget) = 0;
	virtual DataType Read(unsigned int address, TimesliceType readTime) const = 0;
	virtual void Write(unsigned int address, TimesliceType writeTime, const DataType& data) = 0;
	virtual DataType& ReferenceCommitted(unsigned int address) = 0;
	virtual DataType ReadCommitted(unsigned int address) const = 0;
	virtual DataType ReadCommitted(unsigned int address, TimesliceType readTime) const = 0;
	virtual DataType ReadLatest(unsigned int address) const = 0;
	virtual void WriteLatest(unsigned int address, const DataType& data) = 0;
	inline void GetLatestBufferCopy(std::vector<DataType>& buffer) const;

	//Time management functions
	virtual void Initialize() = 0;
	virtual bool DoesLatestTimesliceExist() const = 0;
	virtual Timeslice* GetLatestTimesliceReference() = 0;
	virtual void FreeTimesliceReference(Timeslice* targetTimeslice) = 0;
	virtual void AdvancePastTimeslice(const Timeslice* targetTimeslice) = 0;
	virtual void AdvanceToTimeslice(const Timeslice* targetTimeslice) = 0;
	virtual void AdvanceByTime(TimesliceType step, const Timeslice* targetTimeslice) = 0;
	virtual bool AdvanceByStep(const Timeslice* targetTimeslice) = 0;
	virtual void AdvanceBySession(TimesliceType currentProgress, AdvanceSession& advanceSession, const Timeslice* targetTimeslice) = 0;
	virtual TimesliceType GetNextWriteTime(const Timeslice* targetTimeslice) const = 0;
	virtual WriteInfo GetWriteInfo(unsigned int index, const Timeslice* targetTimeslice) = 0;
	virtual void Commit() = 0;
	virtual void Rollback() = 0;
	virtual void AddTimeslice(TimesliceType timeslice) = 0;

	//Session management functions
	virtual void BeginAdvanceSession(AdvanceSession& advanceSession, const Timeslice* targetTimeslice, bool retrieveWriteInfo) const = 0;

	//Memory locking functions
	virtual void LockMemoryBlock(unsigned int location, unsigned int size, bool state) = 0;
	virtual bool IsByteLocked(unsigned int location) const = 0;

protected:
	//Access functions
	virtual void GetLatestBufferCopy(DataType* buffer, unsigned int bufferSize) const = 0;
};

#include "ITimedBufferInt.inl"
#endif
