#ifndef __ABOUTVIEW_H__
#define __ABOUTVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "AboutViewPresenter.h"
#include "ExodusInterface.h"

class AboutView :public ViewBase
{
public:
	//Constructors
	AboutView(IUIManager& auiManager, AboutViewPresenter& apresenter, ExodusInterface& amodel);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ERASEBKGND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Panel dialog window procedure
	static INT_PTR CALLBACK WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Panel dialog event handlers
	INT_PTR msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPanelWM_NOTIFY(HWND hwnd, WPARAM wparam, LPARAM lparam);
	INT_PTR msgPanelWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam);

private:
	AboutViewPresenter& presenter;
	ExodusInterface& model;
	HFONT fontHandle;
	HWND hwndLayoutGrid;
	HWND hwndNestedDialog;
	int layoutGridBorderWidth;
	int layoutGridBorderHeight;
};

#endif
