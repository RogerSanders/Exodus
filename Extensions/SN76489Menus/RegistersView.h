#ifndef __REGISTERSVIEW_H__
#define __REGISTERSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "RegistersViewPresenter.h"
#include "SN76489/ISN76489.h"

class RegistersView :public ViewBase
{
public:
	//Constructors
	RegistersView(IUIManager& auiManager, RegistersViewPresenter& apresenter, ISN76489& amodel);
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
	INT_PTR msgWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Register locking functions
	bool ControlIDToDataID(int controlID, unsigned int& genericDataID);

private:
	RegistersViewPresenter& presenter;
	ISN76489& model;
	HINSTANCE assemblyHandle;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	COLORREF lockedColor;
	HBRUSH lockedBrush;
};

#endif
