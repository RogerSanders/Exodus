#include "ControlView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::ControlView::ControlView(Processor* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Processor Control");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_CONTROL));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR Processor::ControlView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR Processor::ControlView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::ControlView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::ControlView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;
	CheckDlgButton(hwnd, IDC_PROCESSOR_CONTROL_ENABLED, (device->GetDeviceContext()->DeviceEnabled())? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_PROCESSOR_CONTROL_CLOCK)	UpdateDlgItemDouble(hwnd, IDC_PROCESSOR_CONTROL_CLOCK, device->GetClockSpeed());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::ControlView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
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
			case IDC_PROCESSOR_CONTROL_CLOCK:
				device->OverrideClockSpeed((double)GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_RESTORE_CLOCKSPEED:
			device->RestoreClockSpeed();
			UpdateDlgItemDouble(hwnd, IDC_PROCESSOR_CONTROL_CLOCK, device->GetClockSpeed());
			break;
		case IDC_PROCESSOR_CONTROL_ENABLED:
			device->GetDeviceContext()->SetDeviceEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_INIT:
			device->Initialize();
			device->Reset();
			break;
		case IDC_PROCESSOR_RESET:
			device->Reset();
			break;
		case IDC_PROCESSOR_RUN:
			device->GetDeviceContext()->StopSystem();
			device->SetStepOver(false);
			device->SetStepOut(false);
			//This step before running the system allows the user to just keep on
			//hitting run when stopped at a breakpoint.
			device->GetDeviceContext()->ExecuteDeviceStep();
			device->GetDeviceContext()->RunSystem();
			break;
		case IDC_PROCESSOR_STEP:
			device->GetDeviceContext()->StopSystem();
			device->GetDeviceContext()->ExecuteDeviceStep();
			break;
		case IDC_PROCESSOR_STEPOVER:
			device->GetDeviceContext()->StopSystem();
			device->SetStepOver(true);
			device->GetDeviceContext()->RunSystem();
			break;
		case IDC_PROCESSOR_STEPOUT:
			device->GetDeviceContext()->StopSystem();
			device->SetStepOut(true);
			device->GetDeviceContext()->RunSystem();
			break;
		}
	}

	return TRUE;
}
