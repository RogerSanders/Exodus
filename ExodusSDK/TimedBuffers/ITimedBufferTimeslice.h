#ifndef __ITIMEDBUFFERTIMESLICE_H__
#define __ITIMEDBUFFERTIMESLICE_H__

// Workaround for Clang compiler bug. Originally reported in 2014 under https://bugs.llvm.org/show_bug.cgi?id=21334.
// Still current as of 2018-08-20 on LLVM 6.0.1.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmicrosoft-pure-definition"
#endif

template<class DataType, class TimesliceType> class ITimedBufferTimeslice
{
protected:
	// Make sure the object can't be deleted from this base
	inline virtual ~ITimedBufferTimeslice() = 0;
};
template<class DataType, class TimesliceType>
ITimedBufferTimeslice<DataType, TimesliceType>::~ITimedBufferTimeslice() { }

// Restore the disabled warnings
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif
