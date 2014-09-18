#include "InputMappingDetailsView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
InputMappingDetailsView::InputMappingDetailsView(IUIManager& auiManager, InputMappingDetailsViewPresenter& apresenter, ISystemGUIInterface& amodel, IDevice* atargetDevice)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), systemKeyInputFieldInFocus(false), targetDevice(atargetDevice), joystickWorkerThreadActive(false)
{
	hwndInternal = NULL;
	autoKeyMappingActive = false;
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_INPUTMAPPING_DETAILS));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Target device functions
//----------------------------------------------------------------------------------------
void InputMappingDetailsView::SetTargetDevice(IDevice* atargetDevice)
{
	targetDevice = atargetDevice;
	if(hwndInternal != NULL)
	{
		UpdateTargetDeviceInputMappingsDisplay(hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR InputMappingDetailsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_BOUNCE:
		return msgWM_BOUNCE(hwnd, wparam, lparam);
	case WM_USER:
		return msgWM_USER(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR InputMappingDetailsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	hwndInternal = hwnd;
	selectedInputRegistration = -1;
	UpdateTargetDeviceInputMappingsDisplay(hwndInternal);
	initializedDialog = true;

	SetWindowSubclass(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_AUTOMAPPINGTOGGLE), BounceBackSubclassProc, 0, 0);

	//Start the joystick worker thread
	boost::mutex::scoped_lock lock(joystickWorkerThreadMutex);
	joystickWorkerThreadActive = true;
	boost::thread workerThread(boost::bind(boost::mem_fn(&InputMappingDetailsView::JoystickKeyMappingWorkerThread), this));

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR InputMappingDetailsView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Stop the joystick worker thread
	boost::mutex::scoped_lock lock(joystickWorkerThreadMutex);
	joystickWorkerThreadActive = false;
	joystickWorkerThreadStopped.wait(lock);

	return FALSE;
}

//----------------------------------------------------------------------------------------
//Entry update functions
//----------------------------------------------------------------------------------------
void InputMappingDetailsView::UpdateTargetDeviceInputMappingsDisplay(HWND hwnd)
{
	//Build a string uniquely identifying this input device in the system
	std::wstring deviceNameString;
	model.GetFullyQualifiedDeviceDisplayName(targetDevice, deviceNameString);

	//Update the device name field
	UpdateDlgItemString(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICENAME, deviceNameString);

	//Obtain the current set of registered inputs for the target device
	std::list<unsigned int> deviceKeyCodeList = model.GetDeviceKeyCodeList(targetDevice);
	deviceKeyCodeMap = std::vector<unsigned int>(deviceKeyCodeList.begin(), deviceKeyCodeList.end());

	//Update the list of device keys
	SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_RESETCONTENT, 0, 0);
	for(unsigned int i = 0; i < deviceKeyCodeMap.size(); ++i)
	{
		unsigned int deviceKeyCode = deviceKeyCodeMap[i];
		std::wstring deviceKeyCodeString = targetDevice->GetKeyCodeName(deviceKeyCode);
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_ADDSTRING, 0, (LPARAM)deviceKeyCodeString.c_str());
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_SETITEMDATA, newItemIndex, (LPARAM)i);
	}

	//Update the list of system keys
	SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_RESETCONTENT, 0, 0);
	for(unsigned int i = (unsigned int)ISystemDeviceInterface::KEYCODE_NONE; i < (unsigned int)ISystemDeviceInterface::KEYCODE_ENDOFLIST; ++i)
	{
		ISystemDeviceInterface::KeyCode systemKeyCode = (ISystemDeviceInterface::KeyCode)i;
		std::wstring systemKeyCodeString = model.GetKeyCodeName(systemKeyCode);
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_ADDSTRING, 0, (LPARAM)systemKeyCodeString.c_str());
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_SETITEMDATA, newItemIndex, (LPARAM)systemKeyCode);
	}

	//Turn off the auto key mapping mode
	autoKeyMappingActive = false;

	//Initialize the display state of the selected device input mappings
	selectedInputRegistration = 0;
	UpdateSelectedInputRegistration(hwnd);
}

