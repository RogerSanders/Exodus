//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExecutionManager::ExecutionManager()
:_totalDeviceCount(0), _deviceCount(0), _suspendDeviceCount(0), _transientDeviceCount(0)
{}

//----------------------------------------------------------------------------------------
//Device functions
//----------------------------------------------------------------------------------------
void ExecutionManager::AddDevice(DeviceContext* device)
{
	//Add the specified device to the device arrays
	_deviceArray.push_back(device);
	if(device->UsesExecuteSuspend())
	{
		_suspendDeviceArray.push_back(device);
	}
	if(device->UsesTransientExecution())
	{
		_transientDeviceArray.push_back(device);
	}

	//Update the device counts
	_deviceCount = _deviceArray.size();
	_suspendDeviceCount = _suspendDeviceArray.size();
	_transientDeviceCount = _transientDeviceArray.size();
	_totalDeviceCount = (ReferenceCounterType)_deviceCount;

	//Resize the message results array
	_command.timesliceResult.resize(_deviceCount);
	_command.contextResult.resize(_deviceCount);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::RemoveDevice(DeviceContext* device)
{
	bool done;
	std::vector<DeviceContext*>::iterator i;

	//Remove the specified device from the device array
	done = false;
	i = _deviceArray.begin();
	while(!done && (i != _deviceArray.end()))
	{
		if(*i == device)
		{
			_deviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	//Remove the specified device from the suspend device array
	done = false;
	i = _suspendDeviceArray.begin();
	while(!done && (i != _suspendDeviceArray.end()))
	{
		if(*i == device)
		{
			_suspendDeviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	//Remove the specified device from the transient device array
	done = false;
	i = _transientDeviceArray.begin();
	while(!done && (i != _transientDeviceArray.end()))
	{
		if(*i == device)
		{
			_transientDeviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	//Update the device counts
	_deviceCount = _deviceArray.size();
	_suspendDeviceCount = _suspendDeviceArray.size();
	_transientDeviceCount = _transientDeviceArray.size();
	_totalDeviceCount = (ReferenceCounterType)_deviceCount;

	//Resize the message results array
	_command.timesliceResult.resize(_deviceCount);
	_command.contextResult.resize(_deviceCount);
}

//----------------------------------------------------------------------------------------
void ExecutionManager::ClearAllDevices()
{
	//Clear all device arrays
	_deviceArray.clear();
	_suspendDeviceArray.clear();
	_transientDeviceArray.clear();
	_command.timesliceResult.clear();
	_command.contextResult.clear();

	//Initialize the device counts
	_deviceCount = 0;
	_suspendDeviceCount = 0;
	_totalDeviceCount = 0;
}

//----------------------------------------------------------------------------------------
//Execute functions
//----------------------------------------------------------------------------------------
void ExecutionManager::NotifyUpcomingTimeslice(double nanoseconds)
{
	std::unique_lock<std::mutex> lock(_commandMutex);
	_command.type = DeviceContext::DeviceContextCommand::TYPE_NOTIFYUPCOMINGTIMESLICE;
	_command.timeslice = nanoseconds;
	if(_totalDeviceCount > 0)
	{
		_pendingDeviceCount = _totalDeviceCount;
		_commandSent.notify_all();
		_commandProcessed.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::NotifyBeforeExecuteCalled()
{
	std::unique_lock<std::mutex> lock(_commandMutex);
	_command.type = DeviceContext::DeviceContextCommand::TYPE_NOTIFYBEFOREEXECUTECALLED;
	_pendingDeviceCount = _totalDeviceCount;
	if(_totalDeviceCount > 0)
	{
		_commandSent.notify_all();
		_commandProcessed.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::NotifyAfterExecuteCalled()
{
	std::unique_lock<std::mutex> lock(_commandMutex);
	_command.type = DeviceContext::DeviceContextCommand::TYPE_NOTIFYAFTEREXECUTECALLED;
	_pendingDeviceCount = _totalDeviceCount;
	if(_totalDeviceCount > 0)
	{
		_commandSent.notify_all();
		_commandProcessed.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::ExecuteTimeslice(double nanoseconds)
{
	std::unique_lock<std::mutex> lock(_commandMutex);

	//Enable execution suspend features for devices that support it
	EnableTimesliceExecutionSuspend();

	//Start all devices executing the new timeslice
	_command.type = DeviceContext::DeviceContextCommand::TYPE_EXECUTETIMESLICE;
	_command.timeslice = nanoseconds;
	_suspendedThreadCount = 0;
	if(_totalDeviceCount > 0)
	{
		_pendingDeviceCount = _totalDeviceCount;
		_commandSent.notify_all();
		_commandProcessed.wait(lock);
	}

	//Wait for all devices to finish executing the timeslice
	_command.type = DeviceContext::DeviceContextCommand::TYPE_WAITFOREXECUTECOMPLETE;
	if(_totalDeviceCount > 0)
	{
		_pendingDeviceCount = _totalDeviceCount;
		_commandSent.notify_all();
		_commandProcessed.wait(lock);
	}

	//Disable execution suspend features for devices that support it. Note that execution
	//suspend may be disabled automatically before the timeslice is completed if all
	//remaining devices are in the suspend state.
	DisableTimesliceExecutionSuspend();
}

//----------------------------------------------------------------------------------------
void ExecutionManager::Commit()
{
	std::unique_lock<std::mutex> lock(_commandMutex);
	_command.type = DeviceContext::DeviceContextCommand::TYPE_COMMIT;
	if(_totalDeviceCount > 0)
	{
		_pendingDeviceCount = _totalDeviceCount;
		_commandSent.notify_all();
		_commandProcessed.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::Rollback()
{
	std::unique_lock<std::mutex> lock(_commandMutex);
	_command.type = DeviceContext::DeviceContextCommand::TYPE_ROLLBACK;
	if(_totalDeviceCount > 0)
	{
		_pendingDeviceCount = _totalDeviceCount;
		_commandSent.notify_all();
		_commandProcessed.wait(lock);
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
	for(size_t i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->Initialize();
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::AssertCurrentOutputLineState()
{
	for(size_t i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->GetTargetDevice().AssertCurrentOutputLineState();
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::NegateCurrentOutputLineState()
{
	for(size_t i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->GetTargetDevice().NegateCurrentOutputLineState();
	}
}

//----------------------------------------------------------------------------------------
//Timing functions
//----------------------------------------------------------------------------------------
double ExecutionManager::GetNextTimingPoint(double maximumTimeslice, DeviceContext*& nextDeviceStep, unsigned int& nextDeviceStepContext)
{
	std::unique_lock<std::mutex> lock(_commandMutex);
	_command.type = DeviceContext::DeviceContextCommand::TYPE_GETNEXTTIMINGPOINT;
	if(_totalDeviceCount > 0)
	{
		_pendingDeviceCount = _totalDeviceCount;
		_commandSent.notify_all();
		_commandProcessed.wait(lock);
	}

	//Determine the maximum length of time all devices can run unsynchronized before the
	//next timing point
	double timeslice = maximumTimeslice;
	nextDeviceStep = 0;
	for(size_t i = 0; i < _deviceCount; ++i)
	{
		double deviceTimingPoint = _command.timesliceResult[i];
		if((deviceTimingPoint < timeslice) && (deviceTimingPoint >= 0))
		{
			timeslice = deviceTimingPoint;
			nextDeviceStep = _deviceArray[i];
			nextDeviceStepContext = _command.contextResult[i];
		}
	}

	return timeslice;
}

//----------------------------------------------------------------------------------------
//Worker thread control
//----------------------------------------------------------------------------------------
void ExecutionManager::BeginExecution()
{
	_pendingDeviceCount = _totalDeviceCount;
	for(size_t i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->BeginExecution(i, _pendingDeviceCount, _suspendedThreadCount, _commandMutex, _commandSent, _commandProcessed, this, _command);
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::SuspendExecution()
{
	std::unique_lock<std::mutex> lock(_commandMutex);
	_command.type = DeviceContext::DeviceContextCommand::TYPE_SUSPENDEXECUTION;
	if(_totalDeviceCount > 0)
	{
		_pendingDeviceCount = _totalDeviceCount;
		_commandSent.notify_all();
		_commandProcessed.wait(lock);
	}
}
