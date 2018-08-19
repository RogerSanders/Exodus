#ifndef __PERFORMANCELOCK_H__
#define __PERFORMANCELOCK_H__
#include "PerformanceMutex.h"

struct PerformanceLock
{
public:
	inline PerformanceLock(PerformanceMutex& mutex)
	:_mutex(mutex)
	{
		_mutex.Lock();
	}
	inline ~PerformanceLock()
	{
		_mutex.Unlock();
	}

private:
	// Don't allow the lock to be copied
	PerformanceLock(const PerformanceLock& object)
	:_mutex(object._mutex)
	{}

private:
	PerformanceMutex& _mutex;
};

#endif
