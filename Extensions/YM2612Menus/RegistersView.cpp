#include "RegistersView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RegistersView::RegistersView(IUIManager& uiManager, RegistersViewPresenter& presenter, IYM2612& model)
:ViewBase(uiManager, presenter), _presenter(presenter), _model(model), _initializedDialog(false), _currentControlFocus(0)
{
	SetDialogTemplateSettings(presenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_YM2612_REGISTERS));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------------------------------------
// Member window procedure
//----------------------------------------------------------------------------------------------------------------------
INT_PTR RegistersView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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

//----------------------------------------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	_initializedDialog = true;

	// Update Registers
	if (_currentControlFocus != IDC_YM2612_REGISTERS_STATUS)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_REGISTERS_STATUS, 2, _model.GetStatusRegister());
	}

	for (unsigned int i = 0; i <= 0xB7; ++i)
	{
		if (_currentControlFocus != (IDC_YM2612_REGISTERS_00 + i))
		{
			UpdateDlgItemHex(hwnd, IDC_YM2612_REGISTERS_00 + i, 2, _model.GetRegisterData(i));
		}
	}

	for (unsigned int i = 0; i <= 0xB7; ++i)
	{
		if (_currentControlFocus != (IDC_YM2612_REGISTERS_P2_00 + i))
		{
			UpdateDlgItemHex(hwnd, IDC_YM2612_REGISTERS_P2_00 + i, 2, _model.GetRegisterData(IYM2612::RegisterCountPerPart + i));
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			if (LOWORD(wparam) == IDC_YM2612_REGISTERS_STATUS)
			{
				_model.SetStatusRegister(GetDlgItemHex(hwnd, LOWORD(wparam)));
			}
			else if ((LOWORD(wparam) >= IDC_YM2612_REGISTERS_00) && (LOWORD(wparam) <= IDC_YM2612_REGISTERS_B7))
			{
				unsigned int i = LOWORD(wparam) - IDC_YM2612_REGISTERS_00;
				_model.SetRegisterData(i, GetDlgItemHex(hwnd, LOWORD(wparam)));
			}
			else if ((LOWORD(wparam) >= IDC_YM2612_REGISTERS_P2_00) && (LOWORD(wparam) <= IDC_YM2612_REGISTERS_P2_B7))
			{
				unsigned int i = LOWORD(wparam) - IDC_YM2612_REGISTERS_P2_00;
				_model.SetRegisterData(IYM2612::RegisterCountPerPart + i, GetDlgItemHex(hwnd, LOWORD(wparam)));
			}
		}
	}

	return TRUE;
}
