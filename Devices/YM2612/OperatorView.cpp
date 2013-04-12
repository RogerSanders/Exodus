#include "OperatorView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612::OperatorView::OperatorView(YM2612* adevice, unsigned int achannelNo, unsigned int aoperatorNo)
:device(adevice), initializedDialog(false), currentControlFocus(0), channelNo(achannelNo), operatorNo(aoperatorNo)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Operator");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_YM2612_OPERATOR));
}

//----------------------------------------------------------------------------------------
//Operator functions
//----------------------------------------------------------------------------------------
void YM2612::OperatorView::SetTargetOperator(unsigned int achannelNo, unsigned int aoperatorNo)
{
	channelNo = achannelNo;
	operatorNo = aoperatorNo;
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR YM2612::OperatorView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR YM2612::OperatorView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::OperatorView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::OperatorView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	//##TODO## Ensure if the operator and channel have changed since the last time, that
	//we update them.

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::OperatorView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//##TODO##

	return TRUE;
}
