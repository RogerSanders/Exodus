#include "EventLogView.h"
#include "resource.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "WindowsControls/WindowsControls.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
EventLogView::EventLogView(IUIManager& auiManager, EventLogViewPresenter& apresenter, ISystemGUIInterface& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	fontHandle = NULL;
	hwndLayoutGrid = NULL;
	hwndEventLogGrid = NULL;
	hwndNestedDialog = NULL;
	loggerLevel1Enabled = true;
	loggerLevel2Enabled = true;
	loggerLevel3Enabled = true;
	loggerLevel4Enabled = true;
	loggerLevel5Enabled = true;
	logLastModifiedToken = 0;
	SetWindowSettings(apresenter.GetUnqualifiedViewTitle(), 0, WS_EX_COMPOSITED, 640, 200);
	SetDockableViewType(true, DockPos::Bottom);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT EventLogView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_ERASEBKGND:
		return msgWM_ERASEBKGND(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT EventLogView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the dialog control panel
	hwndNestedDialog = CreateDialogParam(GetAssemblyHandle(), MAKEINTRESOURCE(IDD_EVENTLOG_CONTROL), hwnd, WndProcPanelStatic, (LPARAM)this);
	ShowWindow(hwndNestedDialog, SW_SHOWNORMAL);
	UpdateWindow(hwndNestedDialog);

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
	hwndEventLogGrid = CreateWindowEx(WS_EX_CLIENTEDGE, WC_DataGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, NULL, GetAssemblyHandle(), NULL);

	//Set the default font for the child controls
	SendMessage(hwndEventLogGrid, WM_SETFONT, (WPARAM)fontHandle, (LPARAM)TRUE);
	SendMessage(hwndEventLogGrid, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)fontHandle, (LPARAM)TRUE);

	//Insert our columns into the event log data grid
	SendMessage(hwndEventLogGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Time", 1));
	SendMessage(hwndEventLogGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Level", 2));
	SendMessage(hwndEventLogGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Source", 3));
	SendMessage(hwndEventLogGrid, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Description", 4));

	//Register the LayoutGrid window class
	WC_LayoutGrid::RegisterWindowClass(GetAssemblyHandle());

	//Create the LayoutGrid child control
	hwndLayoutGrid = CreateWindowEx(WS_EX_TRANSPARENT, WC_LayoutGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, GetAssemblyHandle(), NULL);

	//Insert our rows and columns into the layout grid
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddColumn, 0, (LPARAM)&(const WC_LayoutGrid::AddColumnParams&)WC_LayoutGrid::AddColumnParams(WC_LayoutGrid::SizeMode::Content));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddColumn, 0, (LPARAM)&(const WC_LayoutGrid::AddColumnParams&)WC_LayoutGrid::AddColumnParams(WC_LayoutGrid::SizeMode::Proportional));

	//Add each child control to the layout grid
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(hwndNestedDialog, 0, 0).SetSizeMode(WC_LayoutGrid::WindowSizeMode::Fixed));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(hwndEventLogGrid, 0, 1));

	//Start the event log refresh timer
	SetTimer(hwnd, 1, 100, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT EventLogView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
LRESULT EventLogView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
LRESULT EventLogView::msgWM_ERASEBKGND(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Ensure that our window background is painted with the dialog background colour
	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect((HDC)wParam, &rect, GetSysColorBrush(COLOR_3DFACE));
	return TRUE;
}

//----------------------------------------------------------------------------------------
LRESULT EventLogView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	unsigned int newEventLogLastModifiedToken = model.GetEventLogLastModifiedToken();
	if(newEventLogLastModifiedToken != logLastModifiedToken)
	{
		std::vector<ISystemGUIInterface::SystemLogEntry> newEventLog = model.GetEventLog();
		if(eventLog.empty() || (newEventLog.size() < eventLog.size()) || (eventLog[0].eventTimeString != newEventLog[0].eventTimeString) || (eventLog[eventLog.size()-1].eventTimeString != newEventLog[eventLog.size()-1].eventTimeString))
		{
			SendMessage(hwndEventLogGrid, (UINT)WC_DataGrid::WindowMessages::DeleteRows, 0, (LPARAM)&(const WC_DataGrid::Grid_DeleteRows&)WC_DataGrid::Grid_DeleteRows(0, (unsigned int)eventLog.size()));
			eventLog.clear();
		}
		unsigned int currentEntryCount = (unsigned int)eventLog.size();
		unsigned int addedEntryCount = (unsigned int)(newEventLog.size() - eventLog.size());

		//Add log entries which are not currently filtered
		WC_DataGrid::Grid_InsertRows insertRowsInfo;
		insertRowsInfo.targetRowNo = currentEntryCount;
		insertRowsInfo.rowCount = addedEntryCount;
		insertRowsInfo.rowData.reserve(addedEntryCount);
		for(unsigned int i = 0; i < addedEntryCount; ++i)
		{
			const ISystemGUIInterface::SystemLogEntry& logEntry = newEventLog[currentEntryCount + i];
			if((loggerLevel1Enabled && (logEntry.eventLevel == ILogEntry::EventLevel::Info))
			|| (loggerLevel2Enabled && (logEntry.eventLevel == ILogEntry::EventLevel::Debug))
			|| (loggerLevel3Enabled && (logEntry.eventLevel == ILogEntry::EventLevel::Warning))
			|| (loggerLevel4Enabled && (logEntry.eventLevel == ILogEntry::EventLevel::Error))
			|| (loggerLevel5Enabled && (logEntry.eventLevel == ILogEntry::EventLevel::Critical)))
			{
				insertRowsInfo.rowData.resize(i+1);
				insertRowsInfo.rowData[i][1] = logEntry.eventTimeString;
				insertRowsInfo.rowData[i][2] = logEntry.eventLevelString;
				insertRowsInfo.rowData[i][3] = logEntry.source;
				insertRowsInfo.rowData[i][4] = logEntry.text;
			}
		}
		SendMessage(hwndEventLogGrid, (UINT)WC_DataGrid::WindowMessages::InsertRows, 0, (LPARAM)&insertRowsInfo);

		eventLog = newEventLog;
		logLastModifiedToken = newEventLogLastModifiedToken;
	}

	return 0;
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK EventLogView::WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	EventLogView* state = (EventLogView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (EventLogView*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcPanel(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if(state != 0)
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
	if(state != 0)
	{
		result = state->WndProcPanel(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
INT_PTR EventLogView::WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgPanelWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgPanelWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Panel dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR EventLogView::msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CheckDlgButton(hwnd, IDC_EVENTLOG_SHOW_1, (loggerLevel1Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_EVENTLOG_SHOW_2, (loggerLevel2Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_EVENTLOG_SHOW_3, (loggerLevel3Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_EVENTLOG_SHOW_4, (loggerLevel4Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_EVENTLOG_SHOW_5, (loggerLevel5Enabled)? BST_CHECKED: BST_UNCHECKED);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR EventLogView::msgPanelWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_EVENTLOG_CLEAR:
			model.ClearEventLog();
			break;
		case IDC_EVENTLOG_SAVE:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"CSV Files|csv", L"csv", L"", L"", selectedFilePath))
			{
				presenter.SaveEventLog(eventLog, selectedFilePath);
			}
			break;}
		case IDC_EVENTLOG_SHOW_1:
			loggerLevel1Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			eventLog.clear();
			logLastModifiedToken = 0;
			break;
		case IDC_EVENTLOG_SHOW_2:
			loggerLevel2Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			eventLog.clear();
			logLastModifiedToken = 0;
			break;
		case IDC_EVENTLOG_SHOW_3:
			loggerLevel3Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			eventLog.clear();
			logLastModifiedToken = 0;
			break;
		case IDC_EVENTLOG_SHOW_4:
			loggerLevel4Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			eventLog.clear();
			logLastModifiedToken = 0;
			break;
		case IDC_EVENTLOG_SHOW_5:
			loggerLevel5Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			eventLog.clear();
			logLastModifiedToken = 0;
			break;
		}
	}
	return TRUE;
}
