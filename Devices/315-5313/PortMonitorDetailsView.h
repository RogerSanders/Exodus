#include "S315_5313.h"
#ifndef __S315_5313_PORTMONITORDETAILSVIEW_H__
#define __S315_5313_PORTMONITORDETAILSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::PortMonitorDetailsView :public ViewBase
{
public:
	//Constructors
	PortMonitorDetailsView(S315_5313* adevice, const PortMonitorEntry& aentry);

	//Port monitor entry functions
	void SetPortMonitorEntry(const PortMonitorEntry& aentry);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Entry update functions
	void UpdatePortMonitorDisplay(HWND hwnd);

private:
	S315_5313* device;
	HWND hwndInternal;
	PortMonitorEntry entry;
};

#endif
