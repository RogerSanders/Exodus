#ifndef __CALLSTACKVIEW_H__
#define __CALLSTACKVIEW_H__
#include "CallStackViewPresenter.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Processor/Processor.pkg"

class CallStackView :public ViewBase
{
public:
	//Constructors
	CallStackView(IUIManager& auiManager, CallStackViewPresenter& apresenter, IProcessor& amodel);

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
	CallStackViewPresenter& presenter;
	IProcessor& model;
};

#endif
