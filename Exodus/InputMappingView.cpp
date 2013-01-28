#include "InputMappingView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::InputMappingView::InputMappingView(System* asystem)
:system(asystem), initializedDialog(false), currentControlFocus(0)
{
	selectedInputDevice = 0;
	std::wstring windowTitle = L"Input Mapping";
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)system->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_INPUTMAPPING));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR System::InputMappingView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR System::InputMappingView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::InputMappingView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::InputMappingView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	boost::mutex::scoped_lock lock(system->debugMutex);
	initializedDialog = true;

	//Obtain the current set of input devices
	std::set<IDevice*> inputDeviceSet;
	std::set<unsigned int> inputModuleSet;
	std::set<unsigned int> inputModulesWithMultipleInputDevicesSet;
	std::list<IDevice*> inputDeviceList;
	for(InputRegistrationList::const_iterator i = system->inputRegistrationList.begin(); i != system->inputRegistrationList.end(); ++i)
	{
		if(inputDeviceSet.find(i->targetDevice) == inputDeviceSet.end())
		{
			inputDeviceSet.insert(i->targetDevice);
			inputDeviceList.push_back(i->targetDevice);
			if(inputModuleSet.find(i->moduleID) == inputModuleSet.end())
			{
				inputModuleSet.insert(i->moduleID);
			}
			else
			{
				inputModulesWithMultipleInputDevicesSet.insert(i->moduleID);
			}
		}
	}

	//Check if we need to refresh the input device list
	bool refreshInputDeviceList = (inputDevicesCopy.size() != inputDeviceList.size());
	if(!refreshInputDeviceList)
	{
		std::list<IDevice*>::const_iterator inputDevicesIterator = inputDeviceList.begin();
		std::list<IDevice*>::const_iterator inputDevicesCopyIterator = inputDevicesCopy.begin();
		while(!refreshInputDeviceList && (inputDevicesIterator != inputDeviceList.end()) && (inputDevicesCopyIterator != inputDevicesCopy.end()))
		{
			refreshInputDeviceList |= (*inputDevicesIterator != *inputDevicesCopyIterator);
			++inputDevicesIterator;
			++inputDevicesCopyIterator;
		}
	}

	//Refresh the input device list if required
	if(refreshInputDeviceList)
	{
		inputDevicesCopy = inputDeviceList;

		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), WM_SETREDRAW, FALSE, 0);

		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_GETTOPINDEX, 0, 0);
		LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_GETCURSEL, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_RESETCONTENT, 0, NULL);
		for(std::list<IDevice*>::const_iterator i = inputDeviceList.begin(); i != inputDeviceList.end(); ++i)
		{
			//Obtain the ID of the module that contains this device
			unsigned int deviceModuleID = (*i)->GetDeviceModuleID();

			//Obtain the unique display name for the module that contains this device
			std::wstring moduleDisplayName = (*i)->GetModuleDisplayName();

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

			//Build a string uniquely identifying this device in the system
			std::wstring deviceNameString = moduleDisplayName;
			if(inputModulesWithMultipleInputDevicesSet.find(deviceModuleID) != inputModulesWithMultipleInputDevicesSet.end())
			{
				deviceNameString += L"." + (*i)->GetDeviceInstanceName();
			}
			if(importedConnectorFound && !multipleImportedConnectorsFound)
			{
				deviceNameString += L" {" + importedConnectorName + L"}";
			}

			//Add an entry for this input device to the list
			LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_ADDSTRING, 0, (LPARAM)deviceNameString.c_str());
			SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)*i);
		}
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_SETCURSEL, selected, 0);
		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_SETTOPINDEX, top, 0);

		SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), NULL, FALSE);
	}

	//If we currently have an open details view looking at input mappings for a particular
	//device, ensure that device still exists, and close the details view if it does not,
	//since it will now have a pointer to a device object that has been destroyed.
	//##FIX## This isn't entirely safe. System windows such as this one need a hook into
	//the module unload process, where they can be notified before a device is unloaded,
	//and have a chance to fully process the event before the device is removed from the
	//system. Windows such as our input mapping details view must subscribe to this
	//message, and ensure they clean up any references to the device which is being
	//removed when the message is received.
	if(refreshInputDeviceList)
	{
		if(selectedInputDevice != 0)
		{
			if(inputDeviceSet.find(selectedInputDevice) == inputDeviceSet.end())
			{
				selectedInputDevice = 0;
				system->CloseInputMappingDetailsView();
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::InputMappingView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == LBN_DBLCLK)
	{
		//Display the additional info dialog for the selected log entry
		switch(LOWORD(wparam))
		{
		case IDC_INPUTMAPPING_LIST:{
			LRESULT getCurrentSelectionResult = SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_GETCURSEL, 0, 0);
			if(getCurrentSelectionResult != LB_ERR)
			{
				int currentItemListIndex = (int)getCurrentSelectionResult;
				selectedInputDevice = (IDevice*)SendMessage(GetDlgItem(hwnd, IDC_INPUTMAPPING_LIST), LB_GETITEMDATA, currentItemListIndex, NULL);
				system->OpenInputMappingDetailsView(selectedInputDevice);
			}
			break;}
		}
	}

	return TRUE;
}
