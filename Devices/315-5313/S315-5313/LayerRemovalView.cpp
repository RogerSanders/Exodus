#include "LayerRemovalView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::LayerRemovalView::LayerRemovalView(S315_5313* adevice)
:device(adevice)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Layer Removal");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_LAYERREMOVAL));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::LayerRemovalView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR S315_5313::LayerRemovalView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CheckDlgButton(hwnd, IDC_LAYERAHIGH, (device->enableLayerAHigh)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LAYERALOW, (device->enableLayerALow)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LAYERBHIGH, (device->enableLayerBHigh)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LAYERBLOW, (device->enableLayerBLow)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_WINDOWHIGH, (device->enableWindowHigh)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_WINDOWLOW, (device->enableWindowLow)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SPRITEHIGH, (device->enableSpriteHigh)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SPRITELOW, (device->enableSpriteLow)? BST_CHECKED: BST_UNCHECKED);

	CheckDlgButton(hwnd, IDC_LAYERA, (device->enableLayerAHigh && device->enableLayerALow)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LAYERB, (device->enableLayerBHigh && device->enableLayerBLow)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_WINDOW, (device->enableWindowHigh && device->enableWindowLow)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SPRITE, (device->enableSpriteHigh && device->enableSpriteLow)? BST_CHECKED: BST_UNCHECKED);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::LayerRemovalView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::LayerRemovalView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_LAYERAHIGH:
			device->enableLayerAHigh = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERA, (device->enableLayerAHigh && device->enableLayerALow)? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_LAYERALOW:
			device->enableLayerALow = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERA, (device->enableLayerAHigh && device->enableLayerALow)? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_LAYERBHIGH:
			device->enableLayerBHigh = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERB, (device->enableLayerBHigh && device->enableLayerBLow)? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_LAYERBLOW:
			device->enableLayerBLow = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERB, (device->enableLayerBHigh && device->enableLayerBLow)? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_WINDOWHIGH:
			device->enableWindowHigh = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_WINDOW, (device->enableWindowHigh && device->enableWindowLow)? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_WINDOWLOW:
			device->enableWindowLow = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_WINDOW, (device->enableWindowHigh && device->enableWindowLow)? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_SPRITEHIGH:
			device->enableSpriteHigh = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_SPRITE, (device->enableSpriteHigh && device->enableSpriteLow)? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_SPRITELOW:
			device->enableSpriteLow = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_SPRITE, (device->enableSpriteHigh && device->enableSpriteLow)? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_LAYERA:{
			UINT checkState = IsDlgButtonChecked(hwnd, LOWORD(wparam));
			device->enableLayerAHigh = (checkState == BST_CHECKED);
			device->enableLayerALow = (checkState == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERAHIGH, checkState);
			CheckDlgButton(hwnd, IDC_LAYERALOW, checkState);
			break;}
		case IDC_LAYERB:{
			UINT checkState = IsDlgButtonChecked(hwnd, LOWORD(wparam));
			device->enableLayerBHigh = (checkState == BST_CHECKED);
			device->enableLayerBLow = (checkState == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERBHIGH, checkState);
			CheckDlgButton(hwnd, IDC_LAYERBLOW, checkState);
			break;}
		case IDC_WINDOW:{
			UINT checkState = IsDlgButtonChecked(hwnd, LOWORD(wparam));
			device->enableWindowHigh = (checkState == BST_CHECKED);
			device->enableWindowLow = (checkState == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_WINDOWHIGH, checkState);
			CheckDlgButton(hwnd, IDC_WINDOWLOW, checkState);
			break;}
		case IDC_SPRITE:{
			UINT checkState = IsDlgButtonChecked(hwnd, LOWORD(wparam));
			device->enableSpriteHigh = (checkState == BST_CHECKED);
			device->enableSpriteLow = (checkState == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_SPRITEHIGH, checkState);
			CheckDlgButton(hwnd, IDC_SPRITELOW, checkState);
			break;}
		}
	}

	return TRUE;
}
