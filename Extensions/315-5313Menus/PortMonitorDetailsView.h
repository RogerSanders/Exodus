#ifndef __PORTMONITORDETAILSVIEW_H__
#define __PORTMONITORDETAILSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "PortMonitorDetailsViewPresenter.h"
#include "315-5313/IS315_5313.h"

class PortMonitorDetailsView :public ViewBase
{
public:
	//Constructors
	PortMonitorDetailsView(IUIManager& auiManager, PortMonitorDetailsViewPresenter& apresenter, IS315_5313& amodel, const IS315_5313::PortMonitorEntry& aentry);

	//Port monitor entry functions
	void SetPortMonitorEntry(const IS315_5313::PortMonitorEntry& aentry);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Entry update functions
	void UpdatePortMonitorDisplay(HWND hwnd);

private:
	PortMonitorDetailsViewPresenter& presenter;
	IS315_5313& model;
	HWND hwndInternal;
	IS315_5313::PortMonitorEntry entry;
};

#endif
