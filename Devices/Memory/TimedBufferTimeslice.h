#ifndef __TIMEDBUFFERTIMESLICE_H__
#define __TIMEDBUFFERTIMESLICE_H__
#include "TimedBuffers/TimedBuffers.pkg"

template<class DataType, class TimesliceType> class TimedBufferTimeslice :public ITimedBufferTimeslice<DataType, TimesliceType>
{
public:
	//Constructors
	inline TimedBufferTimeslice(typename RandomTimeAccessBuffer<DataType, TimesliceType>::Timeslice atimeslice);

public:
	//Data members
	typename RandomTimeAccessBuffer<DataType, TimesliceType>::Timeslice timeslice;
};

#include "TimedBufferTimeslice.inl"
#endif
