#include "WindowsSupport/WindowsSupport.pkg"

//----------------------------------------------------------------------------------------------------------------------
// Structures
//----------------------------------------------------------------------------------------------------------------------
struct DeviceContext::DeviceDependency
{
	DeviceContext* device;
	volatile bool dependencyEnabled;
};

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
DeviceContext::DeviceContext(IDevice& device, ISystemGUIInterface& systemObject)
:_device(device), _systemObject(systemObject), _deviceDependencies(0), _executingThreadCount(0), _suspendedThreadCount(0), _suspendManager(0), _otherSharedExecuteThreadDevice(0), _currentSharedExecuteThreadOwner(0)
{
	_deviceIndexNo = 0;
	_deviceEnabled = true;
	_executeWorkerThreadActive = false;
	_executeThreadRunningState = false;

	_timesliceCompleted = false;
	_timesliceSuspended = false;
	_timesliceSuspensionDisable = false;
	_transientExecutionActive = false;

	_sharingExecuteThread = false;
	_primarySharedExecuteThreadDevice = false;
	_sharedExecuteThreadSpinoffActive = false;
	_sharedExecuteThreadSpinoffRejoinRequested = false;
	_sharedExecuteThreadSpinoffPaused = false;
	_sharedExecuteThreadSpinoffStopRequested = false;
	_sharedExecuteThreadSpinoffRunning = false;
	_sharedExecuteThreadSpinoffTimesliceAvailable = false;
}

