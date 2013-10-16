#include "System.h"
#ifndef __SYSTEM_INPUTMAPPINGVIEW_H__
#define __SYSTEM_INPUTMAPPINGVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "SystemInterface/SystemInterface.pkg"

class System::EmbeddedROMView :public ViewBase
{
public:
	//Constructors
	EmbeddedROMView(System* asystem);

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
	EmbeddedROMList romInfoListCopy;
	IDevice* selectedROMDevice;
};

#endif
