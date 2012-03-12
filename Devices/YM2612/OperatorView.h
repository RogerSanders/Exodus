#include "YM2612.h"
#ifndef __YM2612_OPERATORVIEW_H__
#define __YM2612_OPERATORVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class YM2612::OperatorView :public ViewBase
{
public:
	//Constructors
	OperatorView(YM2612* adevice, unsigned int achannelNo, unsigned int aoperatorNo);

	//Operator functions
	void SetTargetOperator(unsigned int achannelNo, unsigned int aoperatorNo);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	YM2612* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	unsigned int channelNo;
	unsigned int operatorNo;
};

#endif
