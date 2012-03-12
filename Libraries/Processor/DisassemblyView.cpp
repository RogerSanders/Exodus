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
	hfont = NULL;
	visibleRows = 0;
	track = true;
	forcePCSync = false;
	startLocation = 0;
	endLocation = 0x10000;
	currentLocation = 0;
	readAbove = 0x100;
	lastBufferedOpcodeSize = 0;
	firstVisibleOpcodeSize = 0;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Disassembly");
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

	//Create the default font for this window
	int fontPointSize = 8;
	HDC hdc = GetDC(hwnd);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);
	std::wstring fontTypefaceName = L"MS Shell Dlg";
	hfont = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &fontTypefaceName[0]);

	//Set the default font for the child controls
	SendMessage(hwndGridList, WM_SETFONT, (WPARAM)hfont, (LPARAM)TRUE);

	//Create a timer to trigger updates to the disassembly window
	SetTimer(hwnd, 1, 200, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Delete the default font object
	SendMessage(hwnd, WM_SETFONT, (WPARAM)NULL, (LPARAM)FALSE);
	DeleteObject(hfont);

	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT Processor::DisassemblyView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Check if we need to update the current address
	if(track || forcePCSync)
	{
		forcePCSync = false;

		//Check if the current address has changed
		unsigned int newPC = device->GetCurrentPC();
		if(currentLocation != newPC)
		{
			//If the address has changed, store the new address
			currentLocation = newPC;

			//Update the vertical scroll settings
			WC_GridList::Grid_SetVScrollInfo scrollInfo;
			scrollInfo.minPos = (int)startLocation;
			scrollInfo.maxPos = (int)endLocation;
			scrollInfo.currentPos = (int)currentLocation;
			scrollInfo.valuesPerPage = (int)visibleRows;
			SendMessage(hwndGridList, WC_GridList::GRID_SETVSCROLLINFO, 0, (LPARAM)&scrollInfo);

			//If the current location is outside the previous start and end location
			//boundaries, extend the boundaries to include the current address.
			if(currentLocation > endLocation)
			{
				endLocation = currentLocation;
			}
			if(currentLocation < startLocation)
			{
				startLocation = currentLocation;
			}
		}
	}

	//Update the control panel
	SendMessage(hwndControlPanel, WM_TIMER, wparam, lparam);

	//Update the disassembly
	UpdateDisassembly();

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
			unsigned int minimumOpcodeByteSize = device->GetMinimumOpcodeByteSize();
			unsigned int shiftCount = 0;
			while(shiftCount < info->shiftCount)
			{
				unsigned int initialPosition = currentLocation;
				while(((currentLocation + firstVisibleOpcodeSize) > initialPosition) //The first opcode in the window is below or contains the address of the previous first opcode in the window
					&& (currentLocation >= lastBufferedOpcodeSize) //We're not going to shift above 0
					&& ((currentLocation - lastBufferedOpcodeSize) >= startLocation)) //We're not going to shift past the start location
				{
					currentLocation -= lastBufferedOpcodeSize;
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
				&& ((currentLocation + firstVisibleOpcodeSize) < endLocation))
			{
				currentLocation += firstVisibleOpcodeSize;
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
			currentLocation = info->scrollPos - (info->scrollPos % device->GetMinimumOpcodeByteSize());
			UpdateDisassembly();
		}

		//Update the vertical scroll settings
		WC_GridList::Grid_SetVScrollInfo scrollInfo;
		scrollInfo.minPos = (int)startLocation;
		scrollInfo.maxPos = (int)endLocation;
		scrollInfo.currentPos = (int)currentLocation;
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
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT, device->GetAddressBusCharWidth(), currentLocation);
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
	if(currentControlFocus != IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT)	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT, device->GetAddressBusCharWidth(), currentLocation);
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
	else if(HIWORD(wparam) == EN_KILLFOCUS)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			switch(LOWORD(wparam))
			{
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_START:
				startLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetAddressBusCharWidth(), startLocation);
				break;
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_END:
				endLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetAddressBusCharWidth(), endLocation);
				break;
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT:
				currentLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetAddressBusCharWidth(), currentLocation);
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
	unsigned int minimumOpcodeByteSize = device->GetMinimumOpcodeByteSize();

	//Obtain the address of the first opcode in the buffer
	unsigned int upperReadPosition = currentLocation - readAbove;
	if((readAbove > currentLocation) || (upperReadPosition < startLocation))
	{
		upperReadPosition = startLocation + (currentLocation % minimumOpcodeByteSize);
	}

	//Read each opcode stored in the buffer
	unsigned int offset = 0;
	while((upperReadPosition + offset) < currentLocation)
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
		//Read the opcode info
		unsigned int opcodeSize = minimumOpcodeByteSize;
		Processor::OpcodeInfo opcodeInfo = device->GetOpcodeInfo(upperReadPosition + offset);
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
		IntToStringBase16(upperReadPosition + offset, addressString, device->GetAddressBusCharWidth(), false);
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
			Data rawData(device->GetRawData(upperReadPosition + offset + byteNo));

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
