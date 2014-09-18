#ifndef __REGISTERSVIEW_H__
#define __REGISTERSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "RegistersViewPresenter.h"
#include "Z80/Z80.h"

class RegistersView :public ViewBase
{
public:
	//Constructors
	RegistersView(IUIManager& auiManager, RegistersViewPresenter& apresenter, IZ80& amodel);
	~RegistersView();

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Changed register functions
	bool RegisterContentsChanged(int controlID);

private:
	RegistersViewPresenter& presenter;
	IZ80& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	COLORREF changedColor;
	COLORREF backgroundColor;
	HBRUSH backgroundBrush;
	unsigned int systemPausedToggleCounterCached;
};

#endif
