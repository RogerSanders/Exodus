#include "DeviceControlView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::DeviceControlView::DeviceControlView(System* asystem)
:system(asystem), initializedDialog(false), currentControlFocus(0)
{
	deviceContext = 0;
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
	//Update the device list
	SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_RESETCONTENT, 0, NULL);
	for(DeviceArray::iterator i = system->activeDevices.begin(); i != system->activeDevices.end(); ++i)
	{
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_ADDSTRING, 0, (LPARAM)(*i)->GetTargetDevice()->GetDeviceInstanceName().c_str());
		SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)*i);
	}
	SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), NULL, FALSE);

	//Update the textboxes
	UpdateDlgItemBin(hwnd, IDC_DEVICECONTROL_SYSTEM_EXECUTEAMOUNT, systemStep);
	UpdateDlgItemBin(hwnd, IDC_DEVICECONTROL_DEVICE_STEPAMOUNT, deviceStep);

	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::DeviceControlView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

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
			system->ExecuteSystemStep(systemStep);
			break;
		case IDC_DEVICECONTROL_DEVICE_INITIALIZE:{
			if(deviceContext != 0)
			{
				bool running = system->SystemRunning();
				system->StopSystem();
				deviceContext->Initialize();
				if(running)
				{
					system->RunSystem();
				}
			}
			break;}
		case IDC_DEVICECONTROL_DEVICE_STEP:{
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
		case IDC_DEVICECONTROL_DEVICE_ENABLED:
			if(deviceContext != 0)
			{
				deviceContext->SetDeviceEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			}
			break;
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if(HIWORD(wparam) == EN_KILLFOCUS)
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
			int selectedDeviceIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETCURSEL, 0, NULL);
			deviceContext = (DeviceContext*)SendMessage(GetDlgItem(hwnd, IDC_DEVICECONTROL_LIST), LB_GETITEMDATA, selectedDeviceIndex, NULL);
			if(deviceContext != 0)
			{
				CheckDlgButton(hwnd, IDC_DEVICECONTROL_DEVICE_ENABLED, (deviceContext->DeviceEnabled())? BST_CHECKED: BST_UNCHECKED);
			}
			break;}
		}
	}

	return TRUE;
}
