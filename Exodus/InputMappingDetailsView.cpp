#include "InputMappingDetailsView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::InputMappingDetailsView::InputMappingDetailsView(System* asystem, IDevice* atargetDevice)
:system(asystem), initializedDialog(false), currentControlFocus(0), targetDevice(atargetDevice)
{
	hwndInternal = NULL;
	autoKeyMappingActive = false;
	std::wstring windowTitle = L"Device Input Mapping";
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)system->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_INPUTMAPPING_DETAILS));
}

//----------------------------------------------------------------------------------------
//Target device functions
//----------------------------------------------------------------------------------------
void System::InputMappingDetailsView::SetTargetDevice(IDevice* atargetDevice)
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
INT_PTR System::InputMappingDetailsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_BOUNCE:
		return msgWM_BOUNCE(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
///----------------------------------------------------------------------------------------
INT_PTR System::InputMappingDetailsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	hwndInternal = hwnd;
	selectedInputRegistration = -1;
	UpdateTargetDeviceInputMappingsDisplay(hwndInternal);
	initializedDialog = true;

	SetWindowSubclass(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_AUTOMAPPINGTOGGLE), BounceBackSubclassProc, 0, 0);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::InputMappingDetailsView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Entry update functions
//----------------------------------------------------------------------------------------
void System::InputMappingDetailsView::UpdateTargetDeviceInputMappingsDisplay(HWND hwnd)
{
	//Obtain the ID of the module that contains this device
	unsigned int deviceModuleID = targetDevice->GetDeviceModuleID();

	//Obtain the unique display name for the module that contains this device
	std::wstring moduleDisplayName = targetDevice->GetModuleDisplayName();

	//Check if there's a single connector imported by this module, and retrieve
	//its instance name.
	bool importedConnectorFound = false;
	bool multipleImportedConnectorsFound = false;
	std::wstring importedConnectorName;
	for(ConnectorDetailsMap::const_iterator connectorDetailsMapIterator = system->connectorDetailsMap.begin(); connectorDetailsMapIterator != system->connectorDetailsMap.end(); ++connectorDetailsMapIterator)
	{
		const ConnectorDetails& connectorDetails = connectorDetailsMapIterator->second;
		if(connectorDetails.connectorUsed && (connectorDetails.importingModuleID == deviceModuleID))
		{
			multipleImportedConnectorsFound = importedConnectorFound;
			importedConnectorFound = true;
			importedConnectorName = connectorDetails.exportingModuleConnectorInstanceName;
		}
	}

	//Build a string uniquely identifying this input device in the system
	std::wstring deviceNameString = moduleDisplayName;
	if(importedConnectorFound && !multipleImportedConnectorsFound)
	{
		deviceNameString += L" {" + importedConnectorName + L"}";
	}

	//Update the device name field
	UpdateDlgItemString(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICENAME, deviceNameString);

	//Obtain the current set of registered inputs for the target device
	inputRegistrationsForDevice.clear();
	inputRegistrationsForDevice.reserve(40);
	for(InputRegistrationList::const_iterator i = system->inputRegistrationList.begin(); i != system->inputRegistrationList.end(); ++i)
	{
		if(i->targetDevice == targetDevice)
		{
			inputRegistrationsForDevice.push_back(*i);
		}
	}

	//Update the list of device keys
	SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_RESETCONTENT, 0, 0);
	for(unsigned int i = 0; i < inputRegistrationsForDevice.size(); ++i)
	{
		unsigned int deviceKeyCode = inputRegistrationsForDevice[i].deviceKeyCode;
		std::wstring deviceKeyCodeString = targetDevice->GetKeyCodeName(deviceKeyCode);
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_ADDSTRING, 0, (LPARAM)deviceKeyCodeString.c_str());
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_SETITEMDATA, newItemIndex, (LPARAM)i);
	}

	//Update the list of system keys
	SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_RESETCONTENT, 0, 0);
	for(unsigned int i = (unsigned int)IDeviceContext::KEYCODE_NONE; i < (unsigned int)IDeviceContext::KEYCODE_ENDOFLIST; ++i)
	{
		IDeviceContext::KeyCode systemKeyCode = (IDeviceContext::KeyCode)i;
		std::wstring systemKeyCodeString = system->GetKeyCodeName(systemKeyCode);
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
void System::InputMappingDetailsView::UpdateSelectedInputRegistration(HWND hwnd)
{
	bool autoMappingTextSet = false;
	if(selectedInputRegistration >= 0)
	{
		unsigned int deviceKeyCode = inputRegistrationsForDevice[selectedInputRegistration].deviceKeyCode;
		IDeviceContext::KeyCode systemKeyCode = system->GetDeviceKeyCodeMapping(targetDevice, deviceKeyCode);
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_DEVICEKEY), CB_SETCURSEL, (WPARAM)selectedInputRegistration, 0);
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_SETCURSEL, (WPARAM)((unsigned int)systemKeyCode - (unsigned int)IDeviceContext::KEYCODE_NONE), 0);

		if(autoKeyMappingActive)
		{
			std::wstring autoMappingText = L"Press the button for ";
			autoMappingText += targetDevice->GetKeyCodeName(inputRegistrationsForDevice[selectedInputRegistration].deviceKeyCode);
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
INT_PTR System::InputMappingDetailsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			for(unsigned int i = 0; i < inputRegistrationsForDevice.size(); ++i)
			{
				SetSystemKeySelectionForDeviceKey(i, IDeviceContext::KEYCODE_NONE);
			}
			UpdateSelectedInputRegistration(hwnd);
			break;
		case IDC_INPUTMAPPING_DETAILS_RESTOREDEFAULTS:
			for(unsigned int i = 0; i < inputRegistrationsForDevice.size(); ++i)
			{
				SetSystemKeySelectionForDeviceKey(i, IDeviceContext::KEYCODE_NONE);
			}
			for(unsigned int i = 0; i < inputRegistrationsForDevice.size(); ++i)
			{
				if(inputRegistrationsForDevice[i].preferredSystemKeyCodeSpecified)
				{
					SetSystemKeySelectionForDeviceKey(i, inputRegistrationsForDevice[i].systemKeyCode);
				}
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
				IDeviceContext::KeyCode systemKeyCode = (IDeviceContext::KeyCode)SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_GETITEMDATA, itemIndex, NULL);
				SetSystemKeySelectionForDeviceKey(selectedInputRegistration, systemKeyCode);
			}
			break;}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::InputMappingDetailsView::msgWM_BOUNCE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lparam;
	int controlID = GetDlgCtrlID(bounceMessage->hwnd);
	if((controlID == IDC_INPUTMAPPING_DETAILS_SYSTEMKEY) && initializedDialog)
	{
		switch(bounceMessage->uMsg)
		{
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			IDeviceContext::KeyCode systemKeyCode;
			if(targetDevice->GetDeviceContext()->TranslateKeyCode((unsigned int)bounceMessage->wParam, systemKeyCode))
			{
				SetSystemKeySelectionForDeviceKey(selectedInputRegistration, systemKeyCode);
				SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_SETCURSEL, (WPARAM)((unsigned int)systemKeyCode - (unsigned int)IDeviceContext::KEYCODE_NONE), 0);
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
				IDeviceContext::KeyCode systemKeyCode;
				if(targetDevice->GetDeviceContext()->TranslateKeyCode((unsigned int)bounceMessage->wParam, systemKeyCode))
				{
					SetSystemKeySelectionForDeviceKey(selectedInputRegistration, systemKeyCode);
					SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_DETAILS_SYSTEMKEY), CB_SETCURSEL, (WPARAM)((unsigned int)systemKeyCode - (unsigned int)IDeviceContext::KEYCODE_NONE), 0);
					if(selectedInputRegistration < ((int)inputRegistrationsForDevice.size() - 1))
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
//Input selection functions
//----------------------------------------------------------------------------------------
void System::InputMappingDetailsView::SetSystemKeySelectionForDeviceKey(unsigned int inputRegistrationNo, IDeviceContext::KeyCode systemKeyCode)
{
	system->UnmapKeyCode(targetDevice, inputRegistrationsForDevice[inputRegistrationNo].deviceKeyCode);
	if(systemKeyCode != IDeviceContext::KEYCODE_NONE)
	{
		system->MapKeyCode(targetDevice, inputRegistrationsForDevice[inputRegistrationNo].deviceKeyCode, systemKeyCode);
	}
}
