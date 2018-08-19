#ifndef __TRACEVIEW_H__
#define __TRACEVIEW_H__
#include "TraceViewPresenter.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "Processor/Processor.pkg"

class TraceView :public ViewBase
{
public:
	// Constructors
	TraceView(IUIManager& uiManager, TraceViewPresenter& presenter, IProcessor& model);

protected:
	// Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	// Enumerations
	enum Columns
	{
		COLUMN_ADDRESS,
		COLUMN_DISASSEMBLY
	};
	enum ControlIDList
	{
		CTL_DATAGRID = 100
	};

	// Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam);

	// Panel dialog window procedure
	static INT_PTR CALLBACK WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// Panel dialog event handlers
	INT_PTR msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPanelWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPanelWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	TraceViewPresenter& _presenter;
	IProcessor& _model;
	bool _initializedDialog;
	std::wstring _previousText;
	unsigned int _currentControlFocus;
	HWND _hwndDataGrid;
	HWND _hwndControlPanel;
	HFONT _hfontHeader;
	HFONT _hfontData;
	unsigned int _logLastModifiedToken;
};

#endif
