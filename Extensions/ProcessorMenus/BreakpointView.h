#ifndef __BREAKPOINTVIEW_H__
#define __BREAKPOINTVIEW_H__
#include "BreakpointViewPresenter.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Processor/Processor.pkg"

class BreakpointView :public ViewBase
{
public:
	//Constructors
	BreakpointView(IUIManager& auiManager, BreakpointViewPresenter& apresenter, IProcessor& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Breakpoint functions
	void LoadDataFromBreakpoint(IBreakpoint* breakpoint);
	void SaveDataToBreakpoint(IBreakpoint* breakpoint);
	void ClearBreakpointData();
	void UpdateBreakpointFields(HWND hwnd);

private:
	BreakpointViewPresenter& presenter;
	IProcessor& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	int selectedBreakpointListIndex;
	IBreakpoint* selectedBreakpoint;
	std::list<IBreakpoint*> cachedBreakpointList;

	std::wstring breakpointName;
	bool breakpointEnable;
	bool breakpointLog;
	bool breakpointBreak;
	bool breakpointConditionNot;
	IBreakpoint::Condition breakpointCondition;
	unsigned int breakpointLocation1;
	unsigned int breakpointLocation2;
	unsigned int breakpointLocationMask;
	bool breakpointCounterEnable;
	unsigned int breakpointCounter;
	unsigned int breakpointCounterInterval;
};

#endif
