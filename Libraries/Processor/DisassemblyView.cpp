#include "DisassemblyView.h"
#include "resource.h"
#include "WindowFunctions/WC_GridList.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::DisassemblyView::DisassemblyView(Processor* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	hwndGridList = NULL;
	hwndControlPanel = NULL;
	hfontHeader = NULL;
	hfontData = NULL;
	visibleRows = 0;
	track = true;
	forcePCSync = false;
	startLocation = 0;
	endLocation = 0x10000;
	firstVisibleValueLocation = 0;
	lastVisibleValueLocation = 0;
	readAbove = 0x100;
	lastBufferedOpcodeSize = 0;
	firstVisibleOpcodeSize = 0;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Disassembly");
	SetWindowSettings(windowTitle, WS_SIZEBOX, 0, 440, 500, true);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_PARENTNOTIFY:
		return msgWM_PARENTNOTIFY(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_PAINT:
		msgWM_PAINT(hwnd, wparam, lparam);
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the GridList child control
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WC_GridList::WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = (HINSTANCE)device->GetAssemblyHandle();
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"EX_GridList";
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);
	hwndGridList = CreateWindowEx(WS_EX_CLIENTEDGE, L"EX_GridList", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, (HMENU)CTL_GRIDLIST, (HINSTANCE)device->GetAssemblyHandle(), NULL);

	//Enable manual scrolling for the grid control, so that we only have to generate
	//content for the rows that are currently in view.
	SendMessage(hwndGridList, WC_GridList::GRID_SETMANUALSCROLLING, 1, 0);

	//Insert our columns into the GridList control
	SendMessage(hwndGridList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Address", COLUMN_ADDRESS, 70));
	SendMessage(hwndGridList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Opcode", COLUMN_OPCODE, 70));
	SendMessage(hwndGridList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Args", COLUMN_ARGS, 140));
	SendMessage(hwndGridList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Comment", COLUMN_COMMENT, 80));
	SendMessage(hwndGridList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Machine Code", COLUMN_BINARY, 300));

	//Create the dialog control panel
	hwndControlPanel = CreateDialogParam((HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_DISASSEMBLY_PANEL), hwnd, WndProcPanelStatic, (LPARAM)this);
	ShowWindow(hwndControlPanel, SW_SHOWNORMAL);
	UpdateWindow(hwndControlPanel);

	//Obtain the correct metrics for our custom font object
	int fontPointSize = 8;
	HDC hdc = GetDC(hwnd);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);

	//Create the font for the header in the grid control
	std::wstring headerFontTypefaceName = L"MS Shell Dlg";
	hfontHeader = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &headerFontTypefaceName[0]);

	//Set the header font for the grid control
	SendMessage(hwndGridList, WM_SETFONT, (WPARAM)hfontHeader, (LPARAM)TRUE);

	//Create the font for the data region in the grid control
	std::wstring dataFontTypefaceName = L"Courier New";
	hfontData = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &dataFontTypefaceName[0]);

	//Set the data region font for the grid control
	SendMessage(hwndGridList, WC_GridList::GRID_SETDATAAREAFONT, (WPARAM)hfontData, (LPARAM)TRUE);

	//Create a timer to trigger updates to the disassembly window
	SetTimer(hwnd, 1, 200, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Delete our custom font objects
	SendMessage(hwndGridList, WM_SETFONT, (WPARAM)NULL, (LPARAM)FALSE);
	SendMessage(hwndGridList, WC_GridList::GRID_SETDATAAREAFONT, (WPARAM)NULL, (LPARAM)FALSE);
	DeleteObject(hfontHeader);
	DeleteObject(hfontData);

	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Obtain the current PC address, and check if it has changed since it was last
	//inspected.
	unsigned int newPC = device->GetCurrentPC() & device->GetAddressBusMask();
	bool pcCounterChanged = (currentPCLocation != newPC);
	currentPCLocation = newPC;

	//Determine if we need to scroll the window to make the PC location visible
	if(((pcCounterChanged && track) || forcePCSync)
	&& ((currentPCLocation < firstVisibleValueLocation) || (currentPCLocation > lastVisibleValueLocation)))
	{
		//Set the first visible location on the disassembly window to be the same as the
		//current PC value
		firstVisibleValueLocation = currentPCLocation;

		//Obtain the address of the first opcode in the buffer
		unsigned int minimumOpcodeByteSize = device->GetMinimumOpcodeByteSize();
		unsigned int upperReadPosition = (readAbove > firstVisibleValueLocation)? 0: firstVisibleValueLocation - readAbove;
		if(upperReadPosition < startLocation)
		{
			upperReadPosition = startLocation + (firstVisibleValueLocation % minimumOpcodeByteSize);
		}

		//Record the starting addresses of each opcode which precedes the target location
		//within the buffer region
		std::list<unsigned int> leadingRowsPC;
		unsigned int offset = 0;
		while((upperReadPosition + offset) < firstVisibleValueLocation)
		{
			//Record the address of this opcode
			leadingRowsPC.push_back(upperReadPosition + offset);

			//Read the opcode info
			Processor::OpcodeInfo opcodeInfo = device->GetOpcodeInfo(upperReadPosition + offset);
			unsigned int opcodeSize = minimumOpcodeByteSize;
			if(opcodeInfo.valid)
			{
				opcodeSize = opcodeInfo.opcodeSize;
			}

			//Step to the next opcode
			offset += opcodeSize;
		}

		//Adjust the first visible value on the disassembly window to take into account
		//the requested number of leading rows.
		unsigned int leadingRowCount = visibleRows / 3;
		unsigned int currentLeadingRowNo = 0;
		std::list<unsigned int>::const_reverse_iterator leadingRowsPCIterator = leadingRowsPC.rbegin();
		while((leadingRowsPCIterator != leadingRowsPC.rend()) && (currentLeadingRowNo < leadingRowCount))
		{
			firstVisibleValueLocation = *leadingRowsPCIterator;
			++currentLeadingRowNo;
			++leadingRowsPCIterator;
		}

		//Update the vertical scroll settings
		WC_GridList::Grid_SetVScrollInfo scrollInfo;
		scrollInfo.minPos = (int)startLocation;
		scrollInfo.maxPos = (int)endLocation;
		scrollInfo.currentPos = (int)firstVisibleValueLocation;
		scrollInfo.valuesPerPage = (int)visibleRows;
		SendMessage(hwndGridList, WC_GridList::GRID_SETVSCROLLINFO, 0, (LPARAM)&scrollInfo);

		//If the current location is outside the previous start and end location
		//boundaries, extend the boundaries to include the current address.
		unsigned int extensionSize = (visibleRows * device->GetMinimumOpcodeByteSize() * 2);
		if(currentPCLocation > endLocation)
		{
			endLocation = currentPCLocation + extensionSize;
			if(endLocation > device->GetAddressBusMask())
			{
				endLocation = device->GetAddressBusMask();
			}
		}
		if(currentPCLocation < startLocation)
		{
			if(currentPCLocation <= extensionSize)
			{
				startLocation = 0;
			}
			else
			{
				startLocation = currentPCLocation - extensionSize;
			}
		}
	}

	//Clear the forcePCSync flag now that it has been processed
	forcePCSync = false;

	//Update the control panel
	SendMessage(hwndControlPanel, WM_TIMER, wparam, lparam);

	//Update the disassembly
	UpdateDisassembly();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::msgWM_PARENTNOTIFY(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case WM_LBUTTONDOWN:{
		//If the user has clicked on a child window within our window region, ensure that
		//the child window gets focus.
		POINT mousePos;
		mousePos.x = LOWORD(lParam);
		mousePos.y = HIWORD(lParam);
		HWND targetWindow = ChildWindowFromPoint(hwnd, mousePos);
		if(targetWindow != NULL)
		{
			SetFocus(targetWindow);
		}
		break;}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(LOWORD(wparam) == CTL_GRIDLIST)
	{
		if(HIWORD(wparam) == WC_GridList::GRID_NEWROWCOUNT)
		{
			WC_GridList::Grid_NewRowCount* newRowCountInfo = (WC_GridList::Grid_NewRowCount*)lparam;
			visibleRows = newRowCountInfo->visibleRows;
			UpdateDisassembly();
		}
		else if(HIWORD(wparam) == WC_GridList::GRID_SHIFTROWSUP)
		{
			WC_GridList::Grid_ShiftRowsUp* info = (WC_GridList::Grid_ShiftRowsUp*)lparam;
			unsigned int shiftCount = 0;
			while(shiftCount < info->shiftCount)
			{
				unsigned int initialPosition = firstVisibleValueLocation;
				while(((firstVisibleValueLocation + firstVisibleOpcodeSize) > initialPosition) //The first opcode in the window is below or contains the address of the previous first opcode in the window
					&& (firstVisibleValueLocation >= lastBufferedOpcodeSize) //We're not going to shift above 0
					&& ((firstVisibleValueLocation - lastBufferedOpcodeSize) >= startLocation)) //We're not going to shift past the start location
				{
					firstVisibleValueLocation -= lastBufferedOpcodeSize;
					UpdateDisassembly();
				}
				++shiftCount;
			}
		}
		else if(HIWORD(wparam) == WC_GridList::GRID_SHIFTROWSDOWN)
		{
			WC_GridList::Grid_ShiftRowsDown* info = (WC_GridList::Grid_ShiftRowsDown*)lparam;
			unsigned int shiftCount = 0;
			while((shiftCount < info->shiftCount)
				&& ((firstVisibleValueLocation + firstVisibleOpcodeSize) < endLocation))
			{
				firstVisibleValueLocation += firstVisibleOpcodeSize;
				UpdateDisassembly();
				++shiftCount;
			}
		}
		else if(HIWORD(wparam) == WC_GridList::GRID_NEWSCROLLPOSITION)
		{
			//Move the window to an absolute address as a result of a command from
			//the list control. We align the absolute address to the minimum opcode
			//size first, to ensure alignment rules for the processor are respected
			//when doing a blind scroll.
			WC_GridList::Grid_NewScrollPosition* info = (WC_GridList::Grid_NewScrollPosition*)lparam;
			firstVisibleValueLocation = info->scrollPos - (info->scrollPos % device->GetMinimumOpcodeByteSize());
			UpdateDisassembly();
		}
		else if(HIWORD(wparam) == WC_GridList::GRID_ROWSELECTED)
		{
			WC_GridList::Grid_RowSelected* info = (WC_GridList::Grid_RowSelected*)lparam;

			//If the CTRL key was pressed when the row selection was made, toggle a
			//breakpoint at the opcode being displayed on the target row. If the shift key
			//was also pressed, just toggle the enable/disable state of the breakpoint.
			if(info->keyPressedCtrl)
			{
				ToggleBreakpointStateAtRow(info->visibleRowNo, info->keyPressedShift);
			}
		}

		//Update the vertical scroll settings
		WC_GridList::Grid_SetVScrollInfo scrollInfo;
		scrollInfo.minPos = (int)startLocation;
		scrollInfo.maxPos = (int)endLocation;
		scrollInfo.currentPos = (int)firstVisibleValueLocation;
		scrollInfo.valuesPerPage = (int)visibleRows;
		SendMessage(hwndGridList, WC_GridList::GRID_SETVSCROLLINFO, 0, (LPARAM)&scrollInfo);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Read the new client size of the window
	RECT rect;
	GetClientRect(hwnd, &rect);
	int controlWidth = rect.right;
	int controlHeight = rect.bottom;
	GetClientRect(hwndControlPanel, &rect);
	int controlPanelWidth = rect.right;
	int controlPanelHeight = rect.bottom;

	//Global parameters defining how child windows are positioned
	int borderSize = 4;

	//Calculate the new position of the control panel
	int controlPanelPosX = borderSize;
	int controlPanelPosY = controlHeight - (borderSize + controlPanelHeight);
	MoveWindow(hwndControlPanel, controlPanelPosX, controlPanelPosY, controlPanelWidth, controlPanelHeight, TRUE);

	//Calculate the new size and position of the list
	int listBoxWidth = controlWidth - (borderSize * 2);
	int listBoxPosX = borderSize;
	int listBoxHeight = controlHeight - ((borderSize * 2) + controlPanelHeight);
	int listBoxPosY = borderSize;
	MoveWindow(hwndGridList, listBoxPosX, listBoxPosY, listBoxWidth, listBoxHeight, TRUE);

	//Update the disassembly
	UpdateDisassembly();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::msgWM_PAINT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Fill the background of the control with the dialog background colour
	HDC hdc = GetDC(hwnd);
	HBRUSH hbrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	HBRUSH hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);

	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, hbrush);

	SelectObject(hdc, hbrushOld);
	DeleteObject(hbrush);
	ReleaseDC(hwnd, hdc);

	return 0;
}

//----------------------------------------------------------------------------------------
//Panel dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK Processor::DisassemblyView::WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	DisassemblyView* state = (DisassemblyView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (DisassemblyView*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcPanel(hwnd, msg, wparam, lparam);
		}
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			state->WndProcPanel(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
		}
		return TRUE;
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = TRUE;
	if(state != 0)
	{
		result = state->WndProcPanel(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::DisassemblyView::WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgPanelWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgPanelWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgPanelWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgPanelWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Panel dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR Processor::DisassemblyView::msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	//Set the initial state for the controls
	CheckDlgButton(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_TRACK, (track)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT, device->GetAddressBusCharWidth(), firstVisibleValueLocation);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_START, device->GetAddressBusCharWidth(), startLocation);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_END, device->GetAddressBusCharWidth(), endLocation);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_READABOVE, device->GetAddressBusCharWidth(), readAbove);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::DisassemblyView::msgPanelWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::DisassemblyView::msgPanelWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(currentControlFocus != IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT)	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT, device->GetAddressBusCharWidth(), firstVisibleValueLocation);
	if(currentControlFocus != IDC_PROCESSOR_DISASSEMBLY_PANEL_START)	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_START, device->GetAddressBusCharWidth(), startLocation);
	if(currentControlFocus != IDC_PROCESSOR_DISASSEMBLY_PANEL_END)	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_END, device->GetAddressBusCharWidth(), endLocation);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::DisassemblyView::msgPanelWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_START:
				startLocation = GetDlgItemHex(hwnd, LOWORD(wparam)) & device->GetAddressBusMask();
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetAddressBusCharWidth(), startLocation);
				break;
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_END:
				endLocation = GetDlgItemHex(hwnd, LOWORD(wparam)) & device->GetAddressBusMask();
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetAddressBusCharWidth(), endLocation);
				break;
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT:
				firstVisibleValueLocation = GetDlgItemHex(hwnd, LOWORD(wparam)) & device->GetAddressBusMask();
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetAddressBusCharWidth(), firstVisibleValueLocation);
				break;
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_READABOVE:
				readAbove = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetAddressBusCharWidth(), readAbove);
				break;
			}

			//Update the disassembly
			UpdateDisassembly();
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_DISASSEMBLY_PANEL_TRACK:
			track = !track;
			break;
		case IDC_PROCESSOR_DISASSEMBLY_PANEL_JUMPTOCURRENT:
			forcePCSync = true;
			break;
		}

		//Update the disassembly
		UpdateDisassembly();
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
void Processor::DisassemblyView::UpdateDisassembly()
{
	//Obtain the address of the first opcode in the buffer
	unsigned int minimumOpcodeByteSize = device->GetMinimumOpcodeByteSize();
	unsigned int upperReadPosition = (readAbove > firstVisibleValueLocation)? 0: firstVisibleValueLocation - readAbove;
	if(upperReadPosition < startLocation)
	{
		upperReadPosition = startLocation + (firstVisibleValueLocation % minimumOpcodeByteSize);
	}

	//Skip all opcodes in the buffer which occur before the visible region of the buffer
	unsigned int offset = 0;
	while((upperReadPosition + offset) < firstVisibleValueLocation)
	{
		//Read the opcode info
		Processor::OpcodeInfo opcodeInfo = device->GetOpcodeInfo(upperReadPosition + offset);
		unsigned int opcodeSize = minimumOpcodeByteSize;
		if(opcodeInfo.valid)
		{
			opcodeSize = opcodeInfo.opcodeSize;
		}

		//Update the last buffered opcode size. This will be correct when the loop
		//terminates.
		lastBufferedOpcodeSize = opcodeSize;

		//Step to the next opcode
		offset += opcodeSize;
	}

	//If there are no buffered opcodes, set the size of the last buffered opcode to the
	//minimum opcode size.
	if(offset == 0)
	{
		lastBufferedOpcodeSize = minimumOpcodeByteSize;
	}

	//Read each visible opcode to be shown in the disassembly window
	std::vector<std::wstring> columnDataAddress(visibleRows);
	std::vector<std::wstring> columnDataOpcode(visibleRows);
	std::vector<std::wstring> columnDataArgs(visibleRows);
	std::vector<std::wstring> columnDataComment(visibleRows);
	std::vector<std::wstring> columnDataBinary(visibleRows);
	for(unsigned int i = 0; (i < visibleRows) && ((upperReadPosition + offset) < endLocation); ++i)
	{
		unsigned int rowPCLocation = (upperReadPosition + offset);

		//Set the color for this row
		WC_GridList::Grid_SetRowColor setRowColor;
		setRowColor.defined = false;
		if(rowPCLocation == currentPCLocation)
		{
			//If this row is the same as the current PC location, shade the location in
			//green.
			setRowColor.defined = true;
			setRowColor.colorBackground = WinColor(128, 255, 128);
			setRowColor.colorTextFront = WinColor(0, 0, 0);
			setRowColor.colorTextBack = setRowColor.colorBackground;
		}
		else
		{
			//Look for any breakpoints which trigger at the target address
			boost::mutex::scoped_lock lock(device->debugMutex);
			bool breakpointAtLocation = false;
			bool breakpointEnabled = false;
			BreakpointList::iterator breakpointIterator = device->breakpoints.begin();
			while(!breakpointAtLocation && (breakpointIterator != device->breakpoints.end()))
			{
				breakpointAtLocation = (*breakpointIterator)->PassesLocationCondition(rowPCLocation);
				breakpointEnabled = (*breakpointIterator)->GetEnabled();
				++breakpointIterator;
			}

			//If a breakpoint triggers at this address, shade the location in red.
			if(breakpointAtLocation)
			{
				setRowColor.defined = true;
				setRowColor.colorBackground = breakpointEnabled? WinColor(255, 128, 128): WinColor(192, 0, 0);
				setRowColor.colorTextFront = WinColor(0, 0, 0);
				setRowColor.colorTextBack = setRowColor.colorBackground;
			}
		}
		SendMessage(hwndGridList, WC_GridList::GRID_SETROWCOLOR, i, (LPARAM)&setRowColor);

		//Read the opcode info
		unsigned int opcodeSize = minimumOpcodeByteSize;
		Processor::OpcodeInfo opcodeInfo = device->GetOpcodeInfo(rowPCLocation);
		if(!opcodeInfo.valid)
		{
			//If the target address doesn't reference a valid opcode, we output a generic
			//message.
			opcodeInfo.disassemblyOpcode = L"INVALID";
		}
		else
		{
			opcodeSize = opcodeInfo.opcodeSize;
		}

		//Build the opcode disassembly output
		std::wstring addressString;
		IntToStringBase16(rowPCLocation, addressString, device->GetAddressBusCharWidth(), false);
		columnDataAddress[i] = addressString;
		columnDataOpcode[i] = opcodeInfo.disassemblyOpcode;
		columnDataArgs[i] = opcodeInfo.disassemblyArguments;
		if(!opcodeInfo.disassemblyComment.empty())
		{
			columnDataComment[i] = L";" + opcodeInfo.disassemblyComment;
		}

		//Build the machine code disassembly output
		std::wstring binaryString;
		for(unsigned int byteNo = 0; byteNo < opcodeInfo.opcodeSize;)
		{
			Data rawData(device->GetRawData(rowPCLocation + byteNo));

			if(byteNo > 0)
			{
				binaryString += L' ';
			}
			std::wstring dataString;
			IntToStringBase16(rawData.GetData(), dataString, rawData.GetByteSize() * 2, false);
			binaryString += dataString;

			byteNo += rawData.GetByteSize();
		}
		columnDataBinary[i] = binaryString;

		//If this is the first visible opcode, record the size
		if(i == 0)
		{
			firstVisibleOpcodeSize = opcodeSize;
		}

		//Update the last visible value location
		lastVisibleValueLocation = rowPCLocation;

		//Step to the next opcode
		offset += opcodeSize;
	}

	//Write the disassembly data to the window
	SendMessage(hwndGridList, WC_GridList::GRID_UPDATECOLUMNTEXT, COLUMN_ADDRESS, (LPARAM)&columnDataAddress);
	SendMessage(hwndGridList, WC_GridList::GRID_UPDATECOLUMNTEXT, COLUMN_OPCODE, (LPARAM)&columnDataOpcode);
	SendMessage(hwndGridList, WC_GridList::GRID_UPDATECOLUMNTEXT, COLUMN_ARGS, (LPARAM)&columnDataArgs);
	SendMessage(hwndGridList, WC_GridList::GRID_UPDATECOLUMNTEXT, COLUMN_COMMENT, (LPARAM)&columnDataComment);
	SendMessage(hwndGridList, WC_GridList::GRID_UPDATECOLUMNTEXT, COLUMN_BINARY, (LPARAM)&columnDataBinary);

	//Force the grid control to redraw now that we've updated the text
	InvalidateRect(hwndGridList, NULL, FALSE);
}

