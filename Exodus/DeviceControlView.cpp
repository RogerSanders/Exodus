#include "DeviceControlView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::DeviceControlView::DeviceControlView(System* asystem)
:system(asystem), initializedDialog(false), currentControlFocus(0)
{
	deviceListIndex = -1;
	systemStep = 0;
	deviceStep = 1;
	std::wstring windowTitle = L"Device Control";
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)system->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_DEVICECONTROL));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR System::DeviceControlView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR System::DeviceControlView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::DeviceControlView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::DeviceControlView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	boost::mutex::scoped_lock lock(system->debugMutex);
	initializedDialog = true;

	//Update the textboxes
	if(currentControlFocus != IDC_DEVICECONTROL_SYSTEM_EXECUTEAMOUNT)  UpdateDlgItemBin(hwnd, IDC_DEVICECONTROL_SYSTEM_EXECUTEAMOUNT, systemStep);
	if(currentControlFocus != IDC_DEVICECONTROL_DEVICE_STEPAMOUNT)     UpdateDlgItemBin(hwnd, IDC_DEVICECONTROL_DEVICE_STEPAMOUNT, deviceStep);

	//Check if we need to refresh the device list
	bool refreshDeviceList = (devicesCopy.size() != system->devices.size());
	if(!refreshDeviceList)
	{
		DeviceArray::const_iterator devicesIterator = system->devices.begin();
		DeviceArray::const_iterator devicesCopyIterator = devicesCopy.begin();
		while(!refreshDeviceList && (devicesIterator != system->devices.end()) && (devicesCopyIterator != devicesCopy.end()))
		{
			refreshDeviceList |= (*devicesIterator != *devicesCopyIterator);
			++devicesIterator;
			++devicesCopyIterator;
		}
	}
	devicesCopy = system->devices;

	//Refresh the device list if required
	if(refreshDeviceList)
	{
		SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), WM_SETREDRAW, FALSE, 0);

		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETTOPINDEX, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_RESETCONTENT, 0, NULL);
		for(DeviceArray::iterator i = system->devices.begin(); i != system->devices.end(); ++i)
		{
			DeviceContext* device = *i;
			if(device->ActiveDevice() && (device->GetTargetDevice()->GetUpdateMethod() == IDevice::UPDATEMETHOD_STEP))
			{
				LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_ADDSTRING, 0, (LPARAM)device->GetTargetDevice()->GetDeviceInstanceName().c_str());
				SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)device);
			}
		}
		SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_SETCURSEL, (WPARAM)-1, 0);
		SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_SETTOPINDEX, top, 0);

		SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), NULL, FALSE);

		deviceListIndex = -1;
	}

	//Update the checkboxes
	int selectedDeviceIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETCURSEL, 0, NULL);
	LRESULT getItemDataResult = SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETITEMDATA, selectedDeviceIndex, NULL);
	if(getItemDataResult != LB_ERR)
	{
		DeviceContext* deviceContext = (DeviceContext*)getItemDataResult;
		if(deviceContext != 0)
		{
			CheckDlgButton(hwnd, IDC_DEVICECONTROL_DEVICE_ENABLED, deviceContext->DeviceEnabled()? BST_CHECKED: BST_UNCHECKED);
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::DeviceControlView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_DEVICECONTROL_SYSTEM_RUN:
			system->RunSystem();
			break;
		case IDC_DEVICECONTROL_SYSTEM_STOP:
			system->StopSystem();
			break;
		case IDC_DEVICECONTROL_SYSTEM_EXECUTE:
			system->StopSystem();
			system->ExecuteSystemStepManual(systemStep);
			break;
		case IDC_DEVICECONTROL_DEVICE_INITIALIZE:{
			int selectedDeviceIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETCURSEL, 0, NULL);
			DeviceContext* deviceContext = (DeviceContext*)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETITEMDATA, selectedDeviceIndex, NULL);
			if(deviceContext != 0)
			{
				bool running = system->SystemRunning();
				system->StopSystem();
				deviceContext->GetTargetDevice()->NegateCurrentOutputLineState();
				deviceContext->Initialize();
				deviceContext->GetTargetDevice()->AssertCurrentOutputLineState();

				//Re-assert the current line state for all system lines. This is required,
				//as devices reset their input line state as a result of a call to the
				//Initialize method.
				for(SystemLineMap::const_iterator i = system->systemLines.begin(); i != system->systemLines.end(); ++i)
				{
					system->SetSystemLineState(i->first, Data(i->second.lineWidth, i->second.currentValue));
				}

				if(running)
				{
					system->RunSystem();
				}
			}
			break;}
		case IDC_DEVICECONTROL_DEVICE_STEP:{
			int selectedDeviceIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETCURSEL, 0, NULL);
			DeviceContext* deviceContext = (DeviceContext*)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETITEMDATA, selectedDeviceIndex, NULL);
			if(deviceContext != 0)
			{
				system->StopSystem();
				for(unsigned int i = 0; i < deviceStep; ++i)
				{
					//##TODO## Show a dialog box indicating the current step progress,
					//and giving the user an option to cancel.
					system->ExecuteDeviceStep(deviceContext);
				}
			}
			break;}
		case IDC_DEVICECONTROL_DEVICE_ENABLED:{
			int selectedDeviceIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETCURSEL, 0, NULL);
			DeviceContext* deviceContext = (DeviceContext*)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETITEMDATA, selectedDeviceIndex, NULL);
			if(deviceContext != 0)
			{
				deviceContext->SetDeviceEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			}
			break;}
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			switch(LOWORD(wparam))
			{
			case IDC_DEVICECONTROL_DEVICE_STEPAMOUNT:
				deviceStep = GetDlgItemBin(hwnd, LOWORD(wparam));
				break;
			case IDC_DEVICECONTROL_SYSTEM_EXECUTEAMOUNT:
				systemStep = GetDlgItemBin(hwnd, LOWORD(wparam));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == LBN_SELCHANGE)
	{
		switch(LOWORD(wparam))
		{
		case IDC_DEVICECONTROL_LIST:{
			int selectedItem = (int)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETCURSEL, 0, NULL);
			if(selectedItem != LB_ERR)
			{
				deviceListIndex = selectedItem;
			}
			break;}
		}
	}

	return TRUE;
}
