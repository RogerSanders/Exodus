#include "WC_DataGrid.h"
#include "DataConversion/DataConversion.pkg"
#include <locale>
#include <algorithm>
//##DEBUG##
#include <iostream>

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* WC_DataGrid::windowClassName = L"EX_DataGrid";

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WC_DataGrid::WC_DataGrid(HINSTANCE amoduleHandle, HWND ahwnd)
:moduleHandle(amoduleHandle), hwnd(ahwnd)
{
	hwndHeader = NULL;
	hwndComboBoxList = NULL;
	controlFont = NULL;
	dataAreaFont = NULL;

	userColorData.backgroundColorDefined = false;
	userColorData.textColorDefined = false;
	userColorData.editColorDefined = false;
	userColorData.lineColorDefined = false;
	defaultColorData.colorBackground = WinColor(255, 255, 255);
	defaultColorData.colorTextFront = WinColor(0, 0, 0);
	defaultColorData.colorTextBack = defaultColorData.colorBackground;
	defaultColorData.colorEditTextFront = WinColor(255, 0, 0);
	defaultColorData.colorEditTextBack = defaultColorData.colorBackground;
	defaultColorData.colorLine = WinColor(128, 128, 255);

	controlWidth = 0;
	controlHeight = 0;
	headerWidth = 0;
	headerHeight = 0;
	headerPosX = 0;
	headerPosY = 0;
	visibleRows = 1;
	fullyVisibleRows = 1;

	fontWidth = 1;
	fontHeight = 1;
	marginSize = 2;

	autoScrollingManagement = true;
	vscrollMin = 0;
	vscrollMax = 0;
	vscrollMaxTrueLimit = 0;
	vscrollCurrent = 0;
	vscrollValuesPerPage = 0;
	currentScrollHOffset = 0;

	rowSelected = false;
	columnSelected = false;
	editModeActive = false;
	dragSelectActive = false;
	editModeSelectionPosChangePending = false;
	editModeSelectionPosChangePendingInPixels = false;
	ignoreDragSelectStart = false;
	editModeShowCaretPending = false;
	mouseButtonDown = false;
	editCellHorizontalPixelOffset = 0;

	largestColumnDataArraySize = 0;
}

