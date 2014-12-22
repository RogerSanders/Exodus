#ifndef __EVENTLOGVIEW_H__
#define __EVENTLOGVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "EventLogViewPresenter.h"

class EventLogView :public ViewBase
{
public:
	//Constructors
	EventLogView(IUIManager& auiManager, EventLogViewPresenter& apresenter, ISystemGUIInterface& amodel);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ERASEBKGND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Panel dialog window procedure
	static INT_PTR CALLBACK WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Panel dialog event handlers
	INT_PTR msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPanelWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam);

private:
	EventLogViewPresenter& presenter;
	ISystemGUIInterface& model;
	HFONT fontHandle;
	HWND hwndLayoutGrid;
	HWND hwndEventLogGrid;
	HWND hwndNestedDialog;
	bool loggerLevel1Enabled;
	bool loggerLevel2Enabled;
	bool loggerLevel3Enabled;
	bool loggerLevel4Enabled;
	bool loggerLevel5Enabled;
	unsigned int logLastModifiedToken;
	std::vector<ISystemGUIInterface::SystemLogEntry> eventLog;
};

#endif
