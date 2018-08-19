#ifndef __PORTMONITORVIEW_H__
#define __PORTMONITORVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "PortMonitorViewPresenter.h"
#include "315-5313/IS315_5313.h"

class PortMonitorView :public ViewBase
{
public:
	// Constructors
	PortMonitorView(IUIManager& uiManager, PortMonitorViewPresenter& presenter, IS315_5313& model);

protected:
	// Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	// Enumerations
	enum Columns
	{
		COLUMN_OPERATION,
		COLUMN_DATA,
		COLUMN_HCOUNTER,
		COLUMN_VCOUNTER,
		COLUMN_ACCESSTIME,
		COLUMN_SOURCE
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
	PortMonitorViewPresenter& _presenter;
	IS315_5313& _model;
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