//----------------------------------------------------------------------------------------
WC_DataGrid::~WC_DataGrid()
{
	//Delete any alloated brush objects for embedded cell controls
	for(std::map<unsigned int, std::vector<CellControlInfo>>::iterator columnIterator = customControlForCell.begin(); columnIterator != customControlForCell.end(); ++columnIterator)
	{
		std::vector<CellControlInfo>& rowData = columnIterator->second;
		for(size_t i = 0; i < rowData.size(); ++i)
		{
			CellControlInfo& cellData = rowData[i];
			if(cellData.lastBackgroundColorSet)
			{
				DeleteObject(cellData.lastBackgroundColorBrush);
				cellData.lastBackgroundColorBrush = NULL;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//Class registration
//----------------------------------------------------------------------------------------
bool WC_DataGrid::RegisterWindowClass(HINSTANCE amoduleHandle)
{
	//Attempt to register the window class for this control, and return the result to the
	//caller.
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = amoduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = windowClassName;
	wc.hIconSm       = NULL;
	ATOM registerClassExReturn = RegisterClassEx(&wc);
	return (registerClassExReturn != 0);
}

//----------------------------------------------------------------------------------------
bool WC_DataGrid::UnregisterWindowClass(HINSTANCE amoduleHandle)
{
	//Attempt to unregister the window class for this control, and return the result to
	//the caller.
	BOOL unregisterClassReturn = UnregisterClass(windowClassName, amoduleHandle);
	return (unregisterClassReturn != 0);
}

//----------------------------------------------------------------------------------------
//Drag selection functions
//----------------------------------------------------------------------------------------
void WC_DataGrid::UpdateDragSelectPos(int newSelectPosSigned)
{
	unsigned int newSelectPos;
	if(newSelectPosSigned < 0)
	{
		newSelectPos = 0;
	}
	else if(newSelectPosSigned > (int)editBuffer.size())
	{
		newSelectPos = (int)editBuffer.size();
	}
	else
	{
		newSelectPos = (unsigned int)newSelectPosSigned;
	}

	if(newSelectPos < selectedCharacterNo)
	{
		dragSelectStartPos = newSelectPos;
		dragSelectEndPos = selectedCharacterNo;
	}
	else
	{
		dragSelectStartPos = selectedCharacterNo;
		dragSelectEndPos = newSelectPos;
	}
}

//----------------------------------------------------------------------------------------
unsigned int WC_DataGrid::GetDragSelectPos() const
{
	if(dragSelectEndPos != selectedCharacterNo)
	{
		return dragSelectEndPos;
	}
	else
	{
		return dragSelectStartPos;
	}
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::StartDragSelect()
{
	if(!dragSelectActive && !ignoreDragSelectStart)
	{
		dragSelectActive = true;
		dragSelectStartPos = selectedCharacterNo;
		dragSelectEndPos = selectedCharacterNo;
	}
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::StopDragSelect()
{
	dragSelectActive = false;
}

//----------------------------------------------------------------------------------------
bool WC_DataGrid::DragSelectActive() const
{
	return dragSelectActive;
}

//----------------------------------------------------------------------------------------
//Clipboard functions
//----------------------------------------------------------------------------------------
bool WC_DataGrid::CopyToClipboard()
{
	if(!DragSelectActive())
	{
		return false;
	}
	if(OpenClipboard(hwnd) == 0)
	{
		return false;
	}

	//Copy data to the clipboard
	unsigned int dragSelectTextSize = dragSelectEndPos - dragSelectStartPos;
	HGLOBAL clipboardDataHandle = GlobalAlloc(GMEM_MOVEABLE, (dragSelectTextSize + 1) * sizeof(wchar_t));
	if(clipboardDataHandle == NULL)
	{
		CloseClipboard();
		return false;
	}

	wchar_t* clipboardData = (wchar_t*)GlobalLock(clipboardDataHandle);
	if(clipboardData != NULL)
	{
		for(unsigned int i = 0; i < dragSelectTextSize; ++i)
		{
			*(clipboardData + i) = editBuffer[dragSelectStartPos + i];
		}
		*(clipboardData + dragSelectTextSize) = 0;
		GlobalUnlock(clipboardDataHandle);
		SetClipboardData(CF_UNICODETEXT, clipboardDataHandle);
	}

	CloseClipboard();
	return true;
}

//----------------------------------------------------------------------------------------
bool WC_DataGrid::PasteFromClipboard()
{
	if(IsClipboardFormatAvailable(CF_UNICODETEXT) == 0)
	{
		return false;
	}
	if(OpenClipboard(hwnd) == 0)
	{
		return false;
	}

	HANDLE clipboardDataHandle = GetClipboardData(CF_UNICODETEXT);
	if(clipboardDataHandle == NULL)
	{
		CloseClipboard();
		return false;
	}

	wchar_t* clipboardData = (wchar_t*)GlobalLock(clipboardDataHandle);
	if(clipboardData != NULL)
	{
		unsigned int pastePos = selectedCharacterNo;
		if(DragSelectActive())
		{
			pastePos = dragSelectStartPos;
			editBuffer.erase(dragSelectStartPos, dragSelectEndPos - dragSelectStartPos);
			SelectCharacter(dragSelectStartPos);
			StopDragSelect();
		}

		while(*clipboardData != 0)
		{
			wchar_t nextChar = *clipboardData;
			editBuffer.insert(selectedCharacterNo, 1, nextChar);
			SelectCharacter(selectedCharacterNo + 1);
			++clipboardData;
		}
		GlobalUnlock(clipboardDataHandle);

		ForceEditCellIntoView();
	}

	CloseClipboard();
	return true;
}

//----------------------------------------------------------------------------------------
//Message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK WC_DataGrid::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		WC_DataGrid* object = (WC_DataGrid*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->WndProcPrivate(message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		WC_DataGrid* object = new WC_DataGrid((HINSTANCE)createStruct->lpCreateParams, hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->WndProcPrivate(message, wParam, lParam);}
	case WM_DESTROY:{
		WC_DataGrid* object = (WC_DataGrid*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			LPARAM result = object->WndProcPrivate(message, wParam, lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
			delete object;
			return result;
		}
		break;}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		return msgWM_CREATE(wParam, lParam);
	case WM_SIZE:
		return msgWM_SIZE(wParam, lParam);
	case WM_NOTIFY:
		return msgWM_NOTIFY(wParam, lParam);
	case WM_ERASEBKGND:
		return msgWM_ERASEBKGND(wParam, lParam);
	case WM_PAINT:
		return msgWM_PAINT(wParam, lParam);
	case WM_PRINTCLIENT:
		return msgWM_PRINTCLIENT(wParam, lParam);
	case WM_COMMAND:
		return msgWM_COMMAND(wParam, lParam);
	case WM_SETFONT:
		return msgWM_SETFONT(wParam, lParam);
	case WM_VSCROLL:
		return msgWM_VSCROLL(wParam, lParam);
	case WM_HSCROLL:
		return msgWM_HSCROLL(wParam, lParam);
	case WM_KEYDOWN:
		return msgWM_KEYDOWN(wParam, lParam);
	case WM_CHAR:
		return msgWM_CHAR(wParam, lParam);
	case WM_KILLFOCUS:
		return msgWM_KILLFOCUS(wParam, lParam);
	case WM_SETFOCUS:
		return msgWM_SETFOCUS(wParam, lParam);
	case WM_MOUSEMOVE:
		return msgWM_MOUSEMOVE(wParam, lParam);
	case WM_LBUTTONUP:
		return msgWM_LBUTTONUP(wParam, lParam);
	case WM_LBUTTONDOWN:
		return msgWM_LBUTTONDOWN(wParam, lParam);
	case WM_MOUSEWHEEL:
		return msgWM_MOUSEWHEEL(wParam, lParam);
	case WM_BOUNCE:
		return msgWM_BOUNCE(wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return msgWM_CTLCOLORSTATIC(wParam, lParam);

	case (unsigned int)WindowMessages::SetManualScrolling:
		return msgGRID_SETMANUALSCROLLING(wParam, lParam);
	case (unsigned int)WindowMessages::SetDataAreaFont:
		return msgGRID_SETDATAAREAFONT(wParam, lParam);
	case (unsigned int)WindowMessages::InsertColumn:
		return msgGRID_INSERTCOLUMN(wParam, lParam);
	case (unsigned int)WindowMessages::DeleteColumn:
		return msgGRID_DELETECOLUMN(wParam, lParam);
	case (unsigned int)WindowMessages::InsertRows:
		return msgGRID_INSERTROWS(wParam, lParam);
	case (unsigned int)WindowMessages::DeleteRows:
		return msgGRID_DELETEROWS(wParam, lParam);
	case (unsigned int)WindowMessages::SetColumnInfo:
		return msgGRID_SETCOLUMNINFO(wParam, lParam);
	case (unsigned int)WindowMessages::GetColumnInfo:
		return msgGRID_GETCOLUMNINFO(wParam, lParam);
	case (unsigned int)WindowMessages::SetCellInfo:
		return msgGRID_SETCELLINFO(wParam, lParam);
	case (unsigned int)WindowMessages::UpdateCellText:
		return msgGRID_UPDATECELLTEXT(wParam, lParam);
	case (unsigned int)WindowMessages::UpdateColumnText:
		return msgGRID_UPDATECOLUMNTEXT(wParam, lParam);
	case (unsigned int)WindowMessages::SetControlColor:
		return msgGRID_SETCONTROLCOLOR(wParam, lParam);
	case (unsigned int)WindowMessages::SetRowColor:
		return msgGRID_SETROWCOLOR(wParam, lParam);
	case (unsigned int)WindowMessages::SetCellColor:
		return msgGRID_SETCELLCOLOR(wParam, lParam);
	case (unsigned int)WindowMessages::GetRowCount:
		return msgGRID_GETROWCOUNT(wParam, lParam);
	case (unsigned int)WindowMessages::SetVScrollInfo:
		return msgGRID_SETVSCROLLINFO(wParam, lParam);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::RebuildHeaderOrder()
{
	//Rebuild the sorted column list
	columnSortIndex.clear();
	for(ColumnDataIterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		//Query the new entry for the actual item order
		HDITEM hdItem;
		hdItem.mask = HDI_ORDER;
		LRESULT getItemResult = SendMessage(hwndHeader, HDM_GETITEM, (WPARAM)i->index, (LPARAM)&hdItem);
		if(getItemResult == TRUE)
		{
			//Update the stored order number for the column
			i->order = hdItem.iOrder;

			//Add the column into the column sort index
			columnSortIndex.insert(ColumnSortIndexEntry(i->order, &(*i)));
		}
	}

	//If edit mode is active, flag that a selection pos change event is now pending, since
	//a change in the column order requires us to update the current caret position.
	editModeSelectionPosChangePending = editModeActive;
	editModeSelectionPosChangePendingInPixels = false;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
{
	//Initialize common controls
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_WIN95_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	//Initialize the header
	hwndHeader = CreateWindowEx(0, WC_HEADER, L"", WS_CHILD | WS_VISIBLE | HDS_FULLDRAG | HDS_HOTTRACK | HDS_DRAGDROP, 0, 0, 0, 0, hwnd, (HMENU)IDC_HEADER, moduleHandle, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	int newClientWidth = rect.right;
	int newClientHeight = rect.bottom;

	//If this control has changed in size, process the size change event.
	if((controlWidth != newClientWidth) || (controlHeight != newClientHeight))
	{
		//Update the stored width and height of the control
		controlWidth = newClientWidth;
		controlHeight = newClientHeight;

		//Update the header position and size
		WINDOWPOS headerPos;
		HDLAYOUT headerLayout;
		headerLayout.prc = &rect;
		headerLayout.pwpos = &headerPos;
		SendMessage(hwndHeader, HDM_LAYOUT, 0, (LPARAM)&headerLayout);
		headerWidth = headerPos.cx;
		headerHeight = headerPos.cy;
		headerPosX = headerPos.x;
		headerPosY = headerPos.y;
		MoveWindow(hwndHeader, headerPosX - currentScrollHOffset, headerPosY, headerWidth + currentScrollHOffset, headerHeight, TRUE);

		//Update size information based on the new control width and height
		UpdateWindowSize();

		//Recalculate all automatically sized column widths
		RecalculateColumnWidths();
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_NOTIFY(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = (NMHDR*)lParam;
	if(nmhdr->idFrom == IDC_HEADER)
	{
		if(nmhdr->code == HDN_ITEMCHANGED)
		{
			//If the user has resized a column, update the column width
			NMHEADER* nmHeader = (NMHEADER*)lParam;
			ColumnIndexIterator i = columnIndex.find(nmHeader->iItem);
			if(i != columnIndex.end())
			{
				ColumnData& column = *i->second;
				if(!column.manuallySettingColumnWidth && ((nmHeader->pitem->mask & HDI_WIDTH) != 0))
				{
					//Since the user has manually sized this column, force the column into
					//absolute sizing mode, and set the absolute width of the column to
					//the new width of the column header.
					column.sizeMode = ColumnSizeMode::Absolute;
					column.absoluteWidth = nmHeader->pitem->cxy;
					column.actualWidth = column.absoluteWidth;

					//Recalculate all automatically sized column widths
					RecalculateColumnWidths();
				}
			}

			//Update the current scrollbar state
			UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);

			//Force the control to redraw
			ForceControlRedraw();
		}
		if(nmhdr->code == NM_RELEASEDCAPTURE)
		{
			//If the user has possibly changed the column order, update our column order
			//index. Note that we use this notification rather than HDN_ENDDRAG since the
			//HDN_ENDDRAG notification is sent before the order numbers are updated
			//within the header control, so requesting the order numbers for each element
			//in the header returns the old order numbers.
			RebuildHeaderOrder();

			//Force the control to redraw
			ForceControlRedraw();
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam)
{
	//We handle the WM_ERASEBKGND message here and return 0 to prevent the background
	//being erased by the default window procedure. Returning zero here will leave the
	//existing window content intact, and instead leave an erase pending for our WM_PAINT
	//message handler to process, which is what we want. Without processing this message,
	//there's a noticeable flicker when redrawing the control where the background is
	//erased before the WM_PAINT message is issued, such as when the control is resized.
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_PAINT(WPARAM wParam, LPARAM lParam)
{
	//Update our child controls before beginning the paint process. We need to do this
	//here before calling BeginPaint below so that the clipping region for our paint
	//operation is correctly set to exclude our child controls.
	UpdateChildControls();

	//Begin the paint operation
	PAINTSTRUCT paintInfo;
	BeginPaint(hwnd, &paintInfo);
	HDC hdc = paintInfo.hdc;

	//Create a bitmap we can render the control onto
	HDC hdcControl = hdc;
	HDC hdcBitmap = CreateCompatibleDC(hdcControl);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdcControl, controlWidth, controlHeight);
	HBITMAP hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

	//Send a WM_PRINTCLIENT message to the native control and get it to render into the
	//bitmap
	SendMessage(hwnd, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_CLIENT | PRF_NONCLIENT | PRF_CHILDREN);

	//Transfer the final bitmap for the control into the screen buffer
	BitBlt(hdcControl, 0, headerHeight, controlWidth, controlHeight, hdcBitmap, 0, headerHeight, SRCCOPY);

	//Clean up the allocated handles
	SelectObject(hdcBitmap, hbitmapOriginal);
	DeleteObject(hbitmap);
	DeleteDC(hdcBitmap);

	//Calculate the over-render amount for the header control. Over-render ensures the
	//header always extends all the way across the top of our control, even when we pass
	//the end of the last column.
	int totalHeaderWidth = (int)GetTotalRowWidth();
	int overRenderWidth = controlWidth - (totalHeaderWidth - currentScrollHOffset);
	if(overRenderWidth < 0)
	{
		overRenderWidth = 0;
	}

	//Render the header into the screen buffer
	hdcControl = hdc;
	hdcBitmap = CreateCompatibleDC(hdcControl);
	hbitmap = CreateCompatibleBitmap(hdcControl, totalHeaderWidth + overRenderWidth, headerHeight);
	hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

	//Send a WM_PRINTCLIENT message to the header control and get it to render into the
	//bitmap
	SendMessage(hwndHeader, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_CLIENT | PRF_NONCLIENT | PRF_CHILDREN);

	//Transfer the final bitmap for the control into the screen buffer
	BitBlt(hdcControl, headerPosX, headerPosY, headerWidth  + overRenderWidth, headerHeight, hdcBitmap, currentScrollHOffset, 0, SRCCOPY);

	//Clean up the allocated handles
	SelectObject(hdcBitmap, hbitmapOriginal);
	DeleteObject(hbitmap);
	DeleteDC(hdcBitmap);

	//Validate the entire client area of the control. It's REALLY important that we
	//call this here, otherwise Windows is going to send WM_PAINT messages over and
	//over again waiting for the region to be validated.
	ValidateRect(hwnd, NULL);

	//Complete the paint operation
	EndPaint(hwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::UpdateChildControls()
{
	//Begin a session for processing this batch of window size changes for column
	//controls. Processing all the window size and position changes in a single operation
	//in this manner gives the best performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos(visibleRows);

	//Update child control info for each cell
	std::set<HWND> drawnChildControls;
	std::set<HWND> drawPendingChildControls;
	int gridContentStartPosX = headerPosX;
	int gridContentStartPosY = headerPosY + headerHeight;
	for(unsigned int row = 0; row < visibleRows; ++row)
	{
		//Determine the index of this row into our data arrays
		unsigned int rowDataArrayIndex = (!autoScrollingManagement)? row: (vscrollCurrent + row);

		//Update child control info for each column in this row
		int columnStartPosX = gridContentStartPosX - currentScrollHOffset;
		int columnStartPosY = gridContentStartPosY + ((int)row * (fontHeight + (marginSize * 2)));
		int columnEndPosY = columnStartPosY + fontHeight + (marginSize * 2);
		for(ColumnSortIndexIterator i = columnSortIndex.begin(); i != columnSortIndex.end(); ++i)
		{
			//Retrieve and calculate information for this cell
			ColumnData& columnData = *(i->second);
			int columnEndPosX = columnStartPosX + (int)columnData.actualWidth;
			bool editModeActiveForCell = (editModeActive && (selectedRowNo == rowDataArrayIndex) && (selectedColumnID == columnData.columnID));

			//Obtain a raw pointer to the character string to use for this cell
			const wchar_t* cellDataRaw = 0;
			unsigned int cellDataRawLength = 0;
			if(editModeActiveForCell)
			{
				cellDataRaw = editBuffer.c_str();
				cellDataRawLength = (unsigned int)editBuffer.size();
			}
			else
			{
				if(rowDataArrayIndex < (unsigned int)columnData.dataBuffer.size())
				{
					cellDataRaw = columnData.dataBuffer[rowDataArrayIndex].c_str();
					cellDataRawLength = (unsigned int)columnData.dataBuffer[rowDataArrayIndex].size();
				}
			}

			//If this cell is within the defined data region of the grid, update its child
			//control info.
			if(cellDataRaw != 0)
			{
				//Retrieve the control information for this cell, creating the data
				//structure if necessary.
				std::vector<CellControlInfo>& controlListForColumn = customControlForCell[columnData.columnID];
				if(rowDataArrayIndex >= controlListForColumn.size())
				{
					controlListForColumn.resize(rowDataArrayIndex+1, CellControlInfo(columnData.controlType));
				}
				CellControlInfo& cellControlInfo = controlListForColumn[rowDataArrayIndex];

				//If we already had a child control created for this cell, but it's now of
				//the wrong type, delete the existing child control for this cell.
				if((cellControlInfo.hwnd != NULL) && (cellControlInfo.controlType != cellControlInfo.createdControlType))
				{
					//Destroy the child control
					DestroyWindow(cellControlInfo.hwnd);

					//Delete any colour brush associated with this child control
					if(cellControlInfo.lastBackgroundColorSet)
					{
						DeleteObject(cellControlInfo.lastBackgroundColorBrush);
					}

					//Remove references to this child control
					childControlSet.erase(cellControlInfo.hwnd);
					childControlMapping.erase(cellControlInfo.hwnd);
					cellControlInfo.hwnd = NULL;
				}

				//Save the position and size of this cell
				cellControlInfo.cellPosX = columnStartPosX;
				cellControlInfo.cellPosY = columnStartPosY;
				cellControlInfo.cellWidth = columnEndPosX - columnStartPosX;
				cellControlInfo.cellHeight = columnEndPosY - columnStartPosY;

				//Calculate the latest position and size to use for the child control
				int newControlPosX = columnStartPosX;
				int newControlPosY = columnStartPosY + 1;
				int newControlWidth = (columnEndPosX - newControlPosX) - 1;
				int newControlHeight = columnEndPosY - newControlPosY;
				if(cellControlInfo.controlType == CellControlType::ComboBox)
				{
					int desiredControlWidth = newControlHeight;
					newControlPosX = (((newControlPosX + newControlWidth) - desiredControlWidth) < newControlPosX)? newControlPosX: ((newControlPosX + newControlWidth) - desiredControlWidth);
					newControlWidth = (((columnEndPosX - newControlPosX) - 1) < desiredControlWidth)? ((columnEndPosX - newControlPosX) - 1): desiredControlWidth;
				}
				else if(cellControlInfo.controlType == CellControlType::TreeEntry)
				{
					if(cellControlInfo.treeEntryIndentLevel > 0)
					{
						newControlPosX += (((int)cellControlInfo.treeEntryIndentLevel - 1) * fontHeight) + marginSize;
						newControlWidth = fontHeight;
					}
					else
					{
						newControlPosX = 0;
						newControlWidth = 0;
					}
				}
				else if(cellControlInfo.controlType == CellControlType::TextBoxWithButton)
				{
					HDC hdc = GetDC(hwnd);
					HFONT hfontOld = (HFONT)SelectObject(hdc, dataAreaFont);
					SIZE buttonTextSize;
					BOOL getTextExtentPoint32Return = GetTextExtentPoint32(hdc, cellControlInfo.customButtonText.c_str(), (int)cellControlInfo.customButtonText.size(), &buttonTextSize);
					if(getTextExtentPoint32Return != 0)
					{
						int desiredControlWidth = (buttonTextSize.cx + (marginSize * 2));
						desiredControlWidth = (desiredControlWidth < newControlHeight)? newControlHeight: desiredControlWidth;
						newControlPosX = (((newControlPosX + newControlWidth) - desiredControlWidth) < newControlPosX)? newControlPosX: ((newControlPosX + newControlWidth) - desiredControlWidth);
						newControlWidth = (((columnEndPosX - newControlPosX) - 1) < desiredControlWidth)? ((columnEndPosX - newControlPosX) - 1): desiredControlWidth;
					}
					SelectObject(hdc, hfontOld);
					ReleaseDC(hwnd, hdc);
				}

				//Create or reposition the child control for this cell as required
				if(cellControlInfo.hwnd == NULL)
				{
					if((cellControlInfo.controlType == CellControlType::CheckBox)
					|| (cellControlInfo.controlType == CellControlType::ComboBox)
					|| (cellControlInfo.controlType == CellControlType::Button)
					|| (cellControlInfo.controlType == CellControlType::TextBoxWithButton))
					{
						const wchar_t* buttonText = (cellControlInfo.controlType == CellControlType::TextBoxWithButton)? cellControlInfo.customButtonText.c_str(): (cellControlInfo.controlType == CellControlType::ComboBox)? L"\u25BC": L"";
						int windowStyle = (cellControlInfo.controlType == CellControlType::CheckBox)? WS_CHILD | WS_VISIBLE | BS_CHECKBOX: WS_CHILD | WS_VISIBLE;
						cellControlInfo.hwnd = CreateWindow(WC_BUTTON, buttonText, windowStyle, newControlPosX, newControlPosY, newControlWidth, newControlHeight, hwnd, NULL, moduleHandle, NULL);
						if(cellControlInfo.hwnd != NULL)
						{
							//Set the font for this child control
							SendMessage(cellControlInfo.hwnd, WM_SETFONT, (WPARAM)controlFont, FALSE);

							//Subclass the child control so we can intercept click events
							SetWindowSubclass(cellControlInfo.hwnd, BounceBackSubclassProc, 0, 0);

							//Save information on this created child control
							childControlSet.insert(cellControlInfo.hwnd);
							childControlMapping.insert(std::pair<HWND, CellMapping>(cellControlInfo.hwnd, CellMapping(columnData.columnID, rowDataArrayIndex)));
							cellControlInfo.createdControlType = cellControlInfo.controlType;

							//Add this control to the list of controls with a draw event
							//pending
							drawPendingChildControls.insert(cellControlInfo.hwnd);
						}
					}
				}
				else if((cellControlInfo.controlPosX != newControlPosX) || (cellControlInfo.controlPosY != newControlPosY) || (cellControlInfo.controlWidth != newControlWidth) || (cellControlInfo.controlHeight != newControlHeight))
				{
					//If this control has moved since it was last drawn, update the
					//position of the control.
					DeferWindowPos(deferWindowPosSession, cellControlInfo.hwnd, NULL, newControlPosX, newControlPosY, newControlWidth, newControlHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

					//Add this control to the list of controls with a draw event
					//pending
					drawPendingChildControls.insert(cellControlInfo.hwnd);
				}

				//Save the new control position and size as the last used position and
				//size for this control
				cellControlInfo.controlPosX = newControlPosX;
				cellControlInfo.controlPosY = newControlPosY;
				cellControlInfo.controlWidth = newControlWidth;
				cellControlInfo.controlHeight = newControlHeight;

				//If a control exists for this cell, ensure it is currently visible, and
				//add it to the list of child controls that were drawn in this paint
				//operation.
				if(cellControlInfo.hwnd != NULL)
				{
					DeferWindowPos(deferWindowPosSession, cellControlInfo.hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
					drawnChildControls.insert(cellControlInfo.hwnd);
				}
			}

			//Update the column start position for the next column
			columnStartPosX += (int)columnData.actualWidth;
		}
	}

	//Hide any child control windows that are not currently visible
	std::set<HWND> hiddenControls;
	std::set_difference(childControlSet.begin(), childControlSet.end(), drawnChildControls.begin(), drawnChildControls.end(), std::inserter(hiddenControls, hiddenControls.begin()));
	for(std::set<HWND>::const_iterator i = hiddenControls.begin(); i != hiddenControls.end(); ++i)
	{
		DeferWindowPos(deferWindowPosSession, *i, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_HIDEWINDOW);
	}

	//Process all the window size and position changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);

	//Process all pending draw events for any child controls now. Although we've
	//repositioned our child controls, they haven't actually redrawn yet. Requesting them
	//to update immediately now where a redraw is required, improves the visual appearance
	//of the redraw by making all the draw operations occur closer together.
	for(std::set<HWND>::const_iterator i = drawPendingChildControls.begin(); i != drawPendingChildControls.end(); ++i)
	{
		UpdateWindow(*i);
	}
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_PRINTCLIENT(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;

	//Determine which color values to use for various display elements
	WinColor backgroundColor    = userColorData.backgroundColorDefined? userColorData.colorBackground:    defaultColorData.colorBackground;
	WinColor textFrontColor     = userColorData.textColorDefined?       userColorData.colorTextFront:     defaultColorData.colorTextFront;
	WinColor textBackColor      = userColorData.textColorDefined?       userColorData.colorTextBack:      defaultColorData.colorTextBack;
	WinColor editTextFrontColor = userColorData.editColorDefined?       userColorData.colorEditTextFront: defaultColorData.colorEditTextFront;
	WinColor editTextBackColor  = userColorData.editColorDefined?       userColorData.colorEditTextBack:  defaultColorData.colorEditTextBack;
	WinColor lineColor          = userColorData.lineColorDefined?       userColorData.colorLine:          defaultColorData.colorLine;

	//Fill the window with the background colour
	HBRUSH backgroundBrush = CreateSolidBrush(backgroundColor.GetColorREF());
	HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
	PatBlt(hdc, 0, headerHeight, controlWidth, controlHeight, PATCOPY);
	SelectObject(hdc, backgroundBrushOld);
	DeleteObject(backgroundBrush);

	//Switch to our data area font
	HFONT hfontOld;
	hfontOld = (HFONT)SelectObject(hdc, dataAreaFont);

	//Draw the columns
	bool foundActiveEditCell = false;
	int gridContentStartPosX = headerPosX;
	int gridContentStartPosY = headerPosY + headerHeight;
	for(unsigned int row = 0; row < visibleRows; ++row)
	{
		//Determine the index of this row into our data arrays
		unsigned int rowDataArrayIndex = (!autoScrollingManagement)? row: (vscrollCurrent + row);

		//Determine the color data to use for this row
		bool useCustomBackgroundColor = false;
		WinColor customBackgroundColor;
		WinColor colorTextFrontForRow = textFrontColor;
		WinColor colorTextBackForRow = textBackColor;
		WinColor colorEditTextFrontForRow = editTextFrontColor;
		WinColor colorEditTextBackForRow = editTextBackColor;
		if(rowDataArrayIndex < (unsigned int)rowColorDataArray.size())
		{
			//Select the text colors to use
			CustomColorData& rowColorData = rowColorDataArray[rowDataArrayIndex];
			colorTextFrontForRow     = (rowColorData.textColorDefined)? rowColorData.colorTextFront:     textFrontColor;
			colorTextBackForRow      = (rowColorData.textColorDefined)? rowColorData.colorTextBack:      textBackColor;
			colorEditTextFrontForRow = (rowColorData.editColorDefined)? rowColorData.colorEditTextFront: editTextFrontColor;
			colorEditTextBackForRow  = (rowColorData.editColorDefined)? rowColorData.colorEditTextBack:  editTextBackColor;

			//Check if a custom background color has been specified for this row
			if(rowColorData.backgroundColorDefined)
			{
				useCustomBackgroundColor = true;
				customBackgroundColor = rowColorData.colorBackground;
			}
		}

		//Draw each column in this row
		int columnStartPosX = gridContentStartPosX - currentScrollHOffset;
		int columnStartPosY = gridContentStartPosY + ((int)row * (fontHeight + (marginSize * 2)));
		int columnEndPosY = columnStartPosY + fontHeight + (marginSize * 2);
		for(ColumnSortIndexIterator i = columnSortIndex.begin(); i != columnSortIndex.end(); ++i)
		{
			//Retrieve and calculate information for this cell
			ColumnData& columnData = *(i->second);
			int columnEndPosX = columnStartPosX + (int)columnData.actualWidth;
			bool editModeActiveForCell = (editModeActive && (selectedRowNo == rowDataArrayIndex) && (selectedColumnID == columnData.columnID));

			//Calculate the colour values to use for this cell
			bool useCustomBackgroundColorForCell = useCustomBackgroundColor;
			WinColor customBackgroundColorForCell = customBackgroundColor;
			WinColor colorTextFrontForCell = (editModeActiveForCell)? colorEditTextFrontForRow: colorTextFrontForRow;
			WinColor colorTextBackForCell = (editModeActiveForCell)? colorEditTextBackForRow: colorTextBackForRow;

			//If any custom colours have been specified for this cell, apply them now as
			//overrides.
			std::map<unsigned int, std::map<unsigned int, CustomColorData>>::const_iterator cellCustomColorDataIteratorForRow = cellCustomColorData.find(rowDataArrayIndex);
			if(cellCustomColorDataIteratorForRow != cellCustomColorData.end())
			{
				std::map<unsigned int, CustomColorData>::const_iterator cellCustomColorDataIterator = cellCustomColorDataIteratorForRow->second.find(columnData.columnID);
				if(cellCustomColorDataIterator != cellCustomColorDataIteratorForRow->second.end())
				{
					CustomColorData cellColorData = cellCustomColorDataIterator->second;
					if(cellColorData.textColorDefined && !editModeActiveForCell)
					{
						colorTextFrontForCell = cellColorData.colorTextFront;
						colorTextBackForCell = cellColorData.colorTextBack;
					}
					else if(cellColorData.editColorDefined && editModeActiveForCell)
					{
						colorTextFrontForCell = cellColorData.colorEditTextFront;
						colorTextBackForCell = cellColorData.colorEditTextBack;
					}
					if(cellColorData.backgroundColorDefined)
					{
						useCustomBackgroundColorForCell = true;
						customBackgroundColorForCell = cellColorData.colorBackground;
					}
				}
			}

			//Select the text color to use for this cell
			SetTextColor(hdc, colorTextFrontForCell.GetColorREF());
			SetBkColor(hdc, colorTextBackForCell.GetColorREF());

			//Apply a custom background color for this column if one has been specified
			if(useCustomBackgroundColorForCell)
			{
				HBRUSH backgroundBrush = CreateSolidBrush(customBackgroundColorForCell.GetColorREF());
				HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
				PatBlt(hdc, columnStartPosX, columnStartPosY + 1, columnEndPosX - columnStartPosX, columnEndPosY - columnStartPosY, PATCOPY);
				SelectObject(hdc, backgroundBrushOld);
				DeleteObject(backgroundBrush);
			}

			//Obtain a raw pointer to the character string to use for this cell
			const wchar_t* cellDataRaw = 0;
			unsigned int cellDataRawLength = 0;
			if(editModeActiveForCell)
			{
				cellDataRaw = editBuffer.c_str();
				cellDataRawLength = (unsigned int)editBuffer.size();
			}
			else
			{
				if(rowDataArrayIndex < (unsigned int)columnData.dataBuffer.size())
				{
					cellDataRaw = columnData.dataBuffer[rowDataArrayIndex].c_str();
					cellDataRawLength = (unsigned int)columnData.dataBuffer[rowDataArrayIndex].size();
				}
			}

			//If this cell is within the defined data region of the grid, draw the cell
			//content.
			if(cellDataRaw != 0)
			{
				//Retrieve the control information for this cell. We can assume this
				//structure exists at this point as all child controls should have been
				//correctly initialized before this paint message was called.
				//##FIX## What if WM_PRINTCLIENT was sent outside WM_PAINT?
				std::vector<CellControlInfo>& controlListForColumn = customControlForCell[columnData.columnID];
				CellControlInfo& cellControlInfo = controlListForColumn[rowDataArrayIndex];

				//Calculate the default position of text within this cell
				int textStartPosX = ((columnStartPosX + (int)marginSize) > columnEndPosX)? columnEndPosX: (columnStartPosX + (int)marginSize);
				int textStartPosY = ((columnStartPosY + (int)marginSize) > columnEndPosY)? columnEndPosY: (columnStartPosY + (int)marginSize);
				int textEndPosX = ((columnEndPosX - (int)marginSize) < textStartPosX)? textStartPosX: columnEndPosX - (int)marginSize;
				int textEndPosY = ((columnEndPosY - (int)marginSize) < textStartPosY)? textStartPosY: columnEndPosY - (int)marginSize;

				//Adjust the text position in this cell to take into account the control
				//type used in this cell
				if((cellControlInfo.controlType == CellControlType::TreeEntry) && (cellControlInfo.treeEntryIndentLevel > 0))
				{
					textStartPosX += cellControlInfo.controlPosX + cellControlInfo.controlWidth + (int)marginSize;
					textStartPosX = (textStartPosX > textEndPosX)? textEndPosX: textStartPosX;
				}
				else if((cellControlInfo.controlType == CellControlType::ComboBox) || (cellControlInfo.controlType == CellControlType::TextBoxWithButton))
				{
					textEndPosX -= (cellControlInfo.controlWidth + (int)marginSize);
					textEndPosX = (textEndPosX < textStartPosX)? textStartPosX: textEndPosX;
				}

				//If this cell is in edit mode, process for any pending adjustments which
				//need to be made to the edit position for the cell.
				if(editModeActiveForCell)
				{
					//Flag that we found the active edit cell. We track this to determine
					//if we need to hide the caret in the case that the cell being edited
					//is scrolled outside the visible area of the control.
					foundActiveEditCell = true;

					//If an edit position change is pending, flag that we also need to set
					//the caret position.
					bool setCaretPosPending = editModeSelectionPosChangePending;

					//If an edit position change is pending, and the new position is
					//specified as a pixel location, update the selected character number
					//using this pixel location.
					if(editModeSelectionPosChangePending && editModeSelectionPosChangePendingInPixels)
					{
						//Attempt to locate a character within the data string before or
						//after which to place the edit position.
						bool foundNewEditPos = false;
						int newEditPos = 0;
						int characterSearchPos = 0;
						int stringLength = cellDataRawLength;
						int lastStringEndPosInPixels = textStartPosX - columnStartPosX;
						while(!foundNewEditPos && (characterSearchPos < stringLength))
						{
							SIZE stringSizeToSearchPos;
							BOOL getTextExtentPoint32Return = GetTextExtentPoint32(hdc, cellDataRaw, (characterSearchPos + 1), &stringSizeToSearchPos);
							if(getTextExtentPoint32Return != 0)
							{
								int thisStringEndPosInPixels = (textStartPosX - columnStartPosX) + (int)stringSizeToSearchPos.cx;
								if(thisStringEndPosInPixels  >= (int)(editModeNewSelectionPosInPixels + editCellHorizontalPixelOffset))
								{
									int totalPixelWidthOfThisCharacter = thisStringEndPosInPixels - lastStringEndPosInPixels;
									int selectionPosWithinThisCharacter = (int)(editModeNewSelectionPosInPixels + editCellHorizontalPixelOffset) - lastStringEndPosInPixels;
									newEditPos = (selectionPosWithinThisCharacter >= (totalPixelWidthOfThisCharacter / 2))? characterSearchPos + 1: characterSearchPos;
									foundNewEditPos = true;
								}
								lastStringEndPosInPixels = thisStringEndPosInPixels;
							}
							++characterSearchPos;
						}

						//If we found a new edit position within the data string, set it
						//here, otherwise move the edit position to the end of the string.
						if(foundNewEditPos)
						{
							SelectCharacter(newEditPos);
						}
						else
						{
							SelectCharacter(stringLength);
						}

						//If drag select start requests were being ignored, reactivate
						//them now that the selected character number has been set.
						ignoreDragSelectStart = false;

						//Flag that a selection pixel pos change is no longer pending
						editModeSelectionPosChangePendingInPixels = false;
					}

					//Flag that an edit position change is no longer pending
					editModeSelectionPosChangePending = false;

					//Calculate the total width of the rendered text content for this cell
					SIZE totalTextSize;
					GetTextExtentPoint32(hdc, cellDataRaw, cellDataRawLength, &totalTextSize);
					unsigned int totalTextWidthInPixels = (unsigned int)totalTextSize.cx;

					//Calculate the width of the rendered text content for this cell up to
					//the current selected character number
					SIZE textSizeToSelectedCharacter;
					GetTextExtentPoint32(hdc, cellDataRaw, selectedCharacterNo, &textSizeToSelectedCharacter);
					unsigned int textWidthInPixelsToSelectedCharacter = (unsigned int)textSizeToSelectedCharacter.cx;

					//Ensure the current edit position lies within the visible region of
					//the cell, shifting the content horizontally if it does not.
					bool editPosLiesWithinVisibleCellRegion = false;
					unsigned int availableTextSize = (unsigned int)(textEndPosX - textStartPosX);
					unsigned int editCellPixelOffsetAmount = (availableTextSize / 4) + 1;
					while(!editPosLiesWithinVisibleCellRegion)
					{
						if(textWidthInPixelsToSelectedCharacter > (editCellHorizontalPixelOffset + availableTextSize))
						{
							editCellHorizontalPixelOffset += ((editCellHorizontalPixelOffset + availableTextSize + editCellPixelOffsetAmount) <= totalTextWidthInPixels)? editCellPixelOffsetAmount: totalTextWidthInPixels - (editCellHorizontalPixelOffset + availableTextSize);
							setCaretPosPending = true;
						}
						else if(textWidthInPixelsToSelectedCharacter < editCellHorizontalPixelOffset)
						{
							editCellHorizontalPixelOffset -= (editCellHorizontalPixelOffset > editCellPixelOffsetAmount)? editCellPixelOffsetAmount: editCellHorizontalPixelOffset;
							setCaretPosPending = true;
						}
						else
						{
							editPosLiesWithinVisibleCellRegion = true;
						}
					}

					//If we need to update the caret position, do it now.
					if(setCaretPosPending)
					{
						//Calculate the number of characters in the string for this cell
						//that precede the caret position
						int stringLength = (int)cellDataRawLength;
						int selectedCharacterNoWithinString = selectedCharacterNo;
						if(selectedCharacterNoWithinString > stringLength)
						{
							selectedCharacterNoWithinString = stringLength;
						}

						//Calculate the rendered size of the text string component that
						//precedes the caret position
						int preceedingStringSizeBeforeCaret = 0;
						if(selectedCharacterNoWithinString > 0)
						{
							SIZE preceedingStringSizeBeforeCaretAsSize;
							BOOL getTextExtentPoint32Return = GetTextExtentPoint32(hdc, cellDataRaw, selectedCharacterNoWithinString, &preceedingStringSizeBeforeCaretAsSize);
							if(getTextExtentPoint32Return != 0)
							{
								preceedingStringSizeBeforeCaret = (int)preceedingStringSizeBeforeCaretAsSize.cx;
							}
						}

						//Position the caret at the required location
						SetCaretPos((textStartPosX + preceedingStringSizeBeforeCaret) - editCellHorizontalPixelOffset, textStartPosY);
					}

					//If drag select is active, hide the caret if it is currently visible,
					//and flag it to reappear again when drag select finishes, otherwise
					//show the caret if a show caret operation is pending.
					if(DragSelectActive() && (dragSelectStartPos != dragSelectEndPos))
					{
						if(!editModeShowCaretPending)
						{
							HideCaret(hwnd);
							editModeShowCaretPending = true;
						}
					}
					else if(editModeShowCaretPending)
					{
						ShowCaret(hwnd);
						editModeShowCaretPending = false;
					}
				}

				//Update the content of this cell
				if(cellControlInfo.controlType == CellControlType::CheckBox)
				{
					//Update the checked state for this checkbox cell
					bool checkedState = false;
					StringToBool(cellDataRaw, checkedState);
					SendMessage(cellControlInfo.hwnd, BM_SETCHECK, (checkedState)? BST_CHECKED: BST_UNCHECKED, 0);
				}
				else if(cellControlInfo.controlType == CellControlType::Button)
				{
					SendMessage(cellControlInfo.hwnd, WM_SETTEXT, 0, (LPARAM)cellDataRaw);
				}
				else if((cellControlInfo.controlType == CellControlType::TextBox) || (cellControlInfo.controlType == CellControlType::ComboBox) || (cellControlInfo.controlType == CellControlType::TreeEntry) || (cellControlInfo.controlType == CellControlType::TextBoxWithButton))
				{
					//If this is a tree entry cell and an expand icon needs to be drawn
					//for it, draw it now.
					if((cellControlInfo.controlType == CellControlType::TreeEntry) && cellControlInfo.showExpandIcon && (cellControlInfo.treeEntryIndentLevel > 0))
					{
						//Calculate the position of the expand icon
						RECT expandIconRect;
						expandIconRect.left = cellControlInfo.controlPosX;
						expandIconRect.right = cellControlInfo.controlPosX + cellControlInfo.controlWidth;
						expandIconRect.top = cellControlInfo.controlPosY;
						expandIconRect.bottom = cellControlInfo.controlPosY + cellControlInfo.controlHeight;

						//Select the text character to use to represent the expand icon
						std::wstring expandIconText;
						if(!cellControlInfo.expandIconIsExpanded)
						{
							expandIconText = L"\u25B7";
						}
						else
						{
							expandIconText = L"\u25E2";
						}

						//Determine the colour settings to use for the expand icon
						WinColor treeArrowOpenBorder(60, 60, 60);
						WinColor treeArrowClosedBorder(150, 150, 150);
						COLORREF treeArrowColor = (cellControlInfo.expandIconIsExpanded)? treeArrowOpenBorder.GetColorREF(): treeArrowClosedBorder.GetColorREF();

						//Draw the expand icon for this cell
						SetTextColor(hdc, treeArrowColor);
						DrawText(hdc, expandIconText.c_str(), (int)expandIconText.size(), &expandIconRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						SetTextColor(hdc, colorTextFrontForCell.GetColorREF());
					}

					//Calculate the position and size of the text content within this cell
					RECT rect;
					rect.left = textStartPosX;
					rect.right = textEndPosX;
					rect.top = textStartPosY;
					rect.bottom = textEndPosY;

					//Draw the text for this cell
					if(!editModeActiveForCell)
					{
						//Determine the ellipsis mode to use for this cell
						UINT ellipsisMode = 0;
						switch(cellControlInfo.ellipsisMode)
						{
						case TextEllipsisMode::Text:
							ellipsisMode = DT_WORD_ELLIPSIS;
							break;
						case TextEllipsisMode::Path:
							ellipsisMode = DT_PATH_ELLIPSIS;
							break;
						}

						//Draw the text for this cell
						DrawText(hdc, cellDataRaw, cellDataRawLength, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | ellipsisMode);
					}
					else
					{
						//Setup a clipping region to restrict text drawing to the visible
						//text area of this cell
						HRGN textClippingRegion = CreateRectRgnIndirect(&rect);
						SelectClipRgn(hdc, textClippingRegion);

						//Adjust the text drawing area for this cell to take into account
						//the current horizontal pixel offset value. This will cause us to
						//start the text drawing process before the left border of the
						//text region, with the clipping region we've defined above
						//preventing pixels in this area actually being modified.
						rect.left -= (int)editCellHorizontalPixelOffset;

						//Render the text for this cell
						if(!DragSelectActive())
						{
							DrawText(hdc, cellDataRaw, cellDataRawLength, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
						}
						else
						{
							//Reset the right boundary of our text render region to be the
							//same as the left boundary. We do this because we calculate
							//the right boundary for each element of the selected text
							//string using a rolling calculation, adjusting the left
							//boundary of each element to start at the right boundary of
							//the preceding element.
							rect.right = rect.left;

							//Draw any text before the selected region
							if(dragSelectStartPos > 0)
							{
								SIZE textSize;
								GetTextExtentPoint32(hdc, cellDataRaw, dragSelectStartPos, &textSize);
								rect.left = rect.right;
								rect.right = rect.left + textSize.cx;
								DrawText(hdc, cellDataRaw, dragSelectStartPos, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
							}

							//Set the text colour to the selected text colour
							WinColor colorSelectedEditTextFrontForRow = colorEditTextFrontForRow;
							WinColor colorSelectedEditTextBackForRow = colorEditTextBackForRow;
							colorSelectedEditTextFrontForRow.Invert();
							colorSelectedEditTextBackForRow.Invert();
							SetTextColor(hdc, colorSelectedEditTextFrontForRow.GetColorREF());
							SetBkColor(hdc, colorSelectedEditTextBackForRow.GetColorREF());

							//Draw the selected text
							SIZE textSize;
							GetTextExtentPoint32(hdc, (cellDataRaw + dragSelectStartPos), (dragSelectEndPos - dragSelectStartPos), &textSize);
							rect.left = rect.right;
							rect.right = rect.left + textSize.cx;
							DrawText(hdc, (cellDataRaw + dragSelectStartPos), (dragSelectEndPos - dragSelectStartPos), &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

							//Restore the text colour
							SetTextColor(hdc, colorEditTextFrontForRow.GetColorREF());
							SetBkColor(hdc, colorEditTextBackForRow.GetColorREF());

							//Draw any text after the selected region
							if(dragSelectEndPos < cellDataRawLength)
							{
								SIZE textSize;
								GetTextExtentPoint32(hdc, (cellDataRaw + dragSelectEndPos), (cellDataRawLength - dragSelectEndPos), &textSize);
								rect.left = rect.right;
								rect.right = rect.left + textSize.cx;
								DrawText(hdc, (cellDataRaw + dragSelectEndPos), (cellDataRawLength - dragSelectEndPos), &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
							}
						}

						//Remove the text clipping region for this cell
						SelectClipRgn(hdc, NULL);
						DeleteObject(textClippingRegion);
					}
				}
			}

			//Create a new pen to draw the cell border
			HPEN hpen = CreatePen(PS_SOLID, 0, lineColor.GetColorREF());
			HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

			//Draw the dividing line marking the right end of this column
			MoveToEx(hdc, columnEndPosX - 1, columnStartPosY, NULL);
			LineTo(hdc, columnEndPosX - 1, columnEndPosY);

			//If this row is within the data buffer for this column, draw the dividing
			//line marking the bottom of this column.
			//##FIX## Either make this work properly for columns with mismatched lengths,
			//or require all columns to be defined for the same number of rows.
			if(cellDataRaw != 0)
			{
				MoveToEx(hdc, columnStartPosX, columnEndPosY, NULL);
				LineTo(hdc, columnEndPosX, columnEndPosY);
			}

			//Destroy the pen object used to draw the cell border
			SelectObject(hdc, hpenOld);
			DeleteObject(hpen);

			//Update the column start position for the next column
			columnStartPosX += (int)columnData.actualWidth;
		}
	}

	//If edit mode is active, but the active edit cell isn't currently visible on the
	//screen, hide the caret if it is currently visible, and flag it to be shown again
	//when the cell becomes visible.
	if(editModeActive && !foundActiveEditCell && !editModeShowCaretPending)
	{
		HideCaret(hwnd);
		editModeShowCaretPending = true;
	}

	//Restore the state of the device context, and free any allocated handles
	SelectObject(hdc, hfontOld);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_COMMAND(WPARAM wParam, LPARAM lParam)
{
	//##TODO## Clean up and comment this
	if(HIWORD(wParam) == BN_CLICKED)
	{
		bool foundTargetControl = false;
		std::map<unsigned int, std::vector<CellControlInfo>>::const_iterator columnIterator = customControlForCell.begin();
		while(!foundTargetControl && (columnIterator != customControlForCell.end()))
		{
			const std::vector<CellControlInfo>& cellInfoForColumn = columnIterator->second;
			unsigned int columnID = columnIterator->first;
			unsigned int rowNo = 0;
			while(!foundTargetControl && (rowNo < cellInfoForColumn.size()))
			{
				const CellControlInfo& cellControlInfo = cellInfoForColumn[rowNo];
				if(cellControlInfo.hwnd == (HWND)lParam)
				{
					ColumnData& columnData = *columnIDIndex[columnID];
					if(columnData.editingAllowed)
					{
						if(cellControlInfo.createdControlType == CellControlType::CheckBox)
						{
							std::wstring dataAsString = columnData.dataBuffer[rowNo];
							bool checkedState = false;
							StringToBool(dataAsString, checkedState);
							checkedState = !checkedState;
							BoolToString(checkedState, dataAsString);

							Grid_CellEditEvent info;
							info.targetRowNo = rowNo;
							info.targetColumnID = columnID;
							info.newData = dataAsString;
							SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::CellEdit), (LPARAM)&info);
						}
						else if((cellControlInfo.createdControlType == CellControlType::Button) || (cellControlInfo.createdControlType == CellControlType::TextBoxWithButton))
						{
							Grid_CellButtonClickEvent info;
							info.targetRowNo = rowNo;
							info.targetColumnID = columnID;
							SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::CellButtonClick), (LPARAM)&info);
						}
						else if(cellControlInfo.createdControlType == CellControlType::ComboBox)
						{
							ShowCellSelectionList(columnID, rowNo, cellControlInfo);
						}
					}
					foundTargetControl = true;
					continue;
				}
				++rowNo;
			}
			++columnIterator;
		}
	}
	else if(HIWORD(wParam) == LBN_SELCHANGE)
	{
		if((HWND)lParam == hwndComboBoxList)
		{
			int selectedItem = (int)SendMessage(hwndComboBoxList, LB_GETCURSEL, 0, 0);
			if(selectedItem != LB_ERR)
			{
				const std::vector<CellControlInfo>& cellInfoForColumn = customControlForCell[openComboBoxColumnID];
				if(openComboBoxRowNo < (int)cellInfoForColumn.size())
				{
					const CellControlInfo& cellControlInfo = cellInfoForColumn[openComboBoxRowNo];
					if(selectedItem < (int)cellControlInfo.selectionList.size())
					{
						Grid_CellEditEvent info;
						info.targetRowNo = openComboBoxRowNo;
						info.targetColumnID = openComboBoxColumnID;
						info.newData = cellControlInfo.selectionList[selectedItem];
						SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::CellEdit), (LPARAM)&info);
					}
				}
				DestroyWindow(hwndComboBoxList);
				hwndComboBoxList = NULL;
			}
		}
	}
	else if((HIWORD(wParam) == LBN_SELCANCEL) || (HIWORD(wParam) == LBN_KILLFOCUS))
	{
		//##TODO## Fix this comment
		//If an open combobox selection list loses keyboard focus, destroy the selection
		//list window.
		if((HWND)lParam == hwndComboBoxList)
		{
			DestroyWindow(hwndComboBoxList);
			hwndComboBoxList = NULL;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::ShowCellSelectionList(unsigned int columnID, unsigned int rowNo, const CellControlInfo& cellControlInfo)
{
	//Destroy any other selection list that's currently visible
	if(hwndComboBoxList != NULL)
	{
		DestroyWindow(hwndComboBoxList);
		hwndComboBoxList = NULL;
	}

	//Save the associated row and column iD for the new selection list
	openComboBoxColumnID = columnID;
	openComboBoxRowNo = rowNo;

	//Calculate the desired position of the selection list window
	POINT cellPosAsPoint;
	cellPosAsPoint.x = cellControlInfo.cellPosX;
	cellPosAsPoint.y = cellControlInfo.cellPosY;
	ClientToScreen(hwnd, &cellPosAsPoint);
	int selectionListPosX = cellPosAsPoint.x;
	int selectionListPosY = cellPosAsPoint.y + cellControlInfo.cellHeight;

	//Create the selection list window
	hwndComboBoxList = CreateWindowEx(0, WC_LISTBOX, L"", WS_POPUP | WS_DLGFRAME | WS_HSCROLL | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, selectionListPosX, selectionListPosY, 0, 0, hwnd, NULL, moduleHandle, NULL);
	SendMessage(hwndComboBoxList, WM_SETFONT, (WPARAM)dataAreaFont, FALSE);

	//Populate the selection list with each supplied entry, and calculate the width in
	//pixels of the longest entry, as well as the list index of the currently selected
	//item.
	int currentSelectedItemIndex = -1;
	int maxElementWidth = 0;
	HDC hdc = GetDC(hwndComboBoxList);
	HFONT hfontOld = (HFONT)SelectObject(hdc, dataAreaFont);
	std::wstring currentSelectedItemText = columnIDIndex[columnID]->dataBuffer[rowNo];
	for(size_t i = 0; i < cellControlInfo.selectionList.size(); ++i)
	{
		//Add this entry to the selection list
		const std::wstring& selectionListEntry = cellControlInfo.selectionList[i];
		int newListEntryIndex = (int)SendMessage(hwndComboBoxList, LB_ADDSTRING, 0, (LPARAM)selectionListEntry.c_str());

		//Check if this new selection list item is the currently selected item
		if((currentSelectedItemIndex < 0) && (selectionListEntry == currentSelectedItemText))
		{
			currentSelectedItemIndex = newListEntryIndex;
		}

		//If this selection list item is the widest item in pixels so far, update the max
		//element width.
		SIZE textSize;
		BOOL getTextExtentPoint32Return = GetTextExtentPoint32(hdc, selectionListEntry.c_str(), (int)selectionListEntry.size(), &textSize);
		if(getTextExtentPoint32Return != 0)
		{
			maxElementWidth = ((textSize.cx + 10) > maxElementWidth)? (textSize.cx + 10): maxElementWidth;
		}
	}
	SelectObject(hdc, hfontOld);
	ReleaseDC(hwndComboBoxList, hdc);

	//Set the maximum horizontal extent of the selection list to the calculated pixel
	//width of the largest element. We need to do this in order for the horizontal
	//scrollbar to be displayed and scroll to the correct extents.
	SendMessage(hwndComboBoxList, LB_SETHORIZONTALEXTENT, maxElementWidth, 0);

	//Calculate the desired width and height of the selection list window
	//##TODO## Calculate a more sensible maximum height for the selection list
	//##TODO## Take into account the current monitor screen extents when calculating a
	//position and size of the selection list, to ensure it is always fully visible on the
	//screen.
	int maxSelectionListHeight = 200; //##DEBUG##
	int selectionListWidth = cellControlInfo.cellWidth;
	int selectionListHeight = ((int)cellControlInfo.selectionList.size() * fontHeight) + (GetSystemMetrics(SM_CYDLGFRAME) * 2);
	if(maxElementWidth > selectionListWidth)
	{
		selectionListHeight += GetSystemMetrics(SM_CYHSCROLL);
	}
	selectionListHeight = (selectionListHeight > maxSelectionListHeight)? maxSelectionListHeight: selectionListHeight;

	//Resize the selection list window to the desired width and height
	SetWindowPos(hwndComboBoxList, NULL, 0, 0, selectionListWidth, selectionListHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	//Set the currently selected item as the initial selection for the selection list
	SendMessage(hwndComboBoxList, LB_SETCURSEL, currentSelectedItemIndex, 0);

	//Show the selection list window
	ShowWindow(hwndComboBoxList, SW_SHOW);
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_SETFONT(WPARAM wParam, LPARAM lParam)
{
	//Store the font to use for out controls
	controlFont = (HFONT)wParam;

	//Pass this message on to each created child control
	if(hwndHeader != NULL)
	{
		SendMessage(hwndHeader, WM_SETFONT, wParam, lParam);
	}
	if(hwndComboBoxList != NULL)
	{
		SendMessage(hwndComboBoxList, WM_SETFONT, wParam, lParam);
	}
	for(std::set<HWND>::const_iterator i = childControlSet.begin(); i != childControlSet.end(); ++i)
	{
		SendMessage(*i, WM_SETFONT, wParam, lParam);
	}

	//If no data area font has been defined, use this font for our data area as well.
	if(dataAreaFont == NULL)
	{
		msgGRID_SETDATAAREAFONT(wParam, NULL);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Window size functions
//----------------------------------------------------------------------------------------
unsigned int WC_DataGrid::GetTotalRowWidth() const
{
	unsigned int totalRowWidth = 0;
	for(ColumnDataConstIterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		totalRowWidth += i->actualWidth;
	}
	return totalRowWidth;
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::UpdateWindowSize()
{
	//Calculate the maximum number of rows which can be shown within the control
	int textAreaHeight = controlHeight - headerHeight;
	if(controlHeight < headerHeight)
	{
		textAreaHeight = 0;
	}
	int rowHeight = fontHeight + (marginSize * 2);
	unsigned int newVisibleRows = (unsigned int)((textAreaHeight + (rowHeight - 1)) / rowHeight);
	unsigned int newFullyVisibleRows = (unsigned int)(textAreaHeight / rowHeight);

	//If we're not using automatic scroll management, and more rows need to be shown after
	//the resize, update the data buffer sizes for each column.
	if(!autoScrollingManagement && (newVisibleRows > visibleRows))
	{
		//Resize any data buffers that need to be adjusted
		rowColorDataArray.resize(newVisibleRows);
		bool resizedDataBuffer = false;
		for(ColumnDataIterator i = columnData.begin(); i != columnData.end(); ++i)
		{
			if(newVisibleRows > (unsigned int)i->dataBuffer.size())
			{
				resizedDataBuffer = true;
				i->dataBuffer.resize(newVisibleRows);
			}
		}

		//If we needed to resize at least one data buffer, recalculate the new highest
		//column data index.
		if(resizedDataBuffer)
		{
			largestColumnDataArraySize = CalculateLargestColumnDataArraySize();
		}
	}

	//Ensure the number of fully visible rows is recorded as at least 1, and store the new
	//value now.
	newFullyVisibleRows = (newFullyVisibleRows <= 0)? 1: newFullyVisibleRows;
	fullyVisibleRows = newFullyVisibleRows;

	//If the number of visible rows has changed, update the stored visible row state.
	if(newVisibleRows != visibleRows)
	{
		//Update the number of rows to be displayed
		visibleRows = newVisibleRows;

		//If the number of visible rows has changed, send a message to the parent control
		//to notify it about the change in visible rows.
		Grid_NewRowCount newRowCountState;
		newRowCountState.visibleRows = newVisibleRows;
		SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::NewRowCount), (LPARAM)&newRowCountState);
	}

	//If automatic scroll management is enabled, recalculate the new scroll settings based
	//on the new window size.
	if(autoScrollingManagement)
	{
		RecalculateScrollPosition();
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);

	//Force the control to redraw
	ForceControlRedraw();
}

//----------------------------------------------------------------------------------------
unsigned int WC_DataGrid::CalculateLargestColumnDataArraySize()
{
	//Calculate the largest column data array size
	unsigned int calculatedLargestColumnDataArraySize = 0;
	for(std::list<ColumnData>::const_iterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		unsigned int columnDataArraySize = (unsigned int)i->dataBuffer.size();
		calculatedLargestColumnDataArraySize = (columnDataArraySize > calculatedLargestColumnDataArraySize)? columnDataArraySize: calculatedLargestColumnDataArraySize;
	}
	return calculatedLargestColumnDataArraySize;
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::RecalculateScrollPosition()
{
	//Reset the vertical scroll minimum value and number of values per page
	vscrollMin = 0;
	vscrollValuesPerPage = fullyVisibleRows;

	//Calculate a new maximum vertical scroll position, using the size of the largest
	//content buffer.
	int newVScrollMax = 0;
	for(std::list<ColumnData>::const_iterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		int thisColumnSize = (int)i->dataBuffer.size();
		newVScrollMax = (thisColumnSize > newVScrollMax)? thisColumnSize: newVScrollMax;
	}
	vscrollMax = (newVScrollMax > vscrollValuesPerPage)? newVScrollMax - vscrollValuesPerPage: 0;
	vscrollMaxTrueLimit = newVScrollMax;

	//Clamp the current vertical scroll position to the new maximum vertical scroll
	//position
	vscrollCurrent = (vscrollCurrent > vscrollMax)? vscrollMax: vscrollCurrent;
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::UpdateScrollbarSettings(int newScrollHOffset, int newScrollVOffset, bool controlSizeChanged)
{
	//Calculate the new settings for the horizontal scrollbar
	unsigned int totalRowWidth = GetTotalRowWidth();

	//Clamp the new horizontal scroll offset to the upper and lower boundaries
	if((newScrollHOffset + controlWidth) > (int)totalRowWidth)
	{
		newScrollHOffset = (int)totalRowWidth - controlWidth;
	}
	if(newScrollHOffset < 0)
	{
		newScrollHOffset = 0;
	}

	//If the horizontal scroll offset has changed, update the horizontal scroll settings.
	if(controlSizeChanged || (currentScrollHOffset != newScrollHOffset))
	{
		//Latch the new scroll value
		currentScrollHOffset = newScrollHOffset;

		//Reposition the header control to handle horizontal scrolling
		MoveWindow(hwndHeader, headerPosX - currentScrollHOffset, headerPosY, headerWidth + currentScrollHOffset, headerHeight, TRUE);

		//Apply the latest horizontal scrollbar settings
		SCROLLINFO hscrollInfo;
		hscrollInfo.cbSize = sizeof(hscrollInfo);
		hscrollInfo.nMin = 0;
		hscrollInfo.nMax = (int)totalRowWidth - 1;
		hscrollInfo.nPos = (int)currentScrollHOffset;
		hscrollInfo.nPage = controlWidth;
		hscrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		SetScrollInfo(hwnd, SB_HORZ, &hscrollInfo, TRUE);

		//Flag that the caret position needs to be updated in edit mode
		editModeSelectionPosChangePendingInPixels = false;
		editModeSelectionPosChangePending = true;
	}

	//Clamp the new vertical scroll offset to the upper and lower boundaries
	if(newScrollVOffset < vscrollMin)
	{
		newScrollVOffset = vscrollMin;
	}
	if(newScrollVOffset > vscrollMax)
	{
		newScrollVOffset = vscrollMax;
	}

	//If the vertical scroll offset has changed, update the vertical scroll settings.
	if(controlSizeChanged || (vscrollCurrent != newScrollVOffset))
	{
		//Latch the new scroll value
		vscrollCurrent = newScrollVOffset;

		//Apply the latest vertical scrollbar settings
		SCROLLINFO vscrollInfo;
		vscrollInfo.cbSize = sizeof(vscrollInfo);
		vscrollInfo.nMin = vscrollMin;
		vscrollInfo.nMax = (vscrollMaxTrueLimit > 0)? (vscrollMaxTrueLimit - 1): 0;
		vscrollInfo.nPos = vscrollCurrent;
		vscrollInfo.nPage = vscrollValuesPerPage;
		vscrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		SetScrollInfo(hwnd, SB_VERT, &vscrollInfo, TRUE);

		//Flag that the caret position needs to be updated in edit mode
		editModeSelectionPosChangePendingInPixels = false;
		editModeSelectionPosChangePending = true;
	}
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::ForceControlRedraw()
{
	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);
	for(std::set<HWND>::const_iterator i = childControlSet.begin(); i != childControlSet.end(); ++i)
	{
		InvalidateRect(*i, NULL, FALSE);
	}
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::RecalculateColumnWidths()
{
	//Calculate the size of all columns with a non-proportional size, and build a list of
	//all the proportional columns and the sum total of all proportional width weightings.
	int fixedColumnWidths = 0;
	std::list<ColumnData*> proportionalColumnSet;
	float proportionalColumnWidthWeightingSum = 0;
	for(std::list<ColumnData>::iterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		//If this is a proportional column, add it to the proportional column set, and
		//advance to the next column.
		ColumnData& column = *i;
		if(column.sizeMode == ColumnSizeMode::Proportional)
		{
			proportionalColumnWidthWeightingSum += column.proportionalWidth;
			proportionalColumnSet.push_back(&column);
			continue;
		}

		//Calculate the desired size of this column
		unsigned int newColumnWidth = 0;
		if(column.sizeMode == ColumnSizeMode::Absolute)
		{
			newColumnWidth = column.absoluteWidth;
		}
		else if(column.sizeMode == ColumnSizeMode::SizeToContent)
		{
			//Create a device content to perform text size calculations
			HDC hdc = GetDC(hwnd);

			//Calculate the longest content entry in this column, including the column
			//header.
			HFONT hfontOld = (HFONT)SelectObject(hdc, dataAreaFont);
			unsigned int longestContentWidth = 0;
			std::vector<CellControlInfo>& controlListForColumn = customControlForCell[column.columnID];
			for(size_t columnDataIndex = 0; columnDataIndex < column.dataBuffer.size(); ++columnDataIndex)
			{
				//Calculate the width in pixels of the text for this cell
				SIZE textSize;
				GetTextExtentPoint32(hdc, column.dataBuffer[columnDataIndex].c_str(), (int)column.dataBuffer[columnDataIndex].size(), &textSize);
				unsigned int cellTextWidth = (unsigned int)textSize.cx;

				//Calculate the total width of all the content in this cell
				unsigned int totalCellContentWidth = cellTextWidth;
				if(columnDataIndex < controlListForColumn.size())
				{
					CellControlInfo& cellControlInfo = controlListForColumn[columnDataIndex];
					switch(cellControlInfo.controlType)
					{
					case CellControlType::ComboBox:
						totalCellContentWidth += (fontHeight + (marginSize * 2)) + marginSize;
						break;
					case CellControlType::TextBoxWithButton:{
						SIZE buttonTextSize;
						BOOL getTextExtentPoint32Return = GetTextExtentPoint32(hdc, cellControlInfo.customButtonText.c_str(), (int)cellControlInfo.customButtonText.size(), &buttonTextSize);
						if(getTextExtentPoint32Return != 0)
						{
							totalCellContentWidth += ((unsigned int)buttonTextSize.cx + (marginSize * 2)) + marginSize;
						}
						break;}
					case CellControlType::TreeEntry:
						if(cellControlInfo.treeEntryIndentLevel > 0)
						{
							totalCellContentWidth += ((int)cellControlInfo.treeEntryIndentLevel * fontHeight) + marginSize;
						}
						break;
					}
				}

				//If this cell content is the longest encountered for this column so far,
				//update the longest content width.
				longestContentWidth = (longestContentWidth < totalCellContentWidth)? totalCellContentWidth: longestContentWidth;
			}

			//Calculate the minimum allowable width for this column
			unsigned int minWidth;
			if(column.minWidth >= 0)
			{
				//Set the minimum width to the user supplied value
				minWidth = (unsigned int)column.minWidth;
			}
			else
			{
				//Set the minimum width to the size required to display the header text fully
				SelectObject(hdc, controlFont);
				SIZE textSize;
				GetTextExtentPoint32(hdc, column.name.c_str(), (int)column.name.size(), &textSize);
				minWidth = textSize.cx + (marginSize * 2) + fontHeight;
			}

			//Release the allocated device context
			SelectObject(hdc, hfontOld);
			ReleaseDC(hwnd, hdc);

			//Calculate the desired width for this column
			newColumnWidth = longestContentWidth + (marginSize * 2) + fontHeight;

			//Limit the desired column width by its allowed minimum and maximum sizes
			newColumnWidth = ((column.maxWidth >= 0) && (newColumnWidth > (unsigned int)column.maxWidth))? (unsigned int)column.maxWidth: newColumnWidth;
			newColumnWidth = (newColumnWidth < minWidth)? minWidth: newColumnWidth;
		}

		//If the width for this column has changed, update it in the header control.
		if(column.actualWidth != newColumnWidth)
		{
			//Record the new actual width for this proportional column
			column.actualWidth = newColumnWidth;

			//Update the width of the column within our header control
			column.manuallySettingColumnWidth = true;
			HDITEM hdItem;
			hdItem.mask = HDI_WIDTH;
			hdItem.cxy = column.actualWidth;
			SendMessage(hwndHeader, HDM_SETITEM, column.index, (LPARAM)&hdItem);
			column.manuallySettingColumnWidth = false;
		}

		//Add the width of this column to the sum of fixed column widths
		fixedColumnWidths += (int)column.actualWidth;
	}

	//Update the size of each proportional column
	unsigned int proportionalColumnSpaceRemaining = (headerWidth < fixedColumnWidths)? 0: headerWidth - fixedColumnWidths;
	for(std::list<ColumnData*>::iterator i = proportionalColumnSet.begin(); i != proportionalColumnSet.end(); ++i)
	{
		//Calculate the new desired width for this proportional column
		ColumnData& column = *(*i);
		float proportionalSpaceForColumn = column.proportionalWidth / proportionalColumnWidthWeightingSum;
		unsigned int newColumnWidth = (unsigned int)(((float)proportionalColumnSpaceRemaining * proportionalSpaceForColumn) + 0.5f);

		//Calculate the minimum allowable width for this column
		unsigned int minWidth;
		if(column.minWidth >= 0)
		{
			//Set the minimum width to the user supplied value
			minWidth = (unsigned int)column.minWidth;
		}
		else
		{
			//Set the minimum width to the size required to display the header text fully
			HDC hdc = GetDC(hwnd);
			HFONT hfontOld = (HFONT)SelectObject(hdc, controlFont);
			SIZE textSize;
			GetTextExtentPoint32(hdc, column.name.c_str(), (int)column.name.size(), &textSize);
			SelectObject(hdc, hfontOld);
			ReleaseDC(hwnd, hdc);
			minWidth = textSize.cx + (marginSize * 2) + fontHeight;
		}

		//Limit the desired column width by its allowed minimum and maximum sizes
		newColumnWidth = ((column.maxWidth >= 0) && (newColumnWidth > (unsigned int)column.maxWidth))? (unsigned int)column.maxWidth: newColumnWidth;
		newColumnWidth = (newColumnWidth < minWidth)? minWidth: newColumnWidth;

		//If the width for this column has changed, update it.
		if(column.actualWidth != newColumnWidth)
		{
			//Record the new actual width for this proportional column
			column.actualWidth = newColumnWidth;

			//Update the width of the column within our header control
			column.manuallySettingColumnWidth = true;
			HDITEM hdItem;
			hdItem.mask = HDI_WIDTH;
			hdItem.cxy = column.actualWidth;
			SendMessage(hwndHeader, HDM_SETITEM, column.index, (LPARAM)&hdItem);
			column.manuallySettingColumnWidth = false;
		}
	}
}

//----------------------------------------------------------------------------------------
//Edit functions
//----------------------------------------------------------------------------------------
bool WC_DataGrid::IsCellEditingActive() const
{
	return editModeActive;
}

//----------------------------------------------------------------------------------------
bool WC_DataGrid::BeginCellEditing(int cursorPosInPixels)
{
	//If we're already in cell editing mode, update the current cursor position, and
	//return.
	if(editModeActive)
	{
		UpdateCellEditingPos(cursorPosInPixels);
		return false;
	}

	//Ensure a cell is currently selected
	if(!rowSelected || !columnSelected)
	{
		return false;
	}

	//Retrieve the column data for the currently selected column
	ColumnIDIndexIterator columnIterator = columnIDIndex.find(selectedColumnID);
	if(columnIterator == columnIDIndex.end())
	{
		return false;
	}
	const ColumnData& columnData = *(columnIterator->second);

	//Load the data for the currently selected cell in the target column into the edit
	//buffer
	unsigned int columnDataArrayIndex = selectedRowNo;
	unsigned int columnDataArraySize = (unsigned int)columnData.dataBuffer.size();
	if(columnDataArrayIndex >= columnDataArraySize)
	{
		return false;
	}
	editBuffer = columnData.dataBuffer[columnDataArrayIndex];

	//Flag that edit mode is now active
	editModeActive = true;

	//Set the caret position within the cell we're editing
	UpdateCellEditingPos(cursorPosInPixels);

	//Flag that the caret needs to be shown
	editModeShowCaretPending = true;

	//Clear the horizontal pixel offset for the cell being edited. This will be
	//recalculated by the render process if required, and we don't want the horizontal
	//offset for the previously edited cell to interfere.
	editCellHorizontalPixelOffset = 0;

	//Since the operation has succeeded, return true.
	return true;
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::UpdateCellEditingPos(int cursorPosInPixels)
{
	//Flag that the caret position needs to be updated in edit mode
	editModeNewSelectionPosInPixels = cursorPosInPixels;
	editModeSelectionPosChangePending = true;
	editModeSelectionPosChangePendingInPixels = true;
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::CompleteCellEditing()
{
	//Check if cell editing mode is currently active
	if(!editModeActive)
	{
		return;
	}

	//Notify the parent window of this edit complete event
	Grid_CellEditEvent info;
	info.targetRowNo = selectedRowNo;
	info.targetColumnID = selectedColumnID;
	info.newData = editBuffer;
	SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::CellEdit), (LPARAM)&info);

	//Hide the caret now that we're exiting edit mode
	if(!editModeShowCaretPending)
	{
		HideCaret(hwnd);
	}
	editModeShowCaretPending = false;

	//Flag that cell editing mode is no longer active
	editModeActive = false;

	//Flag that drag select is no longer active
	StopDragSelect();
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::CancelCellEditing()
{
	//Check if cell editing mode is currently active
	if(!editModeActive)
	{
		return;
	}

	//Hide the caret now that we're exiting edit mode
	if(!editModeShowCaretPending)
	{
		HideCaret(hwnd);
	}
	editModeShowCaretPending = false;

	//Flag that cell editing mode is no longer active
	editModeActive = false;

	//Flag that drag select is no longer active
	StopDragSelect();
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::SelectCharacter(int newCharacterPos)
{
	//Calculate the new selected character number, taking into account the actual
	//character extents of the data in the edit buffer.
	unsigned int previousSelectedCharacterNo = selectedCharacterNo;
	if(newCharacterPos < 0)
	{
		selectedCharacterNo = 0;
	}
	else if(newCharacterPos > (int)editBuffer.size())
	{
		selectedCharacterNo = (unsigned int)editBuffer.size();
	}
	else
	{
		selectedCharacterNo = (unsigned int)newCharacterPos;
	}

	//If we didn't actually end up altering the edit position, abort any further
	//processing.
	if(previousSelectedCharacterNo == selectedCharacterNo)
	{
		return;
	}

	//Flag that the caret position needs to be updated in edit mode
	editModeSelectionPosChangePending = true;

	//Adjust the current drag select position to take into account the new character
	//selection
	if(DragSelectActive())
	{
		if(previousSelectedCharacterNo == dragSelectStartPos)
		{
			if(selectedCharacterNo > dragSelectEndPos)
			{
				dragSelectStartPos = dragSelectEndPos;
				dragSelectEndPos = selectedCharacterNo;
			}
			else
			{
				dragSelectStartPos = selectedCharacterNo;
			}
		}
		else
		{
			if(selectedCharacterNo < dragSelectStartPos)
			{
				dragSelectEndPos = dragSelectStartPos;
				dragSelectStartPos = selectedCharacterNo;
			}
			else
			{
				dragSelectEndPos = selectedCharacterNo;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
size_t WC_DataGrid::PerformWordSearchForNextBoundary(size_t searchStartPos, size_t searchEndPos, const std::wstring& text)
{
	//Retrieve the active locale for the current environment
	std::locale currentLocale;

	//Calculate the adjustment value to use to advance to the next character in the
	//requested search direction
	size_t searchAdjustment = (searchStartPos <= searchEndPos)? 1: -1;

	//Skip any leading whitespace or control characters starting from the current search
	//position
	size_t currentSearchPos = searchStartPos;
	while((currentSearchPos != searchEndPos) && (std::isspace(text[currentSearchPos], currentLocale) || std::iscntrl(text[currentSearchPos], currentLocale)))
	{
		currentSearchPos += searchAdjustment;
	}

	//Search for the next point at which the category of character changes. We categorize
	//all characters into three groups:
	//1. Whitespace or control characters
	//2..Alphanumeric characters
	//3. Punctuation characters
	//We generate a word break at each point where the category changes
	size_t previousSearchPos = currentSearchPos;
	while((currentSearchPos != searchEndPos)
		&& ((std::isspace(text[currentSearchPos], currentLocale) || std::iscntrl(text[currentSearchPos], currentLocale)) == (std::isspace(text[previousSearchPos], currentLocale) || std::iscntrl(text[previousSearchPos], currentLocale)))
		&& (std::isalnum(text[currentSearchPos], currentLocale) == std::isalnum(text[previousSearchPos], currentLocale))
		&& (std::ispunct(text[currentSearchPos], currentLocale) == std::ispunct(text[previousSearchPos], currentLocale)))
	{
		currentSearchPos += searchAdjustment;
	}

	//Return the next calculated word break position in the requested direction
	return currentSearchPos;
}

//----------------------------------------------------------------------------------------
void WC_DataGrid::ForceEditCellIntoView()
{
	//##TODO## Adjust the vertical and horizontal scroll settings of the control if
	//necessary, to ensure the cell currently being edited is visible to the user.
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_VSCROLL(WPARAM wParam, LPARAM lParam)
{
	//Handle this scroll event
	int vscrollNew = vscrollCurrent;
	switch(LOWORD(wParam))
	{
	case SB_THUMBTRACK:{
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(hwnd, SB_VERT, &scrollInfo);

		if(autoScrollingManagement)
		{
			vscrollNew = (unsigned int)scrollInfo.nTrackPos;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = (unsigned int)scrollInfo.nTrackPos;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::NewScrollPosition), (LPARAM)&info);
		}
		break;}
	case SB_TOP:
		if(autoScrollingManagement)
		{
			vscrollNew = vscrollMin;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = vscrollMin;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::NewScrollPosition), (LPARAM)&info);
		}
		break;
	case SB_BOTTOM:
		if(autoScrollingManagement)
		{
			vscrollNew = vscrollMax;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = vscrollMax;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::NewScrollPosition), (LPARAM)&info);
		}
		break;
	case SB_PAGEUP:
		if(autoScrollingManagement)
		{
			vscrollNew -= (int)fullyVisibleRows;
		}
		else
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = fullyVisibleRows;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsUp), (LPARAM)&info);
		}
		break;
	case SB_LINEUP:
		if(autoScrollingManagement)
		{
			vscrollNew -= 1;
		}
		else
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = 1;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsUp), (LPARAM)&info);
		}
		break;
	case SB_PAGEDOWN:
		if(autoScrollingManagement)
		{
			vscrollNew += (int)fullyVisibleRows;
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = fullyVisibleRows;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsDown), (LPARAM)&info);
		}
		break;
	case SB_LINEDOWN:
		if(autoScrollingManagement)
		{
			vscrollNew += 1;
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = 1;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsDown), (LPARAM)&info);
		}
		break;
	}

	//Update the current scrollbar state
	if(autoScrollingManagement)
	{
		UpdateScrollbarSettings(currentScrollHOffset, vscrollNew, false);
	}

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_HSCROLL(WPARAM wParam, LPARAM lParam)
{
	//Handle this scroll event
	int hscrollNew = currentScrollHOffset;
	unsigned int totalRowWidth = GetTotalRowWidth();
	switch(LOWORD(wParam))
	{
	case SB_THUMBTRACK:{
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(hwnd, SB_HORZ, &scrollInfo);
		hscrollNew = scrollInfo.nTrackPos;
		break;}
	case SB_LEFT:{
		hscrollNew = 0;
		break;}
	case SB_RIGHT:{
		hscrollNew = (int)totalRowWidth - controlWidth;
		break;}
	case SB_PAGELEFT:{
		hscrollNew -= controlWidth;
		break;}
	case SB_LINELEFT:{
		hscrollNew -= 8;
		break;}
	case SB_PAGERIGHT:{
		hscrollNew += controlWidth;
		break;}
	case SB_LINERIGHT:{
		hscrollNew += 8;
		break;}
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings(hscrollNew, vscrollCurrent, false);

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_KEYDOWN(WPARAM wParam, LPARAM lParam)
{
	unsigned int virtualKeyCode = (unsigned int)wParam;

	int vscrollNew = vscrollCurrent;
	switch(virtualKeyCode)
	{
	default:
		return 0;
	case 'A':
		//If the control key is being held down, capture the keypress as a hotkey.
		if(editModeActive && ((GetKeyState(VK_CONTROL) & 0x8000) != 0))
		{
			SelectCharacter((int)editBuffer.size());
			StartDragSelect();
			UpdateDragSelectPos(0);
			ForceEditCellIntoView();
		}
		break;
	case 'C':
		//If the control key is being held down, capture the keypress as a hotkey.
		if(editModeActive && ((GetKeyState(VK_CONTROL) & 0x8000) != 0))
		{
			CopyToClipboard();
		}
		break;
	case 'V':
		//If the control key is being held down, capture the keypress as a hotkey.
		if(editModeActive && ((GetKeyState(VK_CONTROL) & 0x8000) != 0))
		{
			PasteFromClipboard();
		}
		break;
	case VK_ESCAPE:
		CancelCellEditing();
		break;
	case VK_RETURN:
		CompleteCellEditing();
		break;
	case VK_TAB:
		//##TODO## When the user presses the tab key, tab across to the next editable
		//cell. Note that we can't really make this work properly without introducing the
		//concept of a selected cell independent of an editing cell, which we should do
		//anyway. It should be possible to highlight a cell without editing it, where a
		//left click within the cell or pressing enter activates edit mode for the
		//selected cell.
		//if(IsCellEditingActive())
		//{
		//	unsigned int previousSelectedRowNo = selectedRowNo;
		//	unsigned int previousSelectedColumnID = selectedColumnID;
		//	CompleteCellEditing();
		//}
		break;
	case VK_BACK:
		if(editModeActive)
		{
			if(DragSelectActive())
			{
				editBuffer.erase(dragSelectStartPos, dragSelectEndPos - dragSelectStartPos);
				SelectCharacter((int)dragSelectStartPos);
				StopDragSelect();
			}
			else if(selectedCharacterNo > 0)
			{
				editBuffer.erase(selectedCharacterNo - 1, 1);
				SelectCharacter((int)selectedCharacterNo - 1);
			}
		}
		break;
	case VK_LEFT:
		if(editModeActive)
		{
			if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			{
				StartDragSelect();
			}
			else
			{
				StopDragSelect();
			}
			if((GetKeyState(VK_CONTROL) & 0x8000) != 0)
			{
				SelectCharacter((int)PerformWordSearchForNextBoundary(selectedCharacterNo, 0, editBuffer));
			}
			else
			{
				SelectCharacter((int)selectedCharacterNo - 1);
			}
			ForceEditCellIntoView();
		}
		break;
	case VK_RIGHT:
		if(editModeActive)
		{
			if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			{
				StartDragSelect();
			}
			else
			{
				StopDragSelect();
			}
			if((GetKeyState(VK_CONTROL) & 0x8000) != 0)
			{
				SelectCharacter((int)PerformWordSearchForNextBoundary(selectedCharacterNo, editBuffer.size(), editBuffer));
			}
			else
			{
				SelectCharacter((int)selectedCharacterNo + 1);
			}
			ForceEditCellIntoView();
		}
		break;
	case VK_UP:
		//##FIX## Up and down shouldn't scroll the view, they should move the selected
		//cell.
		//##TODO## If edit mode is currently active on a selection list cell, this should
		//shift to the next or previous entry in the selection list if possible.
		if(autoScrollingManagement)
		{
			vscrollNew -= 1;
		}
		else
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = 1;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsUp), (LPARAM)&info);
		}
		break;
	case VK_DOWN:
		if(autoScrollingManagement)
		{
			vscrollNew += 1;
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = 1;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsDown), (LPARAM)&info);
		}
		break;
	case VK_PRIOR:
		if(autoScrollingManagement)
		{
			vscrollNew -= (int)fullyVisibleRows;
		}
		else
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = fullyVisibleRows;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsUp), (LPARAM)&info);
		}
		break;
	case VK_NEXT:
		if(autoScrollingManagement)
		{
			vscrollNew += (int)fullyVisibleRows;
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = fullyVisibleRows;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsDown), (LPARAM)&info);
		}
		break;
	case VK_HOME:
		if(editModeActive)
		{
			if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			{
				StartDragSelect();
			}
			else
			{
				StopDragSelect();
			}
			SelectCharacter(0);
			ForceEditCellIntoView();
		}
		else if(autoScrollingManagement)
		{
			vscrollNew = vscrollMin;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = 0;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::NewScrollPosition), (LPARAM)&info);
		}
		break;
	case VK_END:
		if(editModeActive)
		{
			if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			{
				StartDragSelect();
			}
			else
			{
				StopDragSelect();
			}
			SelectCharacter((int)editBuffer.size());
			ForceEditCellIntoView();
		}
		else if(autoScrollingManagement)
		{
			vscrollNew = vscrollMax;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = (vscrollMax > 0)? (unsigned int)vscrollMax - 1: 0;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::NewScrollPosition), (LPARAM)&info);
		}
		break;
	}

	//Update the current scrollbar state
	if(autoScrollingManagement)
	{
		UpdateScrollbarSettings(currentScrollHOffset, vscrollNew, false);
	}

	//Force the entire control to redraw
	ForceControlRedraw();

	//Make the redraw operation happen immediately. We do this because it's been noted
	//that when doing the page up/down steps, it's possible for the operation to be slow
	//enough that the control is never redrawn between key repeat messages, causing the
	//interface to not actually update while the user is scrolling.
	UpdateWindow(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_CHAR(WPARAM wParam, LPARAM lParam)
{
	//Ensure we're currently in cell edit mode
	if(!editModeActive)
	{
		return 0;
	}

	//Write the character to the edit buffer, and update the current edit position.
	unsigned char byte = (unsigned char)wParam;
	if((byte >= 0x20) && (byte < 0x7F))
	{
		//If drag select is currently active, delete all selected data from the buffer,
		//and adjust the selected character number.
		if(DragSelectActive())
		{
			editBuffer.erase(dragSelectStartPos, dragSelectEndPos - dragSelectStartPos);
			SelectCharacter((int)dragSelectStartPos);
			StopDragSelect();
		}

		//Write the entered character into the edit buffer at the current selected
		//character position, and advance the selected position past the new character.
		wchar_t byteAsUnicodeChar = (wchar_t)byte;
		editBuffer.insert(selectedCharacterNo, 1, byteAsUnicodeChar);
		SelectCharacter((int)selectedCharacterNo + 1);
	}

	//Ensure the cell currently being edited is within view
	ForceEditCellIntoView();

	//Force the entire control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_KILLFOCUS(WPARAM wParam, LPARAM lParam)
{
	//If we're currently in cell edit mode, complete the edit now that the window is
	//losing focus.
	CompleteCellEditing();

	//Destroy the window caret
	DestroyCaret();

	//Force the entire control to redraw
	ForceControlRedraw();
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_SETFOCUS(WPARAM wParam, LPARAM lParam)
{
	//Create a caret for the window
	CreateCaret(hwnd, NULL, 2, fontHeight);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_MOUSEMOVE(WPARAM wParam, LPARAM lParam)
{
	//Ensure that we're currently in edit mode, and the left mouse button is currently
	//down, indicating a drag select is being performed.
	if(!editModeActive || !mouseButtonDown)
	{
		return 0;
	}

	//Extract the cursor position for this message relative to the control
	int cursorPosX = (short)LOWORD(lParam);

	//Determine the position of the cursor within the content area of the grid
	int selectionPosXWithinGridContent = (cursorPosX - (int)headerPosX) + currentScrollHOffset;

	//Determine the start and end position of the column that's currently being edited
	bool foundSelectedColumn = false;
	int editColumnStartPosX = (int)headerPosX;
	int editColumnEndPosX = editColumnStartPosX;
	ColumnSortIndexIterator sortedColumnIterator = columnSortIndex.begin();
	while(!foundSelectedColumn && (sortedColumnIterator != columnSortIndex.end()))
	{
		//Update the column end position for this column
		ColumnData& columnData = *(sortedColumnIterator->second);
		editColumnEndPosX = editColumnStartPosX + (int)columnData.actualWidth;

		//Check if the selection occurs within this column
		if(columnData.columnID == selectedColumnID)
		{
			foundSelectedColumn = true;
			continue;
		}

		//Update the column start position for the next column
		editColumnStartPosX = editColumnEndPosX;

		//Advance to the next column in the list of columns
		++sortedColumnIterator;
	}

	//Ensure we managed to find the currently selected column
	if(!foundSelectedColumn)
	{
		return 0;
	}

	//Calculate the position of the mouse relative to the selected cell
	int selectionPosXWithinSelectedCellContent = selectionPosXWithinGridContent - editColumnStartPosX;

	//Set the cell editing pos to the selected location
	UpdateCellEditingPos(selectionPosXWithinSelectedCellContent);

	//Initiate drag select if it is not currently active
	if(!DragSelectActive())
	{
		StartDragSelect();
	}

	//Force the entire control to redraw so that changes to drag select are visible
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_LBUTTONUP(WPARAM wParam, LPARAM lParam)
{
	ReleaseCapture();

	mouseButtonDown = false;
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	//Ensure our control currently has focus
	SetFocus(hwnd);

	//Extract the cursor position for this message relative to the control
	int cursorPosX = (short)LOWORD(lParam);
	int cursorPosY = (short)HIWORD(lParam);

	//Determine the position of the cursor within the content area of the grid
	int selectionPosXWithinGridContent = (cursorPosX - headerPosX) + currentScrollHOffset;
	int selectionPosYWithinGridContent = cursorPosY - (headerPosY + headerHeight);

	//Calculate the number of the selected row
	bool foundSelectedRow = false;
	int newSelectedVisibleRowNo = selectionPosYWithinGridContent / (fontHeight + (marginSize * 2));
	int newSelectedRowSelectionOffsetY;
	int columnStartPosY;
	int columnEndPosY;
	unsigned int newSelectedRowNo = (!autoScrollingManagement)? newSelectedVisibleRowNo: (vscrollCurrent + newSelectedVisibleRowNo);
	if((newSelectedVisibleRowNo >= 0) && (newSelectedRowNo < largestColumnDataArraySize))
	{
		columnStartPosY = (headerPosY + headerHeight) + (newSelectedVisibleRowNo * (fontHeight + (marginSize * 2)));
		columnEndPosY = columnStartPosY + fontHeight + (marginSize * 2);
		newSelectedRowSelectionOffsetY = (cursorPosY - columnStartPosY);
		foundSelectedRow = true;
	}

	//Determine the ID number of the column that was selected
	bool foundSelectedColumn = false;
	bool newSelectedColumnSupportsEdit = false;
	unsigned int newSelectedColumnID = 0;
	int newSelectedColumnSelectionOffsetX;
	ColumnSortIndexIterator sortedColumnIterator = columnSortIndex.begin();
	int columnStartPosX = (int)headerPosX;
	while(!foundSelectedColumn && (sortedColumnIterator != columnSortIndex.end()))
	{
		//Calculate the end position of this column, in pixels.
		ColumnData& columnData = *(sortedColumnIterator->second);
		int columnEndPosX = columnStartPosX + (int)columnData.actualWidth;

		//Check if the selection occurs within this column
		if((selectionPosXWithinGridContent >= columnStartPosX) && (selectionPosXWithinGridContent < columnEndPosX))
		{
			newSelectedColumnID = columnData.columnID;
			newSelectedColumnSelectionOffsetX = (cursorPosX - columnStartPosX);
			foundSelectedColumn = true;
			newSelectedColumnSupportsEdit = columnData.editingAllowed;
			continue;
		}

		//Update the column start position for the next column
		columnStartPosX = columnEndPosX;

		//Advance to the next column in the list of columns
		++sortedColumnIterator;
	}

	//Determine if a new selected cell has been found, and if the new selected cell is the
	//same as the currently selected cell.
	bool foundSelectedCell = foundSelectedColumn && foundSelectedRow;
	bool selectedCellIsSameAsPrevious = rowSelected && columnSelected && foundSelectedCell && (selectedRowNo == (unsigned int)newSelectedRowNo) && (selectedColumnID == newSelectedColumnID);

	//If a special area of the target cell was clicked, detect and handle that now
	if(foundSelectedCell)
	{
		std::vector<CellControlInfo>& cellInfoArray = customControlForCell[newSelectedColumnID];
		if(newSelectedRowNo < (unsigned int)cellInfoArray.size())
		{
			CellControlInfo& cellInfoInternal = cellInfoArray[newSelectedRowNo];
			if((cellInfoInternal.controlType == CellControlType::TreeEntry) && cellInfoInternal.showExpandIcon)
			{
				//If the user just clicked on the control region of a tree entry cell that
				//has an expand icon, process this click as a trigger on the expand state
				//change instead of a selection event.
				if((cursorPosX >= cellInfoInternal.controlPosX)
				&& (cursorPosX < (cellInfoInternal.controlPosX + cellInfoInternal.controlWidth))
				&& (cursorPosY >= cellInfoInternal.controlPosY)
				&& (cursorPosY < (cellInfoInternal.controlPosY + cellInfoInternal.controlHeight)))
				{
					Grid_TreeEntryExpandEvent expandEvent;
					expandEvent.expand = !cellInfoInternal.expandIconIsExpanded;
					expandEvent.targetRowNo = newSelectedRowNo;
					expandEvent.targetColumnID = newSelectedColumnID;
					SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::TreeEntryExpandEvent), (LPARAM)&expandEvent);
					return 0;
				}
			}
			else if((cellInfoInternal.controlType == CellControlType::ComboBox) && cellInfoInternal.pickFromSelectionListOnly)
			{
				//If the user selected the text region of a combobox cell that doesn't
				//allow manual entries, expand the selection list for the cell.
				ShowCellSelectionList(newSelectedColumnID, newSelectedRowNo, cellInfoInternal);
				return 0;
			}
		}
	}

	//Notify the parent window of the selection event. Note that we give the handler an
	//option to abort any further processing of this click event by setting the
	//ignoreClickEvent field of this message to true. This allows the owner to extend the
	//selection behaviour of the grid control by performing custom actions in response to
	//this click event, without the control performing actions that interfere.
	Grid_SelectionEvent info;
	info.sameCellAsPrevious = selectedCellIsSameAsPrevious;
	info.rowSelected = foundSelectedRow;
	info.columnSelected = foundSelectedColumn;
	info.selectedRowNo = newSelectedRowNo;
	info.columnID = newSelectedColumnID;
	info.keyPressedCtrl = (wParam & MK_CONTROL) != 0;
	info.keyPressedShift = (wParam & MK_SHIFT) != 0;
	info.ignoreSelectionEvent = false;
	SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::SelectionEvent), (LPARAM)&info);

	//If we were instructed to ignore this click event, abort any further processing.
	if(info.ignoreSelectionEvent)
	{
		return 0;
	}

	//If a different cell has been selected, handle the selected cell change, otherwise
	//update the edit state for the selected cell to respond to the click event.
	if(!selectedCellIsSameAsPrevious)
	{
		//Complete cell editing if it is currently active
		CompleteCellEditing();

		//Update the currently selected cell information
		if(!foundSelectedRow || !foundSelectedColumn)
		{
			rowSelected = false;
			columnSelected = false;
			selectedRowNo = 0;
			selectedColumnID = 0;
		}
		else
		{
			rowSelected = foundSelectedRow;
			columnSelected = foundSelectedColumn;
			selectedRowNo = newSelectedRowNo;
			selectedColumnID = newSelectedColumnID;
		}

		//If a new cell was selected, and edit mode is supported on that cell, enter edit
		//mode for the newly selected cell.
		//##TODO## Make single click editing optional
		if(foundSelectedCell && newSelectedColumnSupportsEdit)
		{
			//Begin the cell editing process, with the edit position specified in pixels.
			//The actual selected character number will be calculated in the next render
			//process.
			BeginCellEditing(newSelectedColumnSelectionOffsetX);

			//Reset the selected character number, so that it's not pointing past the end
			//of the edit buffer in the case that we just switched selected fields. The
			//correct selected character number will be set during the next draw process,
			//but in the case that another event, such as a keyboard event, is processed
			//before the selected character number can be set correctly, it could cause a
			//problem if we have a current selection position that lies outside the edit
			//buffer.
			SelectCharacter(0);

			//Set drag select start events to be ignored. This state will be cleared when
			//the selection position for the newly selected column has been correctly set.
			ignoreDragSelectStart = true;
		}
	}
	else
	{
		//Either enter editing mode for the target cell if supported, or update the
		//current caret position within the cell, depending on whether edit mode is
		//currently active or not.
		if(IsCellEditingActive())
		{
			if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
			{
				//If the user has shift+clicked on the window, start a drag select from
				//the current location.
				StartDragSelect();
			}
			else
			{
				//Stop drag-select if it is currently in progress
				StopDragSelect();
			}

			//Set the cell editing pos to use the selected location
			UpdateCellEditingPos(newSelectedColumnSelectionOffsetX);
		}
		else if(newSelectedColumnSupportsEdit)
		{
			BeginCellEditing(newSelectedColumnSelectionOffsetX);
		}
	}

	//Capture the mouse, so that we can track a drag-select if the user starts a drag
	//operation and moves the cursor outside the window, and flag that the left mouse
	//button is being held down.
	SetCapture(hwnd);
	mouseButtonDown = true;

	//Force the entire control to redraw so that changes to drag select are visible
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam)
{
	//Calculate the number of lines to scroll in the window. We displace the view pos
	//by a minimum of 3 lines per division of the scroll wheel. We also invert the
	//sign, since we want a scroll down to increase the view position.
	int scrollUnits = ((short)HIWORD(wParam) / WHEEL_DELTA);
	scrollUnits *= -3;

	int vscrollNew = vscrollCurrent;
	if(autoScrollingManagement)
	{
		vscrollNew += scrollUnits;
	}
	else
	{
		if(scrollUnits < 0)
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = (unsigned int)(-scrollUnits);
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsUp), (LPARAM)&info);
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = (unsigned int)scrollUnits;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ShiftRowsDown), (LPARAM)&info);
		}
	}

	//Update the current scrollbar state
	if(autoScrollingManagement)
	{
		UpdateScrollbarSettings(currentScrollHOffset, vscrollNew, false);
	}

	//Force the entire control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_BOUNCE(WPARAM wParam, LPARAM lParam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lParam;
	switch(bounceMessage->uMsg)
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		//Attempt to identify a cell associated with the target control
		std::map<HWND, CellMapping>::const_iterator controlMappingIterator = childControlMapping.find(bounceMessage->hwnd);
		if(controlMappingIterator == childControlMapping.end())
		{
			return 0;
		}

		//Retrieve information on the target cell
		const CellMapping& cellMapping = controlMappingIterator->second;

		//Notify the parent window of the selection event. Note that we give the handler
		//an option to abort any further processing of this click event by setting the
		//ignoreClickEvent field of this message to true. This allows the owner to extend
		//the selection behaviour of the grid control by performing custom actions in
		//response to this click event, without the control performing actions that
		//interfere.
		Grid_SelectionEvent info;
		info.sameCellAsPrevious = false;
		info.rowSelected = true;
		info.columnSelected = true;
		info.selectedRowNo = cellMapping.rowNo;
		info.columnID = cellMapping.columnID;
		info.keyPressedCtrl = (bounceMessage->wParam & MK_CONTROL) != 0;
		info.keyPressedShift = (bounceMessage->wParam & MK_SHIFT) != 0;
		info.ignoreSelectionEvent = false;
		SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::SelectionEvent), (LPARAM)&info);

		//If we were instructed to ignore this click event, abort any further processing.
		if(info.ignoreSelectionEvent)
		{
			bounceMessage->SetResult(0);
		}

		//Complete cell editing if it is currently active
		CompleteCellEditing();

		//Update the currently selected cell information
		rowSelected = false;
		columnSelected = false;
		selectedRowNo = 0;
		selectedColumnID = 0;
		break;
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgWM_CTLCOLORSTATIC(WPARAM wParam, LPARAM lParam)
{
	//Attempt to identify a cell associated with the target control
	HWND targetControl = (HWND)lParam;
	std::map<HWND, CellMapping>::const_iterator controlMappingIterator = childControlMapping.find(targetControl);
	if(controlMappingIterator == childControlMapping.end())
	{
		return DefWindowProc(hwnd, WM_CTLCOLORSTATIC, wParam, lParam);
	}

	//Retrieve information on the target cell
	const CellMapping& cellMapping = controlMappingIterator->second;
	CellControlInfo& cellInfo = customControlForCell[cellMapping.columnID][cellMapping.rowNo];

	//Set the background color to be the same as the background colour for the grid
	WinColor backgroundColor = userColorData.backgroundColorDefined? userColorData.colorBackground: defaultColorData.colorBackground;

	//If a background colour override has been specified for the target row, retrieve it.
	if(cellMapping.rowNo < (unsigned int)rowColorDataArray.size())
	{
		CustomColorData& rowColorData = rowColorDataArray[cellMapping.rowNo];
		if(rowColorData.backgroundColorDefined)
		{
			backgroundColor = rowColorData.colorBackground;
		}
	}

	//If a background colour override has been specified for the cell row, retrieve it.
	std::map<unsigned int, std::map<unsigned int, CustomColorData>>::const_iterator cellCustomColorDataIteratorForRow = cellCustomColorData.find(cellMapping.rowNo);
	if(cellCustomColorDataIteratorForRow != cellCustomColorData.end())
	{
		std::map<unsigned int, CustomColorData>::const_iterator cellCustomColorDataIterator = cellCustomColorDataIteratorForRow->second.find(cellMapping.columnID);
		if(cellCustomColorDataIterator != cellCustomColorDataIteratorForRow->second.end())
		{
			CustomColorData cellColorData = cellCustomColorDataIterator->second;
			if(cellColorData.backgroundColorDefined)
			{
				backgroundColor = cellColorData.colorBackground;
			}
		}
	}

	//If a background brush hasn't been generated for this cell, or if the background
	//colour has changed, create a new brush now.
	if(!cellInfo.lastBackgroundColorSet || (cellInfo.lastBackgroundColor != backgroundColor.GetColorREF()))
	{
		if(cellInfo.lastBackgroundColorSet)
		{
			DeleteObject(cellInfo.lastBackgroundColorBrush);
		}
		cellInfo.lastBackgroundColorBrush = CreateSolidBrush(backgroundColor.GetColorREF());
		cellInfo.lastBackgroundColor = backgroundColor.GetColorREF();
		cellInfo.lastBackgroundColorSet = true;
	}

	return (LRESULT)cellInfo.lastBackgroundColorBrush;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_SETMANUALSCROLLING(WPARAM wParam, LPARAM lParam)
{
	//If the new scrolling state is the same as the currently selected state, reject the
	//message and return 1.
	bool newManualScrollingState = (wParam != 0);
	if(newManualScrollingState == !autoScrollingManagement)
	{
		return 1;
	}

	//Apply the new manual scrolling state
	autoScrollingManagement = !newManualScrollingState;

	//If automatic scrolling is being enabled, calculate new vertical scroll values
	//automatically based on the current state.
	if(autoScrollingManagement)
	{
		RecalculateScrollPosition();
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_SETDATAAREAFONT(WPARAM wParam, LPARAM lParam)
{
	//Set the new font for the data area of this grid
	dataAreaFont = (HFONT)wParam;

	//Read the font metrics for our new font
	HDC hdc = GetDC(hwnd);
	HFONT hfontOld = (HFONT)SelectObject(hdc, dataAreaFont);
	TEXTMETRIC textMetric;
	GetTextMetrics(hdc, &textMetric);
	fontWidth = textMetric.tmAveCharWidth;
	fontHeight = textMetric.tmHeight;
	SelectObject(hdc, hfontOld);
	ReleaseDC(hwnd, hdc);

	//Update the window size information based on the new font metrics
	UpdateWindowSize();

	//Force the entire control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_INSERTCOLUMN(WPARAM wParam, LPARAM lParam)
{
	const Grid_InsertColumn& info = *((Grid_InsertColumn*)lParam);

	//Insert the new column into our header array
	columnData.push_back(ColumnData(info.name, info.columnID, info.editingAllowed, info.cellType));
	ColumnData* i = &(*columnData.rbegin());
	i->sizeMode = info.sizeMode;
	i->absoluteWidth = DPIScaleWidth(info.absoluteWidth);
	i->proportionalWidth = info.proportionalWidth;
	i->minWidth = (info.minWidth >= 0)? DPIScaleWidth(info.minWidth): info.minWidth;
	i->maxWidth = (info.maxWidth >= 0)? DPIScaleWidth(info.maxWidth): info.maxWidth;

	//Insert the new column into our ID index
	columnIDIndex.insert(ColumnIDIndexEntry(info.columnID, i));

	//Recalculate the highest column data index
	largestColumnDataArraySize = CalculateLargestColumnDataArraySize();

	//Insert the new column into our header control
	HDITEM hdItem;
	hdItem.mask = HDI_FORMAT | HDI_TEXT;
	hdItem.fmt = HDF_STRING;
	hdItem.cchTextMax = (int)info.name.length();
	hdItem.pszText = &i->name[0];
	int itemIndex = (int)SendMessage(hwndHeader, HDM_INSERTITEM, 99999, (LPARAM)&hdItem);
	if(itemIndex != -1)
	{
		i->index = itemIndex;
		columnIndex.insert(ColumnIndexEntry(itemIndex, i));
	}

	//Since the columns have changed, we need to rebuild our header order index
	RebuildHeaderOrder();

	//Recalculate all automatically sized column widths
	RecalculateColumnWidths();

	//If automatic scroll management is enabled, recalculate the new scroll settings based
	//on the new column list.
	if(autoScrollingManagement)
	{
		RecalculateScrollPosition();
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_DELETECOLUMN(WPARAM wParam, LPARAM lParam)
{
	unsigned int targetColumnID = (unsigned int)wParam;
	for(ColumnDataIterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		if(i->columnID == targetColumnID)
		{
			//Delete the column from the header control
			SendMessage(hwndHeader, HDM_DELETEITEM, (WPARAM)i->index, 0);

			//Erase the column from all our indexes
			columnIndex.erase(i->index);
			columnSortIndex.erase(i->order);
			columnIDIndex.erase(i->columnID);
			columnData.erase(i);

			//Recalculate the highest column data index
			largestColumnDataArraySize = CalculateLargestColumnDataArraySize();

			//Since the columns have changed, we need to rebuild our header order index
			RebuildHeaderOrder();

			//Recalculate all automatically sized column widths
			RecalculateColumnWidths();

			//If automatic scroll management is enabled, recalculate the new scroll
			//settings based on the new column list.
			if(autoScrollingManagement)
			{
				RecalculateScrollPosition();
			}

			//Update the current scrollbar state
			UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);

			//Force the control to redraw
			ForceControlRedraw();

			return 0;
		}
	}
	return 1;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_INSERTROWS(WPARAM wParam, LPARAM lParam)
{
	const Grid_InsertRows& info = *((Grid_InsertRows*)lParam);

	//Resize all our data buffers as required
	if((size_t)info.targetRowNo > rowColorDataArray.size())
	{
		rowColorDataArray.resize(info.targetRowNo);
	}
	rowColorDataArray.insert(rowColorDataArray.begin() + info.targetRowNo, info.rowCount, CustomColorData());
	std::set<unsigned int> cellCustomColorDataKeysToErase;
	std::map<unsigned int, std::map<unsigned int, CustomColorData>> cellCustomColorDataNewValues;
	for(std::map<unsigned int, std::map<unsigned int, CustomColorData>>::iterator i = cellCustomColorData.begin(); i != cellCustomColorData.end(); ++i)
	{
		if(i->first >= info.targetRowNo)
		{
			cellCustomColorDataKeysToErase.insert(i->first);
			cellCustomColorDataNewValues.insert(std::pair<unsigned int, std::map<unsigned int, CustomColorData>>(i->first + info.rowCount, i->second));
		}
	}
	for(std::set<unsigned int>::const_iterator i = cellCustomColorDataKeysToErase.begin(); i != cellCustomColorDataKeysToErase.end(); ++i)
	{
		cellCustomColorData.erase(*i);
	}
	for(std::map<unsigned int, std::map<unsigned int, CustomColorData>>::const_iterator i = cellCustomColorDataNewValues.begin(); i != cellCustomColorDataNewValues.end(); ++i)
	{
		cellCustomColorData.insert(std::pair<unsigned int, std::map<unsigned int, CustomColorData>>(i->first, i->second));
	}
	for(std::map<unsigned int, std::vector<CellControlInfo>>::iterator i = customControlForCell.begin(); i != customControlForCell.end(); ++i)
	{
		std::vector<CellControlInfo>& customControlDataForColumn = i->second;
		if((size_t)info.targetRowNo < customControlDataForColumn.size())
		{
			customControlDataForColumn.insert(customControlDataForColumn.begin() + info.targetRowNo, info.rowCount, CellControlInfo(columnIDIndex[i->first]->controlType));
		}
	}
	for(std::map<HWND, CellMapping>::iterator i = childControlMapping.begin(); i != childControlMapping.end(); ++i)
	{
		CellMapping& cellMapping = i->second;
		if(cellMapping.rowNo >= info.targetRowNo)
		{
			cellMapping.rowNo += info.rowCount;
		}
	}
	for(ColumnDataIterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		if((size_t)info.targetRowNo > i->dataBuffer.size())
		{
			i->dataBuffer.resize(info.targetRowNo);
		}
		i->dataBuffer.insert(i->dataBuffer.begin() + info.targetRowNo, info.rowCount, std::wstring());
	}

	//Insert the new row data into our data arrays
	for(size_t i = 0; (i < info.rowCount) && (i < info.rowData.size()); ++i)
	{
		const std::map<unsigned int, std::wstring>& dataForRow = info.rowData[i];
		for(std::map<unsigned int, std::wstring>::const_iterator columnDataIterator = dataForRow.begin(); columnDataIterator != dataForRow.end(); ++columnDataIterator)
		{
			//Retrieve information on the specified column
			ColumnIDIndexIterator columnIterator = columnIDIndex.find(columnDataIterator->first);
			if(columnIterator == columnIDIndex.end())
			{
				continue;
			}
			ColumnData* column = columnIterator->second;

			//Load the specified data into the data buffer for this column
			column->dataBuffer[i] = columnDataIterator->second;
		}
	}

	//Recalculate the new highest column data index
	largestColumnDataArraySize = CalculateLargestColumnDataArraySize();

	//Update the number of the cell currently being edited if required
	if(editModeActive && (selectedRowNo >= info.targetRowNo))
	{
		selectedRowNo += info.rowCount;
	}

	//Recalculate all automatically sized column widths
	RecalculateColumnWidths();

	//If automatic scroll management is enabled, recalculate the new scroll settings based
	//on the new window size.
	if(autoScrollingManagement)
	{
		RecalculateScrollPosition();
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_DELETEROWS(WPARAM wParam, LPARAM lParam)
{
	const Grid_DeleteRows& info = *((Grid_DeleteRows*)lParam);

	//Resize any data buffers that need to be adjusted
	if((size_t)info.targetRowNo < rowColorDataArray.size())
	{
		size_t elementsToRemove = ((info.targetRowNo + info.rowCount) > rowColorDataArray.size())? rowColorDataArray.size() - info.targetRowNo: (size_t)info.rowCount;
		rowColorDataArray.erase(rowColorDataArray.begin() + info.targetRowNo, rowColorDataArray.begin() + info.targetRowNo + elementsToRemove);
	}
	std::set<unsigned int> cellCustomColorDataKeysToErase;
	std::map<unsigned int, std::map<unsigned int, CustomColorData>> cellCustomColorDataNewValues;
	for(std::map<unsigned int, std::map<unsigned int, CustomColorData>>::iterator i = cellCustomColorData.begin(); i != cellCustomColorData.end(); ++i)
	{
		if(i->first >= info.targetRowNo)
		{
			cellCustomColorDataKeysToErase.insert(i->first);
			if(i->first >= (info.targetRowNo + info.rowCount))
			{
				cellCustomColorDataNewValues.insert(std::pair<unsigned int, std::map<unsigned int, CustomColorData>>(i->first - info.rowCount, i->second));
			}
		}
	}
	for(std::set<unsigned int>::const_iterator i = cellCustomColorDataKeysToErase.begin(); i != cellCustomColorDataKeysToErase.end(); ++i)
	{
		cellCustomColorData.erase(*i);
	}
	for(std::map<unsigned int, std::map<unsigned int, CustomColorData>>::const_iterator i = cellCustomColorDataNewValues.begin(); i != cellCustomColorDataNewValues.end(); ++i)
	{
		cellCustomColorData.insert(std::pair<unsigned int, std::map<unsigned int, CustomColorData>>(i->first, i->second));
	}
	for(std::map<unsigned int, std::vector<CellControlInfo>>::iterator i = customControlForCell.begin(); i != customControlForCell.end(); ++i)
	{
		std::vector<CellControlInfo>& customControlDataForColumn = i->second;
		if((size_t)info.targetRowNo < customControlDataForColumn.size())
		{
			size_t elementsToRemove = ((info.targetRowNo + info.rowCount) > customControlDataForColumn.size())? customControlDataForColumn.size() - info.targetRowNo: (size_t)info.rowCount;
			customControlDataForColumn.erase(customControlDataForColumn.begin() + info.targetRowNo, customControlDataForColumn.begin() + info.targetRowNo + elementsToRemove);
		}
	}
	std::set<HWND> childControlMappingKeysToErase;
	for(std::map<HWND, CellMapping>::iterator i = childControlMapping.begin(); i != childControlMapping.end(); ++i)
	{
		CellMapping& cellMapping = i->second;
		if(cellMapping.rowNo >= info.targetRowNo)
		{
			if(cellMapping.rowNo < (info.targetRowNo + info.rowCount))
			{
				childControlMappingKeysToErase.insert(i->first);
			}
			else
			{
				cellMapping.rowNo -= info.rowCount;
			}
		}
	}
	for(std::set<HWND>::const_iterator i = childControlMappingKeysToErase.begin(); i != childControlMappingKeysToErase.end(); ++i)
	{
		childControlMapping.erase(*i);
	}
	for(ColumnDataIterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		if((size_t)info.targetRowNo < i->dataBuffer.size())
		{
			size_t elementsToRemove = ((info.targetRowNo + info.rowCount) > i->dataBuffer.size())? i->dataBuffer.size() - info.targetRowNo: (size_t)info.rowCount;
			i->dataBuffer.erase(i->dataBuffer.begin() + info.targetRowNo, i->dataBuffer.begin() + info.targetRowNo + elementsToRemove);
		}
	}

	//Recalculate the new highest column data index
	largestColumnDataArraySize = CalculateLargestColumnDataArraySize();

	//Update the number of the cell currently being edited if required
	if(editModeActive && (selectedRowNo >= info.targetRowNo))
	{
		if(selectedRowNo < (info.targetRowNo + info.rowCount))
		{
			CancelCellEditing();
		}
		else
		{
			selectedRowNo -= info.rowCount;
		}
	}

	//Recalculate all automatically sized column widths
	RecalculateColumnWidths();

	//If automatic scroll management is enabled, recalculate the new scroll settings based
	//on the new window size.
	if(autoScrollingManagement)
	{
		RecalculateScrollPosition();
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_GETCOLUMNINFO(WPARAM wParam, LPARAM lParam)
{
	unsigned int targetColumnID = (unsigned int)wParam;
	ColumnIDIndexIterator columnIterator = columnIDIndex.find(targetColumnID);
	if(columnIterator == columnIDIndex.end())
	{
		return 1;
	}

	ColumnData* i = columnIterator->second;
	Grid_GetColumnInfo& info = *((Grid_GetColumnInfo*)lParam);
	info.columnID = i->columnID;
	info.name = i->name;
	info.width = i->actualWidth;
	info.order = i->order;
	info.editingAllowed = i->editingAllowed;
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_SETCOLUMNINFO(WPARAM wParam, LPARAM lParam)
{
	unsigned int targetColumnID = (unsigned int)wParam;
	ColumnIDIndexIterator columnIterator = columnIDIndex.find(targetColumnID);
	if(columnIterator != columnIDIndex.end())
	{
		const Grid_SetColumnInfo& info = *((Grid_SetColumnInfo*)lParam);
		ColumnData* i = columnIterator->second;

		//If cell editing is about to be disabled for this column, and we have a currently
		//active cell editing operation to this column, complete the edit process.
		if((i->editingAllowed && !info.editingAllowed) && editModeActive && (selectedColumnID == i->columnID))
		{
			CompleteCellEditing();
		}

		//Update the info for this column
		i->columnID = info.columnID;
		i->name = info.name;
		i->editingAllowed = info.editingAllowed;
		i->sizeMode = info.sizeMode;
		i->absoluteWidth = info.absoluteWidth;
		i->proportionalWidth = info.proportionalWidth;
		i->minWidth = info.minWidth;
		i->maxWidth = info.maxWidth;

		//Recalculate all automatically sized column widths
		RecalculateColumnWidths();

		//Update the title of this column in our edit control
		HDITEM hdItem;
		hdItem.mask = HDI_FORMAT | HDI_TEXT;
		hdItem.fmt = HDF_STRING;
		hdItem.cchTextMax = (int)i->name.length();
		hdItem.pszText = &i->name[0];
		SendMessage(hwndHeader, HDM_SETITEM, (WPARAM)i->index, (LPARAM)&hdItem);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_SETCELLINFO(WPARAM wParam, LPARAM lParam)
{
	const Grid_SetCellInfo& cellInfo = *((Grid_SetCellInfo*)lParam);

	if(!cellInfo.defined)
	{
		customControlForCell.erase(cellInfo.columnID);
	}
	else
	{
		std::vector<CellControlInfo>& cellInfoArray = customControlForCell[cellInfo.columnID];
		if(cellInfo.rowNo >= (unsigned int)cellInfoArray.size())
		{
			cellInfoArray.resize(cellInfo.rowNo + 1, CellControlInfo(columnIDIndex[cellInfo.columnID]->controlType));
		}
		CellControlInfo& cellInfoInternal = cellInfoArray[cellInfo.rowNo];
		cellInfoInternal.controlType = cellInfo.controlType;
		cellInfoInternal.treeEntryIndentLevel = cellInfo.treeEntryIndentLevel;
		cellInfoInternal.showExpandIcon = cellInfo.showExpandIcon;
		cellInfoInternal.expandIconIsExpanded = cellInfo.expandIconIsExpanded;
		cellInfoInternal.selectionList = cellInfo.selectionList;
		cellInfoInternal.customButtonText = cellInfo.customButtonText;
		cellInfoInternal.pickFromSelectionListOnly = cellInfo.pickFromSelectionListOnly;
		cellInfoInternal.ellipsisMode = cellInfo.ellipsisMode;
	}

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_UPDATECELLTEXT(WPARAM wParam, LPARAM lParam)
{
	const Grid_UpdateCellText& info = *((Grid_UpdateCellText*)lParam);

	//Retrieve information on the specified column
	ColumnIDIndexIterator columnIterator = columnIDIndex.find(info.columnID);
	if(columnIterator == columnIDIndex.end())
	{
		return 1;
	}
	ColumnData* column = columnIterator->second;

	//Ensure the specified row number lies within the data array for the specified column
	bool resizedDataBuffer = false;
	if((size_t)info.rowNo >= column->dataBuffer.size())
	{
		if(!autoScrollingManagement)
		{
			//If auto scrolling management is disabled, and the row number is larger than
			//the data buffer, return a failure, since the specified row number must
			//correspond to a visible row number in this case.
			return 1;
		}
		else
		{
			//If auto scrolling management is active, and the row number is larger than
			//the data buffer, extend the data buffer to include the target row.
			column->dataBuffer.resize((size_t)info.rowNo + 1);
			resizedDataBuffer = true;
		}
	}

	//If the specified cell text is the same as the current cell data, abort any further
	//processing.
	if(!resizedDataBuffer && (column->dataBuffer[info.rowNo] == info.newText))
	{
		return 0;
	}

	//Load the specified cell data into the data buffer for the target column
	column->dataBuffer[info.rowNo] = info.newText;

	//Recalculate all automatically sized column widths
	RecalculateColumnWidths();

	//Calculate the new the highest column data index
	unsigned int newLargestColumnDataArraySize = CalculateLargestColumnDataArraySize();

	//If the highest column data index has changed, apply the new setting, and update
	//scroll information for the control.
	if(largestColumnDataArraySize != newLargestColumnDataArraySize)
	{
		largestColumnDataArraySize = newLargestColumnDataArraySize;
		if(autoScrollingManagement)
		{
			//Calculate new vertical scroll values automatically based on the current state
			RecalculateScrollPosition();

			//Update the current scrollbar state
			UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);
		}
	}

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_UPDATECOLUMNTEXT(WPARAM wParam, LPARAM lParam)
{
	unsigned int targetColumnID = (unsigned int)wParam;
	const std::vector<std::wstring>& text = *((std::vector<std::wstring>*)lParam);

	//Retrieve information on the specified column
	ColumnIDIndexIterator columnIterator = columnIDIndex.find(targetColumnID);
	if(columnIterator == columnIDIndex.end())
	{
		return 1;
	}
	ColumnData* column = columnIterator->second;

	//Update the data for the target column
	if(!autoScrollingManagement)
	{
		for(unsigned int i = 0; (i < text.size()) && (i < column->dataBuffer.size()); ++i)
		{
			column->dataBuffer[i] = text[i];
		}
	}
	else
	{
		column->dataBuffer = text;
	}

	//Recalculate all automatically sized column widths
	RecalculateColumnWidths();

	//Calculate the new the highest column data index
	unsigned int newLargestColumnDataArraySize = CalculateLargestColumnDataArraySize();

	//If the highest column data index has changed, apply the new setting, and update
	//scroll information for the control.
	if(largestColumnDataArraySize != newLargestColumnDataArraySize)
	{
		largestColumnDataArraySize = newLargestColumnDataArraySize;
		if(autoScrollingManagement)
		{
			//Calculate new vertical scroll values automatically based on the current state
			RecalculateScrollPosition();

			//Update the current scrollbar state
			UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);
		}
	}

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_SETCONTROLCOLOR(WPARAM wParam, LPARAM lParam)
{
	const Grid_SetControlColor& controlColorInfo = *((Grid_SetControlColor*)lParam);
	userColorData.backgroundColorDefined = controlColorInfo.backgroundColorDefined;
	userColorData.colorBackground = controlColorInfo.colorBackground;
	userColorData.textColorDefined = controlColorInfo.textColorDefined;
	userColorData.colorTextFront = controlColorInfo.colorTextFront;
	userColorData.colorTextBack = controlColorInfo.colorTextBack;
	userColorData.editColorDefined = controlColorInfo.editColorDefined;
	userColorData.colorEditTextFront = controlColorInfo.colorEditTextFront;
	userColorData.colorEditTextBack = controlColorInfo.colorEditTextBack;
	userColorData.lineColorDefined = controlColorInfo.lineColorDefined;
	userColorData.colorLine = controlColorInfo.colorLine;
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_SETROWCOLOR(WPARAM wParam, LPARAM lParam)
{
	unsigned int targetRowID = (unsigned int)wParam;
	const Grid_SetRowColor& rowColorInfo = *((Grid_SetRowColor*)lParam);
	if(targetRowID >= (unsigned int)rowColorDataArray.size())
	{
		return 1;
	}

	//Save the custom colour information for the target row
	CustomColorData& rowColorData = rowColorDataArray[targetRowID];
	rowColorData.backgroundColorDefined = rowColorInfo.backgroundColorDefined;
	rowColorData.colorBackground = rowColorInfo.colorBackground;
	rowColorData.textColorDefined = rowColorInfo.textColorDefined;
	rowColorData.colorTextFront = rowColorInfo.colorTextFront;
	rowColorData.colorTextBack = rowColorInfo.colorTextBack;
	rowColorData.editColorDefined = rowColorInfo.editColorDefined;
	rowColorData.colorEditTextFront = rowColorInfo.colorEditTextFront;
	rowColorData.colorEditTextBack = rowColorInfo.colorEditTextBack;

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_SETCELLCOLOR(WPARAM wParam, LPARAM lParam)
{
	const Grid_SetCellColor& cellColorInfo = *((Grid_SetCellColor*)lParam);

	//If a custom colour has been undefined for the target cell, remove it, otherwise add
	//it to the set of customized cell colours.
	if(!cellColorInfo.backgroundColorDefined && !cellColorInfo.editColorDefined && !cellColorInfo.textColorDefined)
	{
		cellCustomColorData[cellColorInfo.targetRowNo].erase(cellColorInfo.targetColumnID);
	}
	else
	{
		CustomColorData& cellColorData = cellCustomColorData[cellColorInfo.targetRowNo][cellColorInfo.targetColumnID];
		cellColorData.backgroundColorDefined = cellColorInfo.backgroundColorDefined;
		cellColorData.colorBackground = cellColorInfo.colorBackground;
		cellColorData.textColorDefined = cellColorInfo.textColorDefined;
		cellColorData.colorTextFront = cellColorInfo.colorTextFront;
		cellColorData.colorTextBack = cellColorInfo.colorTextBack;
		cellColorData.editColorDefined = cellColorInfo.editColorDefined;
		cellColorData.colorEditTextFront = cellColorInfo.colorEditTextFront;
		cellColorData.colorEditTextBack = cellColorInfo.colorEditTextBack;
	}

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_GETROWCOUNT(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)visibleRows;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DataGrid::msgGRID_SETVSCROLLINFO(WPARAM wParam, LPARAM lParam)
{
	//If automatic scrolling management is currently enabled, we reject this message, and
	//return 1.
	if(autoScrollingManagement)
	{
		return 1;
	}

	//Apply the new vscroll settings
	const Grid_SetVScrollInfo& info = *((Grid_SetVScrollInfo*)lParam);
	vscrollCurrent = info.currentPos;
	vscrollValuesPerPage = info.valuesPerPage;
	vscrollMin = info.minPos;
	vscrollMax = (info.maxPos > info.valuesPerPage)? info.maxPos - info.valuesPerPage: 0;
	vscrollMaxTrueLimit = info.maxPos;

	//Update the current scrollbar state
	UpdateScrollbarSettings(currentScrollHOffset, vscrollCurrent, true);

	//Force the control to redraw
	ForceControlRedraw();

	return 0;
}
