#ifndef __WATCHPOINTVIEW_H__
#define __WATCHPOINTVIEW_H__
#include "WatchpointViewPresenter.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "Processor/Processor.pkg"

class WatchpointView :public ViewBase
{
public:
	// Constructors
	WatchpointView(IUIManager& uiManager, WatchpointViewPresenter& presenter, IProcessor& model);

protected:
	// Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	// Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	// Watchpoint functions
	void LoadDataFromWatchpoint(IWatchpoint* watchpoint);
	void SaveDataToWatchpoint(IWatchpoint* watchpoint);
	void ClearWatchpointData();
	void UpdateWatchpointFields(HWND hwnd);

private:
	WatchpointViewPresenter& _presenter;
	IProcessor& _model;
	bool _initializedDialog;
	std::wstring _previousText;
	unsigned int _currentControlFocus;
	int _selectedWatchpointListIndex;
	IWatchpoint* _selectedWatchpoint;
	std::list<IWatchpoint*> _cachedWatchpointList;

	std::wstring _watchpointName;
	bool _watchpointEnable;
	bool _watchpointLog;
	bool _watchpointBreak;
	bool _watchpointConditionNot;
	bool _watchpointOnRead;
	bool _watchpointOnWrite;
	bool _watchpointReadConditionEnabled;
	bool _watchpointReadConditionNot;
	bool _watchpointWriteConditionEnabled;
	bool _watchpointWriteConditionNot;
	IWatchpoint::Condition _watchpointCondition;
	IWatchpoint::Condition _watchpointReadCondition;
	IWatchpoint::Condition _watchpointWriteCondition;
	unsigned int _watchpointLocation1;
	unsigned int _watchpointLocation2;
	unsigned int _watchpointLocationMask;
	unsigned int _watchpointReadConditionData1;
	unsigned int _watchpointReadConditionData2;
	unsigned int _watchpointWriteConditionData1;
	unsigned int _watchpointWriteConditionData2;
	bool _watchpointCounterEnable;
	unsigned int _watchpointCounter;
	unsigned int _watchpointCounterInterval;
};

#endif
