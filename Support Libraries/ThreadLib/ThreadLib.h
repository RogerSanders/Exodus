#ifndef __THREADLIB_H__
#define __THREADLIB_H__
#include "Timestamp.h"

enum ThreadPriority
{
	THREADPRIORITY_IDLE,
	THREADPRIORITY_LOWEST,
	THREADPRIORITY_LOW,
	THREADPRIORITY_NORMAL,
	THREADPRIORITY_HIGH,
	THREADPRIORITY_HIGHEST,
	THREADPRIORITY_CRITICAL
};

void SetCurrentThreadPriority(ThreadPriority priority);
Timestamp GetTimestamp();

#endif
