#ifndef __TIMEDBUFFERWRITEINFO_H__
#define __TIMEDBUFFERWRITEINFO_H__

template<class DataType, class TimesliceType> struct TimedBufferWriteInfo
{
public:
	//Constructors
	inline TimedBufferWriteInfo();
	inline TimedBufferWriteInfo(const DataType& defaultValue);

public:
	//Data members
	bool exists;
	unsigned int writeAddress;
	TimesliceType writeTime;
	DataType newValue;
};

#include "TimedBufferWriteInfo.inl"
#endif
