#include "LoggerDetailsView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LoggerDetailsView::LoggerDetailsView(IUIManager& auiManager, LoggerDetailsViewPresenter& apresenter, ISystemGUIInterface& amodel, const ISystemGUIInterface::SystemLogEntry& alogEntry)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), logEntry(alogEntry)
{
	hwndInternal = NULL;
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_LOGGER_DETAILS));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Log entry functions
//----------------------------------------------------------------------------------------
void LoggerDetailsView::SetLogEntry(const ISystemGUIInterface::SystemLogEntry& alogEntry)
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
INT_PTR LoggerDetailsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR LoggerDetailsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	hwndInternal = hwnd;
	UpdateLogEntryDisplay(hwndInternal);

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Entry update functions
//----------------------------------------------------------------------------------------
void LoggerDetailsView::UpdateLogEntryDisplay(HWND hwnd)
{
	UpdateDlgItemString(hwnd, IDC_LOGGER_DETAILS_TIME, logEntry.eventTimeString);
	UpdateDlgItemString(hwnd, IDC_LOGGER_DETAILS_SOURCE, logEntry.source);
	UpdateDlgItemString(hwnd, IDC_LOGGER_DETAILS_LEVEL, logEntry.eventLevelString);
	UpdateDlgItemString(hwnd, IDC_LOGGER_DETAILS_DESCRIPTION, logEntry.text);
}
