#ifndef __EVENTLOGVIEW_H__
#define __EVENTLOGVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "EventLogViewPresenter.h"

class EventLogView :public ViewBase
{
public:
	//Constructors
	EventLogView(IUIManager& uiManager, EventLogViewPresenter& presenter, ISystemGUIInterface& model);

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
	EventLogViewPresenter& _presenter;
	ISystemGUIInterface& _model;
	HFONT _fontHandle;
	HWND _hwndLayoutGrid;
	HWND _hwndEventLogGrid;
	HWND _hwndNestedDialog;
	bool _loggerLevel1Enabled;
	bool _loggerLevel2Enabled;
	bool _loggerLevel3Enabled;
	bool _loggerLevel4Enabled;
	bool _loggerLevel5Enabled;
	unsigned int _logLastModifiedToken;
	std::vector<ISystemGUIInterface::SystemLogEntry> _eventLog;
};

#endif
