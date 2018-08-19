#include "ControlView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ControlView::ControlView(IUIManager& uiManager, ControlViewPresenter& presenter, IProcessor& model)
:ViewBase(uiManager, presenter), _presenter(presenter), _model(model), _initializedDialog(false), _currentControlFocus(0)
{
	SetDialogTemplateSettings(presenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_CONTROL));
	SetDockableViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR ControlView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch (msg)
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
	_initializedDialog = true;
	CheckDlgButton(hwnd, IDC_PROCESSOR_CONTROL_ENABLED, (_model.GetDevice()->GetDeviceContext()->DeviceEnabled())? BST_CHECKED: BST_UNCHECKED);
	if (_currentControlFocus != IDC_PROCESSOR_CONTROL_CLOCK) UpdateDlgItemDouble(hwnd, IDC_PROCESSOR_CONTROL_CLOCK, _model.GetClockSpeed());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ControlView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if ((HIWORD(wparam) == EN_SETFOCUS) && _initializedDialog)
	{
		_previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		_currentControlFocus = LOWORD(wparam);
	}
	else if ((HIWORD(wparam) == EN_KILLFOCUS) && _initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if (newText != _previousText)
		{
			switch (LOWORD(wparam))
			{
			case IDC_PROCESSOR_CONTROL_CLOCK:
				_model.OverrideClockSpeed((double)GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if (HIWORD(wparam) == BN_CLICKED)
	{
		switch (LOWORD(wparam))
		{
		case IDC_PROCESSOR_RESTORE_CLOCKSPEED:
			_model.RestoreClockSpeed();
			UpdateDlgItemDouble(hwnd, IDC_PROCESSOR_CONTROL_CLOCK, _model.GetClockSpeed());
			break;
		case IDC_PROCESSOR_CONTROL_ENABLED:
			_model.GetDevice()->GetDeviceContext()->SetDeviceEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_INIT:
			_model.GetDevice()->Initialize();
			break;
		case IDC_PROCESSOR_RESET:
			_model.Reset();
			break;
		case IDC_PROCESSOR_RUN:
			_model.GetDevice()->GetDeviceContext()->StopSystem();
			//This step before running the system allows the user to just keep on hitting
			//run when stopped at a breakpoint.
			//##FIX## This assumes that executing a single device step will pass the
			//breakpoint, which may not be the case.
			_model.GetDevice()->GetDeviceContext()->ExecuteDeviceStep();
			_model.GetDevice()->GetDeviceContext()->RunSystem();
			break;
		case IDC_PROCESSOR_STEP:
			_model.GetDevice()->GetDeviceContext()->StopSystem();
			_model.GetDevice()->GetDeviceContext()->ExecuteDeviceStep();
			break;
		case IDC_PROCESSOR_STEPOVER:
			_model.GetDevice()->GetDeviceContext()->StopSystem();
			_model.BreakOnStepOverCurrentOpcode();
			_model.GetDevice()->GetDeviceContext()->RunSystem();
			break;
		case IDC_PROCESSOR_STEPOUT:
			_model.GetDevice()->GetDeviceContext()->StopSystem();
			_model.BreakOnStepOutCurrentOpcode();
			_model.GetDevice()->GetDeviceContext()->RunSystem();
			break;
		}
	}

	return TRUE;
}
