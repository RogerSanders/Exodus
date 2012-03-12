#ifndef __PERFORMANCELOCK_H__
#define __PERFORMANCELOCK_H__
#include "PerformanceMutex.h"

class PerformanceLock
{
public:
	PerformanceLock(PerformanceMutex& amutex)
	:mutex(amutex)
	{
		mutex.Lock();
	}
	~PerformanceLock()
	{
		mutex.Unlock();
	}

private:
	PerformanceLock(const PerformanceLock& object)
	:mutex(object.mutex)
	{}

private:
	PerformanceMutex& mutex;
};

#endif
