#include "DebugSettingsView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::DebugSettingsView::DebugSettingsView(S315_5313* adevice)
:device(adevice)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Debug Settings");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_S315_5313_DEBUGSETTINGS));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::DebugSettingsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::DebugSettingsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_DEBUGPORT, device->outputPortAccessDebugMessages? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_DEBUGTIMING, device->outputTimingDebugMessages? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_DEBUGRENDERSYNC, device->outputRenderSyncMessages? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_DEBUGINTERRUPTS, device->outputInterruptDebugMessages? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_VIDEODISABLE, device->videoDisableRenderOutput? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_VIDEOSPRITEBOXING, device->videoEnableSpriteBoxing? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_VIDEOHIGHLIGHTRENDERPOS, device->videoHighlightRenderPos? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_BOUNDARYACTIVEIMAGE, device->videoShowBoundaryActiveImage? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_BOUNDARYTITLE, device->videoShowBoundaryTitleSafe? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_BOUNDARYACTION, device->videoShowBoundaryActionSafe? BST_CHECKED: BST_UNCHECKED);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::DebugSettingsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_S315_5313_DEBUGSETTINGS_DEBUGPORT:
			device->outputPortAccessDebugMessages = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_DEBUGTIMING:
			device->outputTimingDebugMessages = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_DEBUGRENDERSYNC:
			device->outputRenderSyncMessages = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_DEBUGINTERRUPTS:
			device->outputInterruptDebugMessages = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_VIDEODISABLE:
			device->videoDisableRenderOutput = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_VIDEOSPRITEBOXING:
			device->videoEnableSpriteBoxing = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_VIDEOHIGHLIGHTRENDERPOS:
			device->videoHighlightRenderPos = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_BOUNDARYACTIVEIMAGE:
			device->videoShowBoundaryActiveImage = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_BOUNDARYACTION:
			device->videoShowBoundaryActionSafe = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_BOUNDARYTITLE:
			device->videoShowBoundaryTitleSafe = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		}
	}

	return TRUE;
}
