//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExecutionManager::ExecutionManager()
:totalDeviceCount(0), deviceCount(0), suspendDeviceCount(0), transientDeviceCount(0)
{}

//----------------------------------------------------------------------------------------
//Device functions
//----------------------------------------------------------------------------------------
void ExecutionManager::AddDevice(DeviceContext* device)
{
	//Add the specified device to the device arrays
	deviceArray.push_back(device);
	if(device->UsesExecuteSuspend())
	{
		suspendDeviceArray.push_back(device);
	}
	if(device->UsesTransientExecution())
	{
		transientDeviceArray.push_back(device);
	}

	//Update the device counts
	deviceCount = deviceArray.size();
	suspendDeviceCount = suspendDeviceArray.size();
	transientDeviceCount = transientDeviceArray.size();
	totalDeviceCount = (ReferenceCounterType)deviceCount;

	//Resize the message results array
	command.timesliceResult.resize(deviceCount);
	command.contextResult.resize(deviceCount);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::RemoveDevice(DeviceContext* device)
{
	bool done;
	std::vector<DeviceContext*>::iterator i;

	//Remove the specified device from the device array
	done = false;
	i = deviceArray.begin();
	while(!done && (i != deviceArray.end()))
	{
		if(*i == device)
		{
			deviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	//Remove the specified device from the suspend device array
	done = false;
	i = suspendDeviceArray.begin();
	while(!done && (i != suspendDeviceArray.end()))
	{
		if(*i == device)
		{
			suspendDeviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	//Remove the specified device from the transient device array
	done = false;
	i = transientDeviceArray.begin();
	while(!done && (i != transientDeviceArray.end()))
	{
		if(*i == device)
		{
			transientDeviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	//Update the device counts
	deviceCount = deviceArray.size();
	suspendDeviceCount = suspendDeviceArray.size();
	transientDeviceCount = transientDeviceArray.size();
	totalDeviceCount = (ReferenceCounterType)deviceCount;

	//Resize the message results array
	command.timesliceResult.resize(deviceCount);
	command.contextResult.resize(deviceCount);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::ClearAllDevices()
{
	//Clear all device arrays
	deviceArray.clear();
	suspendDeviceArray.clear();
	transientDeviceArray.clear();
	command.timesliceResult.clear();
	command.contextResult.clear();

	//Initialize the device counts
	deviceCount = 0;
	suspendDeviceCount = 0;
	totalDeviceCount = 0;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void ExecutionManager::NotifyUpcomingTimeslice(double nanoseconds)
{
	std::unique_lock<std::mutex> lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_NOTIFYUPCOMINGTIMESLICE;
	command.timeslice = nanoseconds;
	if(totalDeviceCount > 0)
	{
		pendingDeviceCount = totalDeviceCount;
		commandSent.notify_all();
		commandProcessed.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::NotifyBeforeExecuteCalled()
{
	std::unique_lock<std::mutex> lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_NOTIFYBEFOREEXECUTECALLED;
	pendingDeviceCount = totalDeviceCount;
	if(totalDeviceCount > 0)
	{
		commandSent.notify_all();
		commandProcessed.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::NotifyAfterExecuteCalled()
{
	std::unique_lock<std::mutex> lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_NOTIFYAFTEREXECUTECALLED;
	pendingDeviceCount = totalDeviceCount;
	if(totalDeviceCount > 0)
	{
		commandSent.notify_all();
		commandProcessed.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::ExecuteTimeslice(double nanoseconds)
{
	std::unique_lock<std::mutex> lock(commandMutex);

	//Enable execution suspend features for devices that support it
	EnableTimesliceExecutionSuspend();

	//Start all devices executing the new timeslice
	command.type = DeviceContext::DeviceContextCommand::TYPE_EXECUTETIMESLICE;
	command.timeslice = nanoseconds;
	suspendedThreadCount = 0;
	if(totalDeviceCount > 0)
	{
		pendingDeviceCount = totalDeviceCount;
		commandSent.notify_all();
		commandProcessed.wait(lock);
	}

	//Wait for all devices to finish executing the timeslice
	command.type = DeviceContext::DeviceContextCommand::TYPE_WAITFOREXECUTECOMPLETE;
	if(totalDeviceCount > 0)
	{
		pendingDeviceCount = totalDeviceCount;
		commandSent.notify_all();
		commandProcessed.wait(lock);
	}

	//Disable execution suspend features for devices that support it. Note that execution
	//suspend may be disabled automatically before the timeslice is completed if all
	//remaining devices are in the suspend state.
	DisableTimesliceExecutionSuspend();
}

//----------------------------------------------------------------------------------------
void ExecutionManager::Commit()
{
	std::unique_lock<std::mutex> lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_COMMIT;
	if(totalDeviceCount > 0)
	{
		pendingDeviceCount = totalDeviceCount;
		commandSent.notify_all();
		commandProcessed.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::Rollback()
{
	std::unique_lock<std::mutex> lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_ROLLBACK;
	if(totalDeviceCount > 0)
	{
		pendingDeviceCount = totalDeviceCount;
		commandSent.notify_all();
		commandProcessed.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::Initialize()
{
	//Initialize the devices. Note that we only perform initialization on a device when
	//execution for that device is suspended. This is critical, in order to ensure that
	//the device does not have any actively running threads, such as render threads, which
	//might be working with the internal state data we are about to initialize. As the
	//device is currently suspended, we can't send this command in parallel to each
	//device, as the command worker thread for each device is also suspended, so we
	//initialize each device serially on the one thread here.
	for(size_t i = 0; i < deviceCount; ++i)
	{
		deviceArray[i]->Initialize();
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::AssertCurrentOutputLineState()
{
	for(size_t i = 0; i < deviceCount; ++i)
	{
		deviceArray[i]->GetTargetDevice().AssertCurrentOutputLineState();
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::NegateCurrentOutputLineState()
{
	for(size_t i = 0; i < deviceCount; ++i)
	{
		deviceArray[i]->GetTargetDevice().NegateCurrentOutputLineState();
	}
}

//----------------------------------------------------------------------------------------
//Timing functions
//----------------------------------------------------------------------------------------
double ExecutionManager::GetNextTimingPoint(double maximumTimeslice, DeviceContext*& nextDeviceStep, unsigned int& nextDeviceStepContext)
{
	std::unique_lock<std::mutex> lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_GETNEXTTIMINGPOINT;
	if(totalDeviceCount > 0)
	{
		pendingDeviceCount = totalDeviceCount;
		commandSent.notify_all();
		commandProcessed.wait(lock);
	}

	//Determine the maximum length of time all devices can run unsynchronized before the
	//next timing point
	double timeslice = maximumTimeslice;
	nextDeviceStep = 0;
	for(size_t i = 0; i < deviceCount; ++i)
	{
		double deviceTimingPoint = command.timesliceResult[i];
		if((deviceTimingPoint < timeslice) && (deviceTimingPoint >= 0))
		{
			timeslice = deviceTimingPoint;
			nextDeviceStep = deviceArray[i];
			nextDeviceStepContext = command.contextResult[i];
		}
	}

	return timeslice;
}

//----------------------------------------------------------------------------------------
//Worker thread control
//----------------------------------------------------------------------------------------
void ExecutionManager::BeginExecution()
{
	pendingDeviceCount = totalDeviceCount;
	for(size_t i = 0; i < deviceCount; ++i)
	{
		deviceArray[i]->BeginExecution(i, pendingDeviceCount, suspendedThreadCount, commandMutex, commandSent, commandProcessed, this, command);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::SuspendExecution()
{
	std::unique_lock<std::mutex> lock(commandMutex);
	command.type = DeviceContext::DeviceContextCommand::TYPE_SUSPENDEXECUTION;
	if(totalDeviceCount > 0)
	{
		pendingDeviceCount = totalDeviceCount;
		commandSent.notify_all();
		commandProcessed.wait(lock);
	}
}
