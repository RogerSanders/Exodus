#ifndef __OPERATORVIEW_H__
#define __OPERATORVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "OperatorViewPresenter.h"
#include "YM2612/IYM2612.h"

class OperatorView :public ViewBase
{
public:
	// Constructors
	OperatorView(IUIManager& uiManager, OperatorViewPresenter& presenter, IYM2612& model, unsigned int channelNo, unsigned int operatorNo);

	// Operator functions
	void SetTargetOperator(unsigned int channelNo, unsigned int operatorNo);

protected:
	// Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	// Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	OperatorViewPresenter& _presenter;
	IYM2612& _model;
	bool _initializedDialog;
	std::wstring _previousText;
	unsigned int _currentControlFocus;
	unsigned int _channelNo;
	unsigned int _operatorNo;
};

#endif
