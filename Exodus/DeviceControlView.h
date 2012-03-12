#include "System.h"
#ifndef __SYSTEM_DEVICECONTROLVIEW_H__
#define __SYSTEM_DEVICECONTROLVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class System::DeviceControlView :public ViewBase
{
public:
	//Constructors
	DeviceControlView(System* asystem);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	System* system;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	DeviceContext* deviceContext;
	unsigned int systemStep;
	unsigned int deviceStep;
};

#endif
