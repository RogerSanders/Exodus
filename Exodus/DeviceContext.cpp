#include "DeviceContext.h"

//----------------------------------------------------------------------------------------
//Timing functions
//----------------------------------------------------------------------------------------
double DeviceContext::GetCurrentTimesliceProgress() const
{
	return currentTimesliceProgress;
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
void DeviceContext::SetSystemRollback(IDeviceContext* atriggerDevice, IDeviceContext* arollbackDevice, double timeslice, void (*callbackFunction)(void*), void* callbackParams)
{
	systemObject->SetSystemRollback(atriggerDevice, arollbackDevice, timeslice, callbackFunction, callbackParams);
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

	//Characters
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
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Worker thread control
//----------------------------------------------------------------------------------------
void DeviceContext::BeginExecution()
{
	boost::mutex::scoped_lock lock(accessMutex);
	if(!active && (device->GetUpdateMethod() != IDevice::UPDATEMETHOD_NONE))
	{
		active = true;
		boost::thread workerThread(boost::bind(boost::mem_fn(&DeviceContext::WorkerThread), this));
		executeThreadReady.wait(lock);
		device->BeginExecution();
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::SuspendExecution()
{
	boost::mutex::scoped_lock lock(accessMutex);
	if(active)
	{
		active = false;
		stateChanged.notify_all();
		device->SuspendExecution();
		executeThreadStopped.wait(lock);
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::WorkerThread()
{
	if(device->GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP)
	{
		if(dependentTargets.empty())
		{
			WorkerThreadStep();
		}
		else
		{
			WorkerThreadStepWithDependents();
		}
	}
	else if(device->GetUpdateMethod() == IDevice::UPDATEMETHOD_TIMESLICE)
	{
		WorkerThreadTimeslice();
	}
}

//----------------------------------------------------------------------------------------
void DeviceContext::WorkerThreadStep()
{
	boost::mutex::scoped_lock lock(accessMutex);
	executeThreadReady.notify_all();
	stateChanged.wait(lock);
	while(active)
	{
		currentTimesliceProgress = remainingTime;
		while(currentTimesliceProgress < timeslice)
		{
			currentTimesliceProgress += device->ExecuteStep();
		}
		remainingTime = currentTimesliceProgress - timeslice;

		timesliceCompleted = true;
		justCompletedTimeslice.notify_all();
		stateChanged.wait(lock);
	}
	executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::WorkerThreadStepWithDependents()
{
	boost::mutex::scoped_lock lock(accessMutex);
	executeThreadReady.notify_all();
	stateChanged.wait(lock);
	unsigned int dependentTargetCount = (unsigned int)dependentTargets.size();
	while(active)
	{
		currentTimesliceProgress = remainingTime;
		while(currentTimesliceProgress < timeslice)
		{
			for(unsigned int i = 0; i < dependentTargetCount; ++i)
			{
				while(currentTimesliceProgress > dependentTargets[i]->GetCurrentTimesliceProgress())
				{
					Sleep(0);
				}
			}
			currentTimesliceProgress += device->ExecuteStep();
		}
		remainingTime = currentTimesliceProgress - timeslice;

		timesliceCompleted = true;
		justCompletedTimeslice.notify_all();
		stateChanged.wait(lock);
	}
	executeThreadStopped.notify_all();
}

//----------------------------------------------------------------------------------------
void DeviceContext::WorkerThreadTimeslice()
{
	boost::mutex::scoped_lock lock(accessMutex);
	executeThreadReady.notify_all();
	stateChanged.wait(lock);
	while(active)
	{
		currentTimesliceProgress = 0;
		device->ExecuteTimeslice(timeslice);
		remainingTime = 0;
		currentTimesliceProgress = timeslice;

		timesliceCompleted = true;
		justCompletedTimeslice.notify_all();
		stateChanged.wait(lock);
	}
	executeThreadStopped.notify_all();
}