//----------------------------------------------------------------------------------------
void InputMappingDetailsView::UpdateSelectedInputRegistration(HWND hwnd)
{
	bool autoMappingTextSet = false;
	if(selectedInputRegistration >= 0)
	{
		unsigned int deviceKeyCode = deviceKeyCodeMap[selectedInputRegistration];
		ISystemDeviceInterface::KeyCode systemKeyCode = model.GetDeviceKeyCodeMapping(targetDevice, deviceKeyCode);
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_SETCURSEL, (WPARAM)selectedInputRegistration, 0);
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_SETCURSEL, (WPARAM)((unsigned int)systemKeyCode - (unsigned int)ISystemDeviceInterface::KEYCODE_NONE), 0);

		if(autoKeyMappingActive)
		{
			std::wstring autoMappingText = L"Press the button for ";
			autoMappingText += targetDevice->GetKeyCodeName(deviceKeyCodeMap[selectedInputRegistration]);
			UpdateDlgItemString(hwnd, IDC_INPUTMAPPING_DETAILS_AUTOMAPPINGTEXT, autoMappingText);
			autoMappingTextSet = true;
		}
	}
	else
	{
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_SETCURSEL, (WPARAM)-1, 0);
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_SETCURSEL, (WPARAM)-1, 0);
	}
	if(!autoMappingTextSet)
	{
		UpdateDlgItemString(hwnd, IDC_INPUTMAPPING_DETAILS_AUTOMAPPINGTEXT, L"Auto mapping will prompt you to press a key for each input sequentially.");
	}
}

