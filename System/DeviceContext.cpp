#include "DeviceContext.h"
#include "ThreadLib/ThreadLib.pkg"
#include "Debug/Debug.pkg"

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int DeviceContext::GetIDeviceContextVersion() const
{
	return ThisIDeviceContextVersion();
}

//----------------------------------------------------------------------------------------
//Timing functions
//----------------------------------------------------------------------------------------
double DeviceContext::GetCurrentTimesliceProgress() const
{
	return currentTimesliceProgress;
}

//----------------------------------------------------------------------------------------
void DeviceContext::SetCurrentTimesliceProgress(double executionProgress)
{
	currentTimesliceProgress = executionProgress;
	remainingTime = currentTimesliceProgress - timeslice;
}

//----------------------------------------------------------------------------------------
//Control functions
//----------------------------------------------------------------------------------------
bool DeviceContext::DeviceEnabled() const
{
	return deviceEnabled;
}

//----------------------------------------------------------------------------------------
void DeviceContext::SetDeviceEnabled(bool state)
{
	deviceEnabled = state;
}

//----------------------------------------------------------------------------------------
//Device interface
//----------------------------------------------------------------------------------------
IDevice& DeviceContext::GetTargetDevice() const
{
	return device;
}

//----------------------------------------------------------------------------------------
unsigned int DeviceContext::GetDeviceIndexNo() const
{
	return deviceIndexNo;
}

//----------------------------------------------------------------------------------------
//System message functions
//----------------------------------------------------------------------------------------
void DeviceContext::WriteLogEvent(const ILogEntry& entry)
{
	//Ensure that the source of the event is correctly set for the calling device
	std::wstring eventLogSource = GetTargetDevice().GetDeviceInstanceName() + L" [" + GetTargetDevice().GetModuleDisplayName() + L"]";
	entry.OverrideSourceText(eventLogSource);

	//Pass the event log message on to the system
	systemObject.WriteLogEvent(entry);
}

//----------------------------------------------------------------------------------------
void DeviceContext::FlagStopSystem()
{
	systemObject.FlagStopSystem();
}

//----------------------------------------------------------------------------------------
void DeviceContext::StopSystem()
{
	systemObject.StopSystem();
}

//----------------------------------------------------------------------------------------
void DeviceContext::RunSystem()
{
	systemObject.RunSystem();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteDeviceStep()
{
	systemObject.ExecuteDeviceStep(&device);
}

//----------------------------------------------------------------------------------------
void DeviceContext::GetFullyQualifiedDeviceInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetFullyQualifiedDeviceInstanceName());
}

//----------------------------------------------------------------------------------------
void DeviceContext::GetModuleDisplayNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetModuleDisplayName());
}

//----------------------------------------------------------------------------------------
void DeviceContext::GetModuleInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetModuleInstanceName());
}

//----------------------------------------------------------------------------------------
//Suspend functions
//----------------------------------------------------------------------------------------
bool DeviceContext::UsesExecuteSuspend() const
{
	return device.UsesExecuteSuspend();
}

//----------------------------------------------------------------------------------------
bool DeviceContext::UsesTransientExecution() const
{
	return device.UsesTransientExecution();
}

//----------------------------------------------------------------------------------------
bool DeviceContext::TimesliceExecutionSuspended() const
{
	return timesliceSuspended;
}

