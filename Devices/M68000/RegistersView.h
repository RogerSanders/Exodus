#include "M68000.h"
#ifndef __M68000_REGISTERSVIEW_H__
#define __M68000_REGISTERSVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"
namespace M68000{

class M68000::RegistersView :public ViewBase
{
public:
	//Constructors
	RegistersView(M68000* adevice);
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
	M68000* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	COLORREF changedColor;
	COLORREF backgroundColor;
	HBRUSH backgroundBrush;
	unsigned int systemPausedToggleCounterCached;
};

} //Close namespace M68000
#endif
