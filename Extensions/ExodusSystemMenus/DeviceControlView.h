#ifndef __DEVICECONTROLVIEW_H__
#define __DEVICECONTROLVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExtensionInterface/ExtensionInterface.pkg"
#include "DeviceControlViewPresenter.h"
#include <list>

class DeviceControlView :public ViewBase
{
public:
	//Constructors
	DeviceControlView(IUIManager& auiManager, DeviceControlViewPresenter& apresenter, ISystemGUIInterface& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	DeviceControlViewPresenter& presenter;
	ISystemGUIInterface& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	int deviceListIndex;
	unsigned int systemStep;
	unsigned int deviceStep;
	std::list<IDevice*> deviceList;
};

#endif