//----------------------------------------------------------------------------------------------------------------------
// Execute functions
//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::NotifyUpcomingTimeslice(double nanoseconds)
{
	// Note that we reset the current timeslice progress here to the total amount of
	// remaining time for all devices. It is critical that this is done here, before the
	// execute command is sent, since our execute threads for each device may check the
	// timeslice progress of another in order to support device dependencies. We need to
	// reset the current timeslice progress for each device after all devices are finished
	// executing the last timeslice, but before any device begins executing the next
	// timeslice, in order to avoid race conditions in the execute threads. This is a
	// convenient place to do it, as it gives us the synchronization we need between all
	// our devices, and it can be performed here with very little overhead.
	_currentTimesliceProgress = _remainingTime;

	if (_device.SendNotifyUpcomingTimeslice())
	{
		_device.NotifyUpcomingTimeslice(nanoseconds);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::NotifyBeforeExecuteCalled()
{
	if (_device.SendNotifyBeforeExecuteCalled())
	{
		_device.NotifyBeforeExecuteCalled();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::NotifyAfterExecuteCalled()
{
	// Clear the suspend manager state. Note that we need to do this here, as it can't be done safely after execution in
	// the case of a device that executes in timeslice mode, but also supports transient execution.
	ClearSuspendManagerState();

	if (_device.SendNotifyAfterExecuteCalled())
	{
		_device.NotifyAfterExecuteCalled();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::BeginExecuteTimeslice(double nanoseconds, std::atomic<unsigned int>* executingThreadCount, std::atomic<unsigned int>* suspendedThreadCount, IExecutionSuspendManager* suspendManager)
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	_timeslice = nanoseconds;
	_timesliceCompleted = false;
	_executingThreadCount = executingThreadCount;
	_suspendedThreadCount = suspendedThreadCount;
	_suspendManager = suspendManager;
	_executeTaskSent.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
double DeviceContext::ExecuteStep()
{
	double additionalTime = 0;

	if (_device.GetUpdateMethod() == IDevice::UpdateMethod::Step)
	{
		_remainingTime += _device.ExecuteStep();
		additionalTime = _remainingTime;
	}

	return additionalTime;
}

//----------------------------------------------------------------------------------------------------------------------
double DeviceContext::ExecuteStep(unsigned int accessContext)
{
	double additionalTime = 0;

	if (_device.GetUpdateMethod() == IDevice::UpdateMethod::Step)
	{
		_remainingTime += _device.ExecuteStep();
		additionalTime = _remainingTime;
	}
	else if (_device.GetUpdateMethod() == IDevice::UpdateMethod::Timeslice)
	{
		_device.ExecuteTimesliceTimingPointStep(accessContext);
	}

	return additionalTime;
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::WaitForCompletion()
{
	std::unique_lock<std::mutex> executeLock(_executeThreadMutex);
	while (!_timesliceCompleted)
	{
		_executeCompletionStateChanged.wait(executeLock);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::Commit()
{
	_remainingTimeBackup = _remainingTime;
	_device.ExecuteCommit();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::Rollback()
{
	_remainingTime = _remainingTimeBackup;
	_device.ExecuteRollback();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::Initialize()
{
	_device.Initialize();
	_remainingTime = 0;
	_currentTimesliceProgress = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Timing functions
//----------------------------------------------------------------------------------------------------------------------
double DeviceContext::GetNextTimingPoint(unsigned int& accessContext) const
{
	double result = -1;

	if (_device.GetUpdateMethod() == IDevice::UpdateMethod::Step)
	{
		result = _device.GetNextTimingPointInDeviceTime(accessContext);
		if (result >= 0)
		{
			result += _remainingTime;
		}
	}
	else if (_device.GetUpdateMethod() == IDevice::UpdateMethod::Timeslice)
	{
		result = _device.GetNextTimingPointInDeviceTime(accessContext);
	}

	return result;
}

//----------------------------------------------------------------------------------------------------------------------
double DeviceContext::GetCurrentRemainingTime() const
{
	return _remainingTime;
}

//----------------------------------------------------------------------------------------------------------------------
double DeviceContext::GetInitialRemainingTime() const
{
	return _remainingTimeBackup;
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ClearRemainingTime()
{
	_remainingTime = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Device interface
//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::SetDeviceIndexNo(unsigned int deviceIndexNo)
{
	_deviceIndexNo = deviceIndexNo;
}

//----------------------------------------------------------------------------------------------------------------------
bool DeviceContext::ActiveDevice() const
{
	return (GetTargetDevice().GetUpdateMethod() != IDevice::UpdateMethod::None);
}

//----------------------------------------------------------------------------------------------------------------------
// Dependent device functions
//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::AddDeviceDependency(DeviceContext* targetDevice)
{
	// This little catch is just to prevent the same device being added to the list
	// multiple times.
	RemoveDeviceDependency(targetDevice);

	// Add the device to the device dependency list
	DeviceDependency deviceDependency;
	deviceDependency.device = targetDevice;
	deviceDependency.dependencyEnabled = true;
	_deviceDependencies.push_back(deviceDependency);

	// Notify the target device that this device has added it as a dependency
	targetDevice->AddDependentDevice(this);
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::RemoveDeviceDependency(DeviceContext* targetDevice)
{
	bool done = false;
	std::vector<DeviceDependency>::iterator i = _deviceDependencies.begin();
	while (!done && (i != _deviceDependencies.end()))
	{
		if (i->device == targetDevice)
		{
			// Notify the target device that this device has removed it as a dependency
			targetDevice->RemoveDependentDevice(this);

			// Remove the device from the device dependency list
			_deviceDependencies.erase(i);
			done = true;
			continue;
		}
		++i;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::AddDependentDevice(DeviceContext* targetDevice)
{
	RemoveDependentDevice(targetDevice);

	_dependentDevices.push_back(targetDevice);
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::RemoveDependentDevice(DeviceContext* targetDevice)
{
	bool done = false;
	std::vector<DeviceContext*>::iterator i = _dependentDevices.begin();
	while (!done && (i != _dependentDevices.end()))
	{
		if (*i == targetDevice)
		{
			_dependentDevices.erase(i);
			done = true;
			continue;
		}
		++i;
	}
}

//----------------------------------------------------------------------------------------------------------------------
const std::vector<DeviceContext::DeviceDependency>& DeviceContext::GetDeviceDependencyArray() const
{
	return _deviceDependencies;
}

//----------------------------------------------------------------------------------------------------------------------
const std::vector<DeviceContext*>& DeviceContext::GetDependentDeviceArray() const
{
	return _dependentDevices;
}
