//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ExecutionManager::ExecutionManager()
:_deviceCount(0), _activeDeviceCount(0), _suspendDeviceCount(0), _transientDeviceCount(0)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Device functions
//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::AddDevice(DeviceContext* device)
{
	// Add the specified device to the device arrays
	_deviceArray.push_back(device);
	if (device->ActiveDevice())
	{
		_activeDeviceArray.push_back(device);
	}
	if (device->UsesExecuteSuspend())
	{
		_suspendDeviceArray.push_back(device);
	}
	if (device->UsesTransientExecution())
	{
		_transientDeviceArray.push_back(device);
	}

	// Update the device counts
	_deviceCount = (unsigned int)_deviceArray.size();
	_activeDeviceCount = (unsigned int)_activeDeviceArray.size();
	_suspendDeviceCount = (unsigned int)_suspendDeviceArray.size();
	_transientDeviceCount = (unsigned int)_transientDeviceArray.size();

	// Resize the message results array
	_nextTimesliceValues.resize(_deviceCount);
	_nextTimesliceContextValues.resize(_deviceCount);
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::RemoveDevice(DeviceContext* device)
{
	bool done;
	std::vector<DeviceContext*>::iterator i;

	// Remove the specified device from the device array
	done = false;
	i = _deviceArray.begin();
	while (!done && (i != _deviceArray.end()))
	{
		if (*i == device)
		{
			_deviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	// Remove the specified device from the active device array
	done = false;
	i = _activeDeviceArray.begin();
	while (!done && (i != _activeDeviceArray.end()))
	{
		if (*i == device)
		{
			_activeDeviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	// Remove the specified device from the suspend device array
	done = false;
	i = _suspendDeviceArray.begin();
	while (!done && (i != _suspendDeviceArray.end()))
	{
		if (*i == device)
		{
			_suspendDeviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	// Remove the specified device from the transient device array
	done = false;
	i = _transientDeviceArray.begin();
	while (!done && (i != _transientDeviceArray.end()))
	{
		if (*i == device)
		{
			_transientDeviceArray.erase(i);
			done = true;
		}
		else
		{
			++i;
		}
	}

	// Update the device counts
	_deviceCount = (unsigned int)_deviceArray.size();
	_activeDeviceCount = (unsigned int)_activeDeviceArray.size();
	_suspendDeviceCount = (unsigned int)_suspendDeviceArray.size();
	_transientDeviceCount = (unsigned int)_transientDeviceArray.size();

	// Resize the message results array
	_nextTimesliceValues.resize(_deviceCount);
	_nextTimesliceContextValues.resize(_deviceCount);
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::ClearAllDevices()
{
	// Clear all device arrays
	_deviceArray.clear();
	_activeDeviceArray.clear();
	_suspendDeviceArray.clear();
	_transientDeviceArray.clear();
	_nextTimesliceValues.clear();
	_nextTimesliceContextValues.clear();

	// Initialize the device counts
	_deviceCount = 0;
	_activeDeviceCount = 0;
	_suspendDeviceCount = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Execute functions
//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::NotifyUpcomingTimeslice(double nanoseconds)
{
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->NotifyUpcomingTimeslice(nanoseconds);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::NotifyBeforeExecuteCalled()
{
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->NotifyBeforeExecuteCalled();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::NotifyAfterExecuteCalled()
{
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->NotifyAfterExecuteCalled();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::ExecuteTimeslice(double nanoseconds)
{
	// Enable execution suspend features for devices that support it
	EnableTimesliceExecutionSuspend();

	// Start all devices executing the new timeslice
	std::atomic<unsigned int> executingThreadCount(_activeDeviceCount);
	std::atomic<unsigned int> suspendedThreadCount(0);
	for (unsigned int i = 0; i < _activeDeviceCount; ++i)
	{
		_activeDeviceArray[i]->BeginExecuteTimeslice(nanoseconds, &executingThreadCount, &suspendedThreadCount, this);
	}

	// Wait for all devices to finish executing the timeslice
	for (size_t i = 0; i < _activeDeviceCount; ++i)
	{
		_activeDeviceArray[i]->WaitForCompletion();
	}

	// Disable execution suspend features for devices that support it. Note that execution
	// suspend may be disabled automatically before the timeslice is completed if all
	// remaining devices are in the suspend state.
	DisableTimesliceExecutionSuspend();
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::Commit()
{
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->Commit();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::Rollback()
{
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->Rollback();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::Initialize()
{
	// Initialize the devices. Note that we only perform initialization on a device when
	// execution for that device is suspended. This is critical, in order to ensure that
	// the device does not have any actively running threads, such as render threads, which
	// might be working with the internal state data we are about to initialize. As the
	// device is currently suspended, we can't send this command in parallel to each
	// device, as the command worker thread for each device is also suspended, so we
	// initialize each device serially on the one thread here.
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->Initialize();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::AssertCurrentOutputLineState()
{
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->GetTargetDevice().AssertCurrentOutputLineState();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::NegateCurrentOutputLineState()
{
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		_deviceArray[i]->GetTargetDevice().NegateCurrentOutputLineState();
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Timing functions
//----------------------------------------------------------------------------------------------------------------------
double ExecutionManager::GetNextTimingPoint(double maximumTimeslice, DeviceContext*& nextDeviceStep, unsigned int& nextDeviceStepContext)
{
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		_nextTimesliceContextValues[i] = 0;
		_nextTimesliceValues[i] = _deviceArray[i]->GetNextTimingPoint(_nextTimesliceContextValues[i]);
	}

	// Determine the maximum length of time all devices can run unsynchronized before the
	// next timing point
	double timeslice = maximumTimeslice;
	nextDeviceStep = 0;
	for (unsigned int i = 0; i < _deviceCount; ++i)
	{
		double deviceTimingPoint = _nextTimesliceValues[i];
		if ((deviceTimingPoint < timeslice) && (deviceTimingPoint >= 0))
		{
			timeslice = deviceTimingPoint;
			nextDeviceStep = _deviceArray[i];
			nextDeviceStepContext = _nextTimesliceContextValues[i];
		}
	}

	return timeslice;
}

//----------------------------------------------------------------------------------------------------------------------
// Worker thread control
//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::StartExecution()
{
	for (unsigned int i = 0; i < _activeDeviceCount; ++i)
	{
		_activeDeviceArray[i]->StartExecution();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ExecutionManager::StopExecution()
{
	std::vector<std::thread> threads;
	threads.reserve(_activeDeviceCount);
	for (unsigned int i = 0; i < _activeDeviceCount; ++i)
	{
		threads.emplace_back(std::thread([&] { _activeDeviceArray[i]->StopExecution(); }));
	}
	for (unsigned int i = 0; i < _activeDeviceCount; ++i)
	{
		threads[i].join();
	}
}
