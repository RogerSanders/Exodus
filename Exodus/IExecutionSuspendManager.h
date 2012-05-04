#ifndef __IEXECUTIONSUSPENDMANAGER_H__
#define __IEXECUTIONSUSPENDMANAGER_H__

class IExecutionSuspendManager
{
public:
	//Constructors
	virtual ~IExecutionSuspendManager() = 0 {}

	//Suspend functions
	virtual void EnableTimesliceExecutionSuspend() = 0;
	virtual void DisableTimesliceExecutionSuspend() = 0;
	virtual bool AllDevicesSuspended(volatile ReferenceCounterType& suspendedThreadCount, volatile ReferenceCounterType& remainingThreadCount) const = 0;
};

#endif
