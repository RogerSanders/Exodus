#include "DeviceContext.h"
#include "ThreadLib/ThreadLib.pkg"
#include "Debug/Debug.pkg"

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
	capturePathCached = systemObject->GetCapturePath();
	return capturePathCached.c_str();
}

//----------------------------------------------------------------------------------------
const wchar_t* DeviceContext::GetModuleDisplayNameInternal() const
{
	systemObject->GetModuleDisplayName(device->GetDeviceModuleID(), moduleDisplayNameCached);
	return moduleDisplayNameCached.c_str();
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
	if(!state)
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
bool DeviceContext::TranslateKeyCode(unsigned int platformKeyCode, KeyCode& inputKeyCode)
{
	switch(platformKeyCode)
	{
	//Control keys
	case VK_ESCAPE:
		inputKeyCode = KEYCODE_ESCAPE;
		return true;
	case VK_TAB:
		inputKeyCode = KEYCODE_TAB;
		return true;
	case VK_RETURN:
		inputKeyCode = KEYCODE_ENTER;
		return true;
	case VK_SPACE:
		inputKeyCode = KEYCODE_SPACE;
		return true;
	case VK_BACK:
		inputKeyCode = KEYCODE_BACKSPACE;
		return true;
	case VK_INSERT:
		inputKeyCode = KEYCODE_INSERT;
		return true;
	case VK_DELETE:
		inputKeyCode = KEYCODE_DELETE;
		return true;
	case VK_PRIOR:
		inputKeyCode = KEYCODE_PAGEUP;
		return true;
	case VK_NEXT:
		inputKeyCode = KEYCODE_PAGEDOWN;
		return true;
	case VK_HOME:
		inputKeyCode = KEYCODE_HOME;
		return true;
	case VK_END:
		inputKeyCode = KEYCODE_END;
		return true;
	case VK_UP:
		inputKeyCode = KEYCODE_UP;
		return true;
	case VK_DOWN:
		inputKeyCode = KEYCODE_DOWN;
		return true;
	case VK_LEFT:
		inputKeyCode = KEYCODE_LEFT;
		return true;
	case VK_RIGHT:
		inputKeyCode = KEYCODE_RIGHT;
		return true;
	case VK_SNAPSHOT:
		inputKeyCode = KEYCODE_PRINTSCREEN;
		return true;
	case VK_PAUSE:
		inputKeyCode = KEYCODE_PAUSE;
		return true;
	case VK_NUMLOCK:
		inputKeyCode = KEYCODE_NUMLOCK;
		return true;
	case VK_CAPITAL:
		inputKeyCode = KEYCODE_CAPSLOCK;
		return true;
	case VK_SCROLL:
		inputKeyCode = KEYCODE_SCROLLLOCK;
		return true;
	case VK_LWIN:
		inputKeyCode = KEYCODE_LEFTWINDOWS;
		return true;
	case VK_RWIN:
		inputKeyCode = KEYCODE_RIGHTWINDOWS;
		return true;
	case VK_APPS:
		inputKeyCode = KEYCODE_MENU;
		return true;

	//Modifier keys
	case VK_CONTROL:
		inputKeyCode = KEYCODE_CTRL;
		return true;
	case VK_MENU:
		inputKeyCode = KEYCODE_ALT;
		return true;
	case VK_SHIFT:
		inputKeyCode = KEYCODE_SHIFT;
		return true;

	//Function keys
	case VK_F1:
		inputKeyCode = KEYCODE_F1;
		return true;
	case VK_F2:
		inputKeyCode = KEYCODE_F2;
		return true;
	case VK_F3:
		inputKeyCode = KEYCODE_F3;
		return true;
	case VK_F4:
		inputKeyCode = KEYCODE_F4;
		return true;
	case VK_F5:
		inputKeyCode = KEYCODE_F5;
		return true;
	case VK_F6:
		inputKeyCode = KEYCODE_F6;
		return true;
	case VK_F7:
		inputKeyCode = KEYCODE_F7;
		return true;
	case VK_F8:
		inputKeyCode = KEYCODE_F8;
		return true;
	case VK_F9:
		inputKeyCode = KEYCODE_F9;
		return true;
	case VK_F10:
		inputKeyCode = KEYCODE_F10;
		return true;
	case VK_F11:
		inputKeyCode = KEYCODE_F11;
		return true;
	case VK_F12:
		inputKeyCode = KEYCODE_F12;
		return true;

	//Numbers
	case '0':
		inputKeyCode = KEYCODE_0;
		return true;
	case '1':
		inputKeyCode = KEYCODE_1;
		return true;
	case '2':
		inputKeyCode = KEYCODE_2;
		return true;
	case '3':
		inputKeyCode = KEYCODE_3;
		return true;
	case '4':
		inputKeyCode = KEYCODE_4;
		return true;
	case '5':
		inputKeyCode = KEYCODE_5;
		return true;
	case '6':
		inputKeyCode = KEYCODE_6;
		return true;
	case '7':
		inputKeyCode = KEYCODE_7;
		return true;
	case '8':
		inputKeyCode = KEYCODE_8;
		return true;
	case '9':
		inputKeyCode = KEYCODE_9;
		return true;

	//Letters
	case 'A':
		inputKeyCode = KEYCODE_A;
		return true;
	case 'B':
		inputKeyCode = KEYCODE_B;
		return true;
	case 'C':
		inputKeyCode = KEYCODE_C;
		return true;
	case 'D':
		inputKeyCode = KEYCODE_D;
		return true;
	case 'E':
		inputKeyCode = KEYCODE_E;
		return true;
	case 'F':
		inputKeyCode = KEYCODE_F;
		return true;
	case 'G':
		inputKeyCode = KEYCODE_G;
		return true;
	case 'H':
		inputKeyCode = KEYCODE_H;
		return true;
	case 'I':
		inputKeyCode = KEYCODE_I;
		return true;
	case 'J':
		inputKeyCode = KEYCODE_J;
		return true;
	case 'K':
		inputKeyCode = KEYCODE_K;
		return true;
	case 'L':
		inputKeyCode = KEYCODE_L;
		return true;
	case 'M':
		inputKeyCode = KEYCODE_M;
		return true;
	case 'N':
		inputKeyCode = KEYCODE_N;
		return true;
	case 'O':
		inputKeyCode = KEYCODE_O;
		return true;
	case 'P':
		inputKeyCode = KEYCODE_P;
		return true;
	case 'Q':
		inputKeyCode = KEYCODE_Q;
		return true;
	case 'R':
		inputKeyCode = KEYCODE_R;
		return true;
	case 'S':
		inputKeyCode = KEYCODE_S;
		return true;
	case 'T':
		inputKeyCode = KEYCODE_T;
		return true;
	case 'U':
		inputKeyCode = KEYCODE_U;
		return true;
	case 'V':
		inputKeyCode = KEYCODE_V;
		return true;
	case 'W':
		inputKeyCode = KEYCODE_W;
		return true;
	case 'X':
		inputKeyCode = KEYCODE_X;
		return true;
	case 'Y':
		inputKeyCode = KEYCODE_Y;
		return true;
	case 'Z':
		inputKeyCode = KEYCODE_Z;
		return true;

	//Symbol keys
	case VK_OEM_1:
		inputKeyCode = KEYCODE_OEM1;
		return true;
	case VK_OEM_PLUS:
		inputKeyCode = KEYCODE_OEMPLUS;
		return true;
	case VK_OEM_COMMA:
		inputKeyCode = KEYCODE_OEMCOMMA;
		return true;
	case VK_OEM_MINUS:
		inputKeyCode = KEYCODE_OEMMINUS;
		return true;
	case VK_OEM_PERIOD:
		inputKeyCode = KEYCODE_OEMPERIOD;
		return true;
	case VK_OEM_2:
		inputKeyCode = KEYCODE_OEM2;
		return true;
	case VK_OEM_3:
		inputKeyCode = KEYCODE_OEM3;
		return true;
	case VK_OEM_4:
		inputKeyCode = KEYCODE_OEM4;
		return true;
	case VK_OEM_5:
		inputKeyCode = KEYCODE_OEM5;
		return true;
	case VK_OEM_6:
		inputKeyCode = KEYCODE_OEM6;
		return true;
	case VK_OEM_7:
		inputKeyCode = KEYCODE_OEM7;
		return true;
	case VK_OEM_8:
		inputKeyCode = KEYCODE_OEM8;
		return true;
	case VK_OEM_AX:
		inputKeyCode = KEYCODE_OEMAX;
		return true;
	case VK_OEM_102:
		inputKeyCode = KEYCODE_OEM102;
		return true;

	//Numpad keys
	case VK_NUMPAD0:
		inputKeyCode = KEYCODE_NUMPAD0;
		return true;
	case VK_NUMPAD1:
		inputKeyCode = KEYCODE_NUMPAD1;
		return true;
	case VK_NUMPAD2:
		inputKeyCode = KEYCODE_NUMPAD2;
		return true;
	case VK_NUMPAD3:
		inputKeyCode = KEYCODE_NUMPAD3;
		return true;
	case VK_NUMPAD4:
		inputKeyCode = KEYCODE_NUMPAD4;
		return true;
	case VK_NUMPAD5:
		inputKeyCode = KEYCODE_NUMPAD5;
		return true;
	case VK_NUMPAD6:
		inputKeyCode = KEYCODE_NUMPAD6;
		return true;
	case VK_NUMPAD7:
		inputKeyCode = KEYCODE_NUMPAD7;
		return true;
	case VK_NUMPAD8:
		inputKeyCode = KEYCODE_NUMPAD8;
		return true;
	case VK_NUMPAD9:
		inputKeyCode = KEYCODE_NUMPAD9;
		return true;
	case VK_MULTIPLY:
		inputKeyCode = KEYCODE_NUMPADMULTIPLY;
		return true;
	case VK_DIVIDE:
		inputKeyCode = KEYCODE_NUMPADDIVIDE;
		return true;
	case VK_SUBTRACT:
		inputKeyCode = KEYCODE_NUMPADSUBTRACT;
		return true;
	case VK_ADD:
		inputKeyCode = KEYCODE_NUMPADADD;
		return true;
	case VK_DECIMAL:
		inputKeyCode = KEYCODE_NUMPADDECIMAL;
		return true;
	}
	return false;
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
				while((currentTimesliceProgress > deviceDependencies[i].device->GetCurrentTimesliceProgress()) && deviceDependencies[i].dependencyEnabled)
				{
					Sleep(0);
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
