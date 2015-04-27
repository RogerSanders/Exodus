#ifndef __WATCHPOINTVIEW_H__
#define __WATCHPOINTVIEW_H__
#include "WatchpointViewPresenter.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "Processor/Processor.pkg"

class WatchpointView :public ViewBase
{
public:
	//Constructors
	WatchpointView(IUIManager& auiManager, WatchpointViewPresenter& apresenter, IProcessor& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Watchpoint functions
	void LoadDataFromWatchpoint(IWatchpoint* watchpoint);
	void SaveDataToWatchpoint(IWatchpoint* watchpoint);
	void ClearWatchpointData();
	void UpdateWatchpointFields(HWND hwnd);

private:
	WatchpointViewPresenter& presenter;
	IProcessor& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	int selectedWatchpointListIndex;
	IWatchpoint* selectedWatchpoint;
	std::list<IWatchpoint*> cachedWatchpointList;

	std::wstring watchpointName;
	bool watchpointEnable;
	bool watchpointLog;
	bool watchpointBreak;
	bool watchpointConditionNot;
	bool watchpointOnRead;
	bool watchpointOnWrite;
	bool watchpointReadConditionEnabled;
	bool watchpointReadConditionNot;
	bool watchpointWriteConditionEnabled;
	bool watchpointWriteConditionNot;
	IWatchpoint::Condition watchpointCondition;
	IWatchpoint::Condition watchpointReadCondition;
	IWatchpoint::Condition watchpointWriteCondition;
	unsigned int watchpointLocation1;
	unsigned int watchpointLocation2;
	unsigned int watchpointLocationMask;
	unsigned int watchpointReadConditionData1;
	unsigned int watchpointReadConditionData2;
	unsigned int watchpointWriteConditionData1;
	unsigned int watchpointWriteConditionData2;
	bool watchpointCounterEnable;
	unsigned int watchpointCounter;
	unsigned int watchpointCounterInterval;
};

#endif
