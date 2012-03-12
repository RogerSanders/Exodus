#include "WindowFunctions/WindowFunctions.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DeviceContext::DeviceContext(IDevice* adevice)
:device(adevice), systemObject(0), active(false), deviceEnabled(true), dependentTargets(0)
{}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void DeviceContext::NotifyUpcomingTimeslice(double nanoseconds)
{
	device->NotifyUpcomingTimeslice(nanoseconds);
	currentTimesliceProgress = remainingTime;
}

//----------------------------------------------------------------------------------------
void DeviceContext::NotifyBeforeExecuteCalled()
{
	device->NotifyBeforeExecuteCalled();
}

//----------------------------------------------------------------------------------------
void DeviceContext::NotifyAfterExecuteCalled()
{
	device->NotifyAfterExecuteCalled();
}

//----------------------------------------------------------------------------------------
void DeviceContext::ExecuteTimeslice(double nanoseconds)
{
	boost::mutex::scoped_lock lock(accessMutex);
	timeslice = nanoseconds;
	timesliceCompleted = false;
	stateChanged.notify_all();
}

//----------------------------------------------------------------------------------------
double DeviceContext::ExecuteStep()
{
	boost::mutex::scoped_lock lock(accessMutex);
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
	boost::mutex::scoped_lock lock(accessMutex);
	while(!timesliceCompleted)
	{
		justCompletedTimeslice.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::Commit()
{
	boost::mutex::scoped_lock lock(accessMutex);
	remainingTimeBackup = remainingTime;
	device->ExecuteCommit();
}

//----------------------------------------------------------------------------------------
void DeviceContext::Rollback()
{
	boost::mutex::scoped_lock lock(accessMutex);
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
	boost::mutex::scoped_lock lock(accessMutex);
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
bool DeviceContext::SendNotifyUpcomingTimeslice() const
{
	return device->SendNotifyUpcomingTimeslice();
}

//----------------------------------------------------------------------------------------
bool DeviceContext::SendNotifyBeforeExecuteCalled() const
{
	return device->SendNotifyBeforeExecuteCalled();
}

//----------------------------------------------------------------------------------------
bool DeviceContext::SendNotifyAfterExecuteCalled() const
{
	return device->SendNotifyAfterExecuteCalled();
}

//----------------------------------------------------------------------------------------
//Dependent device functions
//----------------------------------------------------------------------------------------
void DeviceContext::AddDependentDevice(DeviceContext* targetDevice)
{
	//This little catch is just to prevent the same device being added to the list
	//multiple times.
	RemoveDependentDevice(targetDevice);

	//Add the device to the dependent device list
	dependentTargets.push_back(targetDevice);
}

//----------------------------------------------------------------------------------------
void DeviceContext::RemoveDependentDevice(DeviceContext* targetDevice)
{
	bool done = false;
	std::vector<DeviceContext*>::iterator i = dependentTargets.begin();
	while(!done && (i != dependentTargets.end()))
	{
		if(*i == targetDevice)
		{
			dependentTargets.erase(i);
			done = true;
			continue;
		}
		++i;
	}
}
