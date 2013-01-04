#include "VDP.h"
#ifndef __VDP_PORTMONITORDETAILSVIEW_H__
#define __VDP_PORTMONITORDETAILSVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class VDP::PortMonitorDetailsView :public ViewBase
{
public:
	//Constructors
	PortMonitorDetailsView(VDP* adevice, const PortMonitorEntry& aentry);

	//Port monitor entry functions
	void SetPortMonitorEntry(const PortMonitorEntry& aentry);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Entry update functions
	void UpdatePortMonitorDisplay(HWND hwnd);

private:
	VDP* device;
	HWND hwndInternal;
	PortMonitorEntry entry;
};

#endif
