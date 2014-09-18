#ifndef __LOGGERVIEW_H__
#define __LOGGERVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "LoggerViewPresenter.h"

class LoggerView :public ViewBase
{
public:
	//Constructors
	LoggerView(IUIManager& auiManager, LoggerViewPresenter& apresenter, ISystemGUIInterface& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Log refresh functions
	void RefreshLogEntryList(HWND hwnd);

private:
	LoggerViewPresenter& presenter;
	ISystemGUIInterface& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	bool loggerLevel1Enabled;
	bool loggerLevel2Enabled;
	bool loggerLevel3Enabled;
	bool loggerLevel4Enabled;
	bool loggerLevel5Enabled;
	unsigned int logLastModifiedToken;
	std::vector<ISystemGUIInterface::SystemLogEntry> eventLog;
};

#endif
