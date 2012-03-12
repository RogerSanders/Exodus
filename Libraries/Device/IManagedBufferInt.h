#ifndef __IMANAGEDBUFFERINT_H__
#define __IMANAGEDBUFFERINT_H__
#include "RandomTimeAccessBuffer.h"

//##TODO## Eliminate the use of STL objects on this interface, so that it can safely be
//used across dll boundaries.
//##TODO## Eliminate any references to the underlying RandomTimeAccessBuffer from this
//interface, for safely as well as design reasons.
//##TODO## Separate this and RandomTimeAccessBuffer out into separate libraries. They
//don't really belong in the Device library.
class IManagedBufferInt
{
public:
	//Typedefs
	//##TODO## Change this to actually be an int, and make a separate IManagedBufferDouble
	//container
//	typedef double TimesliceType;
	typedef unsigned int TimesliceType;
	typedef unsigned char DataType;
	typedef RandomTimeAccessBuffer<DataType, TimesliceType>::AccessTarget AccessTarget;
	typedef RandomTimeAccessBuffer<DataType, TimesliceType>::Timeslice Timeslice;
	typedef RandomTimeAccessBuffer<DataType, TimesliceType>::WriteInfo WriteInfo;
	typedef RandomTimeAccessBuffer<DataType, TimesliceType>::AdvanceSession AdvanceSession;

public: //Device functions
	//Memory interface functions
	virtual void TransparentReadInterface(unsigned int interfaceNumber, unsigned int location, Data& data, IDeviceContext* caller) = 0;
	virtual void TransparentWriteInterface(unsigned int interfaceNumber, unsigned int location, const Data& data, IDeviceContext* caller) = 0;

public: //RandomTimeAccessBuffer functions
	//Size functions
	virtual unsigned int BufferSize() const = 0;

	//Access functions
	virtual DataType BufferRead(unsigned int address, const AccessTarget& accessTarget) const = 0;
	//##FIX## We have the arguments the opposite way around for the two overloads of the
	//BufferWrite function
	virtual void BufferWrite(unsigned int address, const DataType& data, const AccessTarget& accessTarget) = 0;
	virtual DataType BufferRead(unsigned int address, TimesliceType readTime) const = 0;
	virtual void BufferWrite(unsigned int address, TimesliceType writeTime, const DataType& data) = 0;
	virtual DataType& BufferReferenceCommitted(unsigned int address) = 0;
	virtual DataType BufferReadCommitted(unsigned int address) const = 0;
	virtual DataType BufferReadCommitted(unsigned int address, TimesliceType readTime) const = 0;
	//##TODO## Consider removing this function
	virtual void BufferWriteCommitted(unsigned int address, const DataType& data) = 0;
	virtual DataType BufferReadLatest(unsigned int address) const = 0;
	virtual void BufferWriteLatest(unsigned int address, const DataType& data) = 0;
	//##TODO## Consider removing this function
	virtual void BufferGetLatestBufferCopy(std::vector<DataType>& buffer) const = 0;

	//Time management functions
	virtual void BufferInitialize() = 0;
	virtual Timeslice BufferGetLatestTimeslice() = 0;
	virtual void BufferAdvancePastTimeslice(const Timeslice& targetTimeslice) = 0;
	virtual void BufferAdvanceToTimeslice(const Timeslice& targetTimeslice) = 0;
	virtual void BufferAdvanceByTime(TimesliceType step, const Timeslice& targetTimeslice) = 0;
	virtual bool BufferAdvanceByStep(const Timeslice& targetTimeslice) = 0;
	virtual void BufferAdvanceBySession(TimesliceType currentProgress, AdvanceSession& advanceSession, const Timeslice& targetTimeslice) = 0;
	virtual TimesliceType BufferGetNextWriteTime(const Timeslice& targetTimeslice) const = 0;
	virtual WriteInfo BufferGetWriteInfo(unsigned int index, const Timeslice& targetTimeslice) = 0;
	virtual void BufferCommit() = 0;
	virtual void BufferRollback() = 0;
	virtual void BufferAddTimeslice(TimesliceType timeslice) = 0;
};

#endif
