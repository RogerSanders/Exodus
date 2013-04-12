#include "EmbeddedROMView.h"
#include "Stream/Stream.pkg"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::EmbeddedROMView::EmbeddedROMView(System* asystem)
:system(asystem)
{
	selectedROMDevice = 0;
	std::wstring windowTitle = L"Embedded ROM Selection";
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)system->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_EMBEDDEDROM));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR System::EmbeddedROMView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR System::EmbeddedROMView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Set the folder icon on the browse button
	HANDLE folderIconHandle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, 0, 0, LR_SHARED);
	SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_BROWSE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);

	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::EmbeddedROMView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::EmbeddedROMView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	boost::mutex::scoped_lock lock(system->debugMutex);

	//Obtain the current set of ROM devices
	EmbeddedROMList romInfoList;
	std::set<unsigned int> targetModuleSet;
	std::set<unsigned int> modulesWithMultipleTargetDevicesSet;
	for(EmbeddedROMList::const_iterator i = system->embeddedROMInfo.begin(); i != system->embeddedROMInfo.end(); ++i)
	{
		romInfoList.push_back(*i);
		if(targetModuleSet.find(i->moduleID) == targetModuleSet.end())
		{
			targetModuleSet.insert(i->moduleID);
		}
		else
		{
			modulesWithMultipleTargetDevicesSet.insert(i->moduleID);
		}
	}

	//Check if we need to refresh the ROM device list
	bool refreshROMDeviceList = (romInfoListCopy.size() != romInfoList.size());
	if(!refreshROMDeviceList)
	{
		EmbeddedROMList::const_iterator romInfoIterator = romInfoList.begin();
		EmbeddedROMList::const_iterator romInfoCopyIterator = romInfoListCopy.begin();
		while(!refreshROMDeviceList && (romInfoIterator != romInfoList.end()) && (romInfoCopyIterator != romInfoListCopy.end()))
		{
			refreshROMDeviceList |= (romInfoIterator->targetDevice != romInfoCopyIterator->targetDevice) || (romInfoIterator->filePath != romInfoCopyIterator->filePath);
			++romInfoIterator;
			++romInfoCopyIterator;
		}
	}

	//Refresh the ROM device list if required
	if(refreshROMDeviceList)
	{
		romInfoListCopy = romInfoList;
		selectedROMDevice = 0;
		SetDlgItemText(hwnd, IDC_EMBEDDEDROM_PATH, L"");

		SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), WM_SETREDRAW, FALSE, 0);

		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_GETTOPINDEX, 0, 0);
		LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_GETCURSEL, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_RESETCONTENT, 0, NULL);
		for(EmbeddedROMList::const_iterator i = romInfoList.begin(); i != romInfoList.end(); ++i)
		{
			//Obtain the ID of the module that contains this device
			unsigned int deviceModuleID = i->targetDevice->GetDeviceModuleID();

			//Obtain the unique display name for the module that contains this device
			std::wstring moduleDisplayName = i->targetDevice->GetModuleDisplayName();

			//Check if there's a single connector imported by this module, and retrieve
			//its instance name.
			bool importedConnectorFound = false;
			bool multipleImportedConnectorsFound = false;
			std::wstring importedConnectorName;
			for(ConnectorDetailsMap::const_iterator connectorDetailsMapIterator = system->connectorDetailsMap.begin(); connectorDetailsMapIterator != system->connectorDetailsMap.end(); ++connectorDetailsMapIterator)
			{
				const ConnectorInfoInternal& connectorDetails = connectorDetailsMapIterator->second;
				if(connectorDetails.connectorUsed && (connectorDetails.importingModuleID == deviceModuleID))
				{
					multipleImportedConnectorsFound = importedConnectorFound;
					importedConnectorFound = true;
					importedConnectorName = connectorDetails.exportingModuleConnectorInstanceName;
				}
			}

			//Build a string uniquely identifying this device in the system
			std::wstring deviceNameString = moduleDisplayName;
			if(modulesWithMultipleTargetDevicesSet.find(deviceModuleID) != modulesWithMultipleTargetDevicesSet.end())
			{
				deviceNameString += L"." + i->displayName;
			}
			if(importedConnectorFound && !multipleImportedConnectorsFound)
			{
				deviceNameString += L" {" + importedConnectorName + L"}";
			}

			//Add an entry for this device to the list
			LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_ADDSTRING, 0, (LPARAM)deviceNameString.c_str());
			SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)i->targetDevice);
		}
		SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_SETCURSEL, selected, 0);
		SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_SETTOPINDEX, top, 0);

		SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), NULL, FALSE);
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::EmbeddedROMView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		case IDC_EMBEDDEDROM_BROWSE:{
			//Select a new target file
			std::wstring fileNameCurrent = GetDlgItemString(hwnd, IDC_EMBEDDEDROM_PATH);
			std::wstring selectedFilePath;
			if(system->guiExtensionInterface->SelectExistingFile(L"Binary files|bin", L"bin", fileNameCurrent, L"", true, selectedFilePath))
			{
				SetDlgItemText(hwnd, IDC_EMBEDDEDROM_PATH, &selectedFilePath[0]);
			}
			break;}
		case IDC_EMBEDDEDROM_APPLY:
			if(selectedROMDevice != 0)
			{
				for(EmbeddedROMList::iterator romInfoIterator = system->embeddedROMInfo.begin(); romInfoIterator != system->embeddedROMInfo.end(); ++romInfoIterator)
				{
					if(romInfoIterator->targetDevice == selectedROMDevice)
					{
						romInfoIterator->filePath = GetDlgItemString(hwnd, IDC_EMBEDDEDROM_PATH);
					}
				}
			}
		//Note the deliberate fall-through here. We want to reload the embedded ROM data
		//when the apply button is pressed.
		case IDC_EMBEDDEDROM_RELOAD:{
			if(selectedROMDevice != 0)
			{
				for(EmbeddedROMList::const_iterator romInfoIterator = system->embeddedROMInfo.begin(); romInfoIterator != system->embeddedROMInfo.end(); ++romInfoIterator)
				{
					if(romInfoIterator->targetDevice == selectedROMDevice)
					{
						//Save running state and pause system
						bool running = system->SystemRunning();
						system->StopSystem();

						//Attempt to load this ROM data into the target device
						Stream::File file;
						bool result = file.Open(romInfoIterator->filePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN);
						if(result)
						{
							for(unsigned int i = 0; (i < file.Size()) && (i < romInfoIterator->romRegionSize); ++i)
							{
								unsigned char byte;
								if(file.ReadData(byte))
								{
									romInfoIterator->targetDevice->TransparentWriteInterface(romInfoIterator->interfaceNumber, i, Data(8, byte), 0, 0);
								}
							}
						}

						//Restore the running state
						if(running)
						{
							system->RunSystem();
						}
					}
				}
			}
			break;}
		}
	}
	else if(HIWORD(wparam) == LBN_SELCHANGE)
	{
		switch(LOWORD(wparam))
		{
		case IDC_EMBEDDEDROM_LIST:
			int selectedItem = (int)SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_GETCURSEL, 0, NULL);
			if(selectedItem != LB_ERR)
			{
				selectedROMDevice = (IDevice*)SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_GETITEMDATA, selectedItem, 0);
				for(EmbeddedROMList::const_iterator romInfoIterator = system->embeddedROMInfo.begin(); romInfoIterator != system->embeddedROMInfo.end(); ++romInfoIterator)
				{
					if(romInfoIterator->targetDevice == selectedROMDevice)
					{
						SetDlgItemText(hwnd, IDC_EMBEDDEDROM_PATH, &romInfoIterator->filePath[0]);
					}
				}
			}
			break;
		}
	}

	return TRUE;
}