//----------------------------------------------------------------------------------------
void Processor::DisassemblyView::ToggleBreakpointStateAtRow(unsigned int visibleRowNo, bool toggleEnableState)
{
	//Validate the specified row number
	if(visibleRowNo >= visibleRows)
	{
		return;
	}

	//Obtain the address of the first opcode in the buffer
	unsigned int minimumOpcodeByteSize = device->GetMinimumOpcodeByteSize();
	unsigned int upperReadPosition = (readAbove > firstVisibleValueLocation)? 0: firstVisibleValueLocation - readAbove;
	if(upperReadPosition < startLocation)
	{
		upperReadPosition = startLocation + (firstVisibleValueLocation % minimumOpcodeByteSize);
	}

	//Skip all opcodes in the buffer which occur before the visible region of the buffer
	unsigned int offset = 0;
	while((upperReadPosition + offset) < firstVisibleValueLocation)
	{
		//Read the opcode info
		Processor::OpcodeInfo opcodeInfo = device->GetOpcodeInfo(upperReadPosition + offset);
		unsigned int opcodeSize = minimumOpcodeByteSize;
		if(opcodeInfo.valid)
		{
			opcodeSize = opcodeInfo.opcodeSize;
		}

		//Step to the next opcode
		offset += opcodeSize;
	}

	//Locate the opcode which was selected in the disassembly window
	unsigned int currentVisibleRowNo = 0;
	while((currentVisibleRowNo < visibleRowNo) && ((upperReadPosition + offset) < endLocation))
	{
		//Read the opcode info
		Processor::OpcodeInfo opcodeInfo = device->GetOpcodeInfo(upperReadPosition + offset);
		unsigned int opcodeSize = minimumOpcodeByteSize;
		if(opcodeInfo.valid)
		{
			opcodeSize = opcodeInfo.opcodeSize;
		}

		//Step to the next opcode
		offset += opcodeSize;
		++currentVisibleRowNo;
	}
	if(currentVisibleRowNo != visibleRowNo)
	{
		return;
	}

	//Calculate the address of the selected opcode
	unsigned int targetOpcodeLocation = upperReadPosition + offset;

	//Toggle a breakpoint at the target location
	ToggleBreakpointStateAtAddress(targetOpcodeLocation, toggleEnableState);
}

