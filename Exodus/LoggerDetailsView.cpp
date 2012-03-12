#include "LoggerDetailsView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::LoggerDetailsView::LoggerDetailsView(System* asystem, const LogEntryInternal& alogEntry)
:system(asystem), logEntry(alogEntry)
{
	hwndInternal = NULL;
	std::wstring windowTitle = L"Logger Details";
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)system->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_LOGGER_DETAILS));
}

//----------------------------------------------------------------------------------------
//Log entry functions
//----------------------------------------------------------------------------------------
void System::LoggerDetailsView::SetLogEntry(const LogEntryInternal& alogEntry)
{
	logEntry = alogEntry;
	if(hwndInternal != NULL)
	{
		UpdateLogEntryDisplay(hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR System::LoggerDetailsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR System::LoggerDetailsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	hwndInternal = hwnd;
	UpdateLogEntryDisplay(hwndInternal);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR System::LoggerDetailsView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Entry update functions
//----------------------------------------------------------------------------------------
void System::LoggerDetailsView::UpdateLogEntryDisplay(HWND hwnd)
{
	UpdateDlgItemString(hwnd, IDC_LOGGER_DETAILS_TIME, logEntry.eventTimeString);
	UpdateDlgItemString(hwnd, IDC_LOGGER_DETAILS_SOURCE, logEntry.source);
	UpdateDlgItemString(hwnd, IDC_LOGGER_DETAILS_LEVEL, logEntry.eventLevelString);
	UpdateDlgItemString(hwnd, IDC_LOGGER_DETAILS_DESCRIPTION, logEntry.text);
}
