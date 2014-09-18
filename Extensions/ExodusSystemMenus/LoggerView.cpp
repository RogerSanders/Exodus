#include "LoggerView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LoggerView::LoggerView(IUIManager& auiManager, LoggerViewPresenter& apresenter, ISystemGUIInterface& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_LOGGER));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR LoggerView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
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
INT_PTR LoggerView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	loggerLevel1Enabled = true;
	loggerLevel2Enabled = true;
	loggerLevel3Enabled = true;
	loggerLevel4Enabled = true;
	loggerLevel5Enabled = true;

	int tabstops[] = {50, 113, 183};
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_SETTABSTOPS, (WPARAM)3, (LPARAM)&tabstops[0]);

	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_1, (loggerLevel1Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_2, (loggerLevel2Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_3, (loggerLevel3Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_4, (loggerLevel4Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_5, (loggerLevel5Enabled)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemBin(hwnd, IDC_LOGGER_LISTSIZE, model.GetEventLogSize());
	SetTimer(hwnd, 1, 200, NULL);

	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR LoggerView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR LoggerView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	unsigned int newEventLogLastModifiedToken = model.GetEventLogLastModifiedToken();
	if(newEventLogLastModifiedToken != logLastModifiedToken)
	{
		eventLog = model.GetEventLog();
		RefreshLogEntryList(hwnd);
		logLastModifiedToken = newEventLogLastModifiedToken;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR LoggerView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == LBN_DBLCLK)
	{
		//Display the additional info dialog for the selected log entry
		switch(LOWORD(wparam))
		{
		case IDC_LOGGER_LIST:{
			int currentItemListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_GETCURSEL, 0, 0);
			unsigned int targetItemLogIndex = (unsigned int)SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_GETITEMDATA, currentItemListIndex, NULL);

			if(targetItemLogIndex < (unsigned int)eventLog.size())
			{
				presenter.OpenLoggerDetailsView(eventLog[targetItemLogIndex]);
			}
			break;}
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
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
			case IDC_LOGGER_LISTSIZE:
				model.SetEventLogSize(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_LOGGER_CLEAR:
			model.ClearEventLog();
			break;
		case IDC_LOGGER_SAVE:{
			std::wstring selectedFilePath;
			if(SelectNewFile(hwnd, L"CSV Files|csv", L"csv", L"", L"", selectedFilePath))
			{
				presenter.SaveEventLog(eventLog, selectedFilePath);
			}
			break;}
		case IDC_LOGGER_SHOW_1:
			loggerLevel1Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			RefreshLogEntryList(hwnd);
			break;
		case IDC_LOGGER_SHOW_2:
			loggerLevel2Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			RefreshLogEntryList(hwnd);
			break;
		case IDC_LOGGER_SHOW_3:
			loggerLevel3Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			RefreshLogEntryList(hwnd);
			break;
		case IDC_LOGGER_SHOW_4:
			loggerLevel4Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			RefreshLogEntryList(hwnd);
			break;
		case IDC_LOGGER_SHOW_5:
			loggerLevel5Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			RefreshLogEntryList(hwnd);
			break;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Log refresh functions
//----------------------------------------------------------------------------------------
void LoggerView::RefreshLogEntryList(HWND hwnd)
{
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_RESETCONTENT, 0, NULL);
	for(unsigned int currentItemLogIndex = 0; currentItemLogIndex < (unsigned int)eventLog.size(); ++currentItemLogIndex)
	{
		//Add log entries which are not currently filtered
		const ISystemGUIInterface::SystemLogEntry& logEntry = eventLog[currentItemLogIndex];
		if((loggerLevel1Enabled && (logEntry.eventLevel == ILogEntry::EVENTLEVEL_INFO))
			|| (loggerLevel2Enabled && (logEntry.eventLevel == ILogEntry::EVENTLEVEL_DEBUG))
			|| (loggerLevel3Enabled && (logEntry.eventLevel == ILogEntry::EVENTLEVEL_WARNING))
			|| (loggerLevel4Enabled && (logEntry.eventLevel == ILogEntry::EVENTLEVEL_ERROR))
			|| (loggerLevel5Enabled && (logEntry.eventLevel == ILogEntry::EVENTLEVEL_CRITICAL)))
		{
			std::wstringstream text;
			text << logEntry.eventTimeString << '\t' << logEntry.eventLevelString << '\t' << logEntry.source << L'\t' << logEntry.text;
			LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_ADDSTRING, 0, (LPARAM)text.str().c_str());
			SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)currentItemLogIndex);
		}
	}
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_LOGGER_LIST), NULL, FALSE);
}
