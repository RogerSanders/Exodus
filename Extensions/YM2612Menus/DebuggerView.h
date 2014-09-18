#ifndef __DEBUGGERVIEW_H__
#define __DEBUGGERVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "DebuggerViewPresenter.h"
#include "YM2612/IYM2612.h"

class DebuggerView :public ViewBase
{
public:
	//Constructors
	DebuggerView(IUIManager& auiManager, DebuggerViewPresenter& apresenter, IYM2612& amodel);
	~DebuggerView();

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Register locking functions
	bool ControlIDToDataID(int controlID, unsigned int& genericDataID, IYM2612::RegisterDataContext& registerDataContext, IYM2612::ChannelDataContext& channelDataContext, IYM2612::OperatorDataContext& operatorDataContext, const IGenericAccess::DataContext** dataContext);

private:
	DebuggerViewPresenter& presenter;
	IYM2612& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	COLORREF lockedColor;
	HBRUSH lockedBrush;
	unsigned int selectedChannel;
};

#endif
