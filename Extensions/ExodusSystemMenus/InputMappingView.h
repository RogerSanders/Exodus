#ifndef __INPUTMAPPINGVIEW_H__
#define __INPUTMAPPINGVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "InputMappingViewPresenter.h"

class InputMappingView :public ViewBase
{
public:
	//Constructors
	InputMappingView(IUIManager& auiManager, InputMappingViewPresenter& apresenter, ISystemGUIInterface& amodel);

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
	InputMappingViewPresenter& presenter;
	ISystemGUIInterface& model;
	unsigned int inputDeviceListLastModifiedToken;
	IDevice* selectedInputDevice;
};

#endif
