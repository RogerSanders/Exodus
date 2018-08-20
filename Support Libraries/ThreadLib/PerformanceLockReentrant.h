#ifndef __PERFORMANCELOCKREENTRANT_H__
#define __PERFORMANCELOCKREENTRANT_H__
#include "PerformanceMutexReentrant.h"

struct PerformanceLockReentrant
{
public:
	inline PerformanceLockReentrant(PerformanceMutexReentrant& mutex)
	:_mutex(mutex)
	{
		_mutex.Lock();
	}
	inline ~PerformanceLockReentrant()
	{
		_mutex.Unlock();
	}

private:
	// Don't allow the lock to be copied
	PerformanceLockReentrant(const PerformanceLockReentrant& object)
	:_mutex(object._mutex)
	{ }

private:
	PerformanceMutexReentrant& _mutex;
};

#endif
