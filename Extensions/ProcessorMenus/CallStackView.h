#ifndef __CALLSTACKVIEW_H__
#define __CALLSTACKVIEW_H__
#include "CallStackViewPresenter.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "Processor/Processor.pkg"

class CallStackView :public ViewBase
{
public:
	//Constructors
	CallStackView(IUIManager& uiManager, CallStackViewPresenter& presenter, IProcessor& model);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Enumerations
	enum Columns
	{
		COLUMN_SOURCE,
		COLUMN_TARGET,
		COLUMN_RETURN,
		COLUMN_DISASSEMBLY
	};
	enum ControlIDList
	{
		CTL_DATAGRID = 100
	};

	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Panel dialog window procedure
	static INT_PTR CALLBACK WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Panel dialog event handlers
	INT_PTR msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPanelWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPanelWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	CallStackViewPresenter& _presenter;
	IProcessor& _model;
	bool _initializedDialog;
	unsigned int _currentControlFocus;
	HWND _hwndDataGrid;
	HWND _hwndControlPanel;
	HFONT _hfontHeader;
	HFONT _hfontData;
	unsigned int _logLastModifiedToken;
};

#endif
