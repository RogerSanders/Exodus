#include "SpriteListDetailsView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::SpriteListDetailsView::SpriteListDetailsView(S315_5313* adevice, unsigned int aspriteIndex)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	spriteIndex = aspriteIndex;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Sprite Details");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_S315_5313_SPRITELIST_DETAILS));
}

//----------------------------------------------------------------------------------------
//Sprite address functions
//----------------------------------------------------------------------------------------
void S315_5313::SpriteListDetailsView::SetSpriteIndex(unsigned int aspriteIndex)
{
	spriteIndex = aspriteIndex;
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::SpriteListDetailsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR S315_5313::SpriteListDetailsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	spriteMapping = device->GetSpriteMappingTableEntry(spriteIndex);

	PostMessage(hwnd, WM_TIMER, 0, 0);
	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::SpriteListDetailsView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::SpriteListDetailsView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	spriteMapping = device->GetSpriteMappingTableEntry(spriteIndex);
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
	CheckDlgButton(hwnd, IDC_VDP_SPRITELIST_DETAILS_P, spriteMapping.priority? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_SPRITELIST_DETAILS_HFLIP, spriteMapping.hflip? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_SPRITELIST_DETAILS_VFLIP, spriteMapping.vflip? BST_CHECKED: BST_UNCHECKED);
	if((currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_C) && (currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_C_DEC))
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_C, 1, spriteMapping.paletteLine);
		UpdateDlgItemBin(hwnd, IDC_VDP_SPRITELIST_DETAILS_C_DEC, spriteMapping.paletteLine);
	}
	if(currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_WORD0)
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_WORD0, 4, spriteMapping.rawDataWord0.GetData());
	}
	if(currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_WORD1)
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_WORD1, 4, spriteMapping.rawDataWord1.GetData());
	}
	if(currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_WORD2)
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_WORD2, 4, spriteMapping.rawDataWord2.GetData());
	}
	if(currentControlFocus != IDC_VDP_SPRITELIST_DETAILS_WORD3)
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_WORD3, 4, spriteMapping.rawDataWord3.GetData());
	}

	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::SpriteListDetailsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		case IDC_VDP_SPRITELIST_DETAILS_P:
			spriteMapping.priority = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_SPRITELIST_DETAILS_HFLIP:
			spriteMapping.hflip = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_SPRITELIST_DETAILS_VFLIP:
			spriteMapping.vflip = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		}
		device->SetSpriteMappingTableEntry(spriteIndex, spriteMapping, true);
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		bool writeSeparatedData = true;
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			switch(LOWORD(wparam))
			{
			case IDC_VDP_SPRITELIST_DETAILS_INDEX:
				spriteIndex = GetDlgItemHex(hwnd, LOWORD(wparam));
				spriteMapping = device->GetSpriteMappingTableEntry(spriteIndex);
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
			case IDC_VDP_SPRITELIST_DETAILS_WORD0:
				spriteMapping.rawDataWord0 = GetDlgItemHex(hwnd, LOWORD(wparam));
				writeSeparatedData = false;
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_WORD0, 4, spriteMapping.rawDataWord0.GetData());
				break;
			case IDC_VDP_SPRITELIST_DETAILS_WORD1:
				spriteMapping.rawDataWord1 = GetDlgItemHex(hwnd, LOWORD(wparam));
				writeSeparatedData = false;
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_WORD1, 4, spriteMapping.rawDataWord1.GetData());
				break;
			case IDC_VDP_SPRITELIST_DETAILS_WORD2:
				spriteMapping.rawDataWord2 = GetDlgItemHex(hwnd, LOWORD(wparam));
				writeSeparatedData = false;
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_WORD2, 4, spriteMapping.rawDataWord2.GetData());
				break;
			case IDC_VDP_SPRITELIST_DETAILS_WORD3:
				spriteMapping.rawDataWord3 = GetDlgItemHex(hwnd, LOWORD(wparam));
				writeSeparatedData = false;
				UpdateDlgItemHex(hwnd, IDC_VDP_SPRITELIST_DETAILS_WORD3, 4, spriteMapping.rawDataWord3.GetData());
				break;
			}
		}

		device->SetSpriteMappingTableEntry(spriteIndex, spriteMapping, writeSeparatedData);
	}

	return TRUE;
}
