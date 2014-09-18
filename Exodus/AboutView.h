#ifndef __ABOUTVIEW_H__
#define __ABOUTVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "AboutViewPresenter.h"
#include "ExodusInterface.h"

class AboutView :public ViewBase
{
public:
	//Constructors
	AboutView(IUIManager& auiManager, AboutViewPresenter& apresenter, ExodusInterface& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam);
	INT_PTR msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam);
	INT_PTR msgWM_PARENTNOTIFY(HWND hwnd, WPARAM wparam, LPARAM lparam);
	INT_PTR msgWM_NOTIFY(HWND hwnd, WPARAM wparam, LPARAM lparam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	AboutViewPresenter& presenter;
	ExodusInterface& model;
	HFONT fontHandle;
};

#endif