//----------------------------------------------------------------------------------------
void DeviceContext::SuspendTimesliceExecution()
{
	boost::mutex::scoped_lock executeLock(executeThreadMutex);

	//Ensure that the device calling the suspend function has indicated that it uses the
	//suspend feature. Failure to do this may cause deadlocks.
	DebugAssert(device.UsesExecuteSuspend());

	if(!timesliceSuspended && !timesliceSuspensionDisable)
	{
		if((commandMutexPointer != 0) && (suspendedThreadCountPointer != 0))
		{
			executeLock.unlock();
			boost::mutex::scoped_lock commandLock(*commandMutexPointer);
			executeLock.lock();

			if(!timesliceSuspended)
			{
				timesliceSuspended = true;

				//Since this execution thread is currently suspended, increment the
				//suspended thread count.
				ReferenceCounterIncrement(*suspendedThreadCountPointer);

				executeCompletionStateChanged.notify_all();
			}
		}
		else
		{
			timesliceSuspended = true;
			executeCompletionStateChanged.notify_all();
		}
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::WaitForTimesliceExecutionResume() const
{
	boost::mutex::scoped_lock lock(executeThreadMutex);

	//Wait for the timeslice to resume execution, or for timeslice suspension to be
	//disabled.
	while(timesliceSuspended && !timesliceSuspensionDisable)
	{
		executeCompletionStateChanged.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::ResumeTimesliceExecution()
{
	boost::mutex::scoped_lock executeLock(executeThreadMutex);
	if(timesliceSuspended)
	{
		if((commandMutexPointer != 0) && (suspendedThreadCountPointer != 0))
		{
			executeLock.unlock();
			boost::mutex::scoped_lock commandLock(*commandMutexPointer);
			executeLock.lock();

			if(timesliceSuspended)
			{
				timesliceSuspended = false;

				//Since this execution thread has left the suspended state, decrement the
				//suspended thread count.
				ReferenceCounterDecrement(*suspendedThreadCountPointer);

				executeCompletionStateChanged.notify_all();
			}
		}
		else
		{
			timesliceSuspended = false;
			executeCompletionStateChanged.notify_all();
		}
	}
}

//----------------------------------------------------------------------------------------
bool DeviceContext::TimesliceSuspensionDisabled() const
{
	return timesliceSuspensionDisable;
}

//----------------------------------------------------------------------------------------
bool DeviceContext::TransientExecutionActive() const
{
	return transientExecutionActive;
}

//----------------------------------------------------------------------------------------
void DeviceContext::SetTransientExecutionActive(bool state)
{
	transientExecutionActive = state;

	//If a transient execution device has just stopped executing, evaluate whether all
	//remaining devices in the system are suspended or blocked, and unblock them in this
	//case.
	if(!state && (commandMutexPointer != 0))
	{
		//Obtain a lock on the shared command mutex. We need to do this so that we can
		//safely work with the result of the suspendedThreadCount variable.
		boost::mutex::scoped_lock commandLock(*commandMutexPointer);

		//Only perform any checks if we're currently executing a wait for completion
		//command. If we're not executing this command, the remaining thread count value
		//is invalid, and we also don't even need this check here, because when the
		//command is issued later, it will be detected at that time if all devices are
		//suspended or blocked.
		if(executingWaitForCompletionCommand)
		{
			//Evaluate whether all remaining threads are suspended. If they are, release
			//all suspended threads. If not, wait for the completion state of this execute
			//thread to change.
			if(suspendManager->AllDevicesSuspended(*suspendedThreadCountPointer, *remainingThreadCountPointer))
			{
				//Note that we need to not hold a lock on executeThreadMutex here, since
				//the suspend manager will call back into this DeviceContext object and
				//call the DisableTimesliceExecutionSuspend() function in response to the
				//function call below.
				suspendManager->DisableTimesliceExecutionSuspend();
			}
		}
	}
}

//----------------------------------------------------------------------------------------
bool DeviceContext::TimesliceExecutionCompleted() const
{
	return timesliceCompleted && !transientExecutionActive;
}

//----------------------------------------------------------------------------------------
void DeviceContext::DisableTimesliceExecutionSuspend()
{
	//##FIX## We took out the locks here and in EnableTimesliceExecutionSuspend on the
	//executeThreadMutex, to avoid a deadlock that was occurring due to this worker thread
	//holding onto the mutex, but being unable to obtain the command mutex, while another
	//thread called back into this thread and attempted to disable execution suspend. I
	//think we need the lock though, as we need to ensure that the internal state for the
	//condition variable is not corrupted due to concurrent access. We need a lock here to
	//ensure any other threads which wait on this condition are not actively working with
	//it at the time we try and trigger it.
	//##TODO## Looking at the internal implementation of the boost::condition class, it
	//appears that the class has its own internal locks, and based on the design, it
	//appears that we should be able to safely execute this code without a lock, and
	//without causing any internal inconsistencies in the condition variable. This means
	//the only concurrency issues we have to worry about are in our own code. That said,
	//looking at our code, we still need the lock, otherwise a deadlock could occur. Our
	//code tests timesliceSuspensionDisable, then enters a wait state if it is not set. If
	//another thread yields after testing timesliceSuspensionDisable, but before the wait
	//on executeCompletionStateChanged, then this thread runs through and sets the
	//condition, the other thread could then resume, and miss the condition, and enter the
	//wait state indefinitely when timesliceSuspensionDisable is set. This means we
	//absolutely need a lock here.

	boost::mutex::scoped_lock lock(executeThreadMutex);
	timesliceSuspensionDisable = true;
	executeCompletionStateChanged.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::EnableTimesliceExecutionSuspend()
{
	//##FIX## We've disabled this temporarily to try and debug a mysterious deadlock
//	boost::mutex::scoped_lock lock(executeThreadMutex);
	timesliceSuspensionDisable = false;
	executeCompletionStateChanged.notify_all();
}

//----------------------------------------------------------------------------------------
//Dependent device functions
//----------------------------------------------------------------------------------------
void DeviceContext::SetDeviceDependencyEnable(IDeviceContext* targetDevice, bool state)
{
	//Attempt to find a dependency on the specified target device, and update its
	//dependency enable state to the state specified.
	bool foundDeviceDependency = false;
	bool previousDependencyState;
	unsigned int dependentTargetCount = (unsigned int)deviceDependencies.size();
	unsigned int i = 0;
	while(i < dependentTargetCount)
	{
		if(deviceDependencies[i].device == targetDevice)
		{
			foundDeviceDependency = true;
			previousDependencyState = deviceDependencies[i].dependencyEnabled;
			deviceDependencies[i].dependencyEnabled = state;
		}
		++i;
	}

	//If this device is sharing an execution thread with the target device, we now need to
	//either split or combine the execution threads, now that the device dependency enable
	//state has changed.
	if(sharingExecuteThread && foundDeviceDependency && (targetDevice == otherSharedExecuteThreadDevice) && (previousDependencyState != state))
	{
		DeviceContext* primaryDevice = (primarySharedExecuteThreadDevice)? this: otherSharedExecuteThreadDevice;
		if(state)
		{
			//If the device dependency has been enabled, request the spinoff execution
			//thread for the combined execution thread to rejoin. Note that we do not wait
			//for this process to be completed here, for performance reasons. We want to
			//allow the spinoff thread and main execution thread to continue in parallel
			//for as long as possible. Since we've just re-enabled the device dependency,
			//the main execution thread will now not allow the owning device to be
			//advanced beyond the device executing in the spinoff thread, and the spinoff
			//thread will halt after the end of the next device step, so we can safely
			//continue execution here without waiting for the rejoin request to be
			//processed.
			boost::mutex::scoped_lock lock(primaryDevice->executeThreadMutex);
			if(primaryDevice->sharedExecuteThreadSpinoffActive)
			{
				primaryDevice->sharedExecuteThreadSpinoffRejoinRequested = true;
				primaryDevice->sharedExecuteThreadSpinoffStateChangeRequested.notify_all();
			}
		}
		else
		{
			//If the device dependency has been disabled, wait for the spinoff thread to
			//terminate if it is currently active, as it's possible we have previously
			//requested the spinoff thread to rejoin, but the device dependency state was
			//changed again before the spinoff thread processed the request. This is the
			//only case that could have arisen if the spinoff thread is still active here.
			boost::mutex::scoped_lock lock(primaryDevice->executeThreadMutex);
			while(primaryDevice->sharedExecuteThreadSpinoffActive)
			{
				primaryDevice->sharedExecuteThreadSpinoffStoppedOrPaused.wait(lock);
			}

			//Request the spinoff thread for the combined execution thread to begin
			//advancing the device that doesn't currently own the main execution thread in
			//parallel.
			primaryDevice->sharedExecuteThreadSpinoffActive = true;
			primaryDevice->sharedExecuteThreadSpinoffTimesliceAvailable = true;
			primaryDevice->sharedExecuteThreadSpinoffStateChangeRequested.notify_all();
		}
	}
}

//----------------------------------------------------------------------------------------
//Command worker thread control
//----------------------------------------------------------------------------------------
void DeviceContext::StartCommandWorkerThread(size_t deviceIndex, volatile ReferenceCounterType& remainingThreadCount, volatile ReferenceCounterType& suspendedThreadCount, boost::mutex& commandMutex, boost::condition& commandSent, boost::condition& commandProcessed, IExecutionSuspendManager* asuspendManager, const DeviceContextCommand& command)
{
	boost::mutex::scoped_lock lock(commandMutex);
	if(!commandWorkerThreadActive)
	{
		commandWorkerThreadActive = true;
		boost::thread workerThread(boost::bind(boost::mem_fn(&DeviceContext::CommandWorkerThread), this, deviceIndex, boost::ref(remainingThreadCount), boost::ref(suspendedThreadCount), boost::ref(commandMutex), boost::ref(commandSent), boost::ref(commandProcessed), asuspendManager, boost::ref(command)));
		commandThreadReady.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::StopCommandWorkerThread()
{
	commandWorkerThreadActive = false;
}

//----------------------------------------------------------------------------------------
void DeviceContext::CommandWorkerThread(size_t deviceIndex, volatile ReferenceCounterType& remainingThreadCount, volatile ReferenceCounterType& suspendedThreadCount, boost::mutex& commandMutex, boost::condition& commandSent, boost::condition& commandProcessed, IExecutionSuspendManager* asuspendManager, const DeviceContextCommand& command)
{
	//Set the name of this thread for the debugger
	std::wstring debuggerThreadName = L"DCCommand - " + device.GetDeviceInstanceName();
	SetCallingThreadName(debuggerThreadName);

	//Flag that we need to notify the calling thread when this thread is ready, since this
	//worker thread has just been started.
	bool notifyThreadReady = true;

	//Record if we just processed a wait for completion command. We need this to support
	//resuming suspended execute threads.
	executingWaitForCompletionCommand = false;

	//Store pointers to the command mutex and suspended thread count, so that we can
	//access them from our execution thread when required.
	commandMutexPointer = &commandMutex;
	suspendedThreadCountPointer = &suspendedThreadCount;
	remainingThreadCountPointer = &remainingThreadCount;
	suspendManager = asuspendManager;

	//Process each command from the execution manager until we receive a command to stop
	while(commandWorkerThreadActive)
	{
		//Wait for a new command to be received
		{
			//Obtain a lock on the shared command mutex
			boost::mutex::scoped_lock lock(commandMutex);

			//Notify the execution manager when all worker threads have processed the
			//message. Note that to achieve thread safety, we have to perform this
			//operation within the same lock where we wait on the commandSent mutex, so
			//that the execution manager will not resume execution until all command
			//threads are ready and waiting to receive a new command.
			if(ReferenceCounterDecrement(remainingThreadCount) == 0)
			{
				commandProcessed.notify_all();
			}
			else if(executingWaitForCompletionCommand)
			{
				//We need a special catch here for suspended threads. If this device just
				//finished executing a WaitForCompletion command, and at this point, all
				//remaining threads are suspended, we need to disable thread suspension at
				//this point so that the suspended threads can be resumed.
				if(asuspendManager->AllDevicesSuspended(suspendedThreadCount, remainingThreadCount))
				{
					asuspendManager->DisableTimesliceExecutionSuspend();
				}
			}

			//If this worker thread has just been started, notify the calling thread that
			//the worker thread is now ready to receive commands. The calling thread is
			//locked on the commandMutex too, so this is thread safe. The calling thread
			//will only be unblocked when we release the mutex in order to wait for a new
			//command.
			if(notifyThreadReady)
			{
				notifyThreadReady = false;
				commandThreadReady.notify_all();
			}

			//Wait for a new command to be received
			commandSent.wait(lock);
		}

		//Flag if we just received a wait for execute complete command
		executingWaitForCompletionCommand = (command.type == DeviceContextCommand::TYPE_WAITFOREXECUTECOMPLETE);

		//Process the command
		ProcessCommand(deviceIndex, command, remainingThreadCount);
	}

	//If we've received a command to terminate the worker thread, notify the execution
	//manager when all worker threads have terminated. We need this block of code here,
	//since a command to terminate the worker thread will cause us to leave the main
	//command loop immediately.
	{
		boost::mutex::scoped_lock lock(commandMutex);

		//Notify the execution manager that all worker threads have started and are ready
		//to accept commands
		if(ReferenceCounterDecrement(remainingThreadCount) == 0)
		{
			commandProcessed.notify_all();
		}
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::ProcessCommand(size_t deviceIndex, const DeviceContextCommand& command, volatile ReferenceCounterType& remainingThreadCount)
{
	switch(command.type)
	{
	case DeviceContextCommand::TYPE_SUSPENDEXECUTION:
		SuspendExecution();
		break;
	case DeviceContextCommand::TYPE_COMMIT:
		Commit();
		break;
	case DeviceContextCommand::TYPE_ROLLBACK:
		Rollback();
		break;
	case DeviceContextCommand::TYPE_GETNEXTTIMINGPOINT:{
		unsigned int accessContext = 0;
		command.timesliceResult[deviceIndex] = GetNextTimingPoint(accessContext);
		command.contextResult[deviceIndex] = accessContext;
		break;}
	case DeviceContextCommand::TYPE_NOTIFYUPCOMINGTIMESLICE:
		NotifyUpcomingTimeslice(command.timeslice);
		break;
	case DeviceContextCommand::TYPE_NOTIFYBEFOREEXECUTECALLED:
		NotifyBeforeExecuteCalled();
		break;
	case DeviceContextCommand::TYPE_NOTIFYAFTEREXECUTECALLED:
		NotifyAfterExecuteCalled();
		break;
	case DeviceContextCommand::TYPE_EXECUTETIMESLICE:
		if(ActiveDevice())
		{
			ExecuteTimeslice(command.timeslice);
		}
		break;
	case DeviceContextCommand::TYPE_WAITFOREXECUTECOMPLETE:
		if(ActiveDevice())
		{
			WaitForCompletionAndDetectSuspendLock(*suspendedThreadCountPointer, remainingThreadCount, *commandMutexPointer, suspendManager);
		}
		break;
	}
}

//----------------------------------------------------------------------------------------
//Worker thread control
//----------------------------------------------------------------------------------------
void DeviceContext::BeginExecution(size_t deviceIndex, volatile ReferenceCounterType& remainingThreadCount, volatile ReferenceCounterType& suspendedThreadCount, boost::mutex& commandMutex, boost::condition& commandSent, boost::condition& commandProcessed, IExecutionSuspendManager* asuspendManager, const DeviceContextCommand& command)
{
	//Start the command worker thread
	StartCommandWorkerThread(deviceIndex, remainingThreadCount, suspendedThreadCount, commandMutex, commandSent, commandProcessed, asuspendManager, command);

	//Start the execute worker thread
	StartExecuteWorkerThread();
}

//----------------------------------------------------------------------------------------
void DeviceContext::SuspendExecution()
{
	//Terminate the execute worker thread
	StopExecuteWorkerThread();

	//Instruct the command worker thread to terminate. Note that this just sets a flag at
	//this point. Since we receive the suspend command through the command worker thread,
	//we're currently within that thread when this function is called. Once this message
	//has been fully processed, this worker thread will now terminate.
	StopCommandWorkerThread();
}

//----------------------------------------------------------------------------------------
//Execute worker thread control
//----------------------------------------------------------------------------------------
void DeviceContext::StartExecuteWorkerThread()
{
	boost::mutex::scoped_lock lock(executeThreadMutex);
	if(!executeWorkerThreadActive && ActiveDevice())
	{
		//Notify the device that execution is about to begin
		device.BeginExecution();

		//Scan our list of device dependencies. If we have a two-way dependency with
		//another device, and both our device and their device use step execution, we fold
		//the two devices into a single execution thread for efficiency. In this model,
		//one of the devices is considered the primary owner of the execute thread, and
		//the other device is considered the secondary owner. To select this we, make the
		//primary owner the device that has the lowest device index number. We only
		//actually spawn an execute thread here if the target device is the primary owner
		//of this combined execution thread.
		bool foundInterlockedDevice = false;
		bool ourDeviceIsPrimaryInterlockedDevice = false;
		bool interlockedDeviceDependenciesCurrentlyDisabled = false;
		DeviceContext* interlockedDevice = 0;
		if(device.GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP)
		{
			for(unsigned int deviceDependencyIndex = 0; deviceDependencyIndex < (unsigned int)deviceDependencies.size(); ++deviceDependencyIndex)
			{
				DeviceContext* targetDevice = deviceDependencies[deviceDependencyIndex].device;
				if(targetDevice->device.GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP)
				{
					for(unsigned int targetDeviceDependencyIndex = 0; targetDeviceDependencyIndex < (unsigned int)targetDevice->deviceDependencies.size(); ++targetDeviceDependencyIndex)
					{
						if(targetDevice->deviceDependencies[targetDeviceDependencyIndex].device == this)
						{
							foundInterlockedDevice = true;
							interlockedDevice = targetDevice;
							ourDeviceIsPrimaryInterlockedDevice = (deviceIndexNo < targetDevice->deviceIndexNo);
							interlockedDeviceDependenciesCurrentlyDisabled = (!deviceDependencies[deviceDependencyIndex].dependencyEnabled || !targetDevice->deviceDependencies[targetDeviceDependencyIndex].dependencyEnabled);
						}
					}
				}
			}
		}

		//If this device is going to share an execution thread with another device, record
		//information about the shared execution thread.
		sharingExecuteThread = foundInterlockedDevice;
		primarySharedExecuteThreadDevice = ourDeviceIsPrimaryInterlockedDevice;
		otherSharedExecuteThreadDevice = interlockedDevice;

		//Start the execution thread for this device
		executeWorkerThreadActive = true;
		if(!foundInterlockedDevice)
		{
			boost::thread workerThread(boost::bind(boost::mem_fn(&DeviceContext::ExecuteWorkerThread), this));
		}
		else if(ourDeviceIsPrimaryInterlockedDevice)
		{
			//If the interlocked dependencies are present but currently disabled, flag the
			//execute spinoff thread as active, so that it will be restored when we start
			//the execute thread below.
			sharedExecuteThreadSpinoffActive = interlockedDeviceDependenciesCurrentlyDisabled;

			//Start the primary execute thread
			boost::thread workerThread(boost::bind(&DeviceContext::ExecuteWorkerThreadStepMultipleDeviceSharedDependencies, this, interlockedDevice));
		}

		//Wait for confirmation that the execute worker thread is ready to receive
		//commands
		while(!executeThreadRunningState)
		{
			executeThreadReady.wait(lock);
		}
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::StopExecuteWorkerThread()
{
	boost::mutex::scoped_lock lock(executeThreadMutex);
	if(executeWorkerThreadActive)
	{
		//Instruct the execution thread to terminate, and wait for confirmation that it
		//has stopped.
		executeWorkerThreadActive = false;
		while(executeThreadRunningState)
		{
			executeTaskSent.notify_all();
			executeThreadStopped.wait(lock);
		}

		//Notify the device that execution is being suspended
		device.SuspendExecution();
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThread()
{
	//Set the name of this thread for the debugger
	std::wstring debuggerThreadName = L"DCExecute - " + device.GetDeviceInstanceName();
	SetCallingThreadName(debuggerThreadName);

	if(device.GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP)
	{
		if(deviceDependencies.empty())
		{
			ExecuteWorkerThreadStep();
		}
		else
		{
			ExecuteWorkerThreadStepWithDependencies();
		}
	}
	else if(device.GetUpdateMethod() == IDevice::UPDATEMETHOD_TIMESLICE)
	{
		if(deviceDependencies.empty())
		{
			ExecuteWorkerThreadTimeslice();
		}
		else
		{
			ExecuteWorkerThreadTimesliceWithDependencies();
		}
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadStep()
{
	boost::mutex::scoped_lock lock(executeThreadMutex);
	executeThreadRunningState = true;
	executeThreadReady.notify_all();
	executeTaskSent.wait(lock);
	while(executeWorkerThreadActive)
	{
		lock.unlock();
		while(currentTimesliceProgress < timeslice)
		{
			currentTimesliceProgress += device.ExecuteStep();
			if(systemObject.IsSystemRollbackFlagged())
			{
				if(currentTimesliceProgress >= systemObject.SystemRollbackTime())
				{
					currentTimesliceProgress = timeslice;
				}
			}
		}
		remainingTime = currentTimesliceProgress - timeslice;
		device.NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		timesliceSuspended = false;
		timesliceCompleted = true;
		executeCompletionStateChanged.notify_all();
		executeTaskSent.wait(lock);
	}
	executeThreadRunningState = false;
	executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadStepWithDependencies()
{
	boost::mutex::scoped_lock lock(executeThreadMutex);
	executeThreadRunningState = true;
	executeThreadReady.notify_all();
	executeTaskSent.wait(lock);
	while(executeWorkerThreadActive)
	{
		lock.unlock();
		unsigned int dependentTargetCount = (unsigned int)deviceDependencies.size();
		while(currentTimesliceProgress < timeslice)
		{
			for(unsigned int i = 0; i < dependentTargetCount; ++i)
			{
				//SafeMemoryBarrierRead();
				while((currentTimesliceProgress > deviceDependencies[i].device->currentTimesliceProgress) && deviceDependencies[i].dependencyEnabled)
				{
					Sleep(0);
					//SafeMemoryBarrierRead();
				}
			}
			currentTimesliceProgress += device.ExecuteStep();
			if(systemObject.IsSystemRollbackFlagged())
			{
				if(currentTimesliceProgress >= systemObject.SystemRollbackTime())
				{
					currentTimesliceProgress = timeslice;
				}
			}
			//SafeMemoryBarrierWrite();
		}
		remainingTime = currentTimesliceProgress - timeslice;
		device.NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		timesliceSuspended = false;
		timesliceCompleted = true;
		executeCompletionStateChanged.notify_all();
		executeTaskSent.wait(lock);
	}
	executeThreadRunningState = false;
	executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadStepMultipleDeviceSharedDependencies(DeviceContext* device1, DeviceContext* device2)
{
	//Set the name of this thread for the benefit of an attached debugger
	std::wstring debuggerThreadName = L"DCExePri - \"" + device1->device.GetDeviceInstanceName() + L"\" and \"" + device2->device.GetDeviceInstanceName() + L"\"";
	SetCallingThreadName(debuggerThreadName);

	//Notify the command threads for both devices in this shared execution thread that the
	//execution thread for the target devices is up and running. Note that we have a lock
	//here on executeThreadMutex here, so the command threads will not actually be
	//unblocked until we wait on a message from the command thread again and release the
	//lock.
	boost::mutex::scoped_lock lock1(device1->executeThreadMutex);
	boost::mutex::scoped_lock lock2(device2->executeThreadMutex);
	device1->executeThreadRunningState = true;
	device2->executeThreadRunningState = true;
	device1->executeThreadReady.notify_all();
	device2->executeThreadReady.notify_all();

	//Start our parallel execution thread for these locked devices, so that if the device
	//dependency is enabled, the second device can be spun off into this parallel
	//execution thread. We always create this thread here, and leave it in a wait state,
	//because creating and destroying a thread is a relatively expensive operation, and we
	//want there to be minimal overhead spinning off execution to this parallel thread, as
	//this could potentially happen many times during a single timeslice.
	device1->currentSharedExecuteThreadOwner = device1;
	device1->sharedExecuteThreadSpinoffRunning = true;
	device1->sharedExecuteThreadSpinoffStopRequested = false;
	device1->sharedExecuteThreadSpinoffTimesliceAvailable = false;
	device1->sharedExecuteThreadSpinoffRejoinRequested = false;
	boost::thread workerThread(boost::bind(boost::mem_fn(&DeviceContext::ExecuteWorkerThreadStepSharedExecutionThreadSpinoff), device1));

	//Wait for a new execute task to be sent from the command thread to both our target
	//devices
	lock2.unlock();
	device1->executeTaskSent.wait(lock1);
	lock1.unlock();
	lock2.lock();
	while(device1->executeWorkerThreadActive && device2->executeWorkerThreadActive && (device2->timesliceCompleted || (device2->timeslice != device1->timeslice)))
	{
		device2->executeTaskSent.wait(lock2);
	}
	lock1.lock();

	//Process execute commands until a command is sent requesting the execute threads for
	//our target devices to shutdown
	while(device1->executeWorkerThreadActive && device2->executeWorkerThreadActive)
	{
		lock1.unlock();
		lock2.unlock();

		//If our spinoff thread is currently active and waiting to advance a device, send
		//a notification to the spinoff thread about the new timeslice that has been
		//received, and wait for it to acknowledge that it has received the notification.
		if(device1->sharedExecuteThreadSpinoffActive)
		{
			lock1.lock();
			device1->sharedExecuteThreadSpinoffTimesliceAvailable = true;
			device1->sharedExecuteThreadSpinoffStateChangeRequested.notify_all();
			device1->sharedExecuteThreadSpinoffTimesliceProcessingBegun.wait(lock1);
			lock1.unlock();
		}

		//Advance all devices which are executing within this combined execution thread to
		//the end of the current timeslice
		unsigned int device1DependentTargetCount = (unsigned int)device1->deviceDependencies.size();
		unsigned int device2DependentTargetCount = (unsigned int)device2->deviceDependencies.size();
		while((!device1->sharedExecuteThreadSpinoffActive && ((device1->currentTimesliceProgress < device1->timeslice) || (device2->currentTimesliceProgress < device2->timeslice)))
		  || (device1->sharedExecuteThreadSpinoffActive && (device1->currentSharedExecuteThreadOwner->currentTimesliceProgress < device1->currentSharedExecuteThreadOwner->timeslice)))
		{
			//Advance the first device in this execution thread a single step, if it is
			//not currently executing in a spinoff thread.
			if(!device1->sharedExecuteThreadSpinoffActive || (device1->currentSharedExecuteThreadOwner == device1))
			{
				device1->currentSharedExecuteThreadOwner = device1;
				while((device1->sharedExecuteThreadSpinoffActive || (device1->currentTimesliceProgress <= device2->currentTimesliceProgress)) && (device1->currentTimesliceProgress < device1->timeslice))
				{
					for(unsigned int i = 0; i < device1DependentTargetCount; ++i)
					{
						while(((device1->deviceDependencies[i].device != device2) || device1->sharedExecuteThreadSpinoffRejoinRequested) && (device1->currentTimesliceProgress > device1->deviceDependencies[i].device->currentTimesliceProgress) && device1->deviceDependencies[i].dependencyEnabled)
						{
							Sleep(0);
						}
					}
					device1->currentTimesliceProgress += device1->device.ExecuteStep();
					if(device1->systemObject.IsSystemRollbackFlagged())
					{
						if(device1->currentTimesliceProgress >= device1->systemObject.SystemRollbackTime())
						{
							device1->currentTimesliceProgress = device1->timeslice;
						}
					}
				}
			}

			//Advance the second device in this execution thread a single step, if it is
			//not currently executing in a spinoff thread.
			if(!device1->sharedExecuteThreadSpinoffActive || (device1->currentSharedExecuteThreadOwner == device2))
			{
				device1->currentSharedExecuteThreadOwner = device2;
				while((device1->sharedExecuteThreadSpinoffActive || (device2->currentTimesliceProgress <= device1->currentTimesliceProgress)) && (device2->currentTimesliceProgress < device2->timeslice))
				{
					for(unsigned int i = 0; i < device2DependentTargetCount; ++i)
					{
						while(((device2->deviceDependencies[i].device != device1) || device1->sharedExecuteThreadSpinoffRejoinRequested) && (device2->currentTimesliceProgress > device2->deviceDependencies[i].device->currentTimesliceProgress) && device2->deviceDependencies[i].dependencyEnabled)
						{
							Sleep(0);
						}
					}
					device2->currentTimesliceProgress += device2->device.ExecuteStep();
					if(device2->systemObject.IsSystemRollbackFlagged())
					{
						if(device2->currentTimesliceProgress >= device2->systemObject.SystemRollbackTime())
						{
							device2->currentTimesliceProgress = device2->timeslice;
						}
					}
				}
			}
		}

		//Ensure the second device in this execution thread is fully advanced to the end
		//of its timeslice, even if it is currently running in a spinoff execution thread.
		if(device1->sharedExecuteThreadSpinoffActive)
		{
			lock1.lock();
			while(device1->sharedExecuteThreadSpinoffActive && !device1->sharedExecuteThreadSpinoffPaused)
			{
				device1->sharedExecuteThreadSpinoffStoppedOrPaused.wait(lock1);
			}
			lock1.unlock();
		}

		//Set the remainingTime variable and call NotifyAfterExecuteStepFinishedTimeslice
		//for all devices which are currently executing in this combined execution thread
		if(!device1->sharedExecuteThreadSpinoffActive || (device1->currentSharedExecuteThreadOwner == device1))
		{
			device1->remainingTime = device1->currentTimesliceProgress - device1->timeslice;
			device1->device.NotifyAfterExecuteStepFinishedTimeslice();
		}
		if(!device1->sharedExecuteThreadSpinoffActive || (device1->currentSharedExecuteThreadOwner == device2))
		{
			device2->remainingTime = device2->currentTimesliceProgress - device2->timeslice;
			device2->device.NotifyAfterExecuteStepFinishedTimeslice();
		}

		//Notify the command thread that this execute command has been completed for all
		//devices which are currently executing in this combined execution thread
		lock1.lock();
		if(!device1->sharedExecuteThreadSpinoffActive || (device1->currentSharedExecuteThreadOwner == device1))
		{
			device1->timesliceSuspended = false;
			device1->timesliceCompleted = true;
			device1->executeCompletionStateChanged.notify_all();
		}
		if(!device1->sharedExecuteThreadSpinoffActive || (device1->currentSharedExecuteThreadOwner == device2))
		{
			lock2.lock();
			device2->timesliceSuspended = false;
			device2->timesliceCompleted = true;
			device2->executeCompletionStateChanged.notify_all();
			lock2.unlock();
		}

		//Wait for a new execute task to be sent from the command thread to both our
		//target devices
		device1->executeTaskSent.wait(lock1);
		lock1.unlock();
		lock2.lock();
		while(device1->executeWorkerThreadActive && device2->executeWorkerThreadActive && (device2->timesliceCompleted || (device2->timeslice != device1->timeslice)))
		{
			device2->executeTaskSent.wait(lock2);
		}
		lock1.lock();
	}

	//Instruct the spinoff execution thread to shutdown, and wait for it to signal that it
	//is stopped.
	while(device1->sharedExecuteThreadSpinoffRunning)
	{
		device1->sharedExecuteThreadSpinoffStopRequested = true;
		device1->sharedExecuteThreadSpinoffStateChangeRequested.notify_all();
		device1->sharedExecuteThreadSpinoffStoppedOrPaused.wait(lock1);
	}

	//Signal that the execution thread has terminated for all devices running within this
	//shared execution thread
	device1->executeThreadRunningState = false;
	device2->executeThreadRunningState = false;
	device1->executeThreadStopped.notify_all();
	device2->executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadStepSharedExecutionThreadSpinoff()
{
	//Determine which device is the primary device which has ownership over the main
	//execution thread, and which device is the secondary device. Note that either device
	//may have its execution spun off onto this thread, but we need to know which device
	//is the primary device so that we can coordinate with the main execution thread.
	DeviceContext* primaryDevice = (primarySharedExecuteThreadDevice)? this: otherSharedExecuteThreadDevice;
	DeviceContext* secondaryDevice = (!primarySharedExecuteThreadDevice)? this: otherSharedExecuteThreadDevice;

	//Set the name of this thread for the benefit of an attached debugger
	std::wstring debuggerThreadName = L"DCExeSec - \"" + primaryDevice->device.GetDeviceInstanceName() + L"\" and \"" + secondaryDevice->device.GetDeviceInstanceName() + L"\"";
	SetCallingThreadName(debuggerThreadName);

	//Process execute requests from the primary execution thread until this thread is
	//requested to terminate
	while(!primaryDevice->sharedExecuteThreadSpinoffStopRequested)
	{
		//Wait for a new command to be received from the primary execution thread
		boost::mutex::scoped_lock primaryDeviceLock(primaryDevice->executeThreadMutex);
		while(!primaryDevice->sharedExecuteThreadSpinoffStopRequested && (!primaryDevice->sharedExecuteThreadSpinoffActive || (primaryDevice->sharedExecuteThreadSpinoffActive && (!primaryDevice->sharedExecuteThreadSpinoffTimesliceAvailable || primaryDevice->sharedExecuteThreadSpinoffRejoinRequested))))
		{
			//If our spinoff thread has been requested to rejoin the main execution
			//thread, flag that this spinoff thread is no longer active.
			if(primaryDevice->sharedExecuteThreadSpinoffRejoinRequested)
			{
				primaryDevice->sharedExecuteThreadSpinoffRejoinRequested = false;
				primaryDevice->sharedExecuteThreadSpinoffActive = false;
			}

			//Flag that the spinoff thread is currently paused, and Wait for a new command
			//to be received from the main execution thread.
			primaryDevice->sharedExecuteThreadSpinoffPaused = true;
			primaryDevice->sharedExecuteThreadSpinoffStoppedOrPaused.notify_all();
			primaryDevice->sharedExecuteThreadSpinoffStateChangeRequested.wait(primaryDeviceLock);
		}

		//If this spinoff execution thread has been requested to terminate, restart the
		//loop to process the request.
		if(primaryDevice->sharedExecuteThreadSpinoffStopRequested)
		{
			continue;
		}

		//Since we've received a new timeslice notification from the main execution
		//thread, flag that this spinoff execution thread is not paused, and notify the
		//main execution thread we have begun processing this timeslice.
		primaryDevice->sharedExecuteThreadSpinoffPaused = false;
		primaryDevice->sharedExecuteThreadSpinoffTimesliceAvailable = false;
		primaryDevice->sharedExecuteThreadSpinoffTimesliceProcessingBegun.notify_all();
		primaryDeviceLock.unlock();

		//Advance the device state for the device currently assigned to execute within
		//this spinoff thread, until the end of the current timeslice is reached, our our
		//spinoff thread is requested to rejoin the main execution thread.
		DeviceContext* spinoffThreadTargetDevice = (primaryDevice->currentSharedExecuteThreadOwner == this)? otherSharedExecuteThreadDevice: this;
		unsigned int dependentTargetCount = (unsigned int)spinoffThreadTargetDevice->deviceDependencies.size();
		while(!primaryDevice->sharedExecuteThreadSpinoffRejoinRequested && (spinoffThreadTargetDevice->currentTimesliceProgress < spinoffThreadTargetDevice->timeslice))
		{
			for(unsigned int i = 0; i < dependentTargetCount; ++i)
			{
				while((spinoffThreadTargetDevice->currentTimesliceProgress > spinoffThreadTargetDevice->deviceDependencies[i].device->currentTimesliceProgress) && spinoffThreadTargetDevice->deviceDependencies[i].dependencyEnabled)
				{
					Sleep(0);
				}
			}
			spinoffThreadTargetDevice->currentTimesliceProgress += spinoffThreadTargetDevice->device.ExecuteStep();
			if(spinoffThreadTargetDevice->systemObject.IsSystemRollbackFlagged())
			{
				if(spinoffThreadTargetDevice->currentTimesliceProgress >= spinoffThreadTargetDevice->systemObject.SystemRollbackTime())
				{
					spinoffThreadTargetDevice->currentTimesliceProgress = spinoffThreadTargetDevice->timeslice;
				}
			}
		}

		//If our spinoff thread has been requested to rejoin the main execution thread,
		//restart the loop in order to process the request.
		if(primaryDevice->sharedExecuteThreadSpinoffRejoinRequested)
		{
			continue;
		}

		//Since we've reached the end of the current timeslice for the device assigned to
		//this thread, set the remainingTime variable, and call
		//NotifyAfterExecuteStepFinishedTimeslice for the assigned device.
		spinoffThreadTargetDevice->remainingTime = spinoffThreadTargetDevice->currentTimesliceProgress - spinoffThreadTargetDevice->timeslice;
		spinoffThreadTargetDevice->device.NotifyAfterExecuteStepFinishedTimeslice();

		//Notify the command thread that this execute command has been completed for the
		//device currently executing in this spinoff thread
		primaryDeviceLock.lock();
		spinoffThreadTargetDevice->timesliceSuspended = false;
		spinoffThreadTargetDevice->timesliceCompleted = true;
		spinoffThreadTargetDevice->executeCompletionStateChanged.notify_all();
	}

	//Notify the main thread that this spinoff execution thread has terminated
	boost::mutex::scoped_lock primaryDeviceLock(primaryDevice->executeThreadMutex);
	primaryDevice->sharedExecuteThreadSpinoffRunning = false;
	primaryDevice->sharedExecuteThreadSpinoffStoppedOrPaused.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadTimeslice()
{
	boost::mutex::scoped_lock lock(executeThreadMutex);
	executeThreadRunningState = true;
	executeThreadReady.notify_all();
	executeTaskSent.wait(lock);
	while(executeWorkerThreadActive)
	{
		lock.unlock();
		device.ExecuteTimeslice(timeslice);
		remainingTime = 0;
		currentTimesliceProgress = timeslice;
		device.NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		timesliceSuspended = false;
		timesliceCompleted = true;
		executeCompletionStateChanged.notify_all();
		executeTaskSent.wait(lock);
	}
	executeThreadRunningState = false;
	executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadTimesliceWithDependencies()
{
	boost::mutex::scoped_lock lock(executeThreadMutex);
	executeThreadRunningState = true;
	executeThreadReady.notify_all();
	executeTaskSent.wait(lock);
	while(executeWorkerThreadActive)
	{
		lock.unlock();
		device.ExecuteTimeslice(timeslice);
		remainingTime = 0;
		currentTimesliceProgress = timeslice;
		device.NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		unsigned int dependentTargetCount = (unsigned int)deviceDependencies.size();
		for(unsigned int i = 0; i < dependentTargetCount; ++i)
		{
			if(deviceDependencies[i].dependencyEnabled)
			{
				deviceDependencies[i].device->WaitForCompletion();
			}
		}

		timesliceSuspended = false;
		timesliceCompleted = true;
		executeCompletionStateChanged.notify_all();
		executeTaskSent.wait(lock);
	}
	executeThreadRunningState = false;
	executeThreadStopped.notify_all();
}
