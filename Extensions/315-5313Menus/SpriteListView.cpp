#include "SpriteListView.h"
#include "resource.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "WindowsControls/WindowsControls.pkg"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SpriteListView::SpriteListView(IUIManager& auiManager, SpriteListViewPresenter& apresenter, IS315_5313& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	fontHandle = NULL;
	hwndLayoutGrid = NULL;
	hwndDataGrid = NULL;
	SetWindowSettings(apresenter.GetUnqualifiedViewTitle(), 0, 0, 640, 480);
	SetDockableViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT SpriteListView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT SpriteListView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the default font for this window
	int fontPointSize = 8;
	HDC hdc = GetDC(hwnd);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);
	std::wstring fontTypefaceName = L"MS Shell Dlg";
	fontHandle = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &fontTypefaceName[0]);

	//Register the DataGrid window class
	WC_DataGrid::RegisterWindowClass(GetAssemblyHandle());

	//Create the DataGrid child controls
	hwndDataGrid = CreateWindowEx(WS_EX_CLIENTEDGE, WC_DataGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, (HMENU)DATALISTCONTROL, GetAssemblyHandle(), NULL);

	//Set the default font for the child controls
	SendMessage(hwndDataGrid, WM_SETFONT, (WPARAM)fontHandle, (LPARAM)TRUE);
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)fontHandle, (LPARAM)TRUE);

	//Insert our columns into the data grid
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"No", COLUMN_NO));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"XPos", COLUMN_XPOS, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"YPos", COLUMN_YPOS, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Tiles", COLUMN_TILES, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Link", COLUMN_LINK, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"HSize", COLUMN_HSIZE, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"VSize", COLUMN_VSIZE, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"P", COLUMN_P, true, WC_DataGrid::CellControlType::CheckBox));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"HFlip", COLUMN_HFLIP, true, WC_DataGrid::CellControlType::CheckBox));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"VFlip", COLUMN_VFLIP, true, WC_DataGrid::CellControlType::CheckBox));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"C", COLUMN_C, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Word1", COLUMN_RAW0, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Word2", COLUMN_RAW1, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Word3", COLUMN_RAW2, true));
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Word4", COLUMN_RAW3, true));

	//Explicitly insert our list of sprite entries into the data grid
	WC_DataGrid::Grid_InsertRows insertRowsInfo;
	insertRowsInfo.targetRowNo = 0;
	insertRowsInfo.rowCount = 80;
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertRows, 0, (LPARAM)&insertRowsInfo);

	//Register the LayoutGrid window class
	WC_LayoutGrid::RegisterWindowClass(GetAssemblyHandle());

	//Create the LayoutGrid child control
	hwndLayoutGrid = CreateWindowEx(WS_EX_TRANSPARENT, WC_LayoutGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, GetAssemblyHandle(), NULL);

	//Insert our rows and columns into the layout grid
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddColumn, 0, (LPARAM)&(const WC_LayoutGrid::AddColumnParams&)WC_LayoutGrid::AddColumnParams(WC_LayoutGrid::SizeMode::Proportional));

	//Add each child control to the layout grid
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(hwndDataGrid, 0, 0));

	//Start the refresh timer
	SetTimer(hwnd, 1, 100, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT SpriteListView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Delete the default font object. Note that since our window has been removed from the
	//screen at this point, it should be safe to delete this object here even though child
	//controls still hold references to it, as long as we don't attempt to use the font
	//handle while the window is being destroyed.
	DeleteObject(fontHandle);
	KillTimer(hwnd, 1);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT SpriteListView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Read the new client size of the window
	RECT rect;
	GetClientRect(hwnd, &rect);
	int controlWidth = rect.right;
	int controlHeight = rect.bottom;

	//Resize the layout grid to the desired width and height
	SetWindowPos(hwndLayoutGrid, NULL, 0, 0, controlWidth, controlHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT SpriteListView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Update the data grid with the latest text
	unsigned int spriteMappingBaseAddress = model.RegGetNameTableBaseSprite();
	std::map<unsigned int, std::map<unsigned int, std::wstring>> rowText;
	for(unsigned int i = 0; i < 80; ++i)
	{
		IS315_5313::SpriteMappingTableEntry mapping = model.GetSpriteMappingTableEntry(spriteMappingBaseAddress, i);
		std::map<unsigned int, std::wstring>& columnText = rowText[i];
		IntToStringBase16(i, columnText[COLUMN_NO], 2, false);
		IntToStringBase16(mapping.xpos, columnText[COLUMN_XPOS], 4, false);
		IntToStringBase16(mapping.ypos, columnText[COLUMN_YPOS], 4, false);
		IntToStringBase16(mapping.blockNumber, columnText[COLUMN_TILES], 3, false);
		IntToStringBase16(mapping.link, columnText[COLUMN_LINK], 2, false);
		IntToStringBase10(mapping.width, columnText[COLUMN_HSIZE]);
		IntToStringBase10(mapping.height, columnText[COLUMN_VSIZE]);
		BoolToString(mapping.priority, columnText[COLUMN_P], true);
		BoolToString(mapping.hflip, columnText[COLUMN_HFLIP], true);
		BoolToString(mapping.vflip, columnText[COLUMN_VFLIP], true);
		IntToStringBase10(mapping.paletteLine, columnText[COLUMN_C]);
		IntToStringBase16(mapping.rawDataWord0.GetData(), columnText[COLUMN_RAW0], 4, false);
		IntToStringBase16(mapping.rawDataWord1.GetData(), columnText[COLUMN_RAW1], 4, false);
		IntToStringBase16(mapping.rawDataWord2.GetData(), columnText[COLUMN_RAW2], 4, false);
		IntToStringBase16(mapping.rawDataWord3.GetData(), columnText[COLUMN_RAW3], 4, false);
	}
	SendMessage(hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::UpdateMultipleRowText, 0, (LPARAM)&rowText);

	return TRUE;
}

//----------------------------------------------------------------------------------------
LRESULT SpriteListView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(LOWORD(wparam) == DATALISTCONTROL)
	{
		if((WC_DataGrid::WindowNotifications)HIWORD(wparam) == WC_DataGrid::WindowNotifications::CellEdit)
		{
			WC_DataGrid::Grid_CellEditEvent* cellEditEventInfo = (WC_DataGrid::Grid_CellEditEvent*)lparam;
			unsigned int spriteMappingBaseAddress = model.RegGetNameTableBaseSprite();
			IS315_5313::SpriteMappingTableEntry spriteMapping = model.GetSpriteMappingTableEntry(spriteMappingBaseAddress, cellEditEventInfo->targetRowNo);
			bool useSeparatedData = true;
			switch(cellEditEventInfo->targetColumnID)
			{
			case COLUMN_XPOS:
				StringToIntBase16(cellEditEventInfo->newData, spriteMapping.xpos);
				break;
			case COLUMN_YPOS:
				StringToIntBase16(cellEditEventInfo->newData, spriteMapping.ypos);
				break;
			case COLUMN_TILES:
				StringToIntBase16(cellEditEventInfo->newData, spriteMapping.blockNumber);
				break;
			case COLUMN_LINK:
				StringToIntBase16(cellEditEventInfo->newData, spriteMapping.link);
				break;
			case COLUMN_HSIZE:
				StringToIntBase16(cellEditEventInfo->newData, spriteMapping.width);
				break;
			case COLUMN_VSIZE:
				StringToIntBase16(cellEditEventInfo->newData, spriteMapping.height);
				break;
			case COLUMN_P:
				StringToBool(cellEditEventInfo->newData, spriteMapping.priority);
				break;
			case COLUMN_HFLIP:
				StringToBool(cellEditEventInfo->newData, spriteMapping.hflip);
				break;
			case COLUMN_VFLIP:
				StringToBool(cellEditEventInfo->newData, spriteMapping.vflip);
				break;
			case COLUMN_C:
				StringToIntBase16(cellEditEventInfo->newData, spriteMapping.paletteLine);
				break;
			case COLUMN_RAW0:{
				unsigned int newData;
				StringToIntBase16(cellEditEventInfo->newData, newData);
				spriteMapping.rawDataWord0 = newData;
				useSeparatedData = false;
				break;}
			case COLUMN_RAW1:{
				unsigned int newData;
				StringToIntBase16(cellEditEventInfo->newData, newData);
				spriteMapping.rawDataWord1 = newData;
				useSeparatedData = false;
				break;}
			case COLUMN_RAW2:{
				unsigned int newData;
				StringToIntBase16(cellEditEventInfo->newData, newData);
				spriteMapping.rawDataWord2 = newData;
				useSeparatedData = false;
				break;}
			case COLUMN_RAW3:{
				unsigned int newData;
				StringToIntBase16(cellEditEventInfo->newData, newData);
				spriteMapping.rawDataWord3 = newData;
				useSeparatedData = false;
				break;}
			}
			model.SetSpriteMappingTableEntry(spriteMappingBaseAddress, cellEditEventInfo->targetRowNo, spriteMapping, useSeparatedData);
		}
	}
	return TRUE;
}
