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
	CallStackView(IUIManager& auiManager, CallStackViewPresenter& apresenter, IProcessor& amodel);

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
	CallStackViewPresenter& presenter;
	IProcessor& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	HWND hwndDataGrid;
	HWND hwndControlPanel;
	HFONT hfontHeader;
	HFONT hfontData;
};

#endif

//#ifndef __CALLSTACKVIEW_H__
//#define __CALLSTACKVIEW_H__
//#include "CallStackViewPresenter.h"
//#include "WindowsSupport/WindowsSupport.pkg"
//#include "DeviceInterface/DeviceInterface.pkg"
//#include "Processor/Processor.pkg"
//
//class CallStackView :public ViewBase
//{
//public:
//	//Constructors
//	CallStackView(IUIManager& auiManager, CallStackViewPresenter& apresenter, IProcessor& amodel);
//
//protected:
//	//Member window procedure
//	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
//
//private:
//	//Event handlers
//	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
//	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
//	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
//	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
//
//private:
//	CallStackViewPresenter& presenter;
//	IProcessor& model;
//};
//
//#endif
