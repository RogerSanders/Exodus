#ifndef __DEBUGGERVIEW_H__
#define __DEBUGGERVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "DebuggerViewPresenter.h"
#include "YM2612/IYM2612.h"

class DebuggerView :public ViewBase
{
public:
	// Constructors
	DebuggerView(IUIManager& uiManager, DebuggerViewPresenter& presenter, IYM2612& model);
	~DebuggerView();

protected:
	// Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	// Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	// Register locking functions
	bool ControlIDToDataID(int controlID, unsigned int& genericDataID, IYM2612::RegisterDataContext& registerDataContext, IYM2612::ChannelDataContext& channelDataContext, IYM2612::OperatorDataContext& operatorDataContext, const IGenericAccess::DataContext** dataContext);

private:
	DebuggerViewPresenter& _presenter;
	IYM2612& _model;
	bool _initializedDialog;
	std::wstring _previousText;
	unsigned int _currentControlFocus;
	COLORREF _lockedColor;
	HBRUSH _lockedBrush;
	unsigned int _selectedChannel;
};

#endif
