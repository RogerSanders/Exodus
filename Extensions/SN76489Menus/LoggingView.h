#ifndef __LOGGINGVIEW_H__
#define __LOGGINGVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "LoggingViewPresenter.h"
#include "SN76489/ISN76489.h"

class LoggingView :public ViewBase
{
public:
	//Constructors
	LoggingView(IUIManager& auiManager, LoggingViewPresenter& apresenter, ISN76489& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	LoggingViewPresenter& presenter;
	ISN76489& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
};

#endif
