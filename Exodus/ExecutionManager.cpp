#include "ExecutionManager.h"

//----------------------------------------------------------------------------------------
//Suspend functions
//----------------------------------------------------------------------------------------
void ExecutionManager::EnableTimesliceExecutionSuspend()
{
	//Go through each device that supports suspension and enable the suspend feature
	for(size_t i = 0; i < suspendDeviceCount; ++i)
	{
		DeviceContext* device = suspendDeviceArray[i];
		device->EnableTimesliceExecutionSuspend();
	}
}

//----------------------------------------------------------------------------------------
void ExecutionManager::DisableTimesliceExecutionSuspend()
{
	//Go through each device that supports suspension and disable the suspend feature
	for(size_t i = 0; i < suspendDeviceCount; ++i)
	{
		DeviceContext* device = suspendDeviceArray[i];
		device->DisableTimesliceExecutionSuspend();
	}
}

//----------------------------------------------------------------------------------------
bool ExecutionManager::AllDevicesSuspended(volatile ReferenceCounterType& suspendedThreadCount, volatile ReferenceCounterType& remainingThreadCount) const
{
	//Go through each device that supports transient execution, and see if any of them are
	//currently executing.
	for(size_t transientDeviceNo = 0; transientDeviceNo < transientDeviceCount; ++transientDeviceNo)
	{
		const DeviceContext* transientDevice = transientDeviceArray[transientDeviceNo];
		if(!transientDevice->TimesliceExecutionSuspended() && transientDevice->TransientExecutionActive())
		{
			return false;
		}
	}

	//Go through each device that supports suspension and build a set of dependent devices
	//that are still executing, not suspended, and not finished their timeslice.
	std::set<const DeviceContext*> dependentDeviceSet;
	for(size_t suspendedDeviceNo = 0; suspendedDeviceNo < suspendDeviceCount; ++suspendedDeviceNo)
	{
		const DeviceContext* suspendedDevice = suspendDeviceArray[suspendedDeviceNo];
		if(suspendedDevice->TimesliceExecutionSuspended())
		{
			//Recursively iterate through each directly or indirectly dependent device for
			//the suspended device, and confirm which devices are currently blocked by it
			//being suspended.
			BuildBlockedDependentDeviceSet(suspendedDevice, dependentDeviceSet);
		}
	}

	//Calculate the total number of devices that are blocked or suspended
	unsigned int blockedDeviceCount = (unsigned int)suspendedThreadCount + (unsigned int)dependentDeviceSet.size();

	//Determine if all remaining devices are blocked or suspended, and return the result
	//to the caller.
	bool allDevicesBlocked = ((unsigned int)remainingThreadCount == blockedDeviceCount);
	return allDevicesBlocked;
}

//----------------------------------------------------------------------------------------
void ExecutionManager::BuildBlockedDependentDeviceSet(const DeviceContext* sourceDevice, std::set<const DeviceContext*>& dependentDeviceSet) const
{
	const std::vector<DeviceContext*>& dependentDeviceArray = sourceDevice->GetDependentDeviceArray();
	size_t dependentDeviceArraySize = dependentDeviceArray.size();
	for(size_t i = 0; i < dependentDeviceArraySize; ++i)
	{
		const DeviceContext* dependentDevice = dependentDeviceArray[i];
		if(!dependentDevice->TimesliceExecutionSuspended() && !dependentDevice->TimesliceExecutionCompleted())
		{
			dependentDeviceSet.insert(dependentDevice);
			BuildBlockedDependentDeviceSet(dependentDevice, dependentDeviceSet);
		}
	}
}
