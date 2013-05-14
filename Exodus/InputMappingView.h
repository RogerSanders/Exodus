#include "System.h"
#ifndef __SYSTEM_INPUTMAPPINGVIEW_H__
#define __SYSTEM_INPUTMAPPINGVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class System::InputMappingView :public ViewBase
{
public:
	//Constructors
	InputMappingView(System* asystem);

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
	System* system;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	std::list<IDevice*> inputDevicesCopy;
	IDevice* selectedInputDevice;
};

#endif
