#ifndef __LOGGERDETAILSVIEW_H__
#define __LOGGERDETAILSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "LoggerDetailsViewPresenter.h"

class LoggerDetailsView :public ViewBase
{
public:
	//Constructors
	LoggerDetailsView(IUIManager& auiManager, LoggerDetailsViewPresenter& apresenter, ISystemGUIInterface& amodel, const ISystemGUIInterface::SystemLogEntry& alogEntry);

	//Log entry functions
	void SetLogEntry(const ISystemGUIInterface::SystemLogEntry& alogEntry);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Entry update functions
	void UpdateLogEntryDisplay(HWND hwnd);

private:
	LoggerDetailsViewPresenter& presenter;
	ISystemGUIInterface& model;
	HWND hwndInternal;
	ISystemGUIInterface::SystemLogEntry logEntry;
};

#endif
