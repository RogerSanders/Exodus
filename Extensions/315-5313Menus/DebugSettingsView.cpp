#include "DebugSettingsView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugSettingsView::DebugSettingsView(IUIManager& auiManager, DebugSettingsViewPresenter& apresenter, IS315_5313& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_S315_5313_DEBUGSETTINGS));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR DebugSettingsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR DebugSettingsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_DEBUGPORT, model.GetOutputPortAccessDebugMessages()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_DEBUGTIMING, model.GetOutputTimingDebugMessages()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_DEBUGRENDERSYNC, model.GetOutputRenderSyncMessages()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_DEBUGINTERRUPTS, model.GetOutputInterruptDebugMessages()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_VIDEODISABLE, model.GetVideoDisableRenderOutput()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_VIDEOSPRITEBOXING, model.GetVideoEnableSpriteBoxing()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_VIDEOHIGHLIGHTRENDERPOS, model.GetVideoHighlightRenderPos()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_BOUNDARYACTIVEIMAGE, model.GetVideoShowBoundaryActiveImage()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_BOUNDARYTITLE, model.GetVideoShowBoundaryTitleSafe()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_DEBUGSETTINGS_BOUNDARYACTION, model.GetVideoShowBoundaryActionSafe()? BST_CHECKED: BST_UNCHECKED);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR DebugSettingsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_S315_5313_DEBUGSETTINGS_DEBUGPORT:
			model.SetOutputPortAccessDebugMessages(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_DEBUGTIMING:
			model.SetOutputTimingDebugMessages(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_DEBUGRENDERSYNC:
			model.SetOutputRenderSyncMessages(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_DEBUGINTERRUPTS:
			model.SetOutputInterruptDebugMessages(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_VIDEODISABLE:
			model.SetVideoDisableRenderOutput(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_VIDEOSPRITEBOXING:
			model.SetVideoEnableSpriteBoxing(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_VIDEOHIGHLIGHTRENDERPOS:
			model.SetVideoHighlightRenderPos(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_BOUNDARYACTIVEIMAGE:
			model.SetVideoShowBoundaryActiveImage(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_BOUNDARYACTION:
			model.SetVideoShowBoundaryActionSafe(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_S315_5313_DEBUGSETTINGS_BOUNDARYTITLE:
			model.SetVideoShowBoundaryTitleSafe(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		}
	}

	return TRUE;
}
