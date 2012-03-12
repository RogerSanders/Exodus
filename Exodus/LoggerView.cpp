#include "LoggerView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::LoggerView::LoggerView(System* asystem)
:system(asystem), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = L"Logger";
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)system->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_LOGGER));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR System::LoggerView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR System::LoggerView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	int tabstops[] = {50, 113, 183};
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_SETTABSTOPS, (WPARAM)3, (LPARAM)&tabstops[0]);

	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_1, (system->loggerLevel1Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_2, (system->loggerLevel2Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_3, (system->loggerLevel3Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_4, (system->loggerLevel4Enabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_LOGGER_SHOW_5, (system->loggerLevel5Enabled)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemBin(hwnd, IDC_LOGGER_LISTSIZE, system->GetEventLogSize());
	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::LoggerView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::LoggerView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	boost::mutex::scoped_lock lock(system->debugMutex);
	initializedDialog = true;
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_RESETCONTENT, 0, NULL);
	unsigned int currentItemLogIndex = 0;
	for(std::list<LogEntryInternal>::iterator i = system->log.begin(); i != system->log.end(); ++i)
	{
		//Add log entries which are not currently filtered
		if((system->loggerLevel1Enabled && (i->eventLevel == 1)) ||
			(system->loggerLevel2Enabled && (i->eventLevel == 2)) ||
			(system->loggerLevel3Enabled && (i->eventLevel == 3)) ||
			(system->loggerLevel3Enabled && (i->eventLevel == 4)) ||
			(system->loggerLevel4Enabled && (i->eventLevel == 5)))
		{
			std::wstringstream text;
			text << i->eventTimeString << '\t' << i->eventLevelString << '\t' << i->source << L'\t' << i->text;
			LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_ADDSTRING, 0, (LPARAM)text.str().c_str());
			SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)currentItemLogIndex);
		}
		++currentItemLogIndex;
	}
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_LOGGER_LIST), NULL, FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::LoggerView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == LBN_DBLCLK)
	{
		//Display the additional info dialog for the selected log entry
		switch(LOWORD(wparam))
		{
		case IDC_LOGGER_LIST:{
			int currentItemListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_GETCURSEL, 0, 0);
			unsigned int targetItemLogIndex = (unsigned int)SendMessage(GetDlgItem(hwnd, IDC_LOGGER_LIST), LB_GETITEMDATA, currentItemListIndex, NULL);

			boost::mutex::scoped_lock lock(system->debugMutex);
			unsigned int currentItemLogIndex = 0;
			for(std::list<LogEntryInternal>::iterator i = system->log.begin(); i != system->log.end(); ++i)
			{
				if(currentItemLogIndex == targetItemLogIndex)
				{
					system->OpenLoggerDetailsView(*i);
				}
				++currentItemLogIndex;
			}
			break;}
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
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
			case IDC_LOGGER_LISTSIZE:
				system->SetEventLogSize(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_LOGGER_CLEAR:
			system->ClearEventLog();
			break;
		case IDC_LOGGER_SAVE:{
			//Get filename
			TCHAR fileName[MAX_PATH];
			OPENFILENAME openFileParams;
			ZeroMemory(&openFileParams, sizeof(openFileParams));
			openFileParams.lStructSize = sizeof(openFileParams);
			openFileParams.hwndOwner = hwnd;
			openFileParams.lpstrFile = fileName;
			openFileParams.lpstrFile[0] = '\0';
			openFileParams.nMaxFile = sizeof(fileName);
			openFileParams.lpstrFilter = L"csv files (*.csv)\0*.csv\0All (*.*)\0*.*\0";
			openFileParams.lpstrDefExt = L"csv";
			openFileParams.nFilterIndex = 1;
			openFileParams.lpstrFileTitle = NULL;
			openFileParams.nMaxFileTitle = 0;
			openFileParams.lpstrInitialDir = NULL;
			openFileParams.Flags = 0;

			if(GetSaveFileName(&openFileParams) != 0)
			{
				system->SaveEventLog(openFileParams.lpstrFile);
			}
			break;}
		case IDC_LOGGER_SHOW_1:
			system->loggerLevel1Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_LOGGER_SHOW_2:
			system->loggerLevel2Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_LOGGER_SHOW_3:
			system->loggerLevel3Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_LOGGER_SHOW_4:
			system->loggerLevel4Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_LOGGER_SHOW_5:
			system->loggerLevel5Enabled = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		}
	}

	return TRUE;
}
