#ifndef __SETTINGSVIEW_H__
#define __SETTINGSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "SettingsViewPresenter.h"
#include "315-5313/S315_5313.h"

class SettingsView :public ViewBase
{
public:
	//Constructors
	SettingsView(IUIManager& auiManager, SettingsViewPresenter& apresenter, IS315_5313& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	SettingsViewPresenter& presenter;
	IS315_5313& model;
};

#endif
