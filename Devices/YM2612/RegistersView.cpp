#include "RegistersView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612::RegistersView::RegistersView(YM2612* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Raw Registers");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_YM2612_REGISTERS));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR YM2612::RegistersView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR YM2612::RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::RegistersView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::RegistersView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	//Update Registers
	if(currentControlFocus != IDC_YM2612_REGISTERS_STATUS)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_REGISTERS_STATUS, 2, device->GetStatus().GetData());
	}

	for(unsigned int i = 0; i <= 0xB7; ++i)
	{
		if(currentControlFocus != (IDC_YM2612_REGISTERS_00 + i))
		{
			UpdateDlgItemHex(hwnd, IDC_YM2612_REGISTERS_00 + i, 2, device->reg.ReadCommitted(i).GetData());
		}
	}

	for(unsigned int i = 0; i <= 0xB7; ++i)
	{
		if(currentControlFocus != (IDC_YM2612_REGISTERS_P2_00 + i))
		{
			UpdateDlgItemHex(hwnd, IDC_YM2612_REGISTERS_P2_00 + i, 2, device->reg.ReadCommitted(0x100 + i).GetData());
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::RegistersView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
				device->status = GetDlgItemHex(hwnd, LOWORD(wparam));
			}
			else if((LOWORD(wparam) >= IDC_YM2612_REGISTERS_00) && (LOWORD(wparam) <= IDC_YM2612_REGISTERS_B7))
			{
				unsigned int i = LOWORD(wparam) - IDC_YM2612_REGISTERS_00;
				device->SetRegisterData(i, Data(8, GetDlgItemHex(hwnd, LOWORD(wparam))), AccessTarget().AccessLatest());
			}
			else if((LOWORD(wparam) >= IDC_YM2612_REGISTERS_P2_00) && (LOWORD(wparam) <= IDC_YM2612_REGISTERS_P2_B7))
			{
				unsigned int i = LOWORD(wparam) - IDC_YM2612_REGISTERS_P2_00;
				device->SetRegisterData(0x100 + i, Data(8, GetDlgItemHex(hwnd, LOWORD(wparam))), AccessTarget().AccessLatest());
			}
		}
	}

	return TRUE;
}
