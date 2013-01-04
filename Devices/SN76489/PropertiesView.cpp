#include "PropertiesView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489::PropertiesView::PropertiesView(SN76489* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Properties");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_SN76489_PROPERTIES));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR SN76489::PropertiesView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR SN76489::PropertiesView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create our refresh timer
	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::PropertiesView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::PropertiesView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	unsigned int shiftRegisterNibbleCount = (device->shiftRegisterBitCount / 4) + ((device->shiftRegisterBitCount % 4) != 0? 1: 0);

	//Update properties
	if(currentControlFocus != IDC_SN76489_CLOCKFREQUENCY)    UpdateDlgItemFloat(hwnd, IDC_SN76489_CLOCKFREQUENCY, (float)device->externalClockRate);
	if(currentControlFocus != IDC_SN76489_CLOCKDIVIDER)      UpdateDlgItemFloat(hwnd, IDC_SN76489_CLOCKDIVIDER, (float)device->externalClockDivider);

	if(currentControlFocus != IDC_SN76489_SHIFTREGISTERSIZE) UpdateDlgItemBin(hwnd, IDC_SN76489_SHIFTREGISTERSIZE, device->shiftRegisterBitCount);
	if(currentControlFocus != IDC_SN76489_SHIFTREGISTERDEFAULTVALUE) UpdateDlgItemHex(hwnd, IDC_SN76489_SHIFTREGISTERDEFAULTVALUE, shiftRegisterNibbleCount, device->shiftRegisterDefaultValue);
	if(currentControlFocus != IDC_SN76489_WHITENOISE)        UpdateDlgItemHex(hwnd, IDC_SN76489_WHITENOISE, shiftRegisterNibbleCount, device->noiseWhiteTappedBitMask);
	if(currentControlFocus != IDC_SN76489_PERIODICNOISE)     UpdateDlgItemHex(hwnd, IDC_SN76489_PERIODICNOISE, shiftRegisterNibbleCount, device->noisePeriodicTappedBitMask);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::PropertiesView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
				device->externalClockRate = GetDlgItemFloat(hwnd, LOWORD(wparam));
				break;
			case IDC_SN76489_CLOCKDIVIDER:{
				float newClockDivider = GetDlgItemFloat(hwnd, LOWORD(wparam));
				if(newClockDivider == 0)
				{
					newClockDivider = 1;
				}
				device->externalClockDivider = newClockDivider;
				break;}

			case IDC_SN76489_SHIFTREGISTERSIZE:
				device->shiftRegisterBitCount = GetDlgItemBin(hwnd, LOWORD(wparam));
				break;
			case IDC_SN76489_SHIFTREGISTERDEFAULTVALUE:
				device->shiftRegisterDefaultValue = GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			case IDC_SN76489_WHITENOISE:
				device->noiseWhiteTappedBitMask = GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			case IDC_SN76489_PERIODICNOISE:
				device->noisePeriodicTappedBitMask = GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			}
		}
	}

	return TRUE;
}
