#include "RegistersView.h"
#include "resource.h"
namespace Z80{

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Z80::RegistersView::RegistersView(Z80* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Registers");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_Z80_REGISTERS));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR Z80::RegistersView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR Z80::RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Z80::RegistersView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Z80::RegistersView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	//Update Registers
	if(currentControlFocus != IDC_Z80_REG_AF)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_AF, 4, device->GetAF().GetData());
	if(currentControlFocus != IDC_Z80_REG_BC)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_BC, 4, device->GetBC().GetData());
	if(currentControlFocus != IDC_Z80_REG_DE)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_DE, 4, device->GetDE().GetData());
	if(currentControlFocus != IDC_Z80_REG_HL)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_HL, 4, device->GetHL().GetData());
	if(currentControlFocus != IDC_Z80_REG_AF2)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_AF2, 4, device->GetAF2().GetData());
	if(currentControlFocus != IDC_Z80_REG_BC2)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_BC2, 4, device->GetBC2().GetData());
	if(currentControlFocus != IDC_Z80_REG_DE2)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_DE2, 4, device->GetDE2().GetData());
	if(currentControlFocus != IDC_Z80_REG_HL2)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_HL2, 4, device->GetHL2().GetData());

	if(currentControlFocus != IDC_Z80_REG_IX)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_IX, 4, device->GetIX().GetData());
	if(currentControlFocus != IDC_Z80_REG_IY)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_IY, 4, device->GetIY().GetData());

	if(currentControlFocus != IDC_Z80_REG_PC)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_PC, 4, device->GetPC().GetData());
	if(currentControlFocus != IDC_Z80_REG_SP)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_SP, 4, device->GetSP().GetData());

	if(currentControlFocus != IDC_Z80_REG_A)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_A, 2, device->GetA().GetData());
	if(currentControlFocus != IDC_Z80_REG_F)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_F, 2, device->GetF().GetData());
	if(currentControlFocus != IDC_Z80_REG_B)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_B, 2, device->GetB().GetData());
	if(currentControlFocus != IDC_Z80_REG_C)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_C, 2, device->GetC().GetData());
	if(currentControlFocus != IDC_Z80_REG_D)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_D, 2, device->GetD().GetData());
	if(currentControlFocus != IDC_Z80_REG_E)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_E, 2, device->GetE().GetData());
	if(currentControlFocus != IDC_Z80_REG_H)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_H, 2, device->GetH().GetData());
	if(currentControlFocus != IDC_Z80_REG_L)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_L, 2, device->GetL().GetData());

	if(currentControlFocus != IDC_Z80_REG_I)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_I, 2, device->GetI().GetData());
	if(currentControlFocus != IDC_Z80_REG_R)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_R, 2, device->GetR().GetData());

	//Update interrupt registers
	if(currentControlFocus != IDC_Z80_REG_IFF1)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_IFF1, 1, device->GetIFF1());
	if(currentControlFocus != IDC_Z80_REG_IFF2)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_IFF2, 1, device->GetIFF2());
	if(currentControlFocus != IDC_Z80_REG_IM)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_IM, 1, device->GetInterruptMode());

	//Update Flags
	if(currentControlFocus != IDC_Z80_REG_FLAG_S)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_S, 1, device->GetFlagS());
	if(currentControlFocus != IDC_Z80_REG_FLAG_Z)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_Z, 1, device->GetFlagZ());
	if(currentControlFocus != IDC_Z80_REG_FLAG_Y)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_Y, 1, device->GetFlagY());
	if(currentControlFocus != IDC_Z80_REG_FLAG_H)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_H, 1, device->GetFlagH());
	if(currentControlFocus != IDC_Z80_REG_FLAG_X)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_X, 1, device->GetFlagX());
	if(currentControlFocus != IDC_Z80_REG_FLAG_PV)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_PV, 1, device->GetFlagPV());
	if(currentControlFocus != IDC_Z80_REG_FLAG_N)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_N, 1, device->GetFlagN());
	if(currentControlFocus != IDC_Z80_REG_FLAG_C)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_C, 1, device->GetFlagC());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Z80::RegistersView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
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
				//Registers
			case IDC_Z80_REG_AF:
				device->SetAF(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_BC:
				device->SetBC(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_DE:
				device->SetDE(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_HL:
				device->SetHL(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_AF2:
				device->SetAF2(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_BC2:
				device->SetBC2(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_DE2:
				device->SetDE2(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_HL2:
				device->SetHL2(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;

			case IDC_Z80_REG_IX:
				device->SetIX(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_IY:
				device->SetIY(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;

			case IDC_Z80_REG_PC:
				device->SetPC(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_SP:
				device->SetSP(Z80Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;

			case IDC_Z80_REG_A:
				device->SetA(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_F:
				device->SetF(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_B:
				device->SetB(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_C:
				device->SetC(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_D:
				device->SetD(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_E:
				device->SetE(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_H:
				device->SetH(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_L:
				device->SetL(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;

			case IDC_Z80_REG_I:
				device->SetI(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_Z80_REG_R:
				device->SetR(Z80Byte(GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;

				//Update interrupt registers
			case IDC_Z80_REG_IFF1:
				device->SetIFF1(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_IFF2:
				device->SetIFF2(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_IM:
				device->SetInterruptMode(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

				//Flags
			case IDC_Z80_REG_FLAG_S:
				device->SetFlagS(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_Z:
				device->SetFlagZ(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_Y:
				device->SetFlagY(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_H:
				device->SetFlagH(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_X:
				device->SetFlagX(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_PV:
				device->SetFlagPV(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_N:
				device->SetFlagN(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_C:
				device->SetFlagC(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			}
		}
	}

	return TRUE;
}

} //Close namespace Z80
