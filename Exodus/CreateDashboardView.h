#ifndef __CREATEDASHBOARDVIEW_H__
#define __CREATEDASHBOARDVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "CreateDashboardViewPresenter.h"
#include "ExodusInterface.h"

class CreateDashboardView :public ViewBase
{
public:
	//Constructors
	CreateDashboardView(IUIManager& auiManager, CreateDashboardViewPresenter& apresenter, ExodusInterface& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	CreateDashboardViewPresenter& presenter;
	ExodusInterface& model;
};

#endif
