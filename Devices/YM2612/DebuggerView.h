#include "YM2612.h"
#ifndef __YM2612_DEBUGGERVIEW_H__
#define __YM2612_DEBUGGERVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "SystemInterface/SystemInterface.pkg"

class YM2612::DebuggerView :public ViewBase
{
public:
	//Constructors
	DebuggerView(YM2612* adevice);
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
	LockedRegisterKey ControlIDToLockedRegKey(int controlID, unsigned int channelNo);
	void LockControl(int controlID, unsigned int channelNo);

private:
	YM2612* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	COLORREF lockedColor;
	HBRUSH lockedBrush;
	unsigned int selectedChannel;
};

#endif
