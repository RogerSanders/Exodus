#include "PropertiesView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PropertiesView::PropertiesView(IUIManager& auiManager, PropertiesViewPresenter& apresenter, ISN76489& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_SN76489_PROPERTIES));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR PropertiesView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR PropertiesView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create our refresh timer
	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR PropertiesView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR PropertiesView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	unsigned int shiftRegisterBitCount = model.GetShiftRegisterBitCount();
	unsigned int shiftRegisterNibbleCount = (shiftRegisterBitCount / 4) + ((shiftRegisterBitCount % 4) != 0? 1: 0);

	//Update properties
	if(currentControlFocus != IDC_SN76489_CLOCKFREQUENCY)    UpdateDlgItemFloat(hwnd, IDC_SN76489_CLOCKFREQUENCY, (float)model.GetExternalClockRate());
	if(currentControlFocus != IDC_SN76489_CLOCKDIVIDER)      UpdateDlgItemFloat(hwnd, IDC_SN76489_CLOCKDIVIDER, (float)model.GetExternalClockDivider());

	if(currentControlFocus != IDC_SN76489_SHIFTREGISTERSIZE) UpdateDlgItemBin(hwnd, IDC_SN76489_SHIFTREGISTERSIZE, model.GetShiftRegisterBitCount());
	if(currentControlFocus != IDC_SN76489_SHIFTREGISTERDEFAULTVALUE) UpdateDlgItemHex(hwnd, IDC_SN76489_SHIFTREGISTERDEFAULTVALUE, shiftRegisterNibbleCount, model.GetShiftRegisterDefaultValue());
	if(currentControlFocus != IDC_SN76489_WHITENOISE)        UpdateDlgItemHex(hwnd, IDC_SN76489_WHITENOISE, shiftRegisterNibbleCount, model.GetNoiseChannelWhiteNoiseTappedBitMask());
	if(currentControlFocus != IDC_SN76489_PERIODICNOISE)     UpdateDlgItemHex(hwnd, IDC_SN76489_PERIODICNOISE, shiftRegisterNibbleCount, model.GetNoiseChannelPeriodicNoiseTappedBitMask());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR PropertiesView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			//Set property
			switch(LOWORD(wparam))
			{
			case IDC_SN76489_CLOCKFREQUENCY:
				model.SetExternalClockRate(GetDlgItemFloat(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_CLOCKDIVIDER:{
				float newClockDivider = GetDlgItemFloat(hwnd, LOWORD(wparam));
				if(newClockDivider == 0)
				{
					newClockDivider = 1;
				}
				model.SetExternalClockDivider(newClockDivider);
				break;}

			case IDC_SN76489_SHIFTREGISTERSIZE:
				model.SetShiftRegisterBitCount(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_SHIFTREGISTERDEFAULTVALUE:
				model.SetShiftRegisterDefaultValue(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_WHITENOISE:
				model.SetNoiseChannelWhiteNoiseTappedBitMask(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_PERIODICNOISE:
				model.SetNoiseChannelPeriodicNoiseTappedBitMask(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}

	return TRUE;
}
