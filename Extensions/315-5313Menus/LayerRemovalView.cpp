#include "LayerRemovalView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LayerRemovalView::LayerRemovalView(IUIManager& auiManager, LayerRemovalViewPresenter& apresenter, IS315_5313& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_LAYERREMOVAL));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR LayerRemovalView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR LayerRemovalView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CheckDlgButton(hwnd, IDC_LAYERAHIGH, (model.GetEnableLayerAHigh())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LAYERALOW, (model.GetEnableLayerALow())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LAYERBHIGH, (model.GetEnableLayerBHigh())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LAYERBLOW, (model.GetEnableLayerBLow())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_WINDOWHIGH, (model.GetEnableWindowHigh())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_WINDOWLOW, (model.GetEnableWindowLow())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SPRITEHIGH, (model.GetEnableSpriteHigh())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SPRITELOW, (model.GetEnableSpriteLow())? BST_CHECKED: BST_UNCHECKED);

	CheckDlgButton(hwnd, IDC_LAYERA, (model.GetEnableLayerAHigh() && model.GetEnableLayerALow())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LAYERB, (model.GetEnableLayerBHigh() && model.GetEnableLayerBLow())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_WINDOW, (model.GetEnableWindowHigh() && model.GetEnableWindowLow())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SPRITE, (model.GetEnableSpriteHigh() && model.GetEnableSpriteLow())? BST_CHECKED: BST_UNCHECKED);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR LayerRemovalView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_LAYERAHIGH:
			model.SetEnableLayerAHigh(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERA, (model.GetEnableLayerAHigh() && model.GetEnableLayerALow())? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_LAYERALOW:
			model.SetEnableLayerALow(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERA, (model.GetEnableLayerAHigh() && model.GetEnableLayerALow())? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_LAYERBHIGH:
			model.SetEnableLayerBHigh(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERB, (model.GetEnableLayerBHigh() && model.GetEnableLayerBLow())? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_LAYERBLOW:
			model.SetEnableLayerBLow(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERB, (model.GetEnableLayerBHigh() && model.GetEnableLayerBLow())? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_WINDOWHIGH:
			model.SetEnableWindowHigh(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_WINDOW, (model.GetEnableWindowHigh() && model.GetEnableWindowLow())? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_WINDOWLOW:
			model.SetEnableWindowLow(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_WINDOW, (model.GetEnableWindowHigh() && model.GetEnableWindowLow())? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_SPRITEHIGH:
			model.SetEnableSpriteHigh(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_SPRITE, (model.GetEnableSpriteHigh() && model.GetEnableSpriteLow())? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_SPRITELOW:
			model.SetEnableSpriteLow(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_SPRITE, (model.GetEnableSpriteHigh() && model.GetEnableSpriteLow())? BST_CHECKED: BST_UNCHECKED);
			break;
		case IDC_LAYERA:{
			UINT checkState = IsDlgButtonChecked(hwnd, LOWORD(wparam));
			model.SetEnableLayerAHigh(checkState == BST_CHECKED);
			model.SetEnableLayerALow(checkState == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERAHIGH, checkState);
			CheckDlgButton(hwnd, IDC_LAYERALOW, checkState);
			break;}
		case IDC_LAYERB:{
			UINT checkState = IsDlgButtonChecked(hwnd, LOWORD(wparam));
			model.SetEnableLayerBHigh(checkState == BST_CHECKED);
			model.SetEnableLayerBLow(checkState == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_LAYERBHIGH, checkState);
			CheckDlgButton(hwnd, IDC_LAYERBLOW, checkState);
			break;}
		case IDC_WINDOW:{
			UINT checkState = IsDlgButtonChecked(hwnd, LOWORD(wparam));
			model.SetEnableWindowHigh(checkState == BST_CHECKED);
			model.SetEnableWindowLow(checkState == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_WINDOWHIGH, checkState);
			CheckDlgButton(hwnd, IDC_WINDOWLOW, checkState);
			break;}
		case IDC_SPRITE:{
			UINT checkState = IsDlgButtonChecked(hwnd, LOWORD(wparam));
			model.SetEnableSpriteHigh(checkState == BST_CHECKED);
			model.SetEnableSpriteLow(checkState == BST_CHECKED);
			CheckDlgButton(hwnd, IDC_SPRITEHIGH, checkState);
			CheckDlgButton(hwnd, IDC_SPRITELOW, checkState);
			break;}
		}
	}

	return TRUE;
}
