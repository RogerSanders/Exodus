#include "RegistersView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RegistersView::RegistersView(IUIManager& auiManager, RegistersViewPresenter& apresenter, IYM2612& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_YM2612_REGISTERS));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR RegistersView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	//Update Registers
	if(currentControlFocus != IDC_YM2612_REGISTERS_STATUS)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_REGISTERS_STATUS, 2, model.GetStatusRegister());
	}

	for(unsigned int i = 0; i <= 0xB7; ++i)
	{
		if(currentControlFocus != (IDC_YM2612_REGISTERS_00 + i))
		{
			UpdateDlgItemHex(hwnd, IDC_YM2612_REGISTERS_00 + i, 2, model.GetRegisterData(i));
		}
	}

	for(unsigned int i = 0; i <= 0xB7; ++i)
	{
		if(currentControlFocus != (IDC_YM2612_REGISTERS_P2_00 + i))
		{
			UpdateDlgItemHex(hwnd, IDC_YM2612_REGISTERS_P2_00 + i, 2, model.GetRegisterData(IYM2612::registerCountPerPart + i));
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			if(LOWORD(wparam) == IDC_YM2612_REGISTERS_STATUS)
			{
				model.SetStatusRegister(GetDlgItemHex(hwnd, LOWORD(wparam)));
			}
			else if((LOWORD(wparam) >= IDC_YM2612_REGISTERS_00) && (LOWORD(wparam) <= IDC_YM2612_REGISTERS_B7))
			{
				unsigned int i = LOWORD(wparam) - IDC_YM2612_REGISTERS_00;
				model.SetRegisterData(i, GetDlgItemHex(hwnd, LOWORD(wparam)));
			}
			else if((LOWORD(wparam) >= IDC_YM2612_REGISTERS_P2_00) && (LOWORD(wparam) <= IDC_YM2612_REGISTERS_P2_B7))
			{
				unsigned int i = LOWORD(wparam) - IDC_YM2612_REGISTERS_P2_00;
				model.SetRegisterData(IYM2612::registerCountPerPart + i, GetDlgItemHex(hwnd, LOWORD(wparam)));
			}
		}
	}

	return TRUE;
}
