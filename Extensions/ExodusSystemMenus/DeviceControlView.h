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
	DeviceControlView(IUIManager& uiManager, DeviceControlViewPresenter& presenter, ISystemGUIInterface& model);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	DeviceControlViewPresenter& _presenter;
	ISystemGUIInterface& _model;
	bool _initializedDialog;
	std::wstring _previousText;
	unsigned int _currentControlFocus;
	int _deviceListIndex;
	unsigned int _systemStep;
	unsigned int _deviceStep;
	std::list<IDevice*> _deviceList;
};

#endif