//----------------------------------------------------------------------------------------
void Processor::DisassemblyView::ToggleBreakpointStateAtAddress(unsigned int pcLocation, bool toggleEnableState)
{
	//Look for any breakpoints which trigger at the target address
	boost::mutex::scoped_lock lock(device->debugMutex);
	bool breakpointAtLocation = false;
	BreakpointList::iterator breakpointIterator = device->breakpoints.begin();
	while(!breakpointAtLocation && (breakpointIterator != device->breakpoints.end()))
	{
		breakpointAtLocation = (*breakpointIterator)->PassesLocationCondition(pcLocation);
		if(breakpointAtLocation)
		{
			continue;
		}
		++breakpointIterator;
	}

	//Perform the requested operation
	if(toggleEnableState)
	{
		//Toggle the enable state for a breakpoint at the target address
		if(breakpointAtLocation)
		{
			(*breakpointIterator)->SetEnabled(!(*breakpointIterator)->GetEnabled());
		}
	}
	else
	{
		//Either add or remove a breakpoint for the target address
		if(breakpointAtLocation)
		{
			device->breakpoints.erase(breakpointIterator);
			device->breakpointExists = !device->breakpoints.empty();
		}
		else
		{
			Breakpoint* breakpoint = new Breakpoint(device->GetAddressBusWidth());
			breakpoint->SetLocationConditionData1(pcLocation);
			breakpoint->SetName(breakpoint->GenerateName(device->GetAddressBusCharWidth()));
			device->breakpoints.push_back(breakpoint);
			device->breakpointExists = !device->breakpoints.empty();
		}
	}
}
