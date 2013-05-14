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
//Device/System interface
//----------------------------------------------------------------------------------------
IDevice* DeviceContext::GetTargetDevice() const
{
	return device;
}

//----------------------------------------------------------------------------------------
//System message functions
//----------------------------------------------------------------------------------------
void DeviceContext::SetSystemRollback(IDeviceContext* atriggerDevice, IDeviceContext* arollbackDevice, double timeslice, unsigned int accessContext, void (*callbackFunction)(void*), void* callbackParams)
{
	systemObject->SetSystemRollback(atriggerDevice, arollbackDevice, timeslice, accessContext, callbackFunction, callbackParams);
}

//----------------------------------------------------------------------------------------
void DeviceContext::WriteLogEvent(const ILogEntry& entry)
{
	//Ensure that the source of the event is correctly set for the calling device
	std::wstring eventLogSource = GetTargetDevice()->GetDeviceInstanceName() + L" [" + GetTargetDevice()->GetModuleDisplayName() + L"]";
	entry.OverrideSourceText(eventLogSource);

	//Pass the event log message on to the system
	systemObject->WriteLogEvent(entry);
}

//----------------------------------------------------------------------------------------
void DeviceContext::FlagStopSystem()
{
	systemObject->FlagStopSystem();
}

//----------------------------------------------------------------------------------------
void DeviceContext::StopSystem()
{
	systemObject->StopSystem();
}

