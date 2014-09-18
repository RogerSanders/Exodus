#include "RegistersView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RegistersView::RegistersView(IUIManager& auiManager, RegistersViewPresenter& apresenter, IM68000& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	changedColor = RGB(255,0,0);
	backgroundColor = RGB(255,255,255);
	backgroundBrush = CreateSolidBrush(backgroundColor);
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_M68000_REGISTERS));
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

	//Update address registers
	for(unsigned int i = 0; i < model.addressRegCount; ++i)
	{
		if(currentControlFocus != (IDC_REG_A0 + i))	UpdateDlgItemHex(hwnd, IDC_REG_A0 + i, 8, model.GetA(i));
	}
	//Update data registers
	for(unsigned int i = 0; i < model.dataRegCount; ++i)
	{
		if(currentControlFocus != (IDC_REG_D0 + i))	UpdateDlgItemHex(hwnd, IDC_REG_D0 + i, 8, model.GetD(i));
	}
	//Update PC
	if(currentControlFocus != IDC_REG_PC)	UpdateDlgItemHex(hwnd, IDC_REG_PC, 8, model.GetPC());
	//Update CCR flags
	if(currentControlFocus != IDC_REG_X)	UpdateDlgItemHex(hwnd, IDC_REG_X, 1, model.GetX());
	if(currentControlFocus != IDC_REG_N)	UpdateDlgItemHex(hwnd, IDC_REG_N, 1, model.GetN());
	if(currentControlFocus != IDC_REG_Z)	UpdateDlgItemHex(hwnd, IDC_REG_Z, 1, model.GetZ());
	if(currentControlFocus != IDC_REG_V)	UpdateDlgItemHex(hwnd, IDC_REG_V, 1, model.GetV());
	if(currentControlFocus != IDC_REG_C)	UpdateDlgItemHex(hwnd, IDC_REG_C, 1, model.GetC());
	//Update Supervisor Registers
	if(currentControlFocus != IDC_REG_USP)	UpdateDlgItemHex(hwnd, IDC_REG_USP, 8, model.GetUSP());
	if(currentControlFocus != IDC_REG_SSP)	UpdateDlgItemHex(hwnd, IDC_REG_SSP, 8, model.GetSSP());
	if(currentControlFocus != IDC_REG_S)	UpdateDlgItemHex(hwnd, IDC_REG_S, 1, model.GetSR_S());
	if(currentControlFocus != IDC_REG_T)	UpdateDlgItemHex(hwnd, IDC_REG_T, 1, model.GetSR_T());
	if(currentControlFocus != IDC_REG_IPM)	UpdateDlgItemHex(hwnd, IDC_REG_IPM, 1, model.GetSR_IPM());
	if(currentControlFocus != IDC_REG_SR)	UpdateDlgItemHex(hwnd, IDC_REG_SR, 4, model.GetSR());

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
			//Address registers
			if((LOWORD(wparam) >= IDC_REG_A0) && (LOWORD(wparam) < (IDC_REG_A0 + model.addressRegCount)))
			{
				model.SetA((LOWORD(wparam) - IDC_REG_A0), GetDlgItemHex(hwnd, LOWORD(wparam)));
			}
			//Data registers
			else if((LOWORD(wparam) >= IDC_REG_D0) && (LOWORD(wparam) < (IDC_REG_D0 + model.addressRegCount)))
			{
				model.SetD((LOWORD(wparam) - IDC_REG_D0), GetDlgItemHex(hwnd, LOWORD(wparam)));
			}
			else switch(LOWORD(wparam))
			{
			//PC
			case IDC_REG_PC:
				model.SetPC(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			//CCR flags
			case IDC_REG_X:
				model.SetX(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_N:
				model.SetN(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_Z:
				model.SetZ(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_V:
				model.SetV(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_C:
				model.SetC(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			//Update Supervisor Registers
			case IDC_REG_USP:
				model.SetUSP(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_REG_SSP:
				model.SetSSP(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_REG_S:
				model.SetSR_S(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_T:
				model.SetSR_T(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_IPM:
				model.SetSR_IPM(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_REG_SR:
				model.SetSR(GetDlgItemHex(hwnd, LOWORD(wparam)));
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
	case IDC_REG_A0:
	case IDC_REG_A1:
	case IDC_REG_A2:
	case IDC_REG_A3:
	case IDC_REG_A4:
	case IDC_REG_A5:
	case IDC_REG_A6:
	case IDC_REG_A7:{
		int regNo = controlID - IDC_REG_A0;
		return (model.GetOriginalValueA(regNo) != model.GetA(regNo));}
	case IDC_REG_D0:
	case IDC_REG_D1:
	case IDC_REG_D2:
	case IDC_REG_D3:
	case IDC_REG_D4:
	case IDC_REG_D5:
	case IDC_REG_D6:
	case IDC_REG_D7:{
		int regNo = controlID - IDC_REG_D0;
		return (model.GetOriginalValueD(regNo) != model.GetD(regNo));}
	case IDC_REG_PC:
		return (model.GetOriginalValuePC() != model.GetPC());
	case IDC_REG_X:
		return (model.GetOriginalValueX() != model.GetX());
	case IDC_REG_N:
		return (model.GetOriginalValueN() != model.GetN());
	case IDC_REG_Z:
		return (model.GetOriginalValueZ() != model.GetZ());
	case IDC_REG_V:
		return (model.GetOriginalValueV() != model.GetV());
	case IDC_REG_C:
		return (model.GetOriginalValueC() != model.GetC());
	case IDC_REG_USP:
		return (model.GetOriginalValueUSP() != model.GetUSP());
	case IDC_REG_SSP:
		return (model.GetOriginalValueSSP() != model.GetSSP());
	case IDC_REG_S:
		return (model.GetOriginalValueSR_S() != model.GetSR_S());
	case IDC_REG_T:
		return (model.GetOriginalValueSR_T() != model.GetSR_T());
	case IDC_REG_IPM:
		return (model.GetOriginalValueSR_IPM() != model.GetSR_IPM());
	case IDC_REG_SR:
		return (model.GetOriginalValueSR() != model.GetSR());
	}
	return false;
}
