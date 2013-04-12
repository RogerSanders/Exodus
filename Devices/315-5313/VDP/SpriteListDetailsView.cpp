#include "SpriteListDetailsView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::SpriteListDetailsView::SpriteListDetailsView(VDP* adevice, unsigned int aspriteMappingAddress, unsigned int aspriteIndex)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	spriteMappingAddress = aspriteMappingAddress;
	spriteIndex = aspriteIndex;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Sprite Details");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_SPRITELIST_DETAILS));
}

//----------------------------------------------------------------------------------------
//Sprite address functions
//----------------------------------------------------------------------------------------
void VDP::SpriteListDetailsView::SetSpriteAddress(unsigned int aspriteMappingAddress, unsigned int aspriteIndex)
{
	spriteMappingAddress = aspriteMappingAddress;
	spriteIndex = aspriteIndex;
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR VDP::SpriteListDetailsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR VDP::SpriteListDetailsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	spriteMapping = device->GetSpriteMapping(spriteMappingAddress + (spriteIndex * device->GetSpriteMappingSize()));

	PostMessage(hwnd, WM_TIMER, 0, 0);
	SetTimer(hwnd, 1, 500, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::SpriteListDetailsView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::SpriteListDetailsView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	spriteMapping = device->GetSpriteMapping(spriteMappingAddress + (spriteIndex * device->GetSpriteMappingSize()));
	if(currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_LOCATION)
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_LOCATION, 4, spriteMappingAddress);
	}
	if(currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_INDEX)
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_INDEX, 4, spriteIndex);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_XPOS) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_XPOS_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_XPOS, 4, spriteMapping.xpos);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_XPOS_DEC, spriteMapping.xpos);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_YPOS) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_YPOS_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_YPOS, 4, spriteMapping.ypos);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_YPOS_DEC, spriteMapping.ypos);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_TILES) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_TILES_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_TILES, 3, spriteMapping.blockNumber);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_TILES_DEC, spriteMapping.blockNumber);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_LINK) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_LINK_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_LINK, 2, spriteMapping.link);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_LINK_DEC, spriteMapping.link);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_HSIZE) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_HSIZE_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_HSIZE, 1, spriteMapping.width);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_HSIZE_DEC, spriteMapping.width);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_VSIZE) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_VSIZE_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_VSIZE, 1, spriteMapping.height);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_VSIZE_DEC, spriteMapping.height);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_P) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_P_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_P, 1, spriteMapping.priority);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_P_DEC, spriteMapping.priority);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_HFLIP) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_HFLIP_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_HFLIP, 1, spriteMapping.hflip);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_HFLIP_DEC, spriteMapping.hflip);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_VFLIP) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_VFLIP_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_VFLIP, 1, spriteMapping.vflip);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_VFLIP_DEC, spriteMapping.vflip);
	}
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_C) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_C_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_C, 1, spriteMapping.paletteLine);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_C_DEC, spriteMapping.paletteLine);
	}

	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::SpriteListDetailsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			switch(LOWORD(wparam))
			{
			case IDC_VDP_SPRITELIST_DETAILS_LOCATION:
				spriteMappingAddress = GetDlgItemHex(hwnd, LOWORD(wparam));
				spriteMapping = device->GetSpriteMapping(spriteMappingAddress + (spriteIndex * device->GetSpriteMappingSize()));
				break;
			case IDC_VDP_SPRITELIST_DETAILS_INDEX:
				spriteIndex	 = GetDlgItemHex(hwnd, LOWORD(wparam));
				spriteMapping = device->GetSpriteMapping(spriteMappingAddress + (spriteIndex * device->GetSpriteMappingSize()));
				break;
			case IDC_VDP_SPRITELIST_DETAILS_XPOS:
				spriteMapping.xpos = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_XPOS, 4, spriteMapping.xpos);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_XPOS_DEC, spriteMapping.xpos);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_XPOS_DEC:
				spriteMapping.xpos = GetDlgItemBin(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_XPOS, 4, spriteMapping.xpos);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_XPOS_DEC, spriteMapping.xpos);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_YPOS:
				spriteMapping.ypos = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_YPOS, 4, spriteMapping.ypos);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_YPOS_DEC, spriteMapping.ypos);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_YPOS_DEC:
				spriteMapping.ypos = GetDlgItemBin(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_YPOS, 4, spriteMapping.ypos);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_YPOS_DEC, spriteMapping.ypos);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_TILES:
				spriteMapping.blockNumber = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_TILES, 3, spriteMapping.blockNumber);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_TILES_DEC, spriteMapping.blockNumber);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_TILES_DEC:
				spriteMapping.blockNumber = GetDlgItemBin(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_TILES, 3, spriteMapping.blockNumber);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_TILES_DEC, spriteMapping.blockNumber);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_LINK:
				spriteMapping.link = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_LINK, 2, spriteMapping.link);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_LINK_DEC, spriteMapping.link);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_LINK_DEC:
				spriteMapping.link = GetDlgItemBin(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_LINK, 2, spriteMapping.link);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_LINK_DEC, spriteMapping.link);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_HSIZE:
				spriteMapping.width = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_HSIZE, 1, spriteMapping.width);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_HSIZE_DEC, spriteMapping.width);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_HSIZE_DEC:
				spriteMapping.width = GetDlgItemBin(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_HSIZE, 1, spriteMapping.width);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_HSIZE_DEC, spriteMapping.width);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_VSIZE:
				spriteMapping.height = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_VSIZE, 1, spriteMapping.height);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_VSIZE_DEC, spriteMapping.height);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_VSIZE_DEC:
				spriteMapping.height = GetDlgItemBin(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_VSIZE, 1, spriteMapping.height);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_VSIZE_DEC, spriteMapping.height);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_P:
				spriteMapping.priority = (GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_P, 1, spriteMapping.priority);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_P_DEC, spriteMapping.priority);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_P_DEC:
				spriteMapping.priority = (GetDlgItemBin(hwnd, LOWORD(wparam)) != 0);
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_P, 1, spriteMapping.priority);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_P_DEC, spriteMapping.priority);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_HFLIP:
				spriteMapping.hflip = (GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_HFLIP, 1, spriteMapping.hflip);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_HFLIP_DEC, spriteMapping.hflip);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_HFLIP_DEC:
				spriteMapping.hflip = (GetDlgItemBin(hwnd, LOWORD(wparam)) != 0);
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_HFLIP, 1, spriteMapping.hflip);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_HFLIP_DEC, spriteMapping.hflip);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_VFLIP:
				spriteMapping.vflip = (GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_VFLIP, 1, spriteMapping.vflip);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_VFLIP_DEC, spriteMapping.vflip);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_VFLIP_DEC:
				spriteMapping.vflip = (GetDlgItemBin(hwnd, LOWORD(wparam)) != 0);
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_VFLIP, 1, spriteMapping.vflip);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_VFLIP_DEC, spriteMapping.vflip);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_C:
				spriteMapping.paletteLine = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_C, 1, spriteMapping.paletteLine);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_C_DEC, spriteMapping.paletteLine);
				break;
			case IDC_VDP_SPRITELIST_DETAILS_C_DEC:
				spriteMapping.paletteLine = GetDlgItemBin(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_C, 1, spriteMapping.paletteLine);
				UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_C_DEC, spriteMapping.paletteLine);
				break;
			}
		}

		device->SetSpriteMapping(spriteMappingAddress + (spriteIndex * device->GetSpriteMappingSize()), spriteMapping);
	}

	return TRUE;
}
