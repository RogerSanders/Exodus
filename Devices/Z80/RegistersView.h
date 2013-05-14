#include "Z80.h"
#ifndef __Z80_REGISTERSVIEW_H__
#define __Z80_REGISTERSVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"
namespace Z80{

class Z80::RegistersView :public ViewBase
{
public:
	//Constructors
	RegistersView(Z80* adevice);
	~RegistersView();

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Changed register functions
	bool RegisterContentsChanged(int controlID);

private:
	Z80* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	COLORREF changedColor;
	COLORREF backgroundColor;
	HBRUSH backgroundBrush;
	unsigned int systemPausedToggleCounterCached;
};

} //Close namespace Z80
#endif
