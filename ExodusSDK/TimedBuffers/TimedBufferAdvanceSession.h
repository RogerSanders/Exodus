#ifndef __TIMEDBUFFERADVANCESESSION_H__
#define __TIMEDBUFFERADVANCESESSION_H__

template<class DataType, class TimesliceType> struct TimedBufferAdvanceSession
{
public:
	//Constructors
	inline TimedBufferAdvanceSession(const DataType& writeDataDefaultValue);

public:
	//Data members
	bool retrieveWriteInfo;
	TimesliceType timeRemovedDuringSession;
	TimesliceType initialTimeOffset;
	TimesliceType nextWriteTime;
	TimedBufferWriteInfo<DataType, TimesliceType> writeInfo;
};

#include "TimedBufferAdvanceSession.inl"
#endif
