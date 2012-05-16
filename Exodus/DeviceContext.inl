#include "WindowFunctions/WindowFunctions.pkg"

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct DeviceContext::DeviceContextCommand
{
public:
	//Enumerations
	enum Type
	{
		TYPE_SUSPENDEXECUTION,
		TYPE_COMMIT,
		TYPE_ROLLBACK,
		TYPE_GETNEXTTIMINGPOINT,
		TYPE_NOTIFYUPCOMINGTIMESLICE,
		TYPE_NOTIFYBEFOREEXECUTECALLED,
		TYPE_NOTIFYAFTEREXECUTECALLED,
		TYPE_EXECUTETIMESLICE,
		TYPE_WAITFOREXECUTECOMPLETE,
		TYPE_RUNSUSPENDEDEXECUTETOCOMPLETION,
	};

public:
	//Data members
	Type type;
	double timeslice;
	mutable std::vector<double> timesliceResult;
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DeviceContext::DeviceContext(IDevice* adevice)
:device(adevice), systemObject(0), executeWorkerThreadActive(false), commandWorkerThreadActive(false), deviceEnabled(true), deviceDependencies(0), suspendedThreadCountPointer(0), remainingThreadCountPointer(0), commandMutexPointer(0), suspendManager(0), transientExecutionActive(false)
{}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void DeviceContext::NotifyUpcomingTimeslice(double nanoseconds)
{
	if(device->SendNotifyUpcomingTimeslice())
	{
		device->NotifyUpcomingTimeslice(nanoseconds);
		//##TODO## Figure out why this is here. I'm pretty sure this is completely
		//redundant, and should be removed.
		currentTimesliceProgress = remainingTime;
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::NotifyBeforeExecuteCalled()
{
	if(device->SendNotifyBeforeExecuteCalled())
	{
		device->NotifyBeforeExecuteCalled();
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::NotifyAfterExecuteCalled()
{
	if(device->SendNotifyAfterExecuteCalled())
	{
		device->NotifyAfterExecuteCalled();
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteTimeslice(double nanoseconds)
{
	timeslice = nanoseconds;
	timesliceCompleted = false;
	timesliceSuspended = false;
	executeTaskSent.notify_all();
}

//----------------------------------------------------------------------------------------
double DeviceContext::ExecuteStep()
{
	double additionalTime = 0;

	if(device->GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP)
	{
		remainingTime += device->ExecuteStep();
		additionalTime = remainingTime;
	}
	else if(device->GetUpdateMethod() == IDevice::UPDATEMETHOD_TIMESLICE)
	{
		device->ExecuteTimeslice(0);
	}

	return additionalTime;
}

//----------------------------------------------------------------------------------------
void DeviceContext::WaitForCompletion()
{
	boost::mutex::scoped_lock executeLock(executeThreadMutex);
	while(!timesliceCompleted)
	{
		executeCompletionStateChanged.wait(executeLock);
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::WaitForCompletionAndDetectSuspendLock(volatile ReferenceCounterType& suspendedThreadCount, volatile ReferenceCounterType& remainingThreadCount, boost::mutex& commandMutex, IExecutionSuspendManager* asuspendManager)
{
	boost::mutex::scoped_lock executeLock(executeThreadMutex);
	while(!timesliceCompleted)
	{
		if(!timesliceSuspended || timesliceSuspensionDisable)
		{
			//If this execution thread isn't suspended, we need to wait for it to either
			//finish the current timeslice, or enter a suspended state.
			executeCompletionStateChanged.wait(executeLock);
		}
		else
		{
			//This is a little bit complicated. We need to ensure we take our locks in the
			//correct order, to prevent deadlock cases. At any point where we need to
			//obtain a lock on both executeThreadMutex and commandMutex, we must take the
			//lock on commandMutex first, followed by executeThreadMutex. In this case
			//though, we need to test resources for our loop condition above that require
			//us to take a lock on executeThreadMutex. In order to make this whole thing
			//work we do a lot of manual locking and unlocking below, to ensure the locks
			//are always taken in the correct order, and that the correct locks are taken
			//at all times in order to work with each resource.
			executeLock.unlock();

			//Obtain a lock on the shared command mutex. We need to do this so that we can
			//safely work with the result of the suspendedThreadCount variable.
			boost::mutex::scoped_lock commandLock(commandMutex);

			//Evaluate whether all remaining threads are suspended. If they are, release
			//all suspended threads. If not, wait for the completion state of this execute
			//thread to change.
			if(asuspendManager->AllDevicesSuspended(suspendedThreadCount, remainingThreadCount))
			{
				//Note that we need to release executeThreadMutex here, since the suspend
				//manager will call back into this DeviceContext object and call the
				//DisableTimesliceExecutionSuspend() function in response to the function
				//call below. Taking the lock again is technically not necessary at this
				//point, but we do it anyway for the sake of uniformity.
				asuspendManager->DisableTimesliceExecutionSuspend();
			}
			else
			{
				//Note that we need to release the command lock here, since the execute
				//completion state of this device may never change until we disable
				//execute suspension. We can't hold command mutex that whole time,
				//otherwise other threads will deadlock. We also need to re-obtain the
				//execute mutex, so that we can safely trigger the condition below without
				//opening up potential deadlock cases due to other threads missing the
				//condition.
				commandLock.unlock();
				executeLock.lock();
				//##FIX## At this point, we would need to test for all devices being
				//suspended again in order to avoid deadlocks, and this problem would
				//persist recursively. We need to change our locking structure so that we
				//can guarantee that our test above for all devices being suspended is
				//still valid at this point, otherwise we would have to avoid the lock
				//here and actively loop until this timeslice leaves the suspended state,
				//consuming valuble CPU cycles.
				if(!timesliceCompleted)
				{
					executeCompletionStateChanged.wait(executeLock);
				}
				executeLock.unlock();
				commandLock.lock();
			}

			//Re-obtain the execute lock so that we can test the loop condition again
			executeLock.lock();
		}
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::Commit()
{
	remainingTimeBackup = remainingTime;
	device->ExecuteCommit();
}

//----------------------------------------------------------------------------------------
void DeviceContext::Rollback()
{
	remainingTime = remainingTimeBackup;
	device->ExecuteRollback();
}

//----------------------------------------------------------------------------------------
void DeviceContext::Initialize()
{
	device->Initialize();
	remainingTime = 0;
	currentTimesliceProgress = 0;
}

//----------------------------------------------------------------------------------------
//Timing functions
//----------------------------------------------------------------------------------------
double DeviceContext::GetNextTimingPoint() const
{
	double result = -1;

	if(device->GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP)
	{
		result = device->GetNextTimingPointInDeviceTime();
		if(result >= 0)
		{
			result += remainingTime;
		}
	}
	else if(device->GetUpdateMethod() == IDevice::UPDATEMETHOD_TIMESLICE)
	{
		result = device->GetNextTimingPointInDeviceTime();
	}

	return result;
}

//----------------------------------------------------------------------------------------
double DeviceContext::GetCurrentRemainingTime() const
{
	return remainingTime;
}

//----------------------------------------------------------------------------------------
double DeviceContext::GetInitialRemainingTime() const
{
	return remainingTimeBackup;
}

//----------------------------------------------------------------------------------------
void DeviceContext::ClearRemainingTime()
{
	remainingTime = 0;
}

//----------------------------------------------------------------------------------------
//Device/System interface
//----------------------------------------------------------------------------------------
void DeviceContext::SetSystemInterface(ISystemInternal* asystemObject)
{
	systemObject = asystemObject;
}

//----------------------------------------------------------------------------------------
bool DeviceContext::ActiveDevice() const
{
	return (GetTargetDevice()->GetUpdateMethod() != IDevice::UPDATEMETHOD_NONE);
}

//----------------------------------------------------------------------------------------
//Dependent device functions
//----------------------------------------------------------------------------------------
void DeviceContext::AddDeviceDependency(DeviceContext* targetDevice)
{
	//This little catch is just to prevent the same device being added to the list
	//multiple times.
	RemoveDeviceDependency(targetDevice);

	//Add the device to the device dependency list
	deviceDependencies.push_back(targetDevice);

	//Notify the target device that this device has added it as a dependency
	targetDevice->AddDependentDevice(this);
}

//----------------------------------------------------------------------------------------
void DeviceContext::RemoveDeviceDependency(DeviceContext* targetDevice)
{
	bool done = false;
	std::vector<DeviceContext*>::iterator i = deviceDependencies.begin();
	while(!done && (i != deviceDependencies.end()))
	{
		if(*i == targetDevice)
		{
			//Notify the target device that this device has removed it as a dependency
			targetDevice->RemoveDependentDevice(this);

			//Remove the device from the device dependency list
			deviceDependencies.erase(i);
			done = true;
			continue;
		}
		++i;
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::AddDependentDevice(DeviceContext* targetDevice)
{
	RemoveDependentDevice(targetDevice);

	dependentDevices.push_back(targetDevice);
}

//----------------------------------------------------------------------------------------
void DeviceContext::RemoveDependentDevice(DeviceContext* targetDevice)
{
	bool done = false;
	std::vector<DeviceContext*>::iterator i = dependentDevices.begin();
	while(!done && (i != dependentDevices.end()))
	{
		if(*i == targetDevice)
		{
			dependentDevices.erase(i);
			done = true;
			continue;
		}
		++i;
	}
}

//----------------------------------------------------------------------------------------
const std::vector<DeviceContext*>& DeviceContext::GetDeviceDependencyArray() const
{
	return deviceDependencies;
}

//----------------------------------------------------------------------------------------
const std::vector<DeviceContext*>& DeviceContext::GetDependentDeviceArray() const
{
	return dependentDevices;
}
