#include "System.h"
#ifndef __SYSTEM_LOGGERDETAILSVIEW_H__
#define __SYSTEM_LOGGERDETAILSVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class System::LoggerDetailsView :public ViewBase
{
public:
	//Constructors
	LoggerDetailsView(System* asystem, const LogEntryInternal& alogEntry);

	//Log entry functions
	void SetLogEntry(const LogEntryInternal& alogEntry);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Entry update functions
	void UpdateLogEntryDisplay(HWND hwnd);

private:
	System* system;
	HWND hwndInternal;
	LogEntryInternal logEntry;
};

#endif
