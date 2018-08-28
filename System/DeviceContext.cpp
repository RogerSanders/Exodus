#include "DeviceContext.h"
#include "ThreadLib/ThreadLib.pkg"
#include "Debug/Debug.pkg"
#include <thread>

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int DeviceContext::GetIDeviceContextVersion() const
{
	return ThisIDeviceContextVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Timing functions
//----------------------------------------------------------------------------------------------------------------------
double DeviceContext::GetCurrentTimesliceProgress() const
{
	return _currentTimesliceProgress;
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::SetCurrentTimesliceProgress(double executionProgress)
{
	_currentTimesliceProgress = executionProgress;
	_remainingTime = _currentTimesliceProgress - _timeslice;
}

//----------------------------------------------------------------------------------------------------------------------
// Control functions
//----------------------------------------------------------------------------------------------------------------------
bool DeviceContext::DeviceEnabled() const
{
	return _deviceEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::SetDeviceEnabled(bool state)
{
	_deviceEnabled = state;
}

//----------------------------------------------------------------------------------------------------------------------
// Device interface
//----------------------------------------------------------------------------------------------------------------------
IDevice& DeviceContext::GetTargetDevice() const
{
	return _device;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int DeviceContext::GetDeviceIndexNo() const
{
	return _deviceIndexNo;
}

//----------------------------------------------------------------------------------------------------------------------
// System message functions
//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::WriteLogEvent(const ILogEntry& entry)
{
	// Ensure that the source of the event is correctly set for the calling device
	std::wstring eventLogSource = GetTargetDevice().GetDeviceInstanceName() + L" [" + GetTargetDevice().GetModuleDisplayName() + L"]";
	entry.OverrideSourceText(eventLogSource);

	// Pass the event log message on to the system
	_systemObject.WriteLogEvent(entry);
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::FlagStopSystem()
{
	_systemObject.FlagStopSystem();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::StopSystem()
{
	_systemObject.StopSystem();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::RunSystem()
{
	_systemObject.RunSystem();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ExecuteDeviceStep()
{
	_systemObject.ExecuteDeviceStep(&_device);
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> DeviceContext::GetFullyQualifiedDeviceInstanceName() const
{
	std::wstring fullyQualifiedDeviceInstanceName = GetModuleInstanceName() + L"." + _device.GetDeviceInstanceName();
	return fullyQualifiedDeviceInstanceName;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> DeviceContext::GetModuleDisplayName() const
{
	std::wstring moduleDisplayName;
	_systemObject.GetModuleDisplayName(_device.GetDeviceModuleID(), moduleDisplayName);
	return moduleDisplayName;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> DeviceContext::GetModuleInstanceName() const
{
	std::wstring moduleInstanceName;
	_systemObject.GetModuleInstanceName(_device.GetDeviceModuleID(), moduleInstanceName);
	return moduleInstanceName;
}

//----------------------------------------------------------------------------------------------------------------------
// Suspend functions
//----------------------------------------------------------------------------------------------------------------------
bool DeviceContext::UsesExecuteSuspend() const
{
	return _device.UsesExecuteSuspend();
}

//----------------------------------------------------------------------------------------------------------------------
bool DeviceContext::UsesTransientExecution() const
{
	return _device.UsesTransientExecution();
}

//----------------------------------------------------------------------------------------------------------------------
bool DeviceContext::TimesliceExecutionSuspended() const
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	return _timesliceSuspended;
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::SuspendTimesliceExecution()
{
	// Ensure that the device calling the suspend function has indicated that it uses the
	// suspend feature. Failure to do this may cause deadlocks.
	DebugAssert(_device.UsesExecuteSuspend());

	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	if (_timesliceSuspended || _timesliceSuspensionDisable)
	{
		//##TODO## Log an error if _timesliceSuspended is already true
		return;
	}

	_timesliceSuspended = true;
	_executeCompletionStateChanged.notify_all();

	if (_suspendedThreadCount != nullptr)
	{
		_suspendedThreadCount->fetch_add(1);
	}
	lock.unlock();

	WakeSuspendedDevicesIfRequired();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::WaitForTimesliceExecutionResume() const
{
	// Wait for the timeslice to resume execution, or for timeslice suspension to be disabled.
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	while (_timesliceSuspended && !_timesliceSuspensionDisable)
	{
		_executeCompletionStateChanged.wait(lock);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ResumeTimesliceExecution()
{
	std::unique_lock<std::mutex> executeLock(_executeThreadMutex);
	if (!_timesliceSuspended)
	{
		//##TODO## Log an error
		return;
	}

	_timesliceSuspended = false;
	_executeCompletionStateChanged.notify_all();

	if (_suspendedThreadCount != nullptr)
	{
		_suspendedThreadCount->fetch_sub(1);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool DeviceContext::TimesliceSuspensionDisabled() const
{
	return _timesliceSuspensionDisable;
}

//----------------------------------------------------------------------------------------------------------------------
bool DeviceContext::TransientExecutionActive() const
{
	return _transientExecutionActive;
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::SetTransientExecutionActive(bool state)
{
	_transientExecutionActive = state;

	if (!state)
	{
		WakeSuspendedDevicesIfRequired();
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool DeviceContext::TimesliceExecutionCompleted() const
{
	return _timesliceCompleted && !_transientExecutionActive;
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::DisableTimesliceExecutionSuspend()
{
	//##FIX## We took out the locks here and in EnableTimesliceExecutionSuspend on the
	// executeThreadMutex, to avoid a deadlock that was occurring due to this worker thread
	// holding onto the mutex, but being unable to obtain the command mutex, while another
	// thread called back into this thread and attempted to disable execution suspend. I
	// think we need the lock though, as we need to ensure that the internal state for the
	// condition variable is not corrupted due to concurrent access. We need a lock here to
	// ensure any other threads which wait on this condition are not actively working with
	// it at the time we try and trigger it.
	//##TODO## Looking at the internal implementation of the std::condition_variable class, it
	// appears that the class has its own internal locks, and based on the design, it
	// appears that we should be able to safely execute this code without a lock, and
	// without causing any internal inconsistencies in the condition variable. This means
	// the only concurrency issues we have to worry about are in our own code. That said,
	// looking at our code, we still need the lock, otherwise a deadlock could occur. Our
	// code tests timesliceSuspensionDisable, then enters a wait state if it is not set. If
	// another thread yields after testing timesliceSuspensionDisable, but before the wait
	// on executeCompletionStateChanged, then this thread runs through and sets the
	// condition, the other thread could then resume, and miss the condition, and enter the
	// wait state indefinitely when timesliceSuspensionDisable is set. This means we
	// absolutely need a lock here.

	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	_timesliceSuspensionDisable = true;
	_executeCompletionStateChanged.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::EnableTimesliceExecutionSuspend()
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	_timesliceSuspensionDisable = false;
	_executeCompletionStateChanged.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
// Dependent device functions
//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::SetDeviceDependencyEnable(IDeviceContext* targetDevice, bool state)
{
	// Attempt to find a dependency on the specified target device, and update its
	// dependency enable state to the state specified.
	bool foundDeviceDependency = false;
	bool previousDependencyState = { };
	unsigned int dependentTargetCount = (unsigned int)_deviceDependencies.size();
	unsigned int i = 0;
	while (i < dependentTargetCount)
	{
		if (_deviceDependencies[i].device == targetDevice)
		{
			foundDeviceDependency = true;
			previousDependencyState = _deviceDependencies[i].dependencyEnabled;
			_deviceDependencies[i].dependencyEnabled = state;
		}
		++i;
	}

	// If this device is sharing an execution thread with the target device, we now need to
	// either split or combine the execution threads, now that the device dependency enable
	// state has changed.
	if (_sharingExecuteThread && foundDeviceDependency && (targetDevice == _otherSharedExecuteThreadDevice) && (previousDependencyState != state))
	{
		DeviceContext* primaryDevice = (_primarySharedExecuteThreadDevice)? this: _otherSharedExecuteThreadDevice;
		if (state)
		{
			// If the device dependency has been enabled, request the spinoff execution
			// thread for the combined execution thread to rejoin. Note that we do not wait
			// for this process to be completed here, for performance reasons. We want to
			// allow the spinoff thread and main execution thread to continue in parallel
			// for as long as possible. Since we've just re-enabled the device dependency,
			// the main execution thread will now not allow the owning device to be
			// advanced beyond the device executing in the spinoff thread, and the spinoff
			// thread will halt after the end of the next device step, so we can safely
			// continue execution here without waiting for the rejoin request to be
			// processed.
			std::unique_lock<std::mutex> lock(primaryDevice->_executeThreadMutex);
			if (primaryDevice->_sharedExecuteThreadSpinoffActive)
			{
				primaryDevice->_sharedExecuteThreadSpinoffRejoinRequested = true;
				primaryDevice->_sharedExecuteThreadSpinoffStateChangeRequested.notify_all();
			}
		}
		else
		{
			// If the device dependency has been disabled, wait for the spinoff thread to
			// terminate if it is currently active, as it's possible we have previously
			// requested the spinoff thread to rejoin, but the device dependency state was
			// changed again before the spinoff thread processed the request. This is the
			// only case that could have arisen if the spinoff thread is still active here.
			std::unique_lock<std::mutex> lock(primaryDevice->_executeThreadMutex);
			while (primaryDevice->_sharedExecuteThreadSpinoffActive && !primaryDevice->_sharedExecuteThreadSpinoffPaused)
			{
				primaryDevice->_sharedExecuteThreadSpinoffStoppedOrPaused.wait(lock);
			}

			// Request the spinoff thread for the combined execution thread to begin
			// advancing the device that doesn't currently own the main execution thread in
			// parallel.
			primaryDevice->_sharedExecuteThreadSpinoffActive = true;
			primaryDevice->_sharedExecuteThreadSpinoffTimesliceAvailable = true;
			primaryDevice->_sharedExecuteThreadSpinoffStateChangeRequested.notify_all();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Worker thread control
//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::StartExecution()
{
	StartExecuteWorkerThread();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::StopExecution()
{
	StopExecuteWorkerThread();
}

//----------------------------------------------------------------------------------------------------------------------
// Execute worker thread control
//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::StartExecuteWorkerThread()
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	if (_executeWorkerThreadActive || !ActiveDevice())
	{
		return;
	}

	// Initialize the timeslice completed state to true. This signals that there is no more work to be done, IE, that
	// there is no timeslice pending.
	_timesliceCompleted = true;

	// Notify the device that execution is about to begin
	lock.unlock();
	_device.BeginExecution();
	lock.lock();

	// Scan our list of device dependencies. If we have a two-way dependency with
	// another device, and both our device and their device use step execution, we fold
	// the two devices into a single execution thread for efficiency. In this model,
	// one of the devices is considered the primary owner of the execute thread, and
	// the other device is considered the secondary owner. To select this we, make the
	// primary owner the device that has the lowest device index number. We only
	// actually spawn an execute thread here if the target device is the primary owner
	// of this combined execution thread.
	bool foundInterlockedDevice = false;
	bool ourDeviceIsPrimaryInterlockedDevice = false;
	bool interlockedDeviceDependenciesCurrentlyDisabled = false;
	DeviceContext* interlockedDevice = 0;
	if (_device.GetUpdateMethod() == IDevice::UpdateMethod::Step)
	{
		for (unsigned int deviceDependencyIndex = 0; deviceDependencyIndex < (unsigned int)_deviceDependencies.size(); ++deviceDependencyIndex)
		{
			DeviceContext* targetDevice = _deviceDependencies[deviceDependencyIndex].device;
			if (targetDevice->_device.GetUpdateMethod() == IDevice::UpdateMethod::Step)
			{
				for (unsigned int targetDeviceDependencyIndex = 0; targetDeviceDependencyIndex < (unsigned int)targetDevice->_deviceDependencies.size(); ++targetDeviceDependencyIndex)
				{
					if (targetDevice->_deviceDependencies[targetDeviceDependencyIndex].device == this)
					{
						foundInterlockedDevice = true;
						interlockedDevice = targetDevice;
						ourDeviceIsPrimaryInterlockedDevice = (_deviceIndexNo < targetDevice->_deviceIndexNo);
						interlockedDeviceDependenciesCurrentlyDisabled = (!_deviceDependencies[deviceDependencyIndex].dependencyEnabled || !targetDevice->_deviceDependencies[targetDeviceDependencyIndex].dependencyEnabled);
					}
				}
			}
		}
	}

	// If this device is going to share an execution thread with another device, record
	// information about the shared execution thread.
	_sharingExecuteThread = foundInterlockedDevice;
	_primarySharedExecuteThreadDevice = ourDeviceIsPrimaryInterlockedDevice;
	_otherSharedExecuteThreadDevice = interlockedDevice;

	// Start the execution thread for this device
	_executeWorkerThreadActive = true;
	if (!foundInterlockedDevice)
	{
		std::thread workerThread(std::bind(std::mem_fn(&DeviceContext::ExecuteWorkerThread), this));
		workerThread.detach();
	}
	else if (ourDeviceIsPrimaryInterlockedDevice)
	{
		// Since we're handling the execute thread for the dependent device here, mark its execution thread as active
		// too.
		interlockedDevice->_executeWorkerThreadActive = true;

		// If the interlocked dependencies are present but currently disabled, flag the
		// execute spinoff thread as active, so that it will be restored when we start
		// the execute thread below.
		_sharedExecuteThreadSpinoffActive = interlockedDeviceDependenciesCurrentlyDisabled;

		// Start the primary execute thread
		std::thread workerThread(std::bind(&DeviceContext::ExecuteWorkerThreadStepMultipleDeviceSharedDependencies, this, interlockedDevice));
		workerThread.detach();
	}

	// Wait for confirmation that the execute worker thread is ready to receive
	// commands
	while (!_executeThreadRunningState)
	{
		_executeThreadReady.wait(lock);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::StopExecuteWorkerThread()
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	if (_executeWorkerThreadActive)
	{
		// Instruct the execution thread to terminate, and wait for confirmation that it
		// has stopped.
		_executeWorkerThreadActive = false;
		while (_executeThreadRunningState)
		{
			_executeTaskSent.notify_all();
			_executeThreadStopped.wait(lock);
		}

		// Notify the device that execution is being suspended
		lock.unlock();
		_device.SuspendExecution();
		lock.lock();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThread()
{
	// Set the name of this thread for the debugger
	std::wstring debuggerThreadName = L"DCExecute - " + _device.GetDeviceInstanceName();
	SetCallingThreadName(debuggerThreadName);

	if (_device.GetUpdateMethod() == IDevice::UpdateMethod::Step)
	{
		if (_deviceDependencies.empty())
		{
			ExecuteWorkerThreadStep();
		}
		else
		{
			ExecuteWorkerThreadStepWithDependencies();
		}
	}
	else if (_device.GetUpdateMethod() == IDevice::UpdateMethod::Timeslice)
	{
		if (_deviceDependencies.empty())
		{
			ExecuteWorkerThreadTimeslice();
		}
		else
		{
			ExecuteWorkerThreadTimesliceWithDependencies();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadStep()
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	_executeThreadRunningState = true;
	_executeThreadReady.notify_all();
	if (_timesliceCompleted)
	{
		_executeTaskSent.wait(lock);
	}
	while (_executeWorkerThreadActive)
	{
		lock.unlock();
		while (_currentTimesliceProgress < _timeslice)
		{
			_currentTimesliceProgress += _device.ExecuteStep();
			if (_systemObject.IsSystemRollbackFlagged())
			{
				if (_currentTimesliceProgress >= _systemObject.SystemRollbackTime())
				{
					_currentTimesliceProgress = _timeslice;
				}
			}
		}
		_remainingTime = _currentTimesliceProgress - _timeslice;
		_device.NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		_timesliceSuspended = false;
		_timesliceCompleted = true;
		lock.unlock();
		WakeSuspendedDevicesIfRequired();
		lock.lock();
		_executeCompletionStateChanged.notify_all();
		if (_timesliceCompleted)
		{
			_executeTaskSent.wait(lock);
		}
	}
	_executeThreadRunningState = false;
	_executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadStepWithDependencies()
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	_executeThreadRunningState = true;
	_executeThreadReady.notify_all();
	if (_timesliceCompleted)
	{
		_executeTaskSent.wait(lock);
	}
	while (_executeWorkerThreadActive)
	{
		lock.unlock();
		unsigned int dependentTargetCount = (unsigned int)_deviceDependencies.size();
		while (_currentTimesliceProgress < _timeslice)
		{
			for (unsigned int i = 0; i < dependentTargetCount; ++i)
			{
				while ((_currentTimesliceProgress > _deviceDependencies[i].device->_currentTimesliceProgress) && _deviceDependencies[i].dependencyEnabled)
				{
					Sleep(0);
				}
			}
			_currentTimesliceProgress += _device.ExecuteStep();
			if (_systemObject.IsSystemRollbackFlagged())
			{
				if (_currentTimesliceProgress >= _systemObject.SystemRollbackTime())
				{
					_currentTimesliceProgress = _timeslice;
				}
			}
		}
		_remainingTime = _currentTimesliceProgress - _timeslice;
		_device.NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		_timesliceSuspended = false;
		_timesliceCompleted = true;
		lock.unlock();
		WakeSuspendedDevicesIfRequired();
		lock.lock();
		_executeCompletionStateChanged.notify_all();
		if (_timesliceCompleted)
		{
			_executeTaskSent.wait(lock);
		}
	}
	_executeThreadRunningState = false;
	_executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadStepMultipleDeviceSharedDependencies(DeviceContext* device1, DeviceContext* device2)
{
	std::unique_lock<std::mutex> lock1(device1->_executeThreadMutex);
	std::unique_lock<std::mutex> lock2(device2->_executeThreadMutex);

	// Set the name of this thread for the benefit of an attached debugger
	std::wstring debuggerThreadName = L"DCExePri - \"" + device1->_device.GetDeviceInstanceName() + L"\" and \"" + device2->_device.GetDeviceInstanceName() + L"\"";
	SetCallingThreadName(debuggerThreadName);

	// Start our parallel execution thread for these locked devices, so that if the device
	// dependency is enabled, the second device can be spun off into this parallel
	// execution thread. We always create this thread here, and leave it in a wait state,
	// because creating and destroying a thread is a relatively expensive operation, and we
	// want there to be minimal overhead spinning off execution to this parallel thread, as
	// this could potentially happen many times during a single timeslice.
	device1->_currentSharedExecuteThreadOwner = device1;
	device1->_sharedExecuteThreadSpinoffRunning = true;
	device1->_sharedExecuteThreadSpinoffStopRequested = false;
	device1->_sharedExecuteThreadSpinoffTimesliceAvailable = false;
	device1->_sharedExecuteThreadSpinoffRejoinRequested = false;
	std::thread workerThread(std::bind(std::mem_fn(&DeviceContext::ExecuteWorkerThreadStepSharedExecutionThreadSpinoff), device1));
	workerThread.detach();

	// Wait for the spinoff thread to finish starting up
	lock2.unlock();
	device1->_sharedExecuteThreadSpinoffStoppedOrPaused.wait(lock1);
	lock2.lock();

	// Notify the command threads for both devices in this shared execution thread that the
	// execution thread for the target devices is up and running. Note that we have a lock
	// here on executeThreadMutex here, so the command threads will not actually be
	// unblocked until we wait on a message from the command thread again and release the
	// lock.
	device1->_executeThreadRunningState = true;
	device2->_executeThreadRunningState = true;
	device1->_executeThreadReady.notify_all();
	device2->_executeThreadReady.notify_all();

	// Wait for a new execute task to be sent from the command thread to both our
	// target devices
	if (device1->_executeWorkerThreadActive && device1->_timesliceCompleted)
	{
		lock2.unlock();
		device1->_executeTaskSent.wait(lock1);
		lock2.lock();
	}
	if (device2->_executeWorkerThreadActive && device2->_timesliceCompleted)
	{
		lock1.unlock();
		device2->_executeTaskSent.wait(lock2);
		lock1.lock();
	}

	// Process execute commands until a command is sent requesting the execute threads for
	// our target devices to shutdown
	while (device1->_executeWorkerThreadActive && device2->_executeWorkerThreadActive)
	{
		lock1.unlock();
		lock2.unlock();

		// If our spinoff thread is currently active and waiting to advance a device, send
		// a notification to the spinoff thread about the new timeslice that has been
		// received, and wait for it to acknowledge that it has received the notification.
		if (device1->_sharedExecuteThreadSpinoffActive)
		{
			lock1.lock();
			device1->_sharedExecuteThreadSpinoffTimesliceAvailable = true;
			device1->_sharedExecuteThreadSpinoffStateChangeRequested.notify_all();
			device1->_sharedExecuteThreadSpinoffTimesliceProcessingBegun.wait(lock1);
			lock1.unlock();
		}

		// Advance all devices which are executing within this combined execution thread to
		// the end of the current timeslice
		unsigned int device1DependentTargetCount = (unsigned int)device1->_deviceDependencies.size();
		unsigned int device2DependentTargetCount = (unsigned int)device2->_deviceDependencies.size();
		while ((!device1->_sharedExecuteThreadSpinoffActive && ((device1->_currentTimesliceProgress < device1->_timeslice) || (device2->_currentTimesliceProgress < device2->_timeslice)))
		  || (device1->_sharedExecuteThreadSpinoffActive && (device1->_currentSharedExecuteThreadOwner->_currentTimesliceProgress < device1->_currentSharedExecuteThreadOwner->_timeslice)))
		{
			// Advance the first device in this execution thread a single step, if it is
			// not currently executing in a spinoff thread.
			if (!device1->_sharedExecuteThreadSpinoffActive || (device1->_currentSharedExecuteThreadOwner == device1))
			{
				device1->_currentSharedExecuteThreadOwner = device1;
				while ((device1->_sharedExecuteThreadSpinoffActive || (device1->_currentTimesliceProgress <= device2->_currentTimesliceProgress)) && (device1->_currentTimesliceProgress < device1->_timeslice))
				{
					for (unsigned int i = 0; i < device1DependentTargetCount; ++i)
					{
						while (((device1->_deviceDependencies[i].device != device2) || device1->_sharedExecuteThreadSpinoffRejoinRequested) && (device1->_currentTimesliceProgress > device1->_deviceDependencies[i].device->_currentTimesliceProgress) && device1->_deviceDependencies[i].dependencyEnabled)
						{
							Sleep(0);
						}
					}
					device1->_currentTimesliceProgress += device1->_device.ExecuteStep();
					if (device1->_systemObject.IsSystemRollbackFlagged())
					{
						if (device1->_currentTimesliceProgress >= device1->_systemObject.SystemRollbackTime())
						{
							device1->_currentTimesliceProgress = device1->_timeslice;
						}
					}
				}
			}

			// Advance the second device in this execution thread a single step, if it is
			// not currently executing in a spinoff thread.
			if (!device1->_sharedExecuteThreadSpinoffActive || (device1->_currentSharedExecuteThreadOwner == device2))
			{
				device1->_currentSharedExecuteThreadOwner = device2;
				while ((device1->_sharedExecuteThreadSpinoffActive || (device2->_currentTimesliceProgress <= device1->_currentTimesliceProgress)) && (device2->_currentTimesliceProgress < device2->_timeslice))
				{
					for (unsigned int i = 0; i < device2DependentTargetCount; ++i)
					{
						while (((device2->_deviceDependencies[i].device != device1) || device1->_sharedExecuteThreadSpinoffRejoinRequested) && (device2->_currentTimesliceProgress > device2->_deviceDependencies[i].device->_currentTimesliceProgress) && device2->_deviceDependencies[i].dependencyEnabled)
						{
							Sleep(0);
						}
					}
					device2->_currentTimesliceProgress += device2->_device.ExecuteStep();
					if (device2->_systemObject.IsSystemRollbackFlagged())
					{
						if (device2->_currentTimesliceProgress >= device2->_systemObject.SystemRollbackTime())
						{
							device2->_currentTimesliceProgress = device2->_timeslice;
						}
					}
				}
			}
		}

		// Ensure the second device in this execution thread is fully advanced to the end
		// of its timeslice, even if it is currently running in a spinoff execution thread.
		if (device1->_sharedExecuteThreadSpinoffActive)
		{
			lock1.lock();
			while (device1->_sharedExecuteThreadSpinoffActive && !device1->_sharedExecuteThreadSpinoffPaused)
			{
				device1->_sharedExecuteThreadSpinoffStoppedOrPaused.wait(lock1);
			}
			lock1.unlock();
		}

		// Set the remainingTime variable and call NotifyAfterExecuteStepFinishedTimeslice
		// for all devices which are currently executing in this combined execution thread
		if (!device1->_sharedExecuteThreadSpinoffActive || (device1->_currentSharedExecuteThreadOwner == device1))
		{
			device1->_remainingTime = device1->_currentTimesliceProgress - device1->_timeslice;
			device1->_device.NotifyAfterExecuteStepFinishedTimeslice();
		}
		if (!device1->_sharedExecuteThreadSpinoffActive || (device1->_currentSharedExecuteThreadOwner == device2))
		{
			device2->_remainingTime = device2->_currentTimesliceProgress - device2->_timeslice;
			device2->_device.NotifyAfterExecuteStepFinishedTimeslice();
		}

		// Notify the command thread that this execute command has been completed for all
		// devices which are currently executing in this combined execution thread
		lock1.lock();
		if (!device1->_sharedExecuteThreadSpinoffActive || (device1->_currentSharedExecuteThreadOwner == device1))
		{
			device1->_timesliceSuspended = false;
			device1->_timesliceCompleted = true;
			lock1.unlock();
			device1->WakeSuspendedDevicesIfRequired();
			lock1.lock();
			device1->_executeCompletionStateChanged.notify_all();
		}
		if (!device1->_sharedExecuteThreadSpinoffActive || (device1->_currentSharedExecuteThreadOwner == device2))
		{
			lock2.lock();
			device2->_timesliceSuspended = false;
			device2->_timesliceCompleted = true;
			lock2.unlock();
			lock1.unlock();
			device2->WakeSuspendedDevicesIfRequired();
			lock2.lock();
			device2->_executeCompletionStateChanged.notify_all();
			lock2.unlock();
			lock1.lock();
		}

		// Wait for a new execute task to be sent from the command thread to both our
		// target devices
		lock2.lock();
		while (device1->_executeWorkerThreadActive && device2->_executeWorkerThreadActive && device1->_timesliceCompleted)
		{
			lock2.unlock();
			device1->_executeTaskSent.wait(lock1);
			lock2.lock();
		}
		while (device1->_executeWorkerThreadActive && device2->_executeWorkerThreadActive && device2->_timesliceCompleted)
		{
			lock1.unlock();
			device2->_executeTaskSent.wait(lock2);
			lock1.lock();
		}
	}

	// Instruct the spinoff execution thread to shutdown, and wait for it to signal that it
	// is stopped.
	while (device1->_sharedExecuteThreadSpinoffRunning)
	{
		device1->_sharedExecuteThreadSpinoffStopRequested = true;
		device1->_sharedExecuteThreadSpinoffStateChangeRequested.notify_all();
		device1->_sharedExecuteThreadSpinoffStoppedOrPaused.wait(lock1);
	}

	// Signal that the execution thread has terminated for all devices running within this
	// shared execution thread
	device1->_executeThreadRunningState = false;
	device2->_executeThreadRunningState = false;
	device1->_executeThreadStopped.notify_all();
	device2->_executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadStepSharedExecutionThreadSpinoff()
{
	// Determine which device is the primary device which has ownership over the main
	// execution thread, and which device is the secondary device. Note that either device
	// may have its execution spun off onto this thread, but we need to know which device
	// is the primary device so that we can coordinate with the main execution thread.
	DeviceContext* primaryDevice = (_primarySharedExecuteThreadDevice)? this: _otherSharedExecuteThreadDevice;
	DeviceContext* secondaryDevice = (!_primarySharedExecuteThreadDevice)? this: _otherSharedExecuteThreadDevice;

	// Set the name of this thread for the benefit of an attached debugger
	std::wstring debuggerThreadName = L"DCExeSec - \"" + primaryDevice->_device.GetDeviceInstanceName() + L"\" and \"" + secondaryDevice->_device.GetDeviceInstanceName() + L"\"";
	SetCallingThreadName(debuggerThreadName);

	// Process execute requests from the primary execution thread until this thread is
	// requested to terminate
	while (!primaryDevice->_sharedExecuteThreadSpinoffStopRequested)
	{
		// Wait for a new command to be received from the primary execution thread
		std::unique_lock<std::mutex> primaryDeviceLock(primaryDevice->_executeThreadMutex);
		while (!primaryDevice->_sharedExecuteThreadSpinoffStopRequested && (!primaryDevice->_sharedExecuteThreadSpinoffActive || (primaryDevice->_sharedExecuteThreadSpinoffActive && (!primaryDevice->_sharedExecuteThreadSpinoffTimesliceAvailable || primaryDevice->_sharedExecuteThreadSpinoffRejoinRequested))))
		{
			// If our spinoff thread has been requested to rejoin the main execution
			// thread, flag that this spinoff thread is no longer active.
			if (primaryDevice->_sharedExecuteThreadSpinoffRejoinRequested)
			{
				primaryDevice->_sharedExecuteThreadSpinoffRejoinRequested = false;
				primaryDevice->_sharedExecuteThreadSpinoffActive = false;
			}

			// Flag that the spinoff thread is currently paused, and wait for a new command
			// to be received from the main execution thread.
			primaryDevice->_sharedExecuteThreadSpinoffPaused = true;
			primaryDevice->_sharedExecuteThreadSpinoffStoppedOrPaused.notify_all();
			primaryDevice->_sharedExecuteThreadSpinoffStateChangeRequested.wait(primaryDeviceLock);
		}

		// If this spinoff execution thread has been requested to terminate, restart the
		// loop to process the request.
		if (primaryDevice->_sharedExecuteThreadSpinoffStopRequested)
		{
			continue;
		}

		// Since we've received a new timeslice notification from the main execution
		// thread, flag that this spinoff execution thread is not paused, and notify the
		// main execution thread we have begun processing this timeslice.
		primaryDevice->_sharedExecuteThreadSpinoffPaused = false;
		primaryDevice->_sharedExecuteThreadSpinoffTimesliceAvailable = false;
		primaryDevice->_sharedExecuteThreadSpinoffTimesliceProcessingBegun.notify_all();
		primaryDeviceLock.unlock();

		// Advance the device state for the device currently assigned to execute within
		// this spinoff thread, until the end of the current timeslice is reached, or our
		// spinoff thread is requested to rejoin the main execution thread.
		DeviceContext* spinoffThreadTargetDevice = (primaryDevice->_currentSharedExecuteThreadOwner == this)? _otherSharedExecuteThreadDevice: this;
		unsigned int dependentTargetCount = (unsigned int)spinoffThreadTargetDevice->_deviceDependencies.size();
		while (!primaryDevice->_sharedExecuteThreadSpinoffRejoinRequested && (spinoffThreadTargetDevice->_currentTimesliceProgress < spinoffThreadTargetDevice->_timeslice))
		{
			for (unsigned int i = 0; i < dependentTargetCount; ++i)
			{
				while ((spinoffThreadTargetDevice->_currentTimesliceProgress > spinoffThreadTargetDevice->_deviceDependencies[i].device->_currentTimesliceProgress) && spinoffThreadTargetDevice->_deviceDependencies[i].dependencyEnabled)
				{
					Sleep(0);
				}
			}
			spinoffThreadTargetDevice->_currentTimesliceProgress += spinoffThreadTargetDevice->_device.ExecuteStep();
			if (spinoffThreadTargetDevice->_systemObject.IsSystemRollbackFlagged())
			{
				if (spinoffThreadTargetDevice->_currentTimesliceProgress >= spinoffThreadTargetDevice->_systemObject.SystemRollbackTime())
				{
					spinoffThreadTargetDevice->_currentTimesliceProgress = spinoffThreadTargetDevice->_timeslice;
				}
			}
		}

		// If our spinoff thread has been requested to rejoin the main execution thread,
		// restart the loop in order to process the request.
		if (primaryDevice->_sharedExecuteThreadSpinoffRejoinRequested)
		{
			continue;
		}

		// Since we've reached the end of the current timeslice for the device assigned to
		// this thread, set the remainingTime variable, and call
		// NotifyAfterExecuteStepFinishedTimeslice for the assigned device.
		spinoffThreadTargetDevice->_remainingTime = spinoffThreadTargetDevice->_currentTimesliceProgress - spinoffThreadTargetDevice->_timeslice;
		spinoffThreadTargetDevice->_device.NotifyAfterExecuteStepFinishedTimeslice();

		// Notify the command thread that this execute command has been completed for the
		// device currently executing in this spinoff thread
		primaryDeviceLock.lock();
		spinoffThreadTargetDevice->_timesliceSuspended = false;
		spinoffThreadTargetDevice->_timesliceCompleted = true;
		primaryDeviceLock.unlock();
		WakeSuspendedDevicesIfRequired();
		primaryDeviceLock.lock();
		spinoffThreadTargetDevice->_executeCompletionStateChanged.notify_all();
	}

	// Notify the main thread that this spinoff execution thread has terminated
	std::unique_lock<std::mutex> primaryDeviceLock(primaryDevice->_executeThreadMutex);
	primaryDevice->_sharedExecuteThreadSpinoffRunning = false;
	primaryDevice->_sharedExecuteThreadSpinoffStoppedOrPaused.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadTimeslice()
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	_executeThreadRunningState = true;
	_executeThreadReady.notify_all();
	if (_timesliceCompleted)
	{
		_executeTaskSent.wait(lock);
	}
	while (_executeWorkerThreadActive)
	{
		lock.unlock();
		_device.ExecuteTimeslice(_timeslice);
		_remainingTime = 0;
		_currentTimesliceProgress = _timeslice;
		_device.NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		_timesliceSuspended = false;
		_timesliceCompleted = true;
		lock.unlock();
		WakeSuspendedDevicesIfRequired();
		lock.lock();
		_executeCompletionStateChanged.notify_all();
		if (_timesliceCompleted)
		{
			_executeTaskSent.wait(lock);
		}
	}
	_executeThreadRunningState = false;
	_executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadTimesliceWithDependencies()
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	_executeThreadRunningState = true;
	_executeThreadReady.notify_all();
	if (_timesliceCompleted)
	{
		_executeTaskSent.wait(lock);
	}
	while (_executeWorkerThreadActive)
	{
		lock.unlock();
		_device.ExecuteTimeslice(_timeslice);
		_remainingTime = 0;
		_currentTimesliceProgress = _timeslice;
		_device.NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		unsigned int dependentTargetCount = (unsigned int)_deviceDependencies.size();
		for (unsigned int i = 0; i < dependentTargetCount; ++i)
		{
			if (_deviceDependencies[i].dependencyEnabled)
			{
				lock.unlock();
				_deviceDependencies[i].device->WaitForCompletion();
				lock.lock();
			}
		}

		_timesliceSuspended = false;
		_timesliceCompleted = true;
		lock.unlock();
		WakeSuspendedDevicesIfRequired();
		lock.lock();
		_executeCompletionStateChanged.notify_all();
		if (_timesliceCompleted)
		{
			_executeTaskSent.wait(lock);
		}
	}
	_executeThreadRunningState = false;
	_executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::WakeSuspendedDevicesIfRequired()
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	auto executingThreadCount = _executingThreadCount;
	auto suspendedThreadCount = _suspendedThreadCount;
	IExecutionSuspendManager* suspendManager = _suspendManager;
	if (suspendManager == nullptr)
	{
		return;
	}
	lock.unlock();

	// Evaluate whether all remaining threads are suspended. If they are, release all suspended threads.
	if (suspendManager->AllDevicesSuspended(executingThreadCount->load(), suspendedThreadCount->load()))
	{
		suspendManager->DisableTimesliceExecutionSuspend();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceContext::ClearSuspendManagerState()
{
	std::unique_lock<std::mutex> lock(_executeThreadMutex);
	_executingThreadCount = nullptr;
	_suspendedThreadCount = nullptr;
	_suspendManager = nullptr;
}
