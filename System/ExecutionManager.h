#ifndef __EXECUTIONMANAGER_H__
#define __EXECUTIONMANAGER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "WindowsSupport/WindowsSupport.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "DeviceContext.h"
#include "IExecutionSuspendManager.h"
#include <mutex>
#include <condition_variable>
#include <vector>
#include <set>

//##TODO## Consider holding active devices on a different command wait condition than non
// active devices. This would allow us to wake just the active devices, or all devices,
// depending on what command we want to execute.
class ExecutionManager : public IExecutionSuspendManager
{
public:
	// Constructors
	inline ExecutionManager();

	// Device functions
	inline void AddDevice(DeviceContext* device);
	inline void RemoveDevice(DeviceContext* device);
	inline void ClearAllDevices();

	// Execute functions
	inline void NotifyUpcomingTimeslice(double nanoseconds);
	inline void NotifyBeforeExecuteCalled();
	inline void NotifyAfterExecuteCalled();
	inline void ExecuteTimeslice(double nanoseconds);
	inline void Commit();
	inline void Rollback();
	inline void Initialize();
	inline void AssertCurrentOutputLineState();
	inline void NegateCurrentOutputLineState();

	// Suspend functions
	virtual void EnableTimesliceExecutionSuspend();
	virtual void DisableTimesliceExecutionSuspend();
	virtual bool AllDevicesSuspended(volatile ReferenceCounterType& suspendedThreadCount, volatile ReferenceCounterType& remainingThreadCount) const;
	void BuildBlockedDependentDeviceSet(const DeviceContext* sourceDevice, std::set<const DeviceContext*>& dependentDeviceSet) const;

	// Timing functions
	inline double GetNextTimingPoint(double maximumTimeslice, DeviceContext*& nextDeviceStep, unsigned int& nextDeviceStepContext);

	// Worker thread control
	inline void BeginExecution();
	inline void SuspendExecution();

private:
	mutable std::mutex _commandMutex;
	std::condition_variable _commandSent;
	std::condition_variable _commandProcessed;
	DeviceContext::DeviceContextCommand _command;
	ReferenceCounterType _totalDeviceCount;
	volatile ReferenceCounterType _pendingDeviceCount;
	volatile ReferenceCounterType _suspendedThreadCount;

	size_t _deviceCount;
	size_t _suspendDeviceCount;
	size_t _transientDeviceCount;
	std::vector<DeviceContext*> _deviceArray;
	std::vector<DeviceContext*> _suspendDeviceArray;
	std::vector<DeviceContext*> _transientDeviceArray;
};

#include "ExecutionManager.inl"
#endif
