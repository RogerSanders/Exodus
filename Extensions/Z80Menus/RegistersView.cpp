#include "RegistersView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RegistersView::RegistersView(IUIManager& auiManager, RegistersViewPresenter& apresenter, IZ80& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	changedColor = RGB(255,0,0);
	backgroundColor = RGB(255,255,255);
	backgroundBrush = CreateSolidBrush(backgroundColor);
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_Z80_REGISTERS));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
RegistersView::~RegistersView()
{
	DeleteObject(backgroundBrush);
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
	case WM_CTLCOLOREDIT:
		return msgWM_CTLCOLOREDIT(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	systemPausedToggleCounterCached = model.GetOriginalValueChangeCounter();
	SetTimer(hwnd, 1, 50, NULL);
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

	//If the system toggle counter has changed, latch the new value, and force the entire
	//window to redraw, since the highlight state may have changed for some of our
	//register controls.
	if(systemPausedToggleCounterCached != model.GetOriginalValueChangeCounter())
	{
		systemPausedToggleCounterCached = model.GetOriginalValueChangeCounter();
		InvalidateRect(hwnd, NULL, FALSE);
	}

	//Update Registers
	if(currentControlFocus != IDC_Z80_REG_AF)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_AF, 4, model.GetAF());
	if(currentControlFocus != IDC_Z80_REG_BC)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_BC, 4, model.GetBC());
	if(currentControlFocus != IDC_Z80_REG_DE)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_DE, 4, model.GetDE());
	if(currentControlFocus != IDC_Z80_REG_HL)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_HL, 4, model.GetHL());
	if(currentControlFocus != IDC_Z80_REG_AF2)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_AF2, 4, model.GetAF2());
	if(currentControlFocus != IDC_Z80_REG_BC2)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_BC2, 4, model.GetBC2());
	if(currentControlFocus != IDC_Z80_REG_DE2)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_DE2, 4, model.GetDE2());
	if(currentControlFocus != IDC_Z80_REG_HL2)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_HL2, 4, model.GetHL2());

	if(currentControlFocus != IDC_Z80_REG_IX)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_IX, 4, model.GetIX());
	if(currentControlFocus != IDC_Z80_REG_IY)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_IY, 4, model.GetIY());

	if(currentControlFocus != IDC_Z80_REG_PC)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_PC, 4, model.GetPC());
	if(currentControlFocus != IDC_Z80_REG_SP)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_SP, 4, model.GetSP());

	if(currentControlFocus != IDC_Z80_REG_A)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_A, 2, model.GetA());
	if(currentControlFocus != IDC_Z80_REG_F)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_F, 2, model.GetF());
	if(currentControlFocus != IDC_Z80_REG_B)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_B, 2, model.GetB());
	if(currentControlFocus != IDC_Z80_REG_C)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_C, 2, model.GetC());
	if(currentControlFocus != IDC_Z80_REG_D)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_D, 2, model.GetD());
	if(currentControlFocus != IDC_Z80_REG_E)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_E, 2, model.GetE());
	if(currentControlFocus != IDC_Z80_REG_H)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_H, 2, model.GetH());
	if(currentControlFocus != IDC_Z80_REG_L)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_L, 2, model.GetL());

	if(currentControlFocus != IDC_Z80_REG_I)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_I, 2, model.GetI());
	if(currentControlFocus != IDC_Z80_REG_R)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_R, 2, model.GetR());

	//Update interrupt registers
	if(currentControlFocus != IDC_Z80_REG_IFF1)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_IFF1, 1, model.GetIFF1());
	if(currentControlFocus != IDC_Z80_REG_IFF2)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_IFF2, 1, model.GetIFF2());
	if(currentControlFocus != IDC_Z80_REG_IM)		UpdateDlgItemHex(hwnd, IDC_Z80_REG_IM, 1, model.GetInterruptMode());

	//Update Flags
	if(currentControlFocus != IDC_Z80_REG_FLAG_S)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_S, 1, model.GetFlagS());
	if(currentControlFocus != IDC_Z80_REG_FLAG_Z)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_Z, 1, model.GetFlagZ());
	if(currentControlFocus != IDC_Z80_REG_FLAG_Y)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_Y, 1, model.GetFlagY());
	if(currentControlFocus != IDC_Z80_REG_FLAG_H)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_H, 1, model.GetFlagH());
	if(currentControlFocus != IDC_Z80_REG_FLAG_X)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_X, 1, model.GetFlagX());
	if(currentControlFocus != IDC_Z80_REG_FLAG_PV)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_PV, 1, model.GetFlagPV());
	if(currentControlFocus != IDC_Z80_REG_FLAG_N)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_N, 1, model.GetFlagN());
	if(currentControlFocus != IDC_Z80_REG_FLAG_C)	UpdateDlgItemHex(hwnd, IDC_Z80_REG_FLAG_C, 1, model.GetFlagC());

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
			switch(LOWORD(wparam))
			{
				//Registers
			case IDC_Z80_REG_AF:
				model.SetAF(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_BC:
				model.SetBC(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_DE:
				model.SetDE(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_HL:
				model.SetHL(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_AF2:
				model.SetAF2(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_BC2:
				model.SetBC2(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_DE2:
				model.SetDE2(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_HL2:
				model.SetHL2(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

			case IDC_Z80_REG_IX:
				model.SetIX(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_IY:
				model.SetIY(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

			case IDC_Z80_REG_PC:
				model.SetPC(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_SP:
				model.SetSP(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

			case IDC_Z80_REG_A:
				model.SetA(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_F:
				model.SetF(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_B:
				model.SetB(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_C:
				model.SetC(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_D:
				model.SetD(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_E:
				model.SetE(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_H:
				model.SetH(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_L:
				model.SetL(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

			case IDC_Z80_REG_I:
				model.SetI(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_Z80_REG_R:
				model.SetR(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

				//Update interrupt registers
			case IDC_Z80_REG_IFF1:
				model.SetIFF1(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_IFF2:
				model.SetIFF2(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_IM:
				model.SetInterruptMode(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

				//Flags
			case IDC_Z80_REG_FLAG_S:
				model.SetFlagS(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_Z:
				model.SetFlagZ(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_Y:
				model.SetFlagY(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_H:
				model.SetFlagH(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_X:
				model.SetFlagX(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_PV:
				model.SetFlagPV(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_N:
				model.SetFlagN(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_Z80_REG_FLAG_C:
				model.SetFlagC(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Handle text colour changes for registers which have changed since the last time the
	//system was halted
	if((systemPausedToggleCounterCached > 0) && RegisterContentsChanged(GetDlgCtrlID((HWND)lparam)))
	{
		SetTextColor((HDC)wparam, changedColor);
		return (BOOL)HandleToLong(backgroundBrush);
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------
//Changed register functions
//----------------------------------------------------------------------------------------
bool RegistersView::RegisterContentsChanged(int controlID)
{
	switch(controlID)
	{
	case IDC_Z80_REG_AF:
		return (model.GetOriginalValueAF() != model.GetAF());
	case IDC_Z80_REG_BC:
		return (model.GetOriginalValueBC() != model.GetBC());
	case IDC_Z80_REG_DE:
		return (model.GetOriginalValueDE() != model.GetDE());
	case IDC_Z80_REG_HL:
		return (model.GetOriginalValueHL() != model.GetHL());
	case IDC_Z80_REG_AF2:
		return (model.GetOriginalValueAF2() != model.GetAF2());
	case IDC_Z80_REG_BC2:
		return (model.GetOriginalValueBC2() != model.GetBC2());
	case IDC_Z80_REG_DE2:
		return (model.GetOriginalValueDE2() != model.GetDE2());
	case IDC_Z80_REG_HL2:
		return (model.GetOriginalValueHL2() != model.GetHL2());

	case IDC_Z80_REG_IX:
		return (model.GetOriginalValueIX() != model.GetIX());
	case IDC_Z80_REG_IY:
		return (model.GetOriginalValueIY() != model.GetIY());
	case IDC_Z80_REG_PC:
		return (model.GetOriginalValuePC() != model.GetPC());
	case IDC_Z80_REG_SP:
		return (model.GetOriginalValueSP() != model.GetSP());
	case IDC_Z80_REG_A:
		return (model.GetOriginalValueA() != model.GetA());
	case IDC_Z80_REG_F:
		return (model.GetOriginalValueF() != model.GetF());
	case IDC_Z80_REG_B:
		return (model.GetOriginalValueB() != model.GetB());
	case IDC_Z80_REG_C:
		return (model.GetOriginalValueC() != model.GetC());
	case IDC_Z80_REG_D:
		return (model.GetOriginalValueD() != model.GetD());
	case IDC_Z80_REG_E:
		return (model.GetOriginalValueE() != model.GetE());
	case IDC_Z80_REG_H:
		return (model.GetOriginalValueH() != model.GetH());
	case IDC_Z80_REG_L:
		return (model.GetOriginalValueL() != model.GetL());
	case IDC_Z80_REG_I:
		return (model.GetOriginalValueI() != model.GetI());
	case IDC_Z80_REG_R:
		return (model.GetOriginalValueR() != model.GetR());
	case IDC_Z80_REG_IFF1:
		return (model.GetOriginalValueIFF1() != model.GetIFF1());
	case IDC_Z80_REG_IFF2:
		return (model.GetOriginalValueIFF2() != model.GetIFF2());
	case IDC_Z80_REG_IM:
		return (model.GetOriginalValueInterruptMode() != model.GetInterruptMode());
	case IDC_Z80_REG_FLAG_S:
		return (model.GetOriginalValueFlagS() != model.GetFlagS());
	case IDC_Z80_REG_FLAG_Z:
		return (model.GetOriginalValueFlagZ() != model.GetFlagZ());
	case IDC_Z80_REG_FLAG_Y:
		return (model.GetOriginalValueFlagY() != model.GetFlagY());
	case IDC_Z80_REG_FLAG_H:
		return (model.GetOriginalValueFlagH() != model.GetFlagH());
	case IDC_Z80_REG_FLAG_X:
		return (model.GetOriginalValueFlagX() != model.GetFlagX());
	case IDC_Z80_REG_FLAG_PV:
		return (model.GetOriginalValueFlagPV() != model.GetFlagPV());
	case IDC_Z80_REG_FLAG_N:
		return (model.GetOriginalValueFlagN() != model.GetFlagN());
	case IDC_Z80_REG_FLAG_C:
		return (model.GetOriginalValueFlagC() != model.GetFlagC());
	}
	return false;
}
