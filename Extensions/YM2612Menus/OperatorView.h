#ifndef __OPERATORVIEW_H__
#define __OPERATORVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "OperatorViewPresenter.h"
#include "YM2612/IYM2612.h"

class OperatorView :public ViewBase
{
public:
	//Constructors
	OperatorView(IUIManager& auiManager, OperatorViewPresenter& apresenter, IYM2612& amodel, unsigned int achannelNo, unsigned int aoperatorNo);

	//Operator functions
	void SetTargetOperator(unsigned int achannelNo, unsigned int aoperatorNo);

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
	OperatorViewPresenter& presenter;
	IYM2612& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	unsigned int channelNo;
	unsigned int operatorNo;
};

#endif
