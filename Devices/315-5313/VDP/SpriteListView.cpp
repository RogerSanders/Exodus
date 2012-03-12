#include "SpriteListView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::SpriteListView::SpriteListView(VDP* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Sprite List");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_SPRITELIST));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR VDP::SpriteListView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR VDP::SpriteListView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	int tabsize = 20;
	SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabsize);
	SetTimer(hwnd, 1, 500, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::SpriteListView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::SpriteListView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_RESETCONTENT, 0, NULL);
	for(unsigned int i = 0; i < 80; ++i)
	{
		unsigned int spriteMappingAddress = device->GetSpriteTableAddress() + (device->GetSpriteMappingSize() * i);
		SpriteMapping mapping = device->GetSpriteMapping(spriteMappingAddress);
		std::wstringstream stream;

		stream << std::setw(2) << std::setfill(L'0') << std::hex << std::uppercase << i << L'\t';
		stream << std::setw(3) << std::setfill(L'0') << std::hex << std::uppercase << mapping.xpos << L'\t';
		stream << std::setw(3) << std::setfill(L'0') << std::hex << std::uppercase << mapping.ypos << L'\t';
		stream << std::setw(3) << std::setfill(L'0') << std::hex << std::uppercase << mapping.blockNumber << L'\t';
		stream << std::setw(2) << std::setfill(L'0') << std::hex << std::uppercase << mapping.link << L'\t';
		stream << std::setw(1) << std::setfill(L'0') << std::hex << std::uppercase << mapping.width << L'\t';
		stream << std::setw(1) << std::setfill(L'0') << std::hex << std::uppercase << mapping.height << L'\t';
		stream << std::setw(1) << std::setfill(L'0') << std::hex << std::uppercase << mapping.priority << L'\t';
		stream << std::setw(1) << std::setfill(L'0') << std::hex << std::uppercase << mapping.hflip << L'\t';
		stream << std::setw(1) << std::setfill(L'0') << std::hex << std::uppercase << mapping.vflip << L'\t';
		stream << std::setw(1) << std::setfill(L'0') << std::hex << std::uppercase << mapping.paletteLine << L'\t';
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_ADDSTRING, 0, (LPARAM)stream.str().c_str());
		SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)i);
	}
	SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), NULL, FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::SpriteListView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == LBN_DBLCLK)
	{
		//Load the sprite editor for the selected location
		switch(LOWORD(wparam))
		{
		case IDC_VDP_SPRITELIST_LIST:{
			int currentItemListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_GETCURSEL, 0, NULL);
			unsigned int targetSpriteIndex = (unsigned int)SendMessage(GetDlgItem(hwnd, IDC_VDP_SPRITELIST_LIST), LB_GETITEMDATA, currentItemListIndex, NULL);

			device->OpenSpriteListDetailsView(device->GetSpriteTableAddress(), targetSpriteIndex);
			break;}
		}
	}

	return TRUE;
}
