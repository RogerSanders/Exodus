#ifndef __PERFORMANCELOCKREENTRANT_H__
#define __PERFORMANCELOCKREENTRANT_H__
#include "PerformanceMutexReentrant.h"

struct PerformanceLockReentrant
{
public:
	inline PerformanceLockReentrant(PerformanceMutexReentrant& amutex)
	:mutex(amutex)
	{
		mutex.Lock();
	}
	inline ~PerformanceLockReentrant()
	{
		mutex.Unlock();
	}

private:
	//Don't allow the lock to be copied
	PerformanceLockReentrant(const PerformanceLockReentrant& object)
	:mutex(object.mutex)
	{}

private:
	PerformanceMutexReentrant& mutex;
};

#endif
