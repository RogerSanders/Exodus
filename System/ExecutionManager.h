#ifndef __EXECUTIONMANAGER_H__
#define __EXECUTIONMANAGER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "WindowsSupport/WindowsSupport.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "DeviceContext.h"
#include "IExecutionSuspendManager.h"
#include <vector>

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
	virtual bool AllDevicesSuspended(unsigned int executingThreadCount, unsigned int suspendedThreadCount) const;
	void BuildBlockedDependentDeviceSet(const DeviceContext* sourceDevice, std::set<const DeviceContext*>& dependentDeviceSet) const;

	// Timing functions
	inline double GetNextTimingPoint(double maximumTimeslice, DeviceContext*& nextDeviceStep, unsigned int& nextDeviceStepContext);

	// Worker thread control
	inline void StartExecution();
	inline void StopExecution();

private:
	unsigned int _deviceCount;
	unsigned int _activeDeviceCount;
	unsigned int _suspendDeviceCount;
	unsigned int _transientDeviceCount;
	std::vector<DeviceContext*> _deviceArray;
	std::vector<DeviceContext*> _activeDeviceArray;
	std::vector<DeviceContext*> _suspendDeviceArray;
	std::vector<DeviceContext*> _transientDeviceArray;
	std::vector<unsigned int> _nextTimesliceContextValues;
	std::vector<double> _nextTimesliceValues;
};

#include "ExecutionManager.inl"
#endif
