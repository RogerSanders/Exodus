#include "ThreadLib.h"

//----------------------------------------------------------------------------------------
void SetCurrentThreadPriority(ThreadPriority priority)
{
	int threadPriority = THREAD_PRIORITY_NORMAL;
	switch(priority)
	{
	case THREADPRIORITY_IDLE:
		threadPriority = THREAD_PRIORITY_IDLE;
		break;
	case THREADPRIORITY_LOWEST:
		threadPriority = THREAD_PRIORITY_LOWEST;
		break;
	case THREADPRIORITY_LOW:
		threadPriority = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case THREADPRIORITY_NORMAL:
		threadPriority = THREAD_PRIORITY_NORMAL;
		break;
	case THREADPRIORITY_HIGH:
		threadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case THREADPRIORITY_HIGHEST:
		threadPriority = THREAD_PRIORITY_HIGHEST;
		break;
	case THREADPRIORITY_CRITICAL:
		threadPriority = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	}
	SetThreadPriority(GetCurrentThread(), threadPriority);
}

//----------------------------------------------------------------------------------------
Timestamp GetTimestamp()
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	return Timestamp(systemTime);
}
