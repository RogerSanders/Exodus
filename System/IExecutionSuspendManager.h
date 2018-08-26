#ifndef __IEXECUTIONSUSPENDMANAGER_H__
#define __IEXECUTIONSUSPENDMANAGER_H__

class IExecutionSuspendManager
{
public:
	// Constructors
	inline virtual ~IExecutionSuspendManager() = 0;

	// Suspend functions
	virtual void EnableTimesliceExecutionSuspend() = 0;
	virtual void DisableTimesliceExecutionSuspend() = 0;
	virtual bool AllDevicesSuspended(unsigned int executingThreadCount, unsigned int suspendedThreadCount) const = 0;
};
IExecutionSuspendManager::~IExecutionSuspendManager() { }

#endif
