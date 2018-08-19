#ifndef __INPUTMAPPINGVIEW_H__
#define __INPUTMAPPINGVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "InputMappingViewPresenter.h"

class InputMappingView :public ViewBase
{
public:
	//Constructors
	InputMappingView(IUIManager& uiManager, InputMappingViewPresenter& presenter, ISystemGUIInterface& model);

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
	InputMappingViewPresenter& _presenter;
	ISystemGUIInterface& _model;
	unsigned int _inputDeviceListLastModifiedToken;
	IDevice* _selectedInputDevice;
};

#endif