//----------------------------------------------------------------------------------------
void DeviceContext::RunSystem()
{
	systemObject->RunSystem();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteDeviceStep()
{
	systemObject->ExecuteDeviceStep(this);
}

//----------------------------------------------------------------------------------------
const wchar_t* DeviceContext::GetCapturePathInternal() const
{
	capturePathCached = GetCapturePath();
	return capturePathCached.c_str();
}

//----------------------------------------------------------------------------------------
const wchar_t* DeviceContext::GetFullyQualifiedDeviceInstanceNameInternal() const
{
	fullyQualifiedDeviceInstanceNameCached = GetFullyQualifiedDeviceInstanceName();
	return fullyQualifiedDeviceInstanceNameCached.c_str();
}

//----------------------------------------------------------------------------------------
const wchar_t* DeviceContext::GetModuleDisplayNameInternal() const
{
	moduleDisplayNameCached = GetModuleDisplayName();
	return moduleDisplayNameCached.c_str();
}

//----------------------------------------------------------------------------------------
const wchar_t* DeviceContext::GetModuleInstanceNameInternal() const
{
	moduleInstanceNameCached = GetModuleInstanceName();
	return moduleInstanceNameCached.c_str();
}

//----------------------------------------------------------------------------------------
//Suspend functions
//----------------------------------------------------------------------------------------
bool DeviceContext::UsesExecuteSuspend() const
{
	return device->UsesExecuteSuspend();
}

//----------------------------------------------------------------------------------------
bool DeviceContext::UsesTransientExecution() const
{
	return device->UsesTransientExecution();
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
	ReleaseAssert(device->UsesExecuteSuspend());

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
	unsigned int dependentTargetCount = (unsigned int)deviceDependencies.size();
	unsigned int i = 0;
	while(i < dependentTargetCount)
	{
		if(deviceDependencies[i].device == targetDevice)
		{
			deviceDependencies[i].dependencyEnabled = state;
		}
		++i;
	}
}

//----------------------------------------------------------------------------------------
//Input functions
//----------------------------------------------------------------------------------------
bool DeviceContext::TranslateKeyCode(unsigned int platformKeyCode, KeyCode& inputKeyCode)
{
	return systemObject->TranslateKeyCode(platformKeyCode, inputKeyCode);
}

//----------------------------------------------------------------------------------------
bool DeviceContext::TranslateJoystickButton(unsigned int joystickNo, unsigned int buttonNo, KeyCode& inputKeyCode)
{
	return systemObject->TranslateJoystickButton(joystickNo, buttonNo, inputKeyCode);
}

//----------------------------------------------------------------------------------------
bool DeviceContext::TranslateJoystickAxisAsButton(unsigned int joystickNo, unsigned int axisNo, bool positiveAxis, KeyCode& inputKeyCode)
{
	return systemObject->TranslateJoystickAxisAsButton(joystickNo, axisNo, positiveAxis, inputKeyCode);
}

//----------------------------------------------------------------------------------------
bool DeviceContext::TranslateJoystickAxis(unsigned int joystickNo, unsigned int axisNo, AxisCode& inputAxisCode)
{
	return systemObject->TranslateJoystickAxis(joystickNo, axisNo, inputAxisCode);
}

//----------------------------------------------------------------------------------------
void DeviceContext::HandleInputKeyDown(KeyCode keyCode) const
{
	systemObject->HandleInputKeyDown(keyCode);
}

//----------------------------------------------------------------------------------------
void DeviceContext::HandleInputKeyUp(KeyCode keyCode) const
{
	systemObject->HandleInputKeyUp(keyCode);
}

//----------------------------------------------------------------------------------------
void DeviceContext::HandleInputAxisUpdate(AxisCode axisCode, float newValue)
{
	systemObject->HandleInputAxisUpdate(axisCode, newValue);
}

//----------------------------------------------------------------------------------------
void DeviceContext::HandleInputScrollUpdate(ScrollCode scrollCode, int scrollTicks)
{
	systemObject->HandleInputScrollUpdate(scrollCode, scrollTicks);
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
	std::wstring debuggerThreadName = L"DCCommand - " + device->GetDeviceInstanceName();
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
		device->BeginExecution();

		//Start the execution thread, and wait for confirmation that it is ready to
		//receive commands.
		executeWorkerThreadActive = true;
		boost::thread workerThread(boost::bind(boost::mem_fn(&DeviceContext::ExecuteWorkerThread), this));
		executeThreadReady.wait(lock);
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
		executeTaskSent.notify_all();
		executeThreadStopped.wait(lock);

		//Notify the device that execution is being suspended
		device->SuspendExecution();
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThread()
{
	//Set the name of this thread for the debugger
	std::wstring debuggerThreadName = L"DCExecute - " + device->GetDeviceInstanceName();
	SetCallingThreadName(debuggerThreadName);

	if(device->GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP)
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
	else if(device->GetUpdateMethod() == IDevice::UPDATEMETHOD_TIMESLICE)
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
	executeThreadReady.notify_all();
	executeTaskSent.wait(lock);
	while(executeWorkerThreadActive)
	{
		lock.unlock();
		currentTimesliceProgress = remainingTime;
		while(currentTimesliceProgress < timeslice)
		{
			currentTimesliceProgress += device->ExecuteStep();
			if(systemObject->IsSystemRollbackFlagged())
			{
				if(currentTimesliceProgress >= systemObject->SystemRollbackTime())
				{
					currentTimesliceProgress = timeslice;
				}
			}
		}
		remainingTime = currentTimesliceProgress - timeslice;
		device->NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		timesliceSuspended = false;
		timesliceCompleted = true;
		executeCompletionStateChanged.notify_all();
		executeTaskSent.wait(lock);
	}
	executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadStepWithDependencies()
{
	boost::mutex::scoped_lock lock(executeThreadMutex);
	executeThreadReady.notify_all();
	executeTaskSent.wait(lock);
	while(executeWorkerThreadActive)
	{
		lock.unlock();
		currentTimesliceProgress = remainingTime;
		unsigned int dependentTargetCount = (unsigned int)deviceDependencies.size();
		while(currentTimesliceProgress < timeslice)
		{
			for(unsigned int i = 0; i < dependentTargetCount; ++i)
			{
				//SafeMemoryBarrierRead();
				while((currentTimesliceProgress > deviceDependencies[i].device->GetCurrentTimesliceProgress()) && deviceDependencies[i].dependencyEnabled)
				{
					Sleep(0);
					//SafeMemoryBarrierRead();
				}
			}
			currentTimesliceProgress += device->ExecuteStep();
			if(systemObject->IsSystemRollbackFlagged())
			{
				if(currentTimesliceProgress >= systemObject->SystemRollbackTime())
				{
					currentTimesliceProgress = timeslice;
				}
			}
			//SafeMemoryBarrierWrite();
		}
		remainingTime = currentTimesliceProgress - timeslice;
		device->NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		timesliceSuspended = false;
		timesliceCompleted = true;
		executeCompletionStateChanged.notify_all();
		executeTaskSent.wait(lock);
	}
	executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadTimeslice()
{
	boost::mutex::scoped_lock lock(executeThreadMutex);
	executeThreadReady.notify_all();
	executeTaskSent.wait(lock);
	while(executeWorkerThreadActive)
	{
		lock.unlock();
		currentTimesliceProgress = 0;
		device->ExecuteTimeslice(timeslice);
		remainingTime = 0;
		currentTimesliceProgress = timeslice;
		device->NotifyAfterExecuteStepFinishedTimeslice();
		lock.lock();

		timesliceSuspended = false;
		timesliceCompleted = true;
		executeCompletionStateChanged.notify_all();
		executeTaskSent.wait(lock);
	}
	executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteWorkerThreadTimesliceWithDependencies()
{
	boost::mutex::scoped_lock lock(executeThreadMutex);
	executeThreadReady.notify_all();
	executeTaskSent.wait(lock);
	while(executeWorkerThreadActive)
	{
		lock.unlock();
		currentTimesliceProgress = 0;
		device->ExecuteTimeslice(timeslice);
		remainingTime = 0;
		currentTimesliceProgress = timeslice;
		device->NotifyAfterExecuteStepFinishedTimeslice();
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
	executeThreadStopped.notify_all();
}
