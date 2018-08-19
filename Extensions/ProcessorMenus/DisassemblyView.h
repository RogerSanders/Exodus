#ifndef __DISASSEMBLYVIEW_H__
#define __DISASSEMBLYVIEW_H__
#include "DisassemblyViewPresenter.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "Processor/Processor.pkg"

class DisassemblyView :public ViewBase
{
public:
	//Constructors
	DisassemblyView(IUIManager& uiManager, DisassemblyViewPresenter& presenter, IProcessor& model);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Enumerations
	enum Columns
	{
		COLUMN_ADDRESS,
		COLUMN_OPCODE,
		COLUMN_ARGS,
		COLUMN_COMMENT,
		COLUMN_BINARY
	};
	enum ControlIDList
	{
		CTL_DATAGRID = 100
	};

	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Panel dialog window procedure
	static INT_PTR CALLBACK WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Panel dialog event handlers
	INT_PTR msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPanelWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPanelWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPanelWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Disassembly functions
	void UpdateDisassembly();
	void ToggleBreakpointStateAtRow(unsigned int visibleRowNo, bool toggleEnableState);
	void ToggleBreakpointStateAtAddress(unsigned int pcLocation, bool toggleEnableState);

private:
	DisassemblyViewPresenter& _presenter;
	IProcessor& _model;
	bool _initializedDialog;
	std::wstring _previousText;
	unsigned int _currentControlFocus;
	HWND _hwndDataGrid;
	HWND _hwndControlPanel;
	HFONT _hfontHeader;
	HFONT _hfontData;
	unsigned int _visibleRows;
	bool _track;
	bool _forcePCSync;
	unsigned int _currentPCLocation;
	unsigned int _startLocation;
	unsigned int _endLocation;
	unsigned int _firstVisibleValueLocation;
	unsigned int _lastVisibleValueLocation;
	unsigned int _readAbove;
	unsigned int _lastBufferedOpcodeSize;
	unsigned int _firstVisibleOpcodeSize;
};

#endif
