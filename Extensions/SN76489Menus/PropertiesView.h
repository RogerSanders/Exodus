#ifndef __PROPERTIESVIEW_H__
#define __PROPERTIESVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "PropertiesViewPresenter.h"
#include "SN76489/ISN76489.h"

class PropertiesView :public ViewBase
{
public:
	//Constructors
	PropertiesView(IUIManager& auiManager, PropertiesViewPresenter& apresenter, ISN76489& amodel);

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
	PropertiesViewPresenter& presenter;
	ISN76489& model;
	HINSTANCE assemblyHandle;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
};

#endif
