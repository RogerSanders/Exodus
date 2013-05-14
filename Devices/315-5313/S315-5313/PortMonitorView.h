#include "S315_5313.h"
#ifndef __S315_5313_PORTMONITORVIEW_H__
#define __S315_5313_PORTMONITORVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::PortMonitorView :public ViewBase
{
public:
	//Constructors
	PortMonitorView(S315_5313* adevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	S315_5313* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
};

#endif
