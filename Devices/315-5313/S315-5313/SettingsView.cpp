#include "SettingsView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::SettingsView::SettingsView(S315_5313* adevice)
:device(adevice)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Settings");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_S315_5313_SETTINGS));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::SettingsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::SettingsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CheckDlgButton(hwnd, IDC_S315_5313_SETTINGS_VIDEOSINGLEBUFFERING, device->videoSingleBuffering? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_SETTINGS_VIDEOFIXEDASPECTRATIO, device->videoFixedAspectRatio? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_SETTINGS_SHOWSTATUSBAR, device->videoShowStatusBar? BST_CHECKED: BST_UNCHECKED);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::SettingsView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::SettingsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_S315_5313_SETTINGS_VIDEOSINGLEBUFFERING:
			device->videoSingleBuffering = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_SETTINGS_VIDEOFIXEDASPECTRATIO:
			device->videoFixedAspectRatio = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_SETTINGS_SHOWSTATUSBAR:
			device->videoShowStatusBar = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		}
	}

	return TRUE;
}
