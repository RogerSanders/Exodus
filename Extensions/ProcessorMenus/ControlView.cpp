#include "ControlView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ControlView::ControlView(IUIManager& auiManager, ControlViewPresenter& apresenter, IProcessor& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_CONTROL));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR ControlView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
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
INT_PTR ControlView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ControlView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR ControlView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;
	CheckDlgButton(hwnd, IDC_PROCESSOR_CONTROL_ENABLED, (model.GetDevice()->GetDeviceContext()->DeviceEnabled())? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_PROCESSOR_CONTROL_CLOCK) UpdateDlgItemDouble(hwnd, IDC_PROCESSOR_CONTROL_CLOCK, model.GetClockSpeed());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ControlView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
				model.OverrideClockSpeed((double)GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_RESTORE_CLOCKSPEED:
			model.RestoreClockSpeed();
			UpdateDlgItemDouble(hwnd, IDC_PROCESSOR_CONTROL_CLOCK, model.GetClockSpeed());
			break;
		case IDC_PROCESSOR_CONTROL_ENABLED:
			model.GetDevice()->GetDeviceContext()->SetDeviceEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_INIT:
			model.GetDevice()->Initialize();
			break;
		case IDC_PROCESSOR_RESET:
			model.Reset();
			break;
		case IDC_PROCESSOR_RUN:
			model.GetDevice()->GetDeviceContext()->StopSystem();
			//This step before running the system allows the user to just keep on hitting
			//run when stopped at a breakpoint.
			//##FIX## This assumes that executing a single device step will pass the
			//breakpoint, which may not be the case.
			model.GetDevice()->GetDeviceContext()->ExecuteDeviceStep();
			model.GetDevice()->GetDeviceContext()->RunSystem();
			break;
		case IDC_PROCESSOR_STEP:
			model.GetDevice()->GetDeviceContext()->StopSystem();
			model.GetDevice()->GetDeviceContext()->ExecuteDeviceStep();
			break;
		case IDC_PROCESSOR_STEPOVER:
			model.GetDevice()->GetDeviceContext()->StopSystem();
			model.BreakOnStepOverCurrentOpcode();
			model.GetDevice()->GetDeviceContext()->RunSystem();
			break;
		case IDC_PROCESSOR_STEPOUT:
			model.GetDevice()->GetDeviceContext()->StopSystem();
			model.BreakOnStepOutCurrentOpcode();
			model.GetDevice()->GetDeviceContext()->RunSystem();
			break;
		}
	}

	return TRUE;
}
