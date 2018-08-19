#include "DisassemblyView.h"
#include "resource.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "WindowsControls/WindowsControls.pkg"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DisassemblyView::DisassemblyView(IUIManager& uiManager, DisassemblyViewPresenter& presenter, IProcessor& model)
:ViewBase(uiManager, presenter), _presenter(presenter), _model(model), _initializedDialog(false), _currentControlFocus(0)
{
	_hwndDataGrid = NULL;
	_hwndControlPanel = NULL;
	_hfontHeader = NULL;
	_hfontData = NULL;
	_visibleRows = 0;
	_track = true;
	_forcePCSync = false;
	_startLocation = 0;
	_endLocation = 0x10000;
	_firstVisibleValueLocation = 0;
	_lastVisibleValueLocation = 0;
	_readAbove = 0x100;
	_lastBufferedOpcodeSize = 0;
	_firstVisibleOpcodeSize = 0;
	SetWindowSettings(presenter.GetUnqualifiedViewTitle(), 0, 0, 440, 500);
	SetDockableViewType(true, DockPos::Right);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT DisassemblyView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementGiveFocusToChildWindowOnClick(hwnd, msg, wparam, lparam);
	switch (msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_BOUNCE:
		return msgWM_BOUNCE(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_PAINT:
		return msgWM_PAINT(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT DisassemblyView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Register the DataGrid window class
	WC_DataGrid::RegisterWindowClass(GetAssemblyHandle());

	//Create the DataGrid child control
	_hwndDataGrid = CreateWindowEx(WS_EX_CLIENTEDGE, WC_DataGrid::WindowClassName, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, (HMENU)CTL_DATAGRID, GetAssemblyHandle(), NULL);

	//Enable message bounce-back on our grid control so we can add hotkey support
	SetWindowSubclass(_hwndDataGrid, BounceBackSubclassProc, 0, 0);

	//Enable manual scrolling for the grid control, so that we only have to generate
	//content for the rows that are currently in view.
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::SetManualScrolling, 1, 0);

	//Insert our columns into the DataGrid control
	WC_DataGrid::Grid_InsertColumn addressColumn(L"Address", COLUMN_ADDRESS);
	WC_DataGrid::Grid_InsertColumn opcodeColumn(L"Opcode", COLUMN_OPCODE);
	opcodeColumn.sizeMode = WC_DataGrid::ColumnSizeMode::Proportional;
	opcodeColumn.proportionalWidth = 0.4f;
	WC_DataGrid::Grid_InsertColumn argsColumn(L"Args", COLUMN_ARGS);
	argsColumn.sizeMode = WC_DataGrid::ColumnSizeMode::Proportional;
	argsColumn.proportionalWidth = 1.0f;
	WC_DataGrid::Grid_InsertColumn commentColumn(L"Comment", COLUMN_COMMENT);
	WC_DataGrid::Grid_InsertColumn machineCodeColumn(L"Machine Code", COLUMN_BINARY);
	machineCodeColumn.sizeMode = WC_DataGrid::ColumnSizeMode::Absolute;
	machineCodeColumn.absoluteWidth = 100;
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&addressColumn);
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&opcodeColumn);
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&argsColumn);
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&commentColumn);
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&machineCodeColumn);

	//Create the dialog control panel
	_hwndControlPanel = CreateDialogParam(GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_DISASSEMBLY_PANEL), hwnd, WndProcPanelStatic, (LPARAM)this);
	ShowWindow(_hwndControlPanel, SW_SHOWNORMAL);
	UpdateWindow(_hwndControlPanel);

	//Obtain the correct metrics for our custom font object
	int fontPointSize = 8;
	HDC hdc = GetDC(hwnd);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);

	//Create the font for the header in the grid control
	std::wstring headerFontTypefaceName = L"MS Shell Dlg";
	_hfontHeader = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &headerFontTypefaceName[0]);

	//Set the header font for the grid control
	SendMessage(_hwndDataGrid, WM_SETFONT, (WPARAM)_hfontHeader, (LPARAM)TRUE);

	//Create the font for the data region in the grid control
	std::wstring dataFontTypefaceName = L"Courier New";
	_hfontData = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &dataFontTypefaceName[0]);

	//Set the data region font for the grid control
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)_hfontData, (LPARAM)TRUE);

	//Create a timer to trigger updates to the disassembly window
	SetTimer(hwnd, 1, 200, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DisassemblyView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Delete our custom font objects
	SendMessage(_hwndDataGrid, WM_SETFONT, (WPARAM)NULL, (LPARAM)FALSE);
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)NULL, (LPARAM)FALSE);
	DeleteObject(_hfontHeader);
	DeleteObject(_hfontData);

	KillTimer(hwnd, 1);

	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT DisassemblyView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Obtain the current PC address, and check if it has changed since it was last
	//inspected.
	unsigned int newPC = _model.GetCurrentPC() & _model.GetAddressBusMask();
	bool pcCounterChanged = (_currentPCLocation != newPC);
	_currentPCLocation = newPC;

	//Determine if we need to scroll the window to make the PC location visible
	if (((pcCounterChanged && _track) || _forcePCSync)
	&& ((_currentPCLocation < _firstVisibleValueLocation) || (_currentPCLocation > _lastVisibleValueLocation)))
	{
		//Set the first visible location on the disassembly window to be the same as the
		//current PC value
		_firstVisibleValueLocation = _currentPCLocation;

		//Obtain the address of the first opcode in the buffer
		unsigned int minimumOpcodeByteSize = _model.GetMinimumOpcodeByteSize();
		unsigned int upperReadPosition = (_readAbove > _firstVisibleValueLocation)? 0: _firstVisibleValueLocation - _readAbove;
		if (upperReadPosition < _startLocation)
		{
			upperReadPosition = _startLocation + (_firstVisibleValueLocation % minimumOpcodeByteSize);
		}

		//Record the starting addresses of each opcode which precedes the target location
		//within the buffer region
		std::list<unsigned int> leadingRowsPC;
		unsigned int offset = 0;
		while ((upperReadPosition + offset) < _firstVisibleValueLocation)
		{
			//Record the address of this opcode
			leadingRowsPC.push_back(upperReadPosition + offset);

			//Read the opcode info
			OpcodeInfo opcodeInfo;
			unsigned int opcodeSize = minimumOpcodeByteSize;
			if (_model.GetOpcodeInfo(upperReadPosition + offset, opcodeInfo) && opcodeInfo.GetIsValidOpcode())
			{
				opcodeSize = opcodeInfo.GetOpcodeSize();
			}

			//Step to the next opcode
			offset += opcodeSize;
		}

		//Adjust the first visible value on the disassembly window to take into account
		//the requested number of leading rows.
		unsigned int leadingRowCount = _visibleRows / 3;
		unsigned int currentLeadingRowNo = 0;
		std::list<unsigned int>::const_reverse_iterator leadingRowsPCIterator = leadingRowsPC.rbegin();
		while ((leadingRowsPCIterator != leadingRowsPC.rend()) && (currentLeadingRowNo < leadingRowCount))
		{
			_firstVisibleValueLocation = *leadingRowsPCIterator;
			++currentLeadingRowNo;
			++leadingRowsPCIterator;
		}

		//Update the vertical scroll settings
		WC_DataGrid::Grid_SetVScrollInfo scrollInfo;
		scrollInfo.minPos = (int)_startLocation;
		scrollInfo.maxPos = (int)_endLocation;
		scrollInfo.currentPos = (int)_firstVisibleValueLocation;
		scrollInfo.valuesPerPage = (int)_visibleRows;
		SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::SetVScrollInfo, 0, (LPARAM)&scrollInfo);

		//If the current location is outside the previous start and end location
		//boundaries, extend the boundaries to include the current address.
		unsigned int extensionSize = (_visibleRows * _model.GetMinimumOpcodeByteSize() * 2);
		if (_currentPCLocation > _endLocation)
		{
			_endLocation = _currentPCLocation + extensionSize;
			if (_endLocation > _model.GetAddressBusMask())
			{
				_endLocation = _model.GetAddressBusMask();
			}
		}
		if (_currentPCLocation < _startLocation)
		{
			if (_currentPCLocation <= extensionSize)
			{
				_startLocation = 0;
			}
			else
			{
				_startLocation = _currentPCLocation - extensionSize;
			}
		}
	}

	//Clear the forcePCSync flag now that it has been processed
	_forcePCSync = false;

	//Update the control panel
	SendMessage(_hwndControlPanel, WM_TIMER, wparam, lparam);

	//Update the disassembly
	UpdateDisassembly();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DisassemblyView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if (LOWORD(wparam) == CTL_DATAGRID)
	{
		WC_DataGrid::WindowNotifications notification = (WC_DataGrid::WindowNotifications)HIWORD(wparam);
		if (notification == WC_DataGrid::WindowNotifications::NewRowCount)
		{
			WC_DataGrid::Grid_NewVisibleRowCount* newRowCountInfo = (WC_DataGrid::Grid_NewVisibleRowCount*)lparam;
			_visibleRows = newRowCountInfo->visibleRows;
			UpdateDisassembly();
		}
		else if (notification == WC_DataGrid::WindowNotifications::ShiftRowsUp)
		{
			WC_DataGrid::Grid_ShiftRowsUp* info = (WC_DataGrid::Grid_ShiftRowsUp*)lparam;
			unsigned int shiftCount = 0;
			while (shiftCount < info->shiftCount)
			{
				unsigned int initialPosition = _firstVisibleValueLocation;
				while (((_firstVisibleValueLocation + _firstVisibleOpcodeSize) > initialPosition) //The first opcode in the window is below or contains the address of the previous first opcode in the window
					&& (_firstVisibleValueLocation >= _lastBufferedOpcodeSize) //We're not going to shift above 0
					&& ((_firstVisibleValueLocation - _lastBufferedOpcodeSize) >= _startLocation)) //We're not going to shift past the start location
				{
					_firstVisibleValueLocation -= _lastBufferedOpcodeSize;
					UpdateDisassembly();
				}
				++shiftCount;
			}
		}
		else if (notification == WC_DataGrid::WindowNotifications::ShiftRowsDown)
		{
			WC_DataGrid::Grid_ShiftRowsDown* info = (WC_DataGrid::Grid_ShiftRowsDown*)lparam;
			unsigned int shiftCount = 0;
			while ((shiftCount < info->shiftCount)
				&& ((_firstVisibleValueLocation + _firstVisibleOpcodeSize) < _endLocation))
			{
				_firstVisibleValueLocation += _firstVisibleOpcodeSize;
				UpdateDisassembly();
				++shiftCount;
			}
		}
		else if (notification == WC_DataGrid::WindowNotifications::NewScrollPosition)
		{
			//Move the window to an absolute address as a result of a command from
			//the list control. We align the absolute address to the minimum opcode
			//size first, to ensure alignment rules for the processor are respected
			//when doing a blind scroll.
			WC_DataGrid::Grid_NewScrollPosition* info = (WC_DataGrid::Grid_NewScrollPosition*)lparam;
			_firstVisibleValueLocation = info->scrollPos - (info->scrollPos % _model.GetMinimumOpcodeByteSize());
			UpdateDisassembly();
		}
		else if (notification == WC_DataGrid::WindowNotifications::SelectionEvent)
		{
			WC_DataGrid::Grid_SelectionEvent* info = (WC_DataGrid::Grid_SelectionEvent*)lparam;

			//If the CTRL key was pressed when the row selection was made, toggle a
			//breakpoint at the opcode being displayed on the target row. If the shift key
			//was also pressed, just toggle the enable/disable state of the breakpoint.
			if (info->rowSelected && info->keyPressedCtrl)
			{
				ToggleBreakpointStateAtRow(info->selectedRowNo, info->keyPressedShift);

				//Since we're handling this selection event ourselves, and we don't want a
				//grid selection to occur in response, instruct the data grid to ignore
				//the selection.
				info->ignoreSelectionEvent = true;
			}
		}

		//Update the vertical scroll settings
		WC_DataGrid::Grid_SetVScrollInfo scrollInfo;
		scrollInfo.minPos = (int)_startLocation;
		scrollInfo.maxPos = (int)_endLocation;
		scrollInfo.currentPos = (int)_firstVisibleValueLocation;
		scrollInfo.valuesPerPage = (int)_visibleRows;
		SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::SetVScrollInfo, 0, (LPARAM)&scrollInfo);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DisassemblyView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Read the new client size of the window
	RECT rect;
	GetClientRect(hwnd, &rect);
	int controlWidth = rect.right;
	int controlHeight = rect.bottom;
	GetClientRect(_hwndControlPanel, &rect);
	int controlPanelWidth = rect.right;
	int controlPanelHeight = rect.bottom;

	//Global parameters defining how child windows are positioned
	int borderSize = 4;

	//Calculate the new position of the control panel
	int controlPanelPosX = borderSize;
	int controlPanelPosY = controlHeight - (borderSize + controlPanelHeight);
	MoveWindow(_hwndControlPanel, controlPanelPosX, controlPanelPosY, controlPanelWidth, controlPanelHeight, TRUE);

	//Calculate the new size and position of the list
	int listBoxWidth = controlWidth - (borderSize * 2);
	int listBoxPosX = borderSize;
	int listBoxHeight = controlHeight - ((borderSize * 2) + controlPanelHeight);
	int listBoxPosY = borderSize;
	MoveWindow(_hwndDataGrid, listBoxPosX, listBoxPosY, listBoxWidth, listBoxHeight, TRUE);

	//Update the disassembly
	UpdateDisassembly();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DisassemblyView::msgWM_PAINT(HWND hwnd, WPARAM wparam, LPARAM lparam)
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

	return DefWindowProc(hwnd, WM_PAINT, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT DisassemblyView::msgWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lParam;
	if ((bounceMessage->uMsg == WM_KEYDOWN) && (bounceMessage->wParam == VK_F11))
	{
		//Step a single opcode
		_model.GetDevice()->GetDeviceContext()->StopSystem();
		_model.GetDevice()->GetDeviceContext()->ExecuteDeviceStep();
		bounceMessage->caught = true;
	}
	else if ((bounceMessage->uMsg == WM_SYSKEYDOWN) && (bounceMessage->wParam == VK_F10))
	{
		//Step over the current opcode
		_model.GetDevice()->GetDeviceContext()->StopSystem();
		_model.BreakOnStepOverCurrentOpcode();
		_model.GetDevice()->GetDeviceContext()->RunSystem();
		bounceMessage->caught = true;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Panel dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK DisassemblyView::WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	DisassemblyView* state = (DisassemblyView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch (msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (DisassemblyView*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if (state != 0)
		{
			return state->WndProcPanel(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if (state != 0)
		{
			//Pass this message on to the member window procedure function
			INT_PTR result = state->WndProcPanel(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = FALSE;
	if (state != 0)
	{
		result = state->WndProcPanel(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
INT_PTR DisassemblyView::WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch (msg)
	{
	case WM_INITDIALOG:
		return msgPanelWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgPanelWM_DESTROY(hwnd, wparam, lparam);
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
INT_PTR DisassemblyView::msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	_initializedDialog = true;

	//Set the initial state for the controls
	CheckDlgButton(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_TRACK, (_track)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT, _model.GetAddressBusCharWidth(), _firstVisibleValueLocation);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_START, _model.GetAddressBusCharWidth(), _startLocation);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_END, _model.GetAddressBusCharWidth(), _endLocation);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_READABOVE, _model.GetAddressBusCharWidth(), _readAbove);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR DisassemblyView::msgPanelWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR DisassemblyView::msgPanelWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if (_currentControlFocus != IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT)	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT, _model.GetAddressBusCharWidth(), _firstVisibleValueLocation);
	if (_currentControlFocus != IDC_PROCESSOR_DISASSEMBLY_PANEL_START)	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_START, _model.GetAddressBusCharWidth(), _startLocation);
	if (_currentControlFocus != IDC_PROCESSOR_DISASSEMBLY_PANEL_END)	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_PANEL_END, _model.GetAddressBusCharWidth(), _endLocation);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR DisassemblyView::msgPanelWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if ((HIWORD(wparam) == EN_SETFOCUS) && _initializedDialog)
	{
		_previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		_currentControlFocus = LOWORD(wparam);
	}
	else if ((HIWORD(wparam) == EN_KILLFOCUS) && _initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if (newText != _previousText)
		{
			switch (LOWORD(wparam))
			{
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_START:
				_startLocation = GetDlgItemHex(hwnd, LOWORD(wparam)) & _model.GetAddressBusMask();
				UpdateDlgItemHex(hwnd, LOWORD(wparam), _model.GetAddressBusCharWidth(), _startLocation);
				break;
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_END:
				_endLocation = GetDlgItemHex(hwnd, LOWORD(wparam)) & _model.GetAddressBusMask();
				UpdateDlgItemHex(hwnd, LOWORD(wparam), _model.GetAddressBusCharWidth(), _endLocation);
				break;
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_CURRENT:
				_firstVisibleValueLocation = GetDlgItemHex(hwnd, LOWORD(wparam)) & _model.GetAddressBusMask();
				UpdateDlgItemHex(hwnd, LOWORD(wparam), _model.GetAddressBusCharWidth(), _firstVisibleValueLocation);
				break;
			case IDC_PROCESSOR_DISASSEMBLY_PANEL_READABOVE:
				_readAbove = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, LOWORD(wparam), _model.GetAddressBusCharWidth(), _readAbove);
				break;
			}

			//Update the disassembly
			UpdateDisassembly();
		}
	}
	else if (HIWORD(wparam) == BN_CLICKED)
	{
		switch (LOWORD(wparam))
		{
		case IDC_PROCESSOR_DISASSEMBLY_PANEL_TRACK:
			_track = !_track;
			break;
		case IDC_PROCESSOR_DISASSEMBLY_PANEL_JUMPTOCURRENT:
			_forcePCSync = true;
			break;
		case IDC_PROCESSOR_DISASSEMBLY_PANEL_STEPINTO:
			//Step a single opcode
			_model.GetDevice()->GetDeviceContext()->StopSystem();
			_model.GetDevice()->GetDeviceContext()->ExecuteDeviceStep();
			break;
		case IDC_PROCESSOR_DISASSEMBLY_PANEL_STEPOVER:
			//Step over the current opcode
			_model.GetDevice()->GetDeviceContext()->StopSystem();
			_model.BreakOnStepOverCurrentOpcode();
			_model.GetDevice()->GetDeviceContext()->RunSystem();
			break;
		case IDC_PROCESSOR_DISASSEMBLY_PANEL_STEPOUT:
			//Step out one call stack level
			_model.GetDevice()->GetDeviceContext()->StopSystem();
			_model.BreakOnStepOutCurrentOpcode();
			_model.GetDevice()->GetDeviceContext()->RunSystem();
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
void DisassemblyView::UpdateDisassembly()
{
	//Obtain the address of the first opcode in the buffer
	unsigned int minimumOpcodeByteSize = _model.GetMinimumOpcodeByteSize();
	unsigned int upperReadPosition = (_readAbove > _firstVisibleValueLocation)? 0: _firstVisibleValueLocation - _readAbove;
	if (upperReadPosition < _startLocation)
	{
		upperReadPosition = _startLocation + (_firstVisibleValueLocation % minimumOpcodeByteSize);
	}

	//Skip all opcodes in the buffer which occur before the visible region of the buffer
	unsigned int offset = 0;
	while ((upperReadPosition + offset) < _firstVisibleValueLocation)
	{
		//Read the opcode info
		OpcodeInfo opcodeInfo;
		unsigned int opcodeSize = minimumOpcodeByteSize;
		if (_model.GetOpcodeInfo(upperReadPosition + offset, opcodeInfo) && opcodeInfo.GetIsValidOpcode())
		{
			opcodeSize = opcodeInfo.GetOpcodeSize();
		}

		//Update the last buffered opcode size. This will be correct when the loop
		//terminates.
		_lastBufferedOpcodeSize = opcodeSize;

		//Step to the next opcode
		offset += opcodeSize;
	}

	//If there are no buffered opcodes, set the size of the last buffered opcode to the
	//minimum opcode size.
	if (offset == 0)
	{
		_lastBufferedOpcodeSize = minimumOpcodeByteSize;
	}

	//Read each visible opcode to be shown in the disassembly window
	std::list<IBreakpoint*> breakpointList = _model.GetBreakpointList();
	std::vector<std::wstring> columnDataAddress(_visibleRows);
	std::vector<std::wstring> columnDataOpcode(_visibleRows);
	std::vector<std::wstring> columnDataArgs(_visibleRows);
	std::vector<std::wstring> columnDataComment(_visibleRows);
	std::vector<std::wstring> columnDataBinary(_visibleRows);
	for (unsigned int i = 0; (i < _visibleRows) && ((upperReadPosition + offset) < _endLocation); ++i)
	{
		unsigned int rowPCLocation = (upperReadPosition + offset);

		//Set the color for this row
		WC_DataGrid::Grid_SetRowColor setRowColor;
		if (rowPCLocation == _currentPCLocation)
		{
			//If this row is the same as the current PC location, shade the location in
			//green.
			setRowColor.backgroundColorDefined = true;
			setRowColor.colorBackground = WinColor(128, 255, 128);
			setRowColor.textColorDefined = true;
			setRowColor.colorTextFront = WinColor(0, 0, 0);
			setRowColor.colorTextBack = setRowColor.colorBackground;
		}
		else
		{
			//Look for any breakpoints which trigger at the target address
			bool breakpointAtLocation = false;
			bool breakpointEnabled = false;
			std::list<IBreakpoint*>::const_iterator breakpointIterator = breakpointList.begin();
			while (!breakpointAtLocation && (breakpointIterator != breakpointList.end()))
			{
				breakpointAtLocation = (*breakpointIterator)->PassesLocationCondition(rowPCLocation);
				breakpointEnabled = (*breakpointIterator)->GetEnabled();
				++breakpointIterator;
			}

			//If a breakpoint triggers at this address, shade the location in red.
			if (breakpointAtLocation)
			{
				setRowColor.backgroundColorDefined = true;
				setRowColor.colorBackground = breakpointEnabled? WinColor(255, 128, 128): WinColor(192, 0, 0);
				setRowColor.textColorDefined = true;
				setRowColor.colorTextFront = WinColor(0, 0, 0);
				setRowColor.colorTextBack = setRowColor.colorBackground;
			}
		}
		SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::SetRowColor, i, (LPARAM)&setRowColor);

		//Read the opcode info
		OpcodeInfo opcodeInfo;
		unsigned int opcodeSize = minimumOpcodeByteSize;
		if (!_model.GetOpcodeInfo(rowPCLocation, opcodeInfo))
		{
			//If the target processor doesn't support disassembly, we output a generic
			//message.
			opcodeInfo.SetOpcodeNameDisassembly(L"UNSUPPORTED");
		}
		else if (!opcodeInfo.GetIsValidOpcode())
		{
			//If the target address doesn't reference a valid opcode, we output a generic
			//message.
			opcodeInfo.SetOpcodeNameDisassembly(L"INVALID");
		}
		else
		{
			opcodeSize = opcodeInfo.GetOpcodeSize();
		}

		//Build the opcode disassembly output
		std::wstring addressString;
		IntToStringBase16(rowPCLocation, addressString, _model.GetAddressBusCharWidth(), false);
		columnDataAddress[i] = addressString;
		columnDataOpcode[i] = opcodeInfo.GetOpcodeNameDisassembly();
		columnDataArgs[i] = opcodeInfo.GetOpcodeArgumentsDisassembly();
		std::wstring disassemblyComment = opcodeInfo.GetDisassemblyComment();
		if (!disassemblyComment.empty())
		{
			columnDataComment[i] = L";" + disassemblyComment;
		}

		//Build the machine code disassembly output
		std::wstring binaryString;
		for (unsigned int byteNo = 0; byteNo < opcodeInfo.GetOpcodeSize(); ++byteNo)
		{
			_model.GetByteBitCount();
			unsigned int rawData = _model.GetMemorySpaceByte(rowPCLocation + byteNo);
			if (byteNo > 0)
			{
				binaryString += L' ';
			}
			std::wstring dataString;
			IntToStringBase16(rawData, dataString, _model.GetByteCharWidth(), false);
			binaryString += dataString;
		}
		columnDataBinary[i] = binaryString;

		//If this is the first visible opcode, record the size
		if (i == 0)
		{
			_firstVisibleOpcodeSize = opcodeSize;
		}

		//Update the last visible value location
		_lastVisibleValueLocation = rowPCLocation;

		//Step to the next opcode
		offset += opcodeSize;
	}

	//Write the disassembly data to the window
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, COLUMN_ADDRESS, (LPARAM)&columnDataAddress);
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, COLUMN_OPCODE, (LPARAM)&columnDataOpcode);
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, COLUMN_ARGS, (LPARAM)&columnDataArgs);
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, COLUMN_COMMENT, (LPARAM)&columnDataComment);
	SendMessage(_hwndDataGrid, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, COLUMN_BINARY, (LPARAM)&columnDataBinary);

	//Force the grid control to redraw now that we've updated the text
	InvalidateRect(_hwndDataGrid, NULL, FALSE);
}

//----------------------------------------------------------------------------------------
void DisassemblyView::ToggleBreakpointStateAtRow(unsigned int visibleRowNo, bool toggleEnableState)
{
	//Validate the specified row number
	if (visibleRowNo >= _visibleRows)
	{
		return;
	}

	//Obtain the address of the first opcode in the buffer
	unsigned int minimumOpcodeByteSize = _model.GetMinimumOpcodeByteSize();
	unsigned int upperReadPosition = (_readAbove > _firstVisibleValueLocation)? 0: _firstVisibleValueLocation - _readAbove;
	if (upperReadPosition < _startLocation)
	{
		upperReadPosition = _startLocation + (_firstVisibleValueLocation % minimumOpcodeByteSize);
	}

	//Skip all opcodes in the buffer which occur before the visible region of the buffer
	unsigned int offset = 0;
	while ((upperReadPosition + offset) < _firstVisibleValueLocation)
	{
		//Read the opcode info
		OpcodeInfo opcodeInfo;
		unsigned int opcodeSize = minimumOpcodeByteSize;
		if (_model.GetOpcodeInfo(upperReadPosition + offset, opcodeInfo) && opcodeInfo.GetIsValidOpcode())
		{
			opcodeSize = opcodeInfo.GetOpcodeSize();
		}

		//Step to the next opcode
		offset += opcodeSize;
	}

	//Locate the opcode which was selected in the disassembly window
	unsigned int currentVisibleRowNo = 0;
	while ((currentVisibleRowNo < visibleRowNo) && ((upperReadPosition + offset) < _endLocation))
	{
		//Read the opcode info
		OpcodeInfo opcodeInfo;
		unsigned int opcodeSize = minimumOpcodeByteSize;
		if (_model.GetOpcodeInfo(upperReadPosition + offset, opcodeInfo) && opcodeInfo.GetIsValidOpcode())
		{
			opcodeSize = opcodeInfo.GetOpcodeSize();
		}

		//Step to the next opcode
		offset += opcodeSize;
		++currentVisibleRowNo;
	}
	if (currentVisibleRowNo != visibleRowNo)
	{
		return;
	}

	//Calculate the address of the selected opcode
	unsigned int targetOpcodeLocation = upperReadPosition + offset;

	//Toggle a breakpoint at the target location
	ToggleBreakpointStateAtAddress(targetOpcodeLocation, toggleEnableState);
}

//----------------------------------------------------------------------------------------
void DisassemblyView::ToggleBreakpointStateAtAddress(unsigned int pcLocation, bool toggleEnableState)
{
	//Look for any breakpoints which trigger at the target address
	bool breakpointAtLocation = false;
	IBreakpoint* targetBreakpoint = 0;
	std::list<IBreakpoint*> breakpointList = _model.GetBreakpointList();
	std::list<IBreakpoint*>::const_iterator breakpointIterator = breakpointList.begin();
	while (!breakpointAtLocation && (breakpointIterator != breakpointList.end()))
	{
		if ((*breakpointIterator)->PassesLocationCondition(pcLocation))
		{
			breakpointAtLocation = true;
			targetBreakpoint = *breakpointIterator;
			continue;
		}
		++breakpointIterator;
	}

	//Perform the requested operation
	if (toggleEnableState)
	{
		//Toggle the enable state for a breakpoint at the target address
		if (breakpointAtLocation)
		{
			if (_model.LockBreakpoint(targetBreakpoint))
			{
				targetBreakpoint->SetEnabled(!targetBreakpoint->GetEnabled());
				_model.UnlockBreakpoint(targetBreakpoint);
			}
		}
	}
	else
	{
		//Either add or remove a breakpoint for the target address
		if (breakpointAtLocation)
		{
			_model.DeleteBreakpoint(targetBreakpoint);
		}
		else
		{
			targetBreakpoint = _model.CreateBreakpoint();
			if (_model.LockBreakpoint(targetBreakpoint))
			{
				targetBreakpoint->SetLocationConditionData1(pcLocation);
				targetBreakpoint->SetName(targetBreakpoint->GenerateName());
				targetBreakpoint->SetEnabled(true);
				_model.UnlockBreakpoint(targetBreakpoint);
			}
		}
	}
}
