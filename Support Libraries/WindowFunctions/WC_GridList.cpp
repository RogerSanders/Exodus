#include "WC_GridList.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WC_GridList::WC_GridList(HINSTANCE amoduleHandle, HWND ahwnd)
:moduleHandle(amoduleHandle), hwnd(ahwnd)
{
	hwndHeader = NULL;
	hwndList = NULL;
	hfont = NULL;

	userColorData.defined = false;
	defaultColorData.colorBackground = WinColor(255, 255, 255);
	defaultColorData.colorTextFront = WinColor(0, 0, 0);
	defaultColorData.colorTextBack = defaultColorData.colorBackground;
	defaultColorData.colorLine = WinColor(128, 128, 255);

	controlWidth = 0;
	controlHeight = 0;

	autoScrollingManagement = true;
	vscrollMin = 0;
	vscrollMax = 0;
	vscrollCurrent = 0;
	vscrollValuesPerPage = 0;
	currentScrollHOffset = 0;
}

//----------------------------------------------------------------------------------------
//Message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK WC_GridList::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		WC_GridList* object = (WC_GridList*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->WndProcPrivate(message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		WC_GridList* object = new WC_GridList((HINSTANCE)createStruct->lpCreateParams, hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->WndProcPrivate(message, wParam, lParam);}
	case WM_CLOSE:{
		WC_GridList* object = (WC_GridList*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		LPARAM result = object->WndProcPrivate(message, wParam, lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
		delete object;
		return result;}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		return msgWM_CREATE(wParam, lParam);
	case WM_SIZE:
		return msgWM_SIZE(wParam, lParam);
	case WM_NOTIFY:
		return msgWM_NOTIFY(wParam, lParam);
	case WM_PAINT:
		return msgWM_PAINT(wParam, lParam);
	case WM_PRINTCLIENT:
		return msgWM_PRINTCLIENT(wParam, lParam);
	case WM_SETFONT:
		return msgWM_SETFONT(wParam, lParam);
	case WM_VSCROLL:
		return msgWM_VSCROLL(wParam, lParam);
	case WM_HSCROLL:
		return msgWM_HSCROLL(wParam, lParam);
	case WM_KEYDOWN:
		return msgWM_KEYDOWN(wParam, lParam);
	case WM_LBUTTONDOWN:
		return msgWM_LBUTTONDOWN(wParam, lParam);
	case WM_MOUSEWHEEL:
		return msgWM_MOUSEWHEEL(wParam, lParam);

	case GRID_SETMANUALSCROLLING:
		return msgGRID_SETMANUALSCROLLING(wParam, lParam);
	case GRID_SETDATAAREAFONT:
		return msgGRID_SETDATAAREAFONT(wParam, lParam);
	case GRID_INSERTCOLUMN:
		return msgGRID_INSERTCOLUMN(wParam, lParam);
	case GRID_DELETECOLUMN:
		return msgGRID_DELETECOLUMN(wParam, lParam);
	case GRID_SETCOLUMNINFO:
		return msgGRID_SETCOLUMNINFO(wParam, lParam);
	case GRID_GETCOLUMNINFO:
		return msgGRID_GETCOLUMNINFO(wParam, lParam);
	case GRID_UPDATECOLUMNTEXT:
		return msgGRID_UPDATECOLUMNTEXT(wParam, lParam);
	case GRID_SETCONTROLCOLOR:
		return msgGRID_SETCONTROLCOLOR(wParam, lParam);
	case GRID_SETROWCOLOR:
		return msgGRID_SETROWCOLOR(wParam, lParam);
	case GRID_GETROWCOUNT:
		return msgGRID_GETROWCOUNT(wParam, lParam);
	case GRID_SETVSCROLLINFO:
		return msgGRID_SETVSCROLLINFO(wParam, lParam);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
void WC_GridList::RebuildHeaderOrder()
{
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
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
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
LRESULT WC_GridList::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	controlWidth = rect.right;
	controlHeight = rect.bottom;

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
	MoveWindow(hwndHeader, (int)headerPosX - currentScrollHOffset, headerPosY, (int)headerWidth + currentScrollHOffset, headerHeight, FALSE);

	//Update size information based on the new control width and height
	UpdateWindowSize();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_NOTIFY(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = (NMHDR*)lParam;
	if(nmhdr->idFrom == IDC_HEADER)
	{
		if((nmhdr->code == HDN_ITEMCHANGED))
		{
			//If the user has resized a column, update the column width
			NMHEADER* nmHeader = (NMHEADER*)lParam;
			ColumnIndexIterator i = columnIndex.find(nmHeader->iItem);
			if(i != columnIndex.end())
			{
				if((nmHeader->pitem->mask & HDI_WIDTH) != 0)
				{
					i->second->width = nmHeader->pitem->cxy;
				}
			}

			//Update the current scrollbar state
			UpdateScrollbarSettings();

			//Force the control to redraw
			InvalidateRect(hwnd, NULL, FALSE);
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
			InvalidateRect(hwnd, NULL, FALSE);
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_PAINT(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintInfo;
	BeginPaint(hwnd, &paintInfo);
	HDC hdc = paintInfo.hdc;


	//Create a bitmap we can render the control onto
	HDC hdcControl = hdc;
	HDC hdcBitmap = CreateCompatibleDC(hdcControl);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdcControl, controlWidth, controlHeight);
	HBITMAP hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

	//Send a WM_PRINTCLIENT message to the native control and get it to render into
	//the bitmap.
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
	int overRenderWidth = (int)controlWidth - (totalHeaderWidth - currentScrollHOffset);
	if(overRenderWidth < 0)
	{
		overRenderWidth = 0;
	}

	//Render the header into the screen buffer
	hdcControl = hdc;
	hdcBitmap = CreateCompatibleDC(hdcControl);
	hbitmap = CreateCompatibleBitmap(hdcControl, totalHeaderWidth + overRenderWidth, headerHeight);
	hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

	//Send a WM_PRINTCLIENT message to the native control and get it to render into
	//the bitmap.
	SendMessage(hwndHeader, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_CLIENT | PRF_NONCLIENT | PRF_CHILDREN);

	//Transfer the final bitmap for the control into the screen buffer
	BitBlt(hdcControl, (int)headerPosX, (int)headerPosY, (int)headerWidth  + overRenderWidth, (int)headerHeight, hdcBitmap, currentScrollHOffset, 0, SRCCOPY);

	//Clean up the allocated handles
	SelectObject(hdcBitmap, hbitmapOriginal);
	DeleteObject(hbitmap);
	DeleteDC(hdcBitmap);


	//Validate the entire client area of the control. It's REALLY important that we
	//call this here, otherwise Windows is going to send WM_PAINT messages over and
	//over again waiting for the region to be validated.
	ValidateRect(hwnd, NULL);

	EndPaint(hwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_PRINTCLIENT(WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;

	//Determine which color values to use for various display elements
	COLORREF backgroundColor = userColorData.defined? userColorData.colorBackground.GetColorREF(): defaultColorData.colorBackground.GetColorREF();
	COLORREF textFrontColor = userColorData.defined? userColorData.colorTextFront.GetColorREF(): defaultColorData.colorTextFront.GetColorREF();
	COLORREF textBackColor = userColorData.defined? userColorData.colorTextBack.GetColorREF(): defaultColorData.colorTextBack.GetColorREF();
	COLORREF lineColor = userColorData.defined? userColorData.colorLine.GetColorREF(): defaultColorData.colorLine.GetColorREF();

	//Fill the window with the background colour
	HBRUSH backgroundBrush = CreateSolidBrush(backgroundColor);
	HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
	PatBlt(hdc, 0, headerHeight, controlWidth, controlHeight, PATCOPY);
	SelectObject(hdc, backgroundBrushOld);
	DeleteObject(backgroundBrush);

	//Switch to our font
	HFONT hfontOld;
	hfontOld = (HFONT)SelectObject(hdc, hfont);

	//Draw the columns
	int textStartPosX = (int)headerPosX;
	int textStartPosY = (int)(headerPosY + headerHeight);
	for(unsigned int row = 0; row < visibleRows; ++row)
	{
		//Determine the color data to use for this row
		bool useCustomBackgroundColor = false;
		int customBackgroundColor;
		unsigned int rowColorDataArrayIndex = (!autoScrollingManagement)? row: (vscrollCurrent + row);
		if((rowColorDataArrayIndex < (unsigned int)rowColorDataArray.size()) && rowColorDataArray[rowColorDataArrayIndex].defined)
		{
			CustomColorData& rowColorData = rowColorDataArray[rowColorDataArrayIndex];
			SetTextColor(hdc, rowColorData.colorTextFront.GetColorREF());
			SetBkColor(hdc, rowColorData.colorTextBack.GetColorREF());

			//Check if a background color has been specified which is different from the
			//default background color
			if(rowColorData.colorBackground.GetColorREF() != backgroundColor)
			{
				useCustomBackgroundColor = true;
				customBackgroundColor = rowColorData.colorBackground.GetColorREF();
			}
		}
		else
		{
			SetTextColor(hdc, textFrontColor);
			SetBkColor(hdc, textBackColor);
		}

		//Draw each column in this row
		int columnStartPos = 0 - currentScrollHOffset;
		for(ColumnSortIndexIterator i = columnSortIndex.begin(); i != columnSortIndex.end(); ++i)
		{
			ColumnData& columnData = *(i->second);

			//Apply a custom background color for this column if one has been specified
			if(useCustomBackgroundColor)
			{
				HBRUSH backgroundBrush = CreateSolidBrush(customBackgroundColor);
				HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
				PatBlt(hdc, textStartPosX + columnStartPos, textStartPosY + (int)(row * fontHeight), (int)columnData.width, (int)fontHeight, PATCOPY);
				SelectObject(hdc, backgroundBrushOld);
				DeleteObject(backgroundBrush);
			}

			//Draw the text for this column, this row
			unsigned int columnDataArrayIndex = (!autoScrollingManagement)? row: (vscrollCurrent + row);
			if(columnDataArrayIndex < columnData.dataBuffer.size())
			{
				int marginSize = 2;
				RECT rect;
				rect.left = textStartPosX + columnStartPos + marginSize;
				rect.right = (textStartPosX + columnStartPos + (int)columnData.width) - marginSize;
				rect.top = textStartPosY + (int)(row * fontHeight);
				rect.bottom = textStartPosY + (int)((row + 1) * fontHeight);
				DrawText(hdc, &columnData.dataBuffer[columnDataArrayIndex][0], (int)columnData.dataBuffer[columnDataArrayIndex].size(), &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			}

			//Draw the dividing line marking the end of this column
			HPEN hpen = CreatePen(PS_SOLID, 0, lineColor);
			HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
			MoveToEx(hdc, columnStartPos + (int)(columnData.width - 1), textStartPosY, NULL);
			LineTo(hdc, columnStartPos + (int)(columnData.width - 1), textStartPosY + (int)(visibleRows * fontHeight));
			SelectObject(hdc, hpenOld);
			DeleteObject(hpen);

			//Update columnStartPos for the next column
			columnStartPos += (int)columnData.width;
		}
	}

	//Restore the state of the device context, and free any allocated handles
	SelectObject(hdc, hfontOld);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_SETFONT(WPARAM wParam, LPARAM lParam)
{
	return SendMessage(hwndHeader, WM_SETFONT, wParam, lParam);
}

//----------------------------------------------------------------------------------------
//Window size functions
//----------------------------------------------------------------------------------------
unsigned int WC_GridList::GetTotalRowWidth() const
{
	unsigned int totalRowWidth = 0;
	for(ColumnDataConstIterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		totalRowWidth += i->width;
	}
	return totalRowWidth;
}

//----------------------------------------------------------------------------------------
void WC_GridList::UpdateWindowSize()
{
	//Calculate the maximum number of rows which can be shown within the control
	unsigned int textAreaHeight = controlHeight - headerHeight;
	if(controlHeight < headerHeight)
	{
		textAreaHeight = 0;
	}
	unsigned int newVisibleRows = (textAreaHeight / fontHeight) + 1;

	//If we're not using automatic scroll management, and more rows need to be shown after
	//the resize, update the data buffer sizes for each column.
	if(!autoScrollingManagement && (newVisibleRows > visibleRows))
	{
		rowColorDataArray.resize(newVisibleRows);
		for(ColumnDataIterator i = columnData.begin(); i != columnData.end(); ++i)
		{
			if(newVisibleRows > (unsigned int)i->dataBuffer.size())
			{
				i->dataBuffer.resize(newVisibleRows);
			}
		}
	}

	//If the number of visible rows has changed, update the stored visible row state.
	if(newVisibleRows != visibleRows)
	{
		//Update the number of rows to be displayed
		visibleRows = newVisibleRows;

		//If the number of visible rows has changed, send a message to the parent control
		//to notify it about the change in visible rows.
		Grid_NewRowCount newRowCountState;
		newRowCountState.visibleRows = newVisibleRows;
		SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_NEWROWCOUNT), (LPARAM)&newRowCountState);
	}

	//If automatic scroll management is enabled, recalculate the new scroll settings based
	//on the new window size.
	if(autoScrollingManagement)
	{
		RecalculateScrollPosition();
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);
}

//----------------------------------------------------------------------------------------
void WC_GridList::RecalculateScrollPosition()
{
	//Reset the vertical scroll minimum value and number of values per page
	vscrollMin = 0;
	vscrollValuesPerPage = visibleRows;

	//Calculate a new maximum vertical scroll position, using the size of the largest
	//content buffer.
	int newVScrollMax = 0;
	for(std::list<ColumnData>::const_iterator i = columnData.begin(); i != columnData.end(); ++i)
	{
		int thisColumnSize = (int)i->dataBuffer.size();
		newVScrollMax = (thisColumnSize > newVScrollMax)? thisColumnSize: newVScrollMax;
	}
	vscrollMax = (newVScrollMax > vscrollValuesPerPage)? newVScrollMax - vscrollValuesPerPage: 0;

	//Clamp the current vertical scroll position to the new maximum vertical scroll
	//position
	vscrollCurrent = (vscrollCurrent > vscrollMax)? vscrollMax: vscrollCurrent;
}

//----------------------------------------------------------------------------------------
void WC_GridList::UpdateScrollbarSettings()
{
	//Calculate the new settings for the horizontal scrollbar
	unsigned int totalRowWidth = GetTotalRowWidth();

	//Clamp the horizontal scroll offset to the upper and lower boundaries
	if((currentScrollHOffset + (int)controlWidth) > (int)totalRowWidth)
	{
		currentScrollHOffset = (int)totalRowWidth - (int)controlWidth;
	}
	if(currentScrollHOffset < 0)
	{
		currentScrollHOffset = 0;
	}

	//Reposition the header control to handle horizontal scrolling
	MoveWindow(hwndHeader, (int)headerPosX - currentScrollHOffset, headerPosY, (int)headerWidth + currentScrollHOffset, headerHeight, FALSE);

	//Apply the latest horizontal scrollbar settings
	SCROLLINFO hscrollInfo;
	hscrollInfo.cbSize = sizeof(hscrollInfo);
	hscrollInfo.nMin = 0;
	hscrollInfo.nMax = (int)totalRowWidth - 1;
	hscrollInfo.nPos = (int)currentScrollHOffset;
	hscrollInfo.nPage = (int)controlWidth;
	hscrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	SetScrollInfo(hwnd, SB_HORZ, &hscrollInfo, TRUE);

	//Apply the latest vertical scrollbar settings
	SCROLLINFO vscrollInfo;
	vscrollInfo.cbSize = sizeof(vscrollInfo);
	vscrollInfo.nMin = vscrollMin;
	vscrollInfo.nMax = vscrollMax;
	vscrollInfo.nPos = vscrollCurrent;
	vscrollInfo.nPage = vscrollValuesPerPage;
	vscrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	SetScrollInfo(hwnd, SB_VERT, &vscrollInfo, TRUE);
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_VSCROLL(WPARAM wParam, LPARAM lParam)
{
	//Handle this scroll event
	switch(LOWORD(wParam))
	{
	case SB_THUMBTRACK:{
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(hwnd, SB_VERT, &scrollInfo);

		if(autoScrollingManagement)
		{
			vscrollCurrent = (unsigned int)scrollInfo.nTrackPos;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = (unsigned int)scrollInfo.nTrackPos;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_NEWSCROLLPOSITION), (LPARAM)&info);
		}
		break;}
	case SB_TOP:
		if(autoScrollingManagement)
		{
			vscrollCurrent = vscrollMin;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = vscrollMin;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_NEWSCROLLPOSITION), (LPARAM)&info);
		}
		break;
	case SB_BOTTOM:
		if(autoScrollingManagement)
		{
			vscrollCurrent = vscrollMax;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = vscrollMax;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_NEWSCROLLPOSITION), (LPARAM)&info);
		}
		break;
	case SB_PAGEUP:
		if(autoScrollingManagement)
		{
			vscrollCurrent = ((vscrollCurrent >= (int)visibleRows) && ((vscrollCurrent - (int)visibleRows) >= vscrollMin))? vscrollCurrent - (int)visibleRows: vscrollMin;
		}
		else
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = visibleRows;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSUP), (LPARAM)&info);
		}
		break;
	case SB_LINEUP:
		if(autoScrollingManagement)
		{
			vscrollCurrent = ((vscrollCurrent >= 1) && ((vscrollCurrent - 1) >= vscrollMin))? vscrollCurrent - 1: vscrollMin;
		}
		else
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = 1;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSUP), (LPARAM)&info);
		}
		break;
	case SB_PAGEDOWN:
		if(autoScrollingManagement)
		{
			vscrollCurrent = ((vscrollCurrent + (int)visibleRows) <= vscrollMax)? vscrollCurrent + (int)visibleRows: vscrollMax;
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = visibleRows;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSDOWN), (LPARAM)&info);
		}
		break;
	case SB_LINEDOWN:
		if(autoScrollingManagement)
		{
			vscrollCurrent = ((vscrollCurrent + 1) <= vscrollMax)? vscrollCurrent + 1: vscrollMax;
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = 1;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSDOWN), (LPARAM)&info);
		}
		break;
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_HSCROLL(WPARAM wParam, LPARAM lParam)
{
	//Handle this scroll event
	unsigned int totalRowWidth = GetTotalRowWidth();
	switch(LOWORD(wParam))
	{
	case SB_THUMBTRACK:{
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(hwnd, SB_HORZ, &scrollInfo);
		currentScrollHOffset = scrollInfo.nTrackPos;
		break;}
	case SB_LEFT:{
		currentScrollHOffset = 0;
		break;}
	case SB_RIGHT:{
		currentScrollHOffset = (int)totalRowWidth - (int)controlWidth;
		break;}
	case SB_PAGELEFT:{
		currentScrollHOffset -= (int)controlWidth;
		break;}
	case SB_LINELEFT:{
		currentScrollHOffset -= 8;
		break;}
	case SB_PAGERIGHT:{
		currentScrollHOffset += (int)controlWidth;
		break;}
	case SB_LINERIGHT:{
		currentScrollHOffset += 8;
		break;}
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_KEYDOWN(WPARAM wParam, LPARAM lParam)
{
	unsigned int virtualKeyCode = (unsigned int)wParam;

	switch(virtualKeyCode)
	{
	default:
		return 0;
	case VK_UP:
		if(autoScrollingManagement)
		{
			vscrollCurrent = ((vscrollCurrent >= 1) && ((vscrollCurrent - 1) >= vscrollMin))? vscrollCurrent - 1: vscrollMin;
		}
		else
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = 1;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSUP), (LPARAM)&info);
		}
		break;
	case VK_DOWN:
		if(autoScrollingManagement)
		{
			vscrollCurrent = ((vscrollCurrent + 1) <= vscrollMax)? vscrollCurrent + 1: vscrollMax;
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = 1;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSDOWN), (LPARAM)&info);
		}
		break;
	case VK_PRIOR:
		if(autoScrollingManagement)
		{
			vscrollCurrent = ((vscrollCurrent >= (int)visibleRows) && ((vscrollCurrent - (int)visibleRows) >= vscrollMin))? vscrollCurrent - (int)visibleRows: vscrollMin;
		}
		else
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = visibleRows;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSUP), (LPARAM)&info);
		}
		break;
	case VK_NEXT:
		if(autoScrollingManagement)
		{
			vscrollCurrent = ((vscrollCurrent + (int)visibleRows) <= vscrollMax)? vscrollCurrent + (int)visibleRows: vscrollMax;
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = visibleRows;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSDOWN), (LPARAM)&info);
		}
		break;
	case VK_HOME:
		if(autoScrollingManagement)
		{
			vscrollCurrent = vscrollMin;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = 0;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_NEWSCROLLPOSITION), (LPARAM)&info);
		}
		break;
	case VK_END:
		if(autoScrollingManagement)
		{
			vscrollCurrent = vscrollMax;
		}
		else
		{
			Grid_NewScrollPosition info;
			info.scrollPos = (vscrollMax > 0)? (unsigned int)vscrollMax - 1: 0;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_NEWSCROLLPOSITION), (LPARAM)&info);
		}
		break;
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings();

	//Force the entire control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	//Make the redraw operation happen immediately. We do this because it's been noted
	//that when doing the page up/down steps, it's possible for the operation to be slow
	//enough that the control is never redrawn between key repeat messages, causing the
	//interface to not actually update while the user is scrolling.
	UpdateWindow(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	//Convert the cursor position to row number
	int cursorPosX = LOWORD(lParam);
	int cursorPosY = HIWORD(lParam);
	int selectedRowNo = (cursorPosY - (headerPosY + headerHeight)) / fontHeight;

	//Notify the parent window of the row selection
	if(selectedRowNo >= 0)
	{
		Grid_RowSelected info;
		info.visibleRowNo = (unsigned int)selectedRowNo;
		info.keyPressedCtrl = (wParam & MK_CONTROL) != 0;
		info.keyPressedShift = (wParam & MK_SHIFT) != 0;
		SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_ROWSELECTED), (LPARAM)&info);
	}

	//##TODO## Send a column selection notification too

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgWM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam)
{
	//Calculate the number of lines to scroll in the window. We displace the view pos
	//by a minimum of 3 lines per division of the scroll wheel. We also invert the
	//sign, since we want a scroll down to increase the view position.
	int scrollUnits = ((short)HIWORD(wParam) / WHEEL_DELTA);
	scrollUnits *= -3;

	if(autoScrollingManagement)
	{
		vscrollCurrent += scrollUnits;
		vscrollCurrent = (vscrollCurrent < vscrollMin)? vscrollMin: vscrollCurrent;
		vscrollCurrent = (vscrollCurrent > vscrollMax)? vscrollMax: vscrollCurrent;
	}
	else
	{
		if(scrollUnits < 0)
		{
			Grid_ShiftRowsUp info;
			info.shiftCount = (unsigned int)(-scrollUnits);
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSUP), (LPARAM)&info);
		}
		else
		{
			Grid_ShiftRowsDown info;
			info.shiftCount = (unsigned int)scrollUnits;
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), GRID_SHIFTROWSDOWN), (LPARAM)&info);
		}
	}

	//Update the current scrollbar state
	UpdateScrollbarSettings();

	//Force the entire control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_SETMANUALSCROLLING(WPARAM wParam, LPARAM lParam)
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
	UpdateScrollbarSettings();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_SETDATAAREAFONT(WPARAM wParam, LPARAM lParam)
{
	//Set the new font for the data area of this grid
	hfont = (HFONT)wParam;

	//Read the font metrics for our new font
	HDC hdc = GetDC(hwnd);
	HFONT hfontOld = (HFONT)SelectObject(hdc, hfont);
	TEXTMETRIC textMetric;
	GetTextMetrics(hdc, &textMetric);
	fontWidth = textMetric.tmAveCharWidth;
	fontHeight = textMetric.tmHeight;
	SelectObject(hdc, hfontOld);
	ReleaseDC(hwnd, hdc);

	//Update the window size information based on the new font metrics
	UpdateWindowSize();

	//Force the entire control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_INSERTCOLUMN(WPARAM wParam, LPARAM lParam)
{
	const Grid_InsertColumn& info = *((Grid_InsertColumn*)lParam);

	//Insert the new column into our header array
	columnData.push_back(ColumnData(info.name, info.columnID, info.width));
	ColumnData* i = &(*columnData.rbegin());

	//Insert the new column into our ID index
	columnIDIndex.insert(ColumnIDIndexEntry(info.columnID, i));

	//Insert the new column into our header control
	HDITEM hdItem;
	hdItem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
	hdItem.fmt = HDF_STRING;
	hdItem.cchTextMax = (int)info.name.length();
	hdItem.pszText = &i->name[0];
	hdItem.cxy = i->width;
	int itemIndex = (int)SendMessage(hwndHeader, HDM_INSERTITEM, 99999, (LPARAM)&hdItem);
	if(itemIndex != -1)
	{
		i->index = itemIndex;
		columnIndex.insert(ColumnIndexEntry(itemIndex, i));
	}

	//Since the columns have changed, we need to rebuild our header order index
	RebuildHeaderOrder();

	//Update the current scrollbar state
	UpdateScrollbarSettings();

	//Force the entire control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_DELETECOLUMN(WPARAM wParam, LPARAM lParam)
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

			//Since the columns have changed, we need to rebuild our header order index
			RebuildHeaderOrder();

			//If automatic scroll management is enabled, recalculate the new scroll
			//settings based on the new column list.
			if(autoScrollingManagement)
			{
				RecalculateScrollPosition();
			}

			//Update the current scrollbar state
			UpdateScrollbarSettings();

			//Force the entire control to redraw
			InvalidateRect(hwnd, NULL, FALSE);

			return 0;
		}
	}
	return 1;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_GETCOLUMNINFO(WPARAM wParam, LPARAM lParam)
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
	info.width = i->width;
	info.order = i->order;
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_SETCOLUMNINFO(WPARAM wParam, LPARAM lParam)
{
	unsigned int targetColumnID = (unsigned int)wParam;
	ColumnIDIndexIterator columnIterator = columnIDIndex.find(targetColumnID);
	if(columnIterator != columnIDIndex.end())
	{
		ColumnData* i = columnIterator->second;

		const Grid_SetColumnInfo& info = *((Grid_SetColumnInfo*)lParam);
		i->columnID = info.columnID;
		i->name = info.name;
		i->width = info.width;

		//Insert the new column into our header control
		HDITEM hdItem;
		hdItem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
		hdItem.fmt = HDF_STRING;
		hdItem.cchTextMax = (int)i->name.length();
		hdItem.pszText = &i->name[0];
		hdItem.cxy = i->width;
		SendMessage(hwndHeader, HDM_SETITEM, (WPARAM)i->index, (LPARAM)&hdItem);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_UPDATECOLUMNTEXT(WPARAM wParam, LPARAM lParam)
{
	unsigned int targetColumnID = (unsigned int)wParam;
	const std::vector<std::wstring>& text = *((std::vector<std::wstring>*)lParam);
	ColumnIDIndexIterator columnIterator = columnIDIndex.find(targetColumnID);
	if(columnIterator == columnIDIndex.end())
	{
		return 1;
	}

	ColumnData* column = columnIterator->second;
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
		unsigned int newColumnDataBufferSize = (unsigned int)column->dataBuffer.size();
		RecalculateScrollPosition();
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_SETCONTROLCOLOR(WPARAM wParam, LPARAM lParam)
{
	const Grid_SetControlColor& controlColorInfo = *((Grid_SetControlColor*)lParam);
	userColorData.defined = controlColorInfo.defined;
	userColorData.colorBackground = controlColorInfo.colorBackground;
	userColorData.colorTextFront = controlColorInfo.colorTextFront;
	userColorData.colorTextBack = controlColorInfo.colorTextBack;
	userColorData.colorLine = controlColorInfo.colorLine;
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_SETROWCOLOR(WPARAM wParam, LPARAM lParam)
{
	unsigned int targetRowID = (unsigned int)wParam;
	const Grid_SetRowColor& rowColorInfo = *((Grid_SetRowColor*)lParam);
	if(targetRowID >= (unsigned int)rowColorDataArray.size())
	{
		return 1;
	}

	CustomColorData& rowColorData = rowColorDataArray[targetRowID];
	rowColorData.defined = rowColorInfo.defined;
	rowColorData.colorBackground = rowColorInfo.colorBackground;
	rowColorData.colorTextFront = rowColorInfo.colorTextFront;
	rowColorData.colorTextBack = rowColorInfo.colorTextBack;
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_GETROWCOUNT(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)visibleRows;
}

//----------------------------------------------------------------------------------------
LRESULT WC_GridList::msgGRID_SETVSCROLLINFO(WPARAM wParam, LPARAM lParam)
{
	//If automatic scrolling management is currently enabled, we reject this message, and
	//return 1.
	if(autoScrollingManagement)
	{
		return 1;
	}

	//Apply the new vscroll settings
	const Grid_SetVScrollInfo& info = *((Grid_SetVScrollInfo*)lParam);
	vscrollMin = info.minPos;
	vscrollMax = info.maxPos;
	vscrollCurrent = info.currentPos;
	vscrollValuesPerPage = info.valuesPerPage;

	//Update the current scrollbar state
	UpdateScrollbarSettings();

	//Force the entire control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}
