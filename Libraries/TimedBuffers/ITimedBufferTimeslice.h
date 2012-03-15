#ifndef __ITIMEDBUFFERTIMESLICE_H__
#define __ITIMEDBUFFERTIMESLICE_H__

template<class DataType, class TimesliceType> class ITimedBufferTimeslice
{
	//Make sure the object can't be deleted from this base
	protected: virtual ~ITimedBufferTimeslice() = 0 {} public:
};

#endif
