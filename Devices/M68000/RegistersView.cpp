#include "RegistersView.h"
#include "resource.h"
namespace M68000{

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
M68000::RegistersView::RegistersView(M68000* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Registers");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_M68000_REGISTERS));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR M68000::RegistersView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR M68000::RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR M68000::RegistersView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR M68000::RegistersView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	Data data(BITCOUNT_LONG);
	//Update address registers
	for(unsigned int i = 0; i < device->addressRegCount; ++i)
	{
		device->GetA(i, data);
		if(currentControlFocus != (IDC_REG_A0 + i))	UpdateDlgItemHex(hwnd, IDC_REG_A0 + i, 8, data.GetData());
	}
	//Update data registers
	for(unsigned int i = 0; i < device->dataRegCount; ++i)
	{
		device->GetD(i, data);
		if(currentControlFocus != (IDC_REG_D0 + i))	UpdateDlgItemHex(hwnd, IDC_REG_D0 + i, 8, data.GetData());
	}
	//Update PC
	if(currentControlFocus != IDC_REG_PC)	UpdateDlgItemHex(hwnd, IDC_REG_PC, 8, device->GetPC().GetData());
	//Update CCR flags
	if(currentControlFocus != IDC_REG_X)	UpdateDlgItemHex(hwnd, IDC_REG_X, 1, device->GetX());
	if(currentControlFocus != IDC_REG_N)	UpdateDlgItemHex(hwnd, IDC_REG_N, 1, device->GetN());
	if(currentControlFocus != IDC_REG_Z)	UpdateDlgItemHex(hwnd, IDC_REG_Z, 1, device->GetZ());
	if(currentControlFocus != IDC_REG_V)	UpdateDlgItemHex(hwnd, IDC_REG_V, 1, device->GetV());
	if(currentControlFocus != IDC_REG_C)	UpdateDlgItemHex(hwnd, IDC_REG_C, 1, device->GetC());
	//Update Supervisor Registers
	device->GetUSP(data);
	if(currentControlFocus != IDC_REG_USP)	UpdateDlgItemHex(hwnd, IDC_REG_USP, 8, data.GetData());
	device->GetSSP(data);
	if(currentControlFocus != IDC_REG_SSP)	UpdateDlgItemHex(hwnd, IDC_REG_SSP, 8, data.GetData());
	if(currentControlFocus != IDC_REG_S)	UpdateDlgItemHex(hwnd, IDC_REG_S, 1, device->GetSR_S());
	if(currentControlFocus != IDC_REG_T)	UpdateDlgItemHex(hwnd, IDC_REG_T, 1, device->GetSR_T());
	if(currentControlFocus != IDC_REG_IPM)	UpdateDlgItemHex(hwnd, IDC_REG_IPM, 1, device->GetSR_IPM());
	if(currentControlFocus != IDC_REG_SR)	UpdateDlgItemHex(hwnd, IDC_REG_SR, 4, device->GetSR().GetData());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR M68000::RegistersView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			if((LOWORD(wparam) >= IDC_REG_A0) && (LOWORD(wparam) < (IDC_REG_A0 + device->addressRegCount)))
			{
				device->SetA((LOWORD(wparam) - IDC_REG_A0), M68000Long(GetDlgItemHex(hwnd, LOWORD(wparam))));
			}
			//Data registers
			else if((LOWORD(wparam) >= IDC_REG_D0) && (LOWORD(wparam) < (IDC_REG_D0 + device->addressRegCount)))
			{
				device->SetD((LOWORD(wparam) - IDC_REG_D0), M68000Long(GetDlgItemHex(hwnd, LOWORD(wparam))));
			}
			else switch(LOWORD(wparam))
			{
			//PC
			case IDC_REG_PC:
				device->SetPC(M68000Long(GetDlgItemHex(hwnd, LOWORD(wparam))));
			break;
			//CCR flags
			case IDC_REG_X:
				device->SetX(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_N:
				device->SetN(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_Z:
				device->SetZ(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_V:
				device->SetV(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_C:
				device->SetC(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			//Update Supervisor Registers
			case IDC_REG_USP:
				device->SetUSP(M68000Long(GetDlgItemHex(hwnd, LOWORD(wparam))));
			break;
			case IDC_REG_SSP:
				device->SetSSP(M68000Long(GetDlgItemHex(hwnd, LOWORD(wparam))));
			break;
			case IDC_REG_S:
				device->SetSR_S(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_T:
				device->SetSR_T(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_REG_IPM:
				device->SetSR_IPM(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_REG_SR:
				device->SetSR(M68000Word(GetDlgItemHex(hwnd, LOWORD(wparam))));
			break;
			}
		}
	}

	return TRUE;
}

} //Close namespace M68000