//----------------------------------------------------------------------------------------
INT_PTR InputMappingDetailsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_INPUTMAPPING_DETAILS_AUTOMAPPINGTOGGLE:
			if(!autoKeyMappingActive)
			{
				autoKeyMappingActive = true;
				UpdateDlgItemString(hwnd, IDC_INPUTMAPPING_DETAILS_AUTOMAPPINGTOGGLE, L"Stop Auto Mapping");
				UpdateSelectedInputRegistration(hwnd);
			}
			else
			{
				autoKeyMappingActive = false;
				UpdateDlgItemString(hwnd, IDC_INPUTMAPPING_DETAILS_AUTOMAPPINGTOGGLE, L"Start Auto Mapping");
				UpdateSelectedInputRegistration(hwnd);
			}
			break;
		case IDC_INPUTMAPPING_DETAILS_CLEARALL:
			for(unsigned int i = 0; i < deviceKeyCodeMap.size(); ++i)
			{
				model.SetDeviceKeyCodeMapping(targetDevice, deviceKeyCodeMap[i], ISystemDeviceInterface::KEYCODE_NONE);
			}
			UpdateSelectedInputRegistration(hwnd);
			break;
		case IDC_INPUTMAPPING_DETAILS_RESTOREDEFAULTS:
			for(unsigned int i = 0; i < deviceKeyCodeMap.size(); ++i)
			{
				std::list<ISystemGUIInterface::KeyCode> preferredDefaultKeyCodeList = model.GetDeviceKeyCodePreferredDefaultMappingList(targetDevice, deviceKeyCodeMap[i]);
				ISystemGUIInterface::KeyCode defaultKeyCode = (!preferredDefaultKeyCodeList.empty())? preferredDefaultKeyCodeList.front(): ISystemDeviceInterface::KEYCODE_NONE;
				model.SetDeviceKeyCodeMapping(targetDevice, deviceKeyCodeMap[i], defaultKeyCode);
			}
			UpdateSelectedInputRegistration(hwnd);
			break;
		}
	}
	else if((HIWORD(wparam) == CBN_SELCHANGE) && initializedDialog)
	{
		switch(LOWORD(wparam))
		{
		case IDC_INPUTMAPPING_DETAILS_DEVICEKEY:{
			int itemIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_GETCURSEL, 0, 0);
			selectedInputRegistration = (unsigned int)SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_GETITEMDATA, itemIndex, NULL);
			UpdateSelectedInputRegistration(hwnd);
			break;}
		case IDC_INPUTMAPPING_DETAILS_SYSTEMKEY:{
			if(selectedInputRegistration >= 0)
			{
				int itemIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_GETCURSEL, 0, 0);
				ISystemDeviceInterface::KeyCode systemKeyCode = (ISystemDeviceInterface::KeyCode)SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_GETITEMDATA, itemIndex, NULL);
				model.SetDeviceKeyCodeMapping(targetDevice, deviceKeyCodeMap[selectedInputRegistration], systemKeyCode);
			}
			break;}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR InputMappingDetailsView::msgWM_BOUNCE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lparam;
	int controlID = GetDlgCtrlID(bounceMessage->hwnd);
	if((controlID == IDC_INPUTMAPPING_DETAILS_SYSTEMKEY) && initializedDialog)
	{
		switch(bounceMessage->uMsg)
		{
		case WM_SETFOCUS:
			systemKeyInputFieldInFocus = true;
			break;
		case WM_KILLFOCUS:
			systemKeyInputFieldInFocus = false;
			break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			ISystemDeviceInterface::KeyCode systemKeyCode;
			if(model.TranslateKeyCode((unsigned int)bounceMessage->wParam, systemKeyCode))
			{
				SendMessage(hwnd, WM_USER, (WPARAM)systemKeyCode, 0);
			}
			bounceMessage->caught = true;
			break;
		case WM_CHAR:
			bounceMessage->caught = true;
			break;
		}
	}
	else if((controlID == IDC_INPUTMAPPING_DETAILS_AUTOMAPPINGTOGGLE) && initializedDialog)
	{
		switch(bounceMessage->uMsg)
		{
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if(autoKeyMappingActive)
			{
				ISystemDeviceInterface::KeyCode systemKeyCode;
				if(model.TranslateKeyCode((unsigned int)bounceMessage->wParam, systemKeyCode))
				{
					SendMessage(hwnd, WM_USER, (WPARAM)systemKeyCode, 0);
				}
				bounceMessage->caught = true;
			}
			break;
		case WM_CHAR:
			if(autoKeyMappingActive)
			{
				bounceMessage->caught = true;
			}
			break;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR InputMappingDetailsView::msgWM_USER(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if(autoKeyMappingActive || systemKeyInputFieldInFocus)
	{
		ISystemDeviceInterface::KeyCode systemKeyCode = (ISystemDeviceInterface::KeyCode)wParam;
		model.SetDeviceKeyCodeMapping(targetDevice, deviceKeyCodeMap[selectedInputRegistration], systemKeyCode);
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_SETCURSEL, (WPARAM)((unsigned int)systemKeyCode - (unsigned int)ISystemDeviceInterface::KEYCODE_NONE), 0);

		if(autoKeyMappingActive)
		{
			if(selectedInputRegistration < ((int)deviceKeyCodeMap.size() - 1))
			{
				++selectedInputRegistration;
			}
			else
			{
				selectedInputRegistration = 0;
				autoKeyMappingActive = false;
				UpdateDlgItemString(hwnd, IDC_INPUTMAPPING_DETAILS_AUTOMAPPINGTOGGLE, L"Start Auto Mapping");
			}
			UpdateSelectedInputRegistration(hwnd);
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Joystick functions
//----------------------------------------------------------------------------------------
void InputMappingDetailsView::JoystickKeyMappingWorkerThread()
{
	//Calculate the maximum number of joysticks, and the maximum number of buttons and
	//axis per joystick.
	const unsigned int maxButtonCount = 32;
	const unsigned int maxAxisCount = 6;
	UINT maxJoystickCount = joyGetNumDevs();

	//Initialize the button and axis state for each joystick
	std::vector<std::vector<bool>> buttonState;
	std::vector<std::vector<float>> axisState;
	buttonState.resize(maxJoystickCount);
	axisState.resize(maxJoystickCount);
	for(unsigned int i = 0; i < maxJoystickCount; ++i)
	{
		buttonState[i].resize(maxButtonCount);
		axisState[i].resize(maxAxisCount);
	}

	//Process input state changes from joysticks until we're requested to stop
	while(joystickWorkerThreadActive)
	{
		//Latch new values from each joystick
		for(unsigned int i = 0; i < maxJoystickCount; ++i)
		{
			//Obtain info on the capabilities of this joystick
			JOYCAPS joystickCapabilities;
			MMRESULT joyGetDevCapsReturn = joyGetDevCaps(i, &joystickCapabilities, sizeof(joystickCapabilities));
			if(joyGetDevCapsReturn != JOYERR_NOERROR)
			{
				continue;
			}

			//Obtain info on the current button state for this joystick
			JOYINFOEX joystickInfo;
			joystickInfo.dwSize = sizeof(joystickInfo);
			joystickInfo.dwFlags = JOY_RETURNALL;
			MMRESULT joyGetPosExResult = joyGetPosEx(i, &joystickInfo);
			if(joyGetPosExResult != JOYERR_NOERROR)
			{
				continue;
			}

			//Latch the new state for each button in this joystick
			unsigned int activeButtonCount = (joystickCapabilities.wNumButtons > maxButtonCount)? maxButtonCount: joystickCapabilities.wNumButtons;
			Data buttonData(activeButtonCount, joystickInfo.dwButtons);
			for(unsigned int buttonNo = 0; buttonNo < activeButtonCount; ++buttonNo)
			{
				//If the current button state matches the previous button state, advance
				//to the next button.
				bool buttonStateNew = buttonData.GetBit(buttonNo);
				if(buttonState[i][buttonNo] == buttonStateNew)
				{
					continue;
				}

				//Latch the new button state
				buttonState[i][buttonNo] = buttonStateNew;

				//Notify the main window of this button press
				ISystemDeviceInterface::KeyCode keyCode;
				if(model.TranslateJoystickButton(i, buttonNo, keyCode))
				{
					if(buttonStateNew)
					{
						SendMessage(hwndInternal, WM_USER, (WPARAM)keyCode, 0);
					}
				}
			}

			//Latch the new state for each axis in this joystick
			unsigned int reportedAxesCount = joystickCapabilities.wNumAxes;
			bool hasAxisZ = (joystickCapabilities.wCaps & JOYCAPS_HASZ) != 0;
			bool hasAxisR = (joystickCapabilities.wCaps & JOYCAPS_HASR) != 0;
			bool hasAxisU = (joystickCapabilities.wCaps & JOYCAPS_HASU) != 0;
			bool hasAxisV = (joystickCapabilities.wCaps & JOYCAPS_HASV) != 0;
			unsigned int knownAxesCount = (hasAxisZ? 1: 0) + (hasAxisR? 1: 0) + (hasAxisU? 1: 0) + (hasAxisV? 1: 0);
			bool hasAxisX = ((knownAxesCount + 2) <= reportedAxesCount);
			bool hasAxisY = ((knownAxesCount + 2) <= reportedAxesCount);
			std::vector<float> newAxisState;
			if(hasAxisX)
			{
				float newAxisStateX = ((float)(joystickInfo.dwXpos - joystickCapabilities.wXmin) / ((float)(joystickCapabilities.wXmax - joystickCapabilities.wXmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateX);
			}
			if(hasAxisY)
			{
				float newAxisStateY = ((float)(joystickInfo.dwYpos - joystickCapabilities.wYmin) / ((float)(joystickCapabilities.wYmax - joystickCapabilities.wYmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateY);
			}
			if(hasAxisZ)
			{
				float newAxisStateZ = ((float)(joystickInfo.dwZpos - joystickCapabilities.wZmin) / ((float)(joystickCapabilities.wZmax - joystickCapabilities.wZmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateZ);
			}
			if(hasAxisR)
			{
				float newAxisStateR = ((float)(joystickInfo.dwRpos - joystickCapabilities.wRmin) / ((float)(joystickCapabilities.wRmax - joystickCapabilities.wRmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateR);
			}
			if(hasAxisU)
			{
				float newAxisStateU = ((float)(joystickInfo.dwUpos - joystickCapabilities.wUmin) / ((float)(joystickCapabilities.wUmax - joystickCapabilities.wUmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateU);
			}
			if(hasAxisV)
			{
				float newAxisStateV = ((float)(joystickInfo.dwVpos - joystickCapabilities.wVmin) / ((float)(joystickCapabilities.wVmax - joystickCapabilities.wVmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateV);
			}
			for(unsigned int axisNo = 0; axisNo < newAxisState.size(); ++axisNo)
			{
				//If the current axis state matches the previous axis state, advance to
				//the next axis.
				if(axisState[i][axisNo] == newAxisState[axisNo])
				{
					continue;
				}

				//Latch the new axis state
				float axisStateNew = newAxisState[axisNo];
				float axisStateOld = axisState[i][axisNo];
				axisState[i][axisNo] = axisStateNew;

				//Notify the main window of a button state change linked to this axis
				if((axisStateOld < 0.25f) && (axisStateNew >= 0.25f))
				{
					ISystemDeviceInterface::KeyCode keyCode;
					if(model.TranslateJoystickAxisAsButton(i, axisNo, true, keyCode))
					{
						SendMessage(hwndInternal, WM_USER, (WPARAM)keyCode, 0);
					}
				}
				if((axisStateOld > -0.25f) && (axisStateNew <= -0.25f))
				{
					ISystemDeviceInterface::KeyCode keyCode;
					if(model.TranslateJoystickAxisAsButton(i, axisNo, false, keyCode))
					{
						SendMessage(hwndInternal, WM_USER, (WPARAM)keyCode, 0);
					}
				}
			}
		}

		//Delay until it's time to latch the joystick input state again
		Sleep(10);
	}

	//Since this thread is terminating, notify any waiting threads.
	boost::mutex::scoped_lock lock(joystickWorkerThreadMutex);
	joystickWorkerThreadStopped.notify_all();
}
