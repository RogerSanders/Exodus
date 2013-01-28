#ifndef __PERFORMANCELOCK_H__
#define __PERFORMANCELOCK_H__
#include "PerformanceMutex.h"

struct PerformanceLock
{
public:
	inline PerformanceLock(PerformanceMutex& amutex)
	:mutex(amutex)
	{
		mutex.Lock();
	}
	inline ~PerformanceLock()
	{
		mutex.Unlock();
	}

private:
	//Don't allow the lock to be copied
	PerformanceLock(const PerformanceLock& object)
	:mutex(object.mutex)
	{}

private:
	PerformanceMutex& mutex;
};

#endif
