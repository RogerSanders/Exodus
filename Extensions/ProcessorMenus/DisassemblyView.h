#ifndef __DISASSEMBLYVIEW_H__
#define __DISASSEMBLYVIEW_H__
#include "DisassemblyViewPresenter.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Processor/Processor.pkg"

class DisassemblyView :public ViewBase
{
public:
	//Constructors
	DisassemblyView(IUIManager& auiManager, DisassemblyViewPresenter& apresenter, IProcessor& amodel);

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
	LRESULT msgWM_PARENTNOTIFY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
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
	DisassemblyViewPresenter& presenter;
	IProcessor& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	HWND hwndDataGrid;
	HWND hwndControlPanel;
	HFONT hfontHeader;
	HFONT hfontData;
	unsigned int visibleRows;
	bool track;
	bool forcePCSync;
	unsigned int currentPCLocation;
	unsigned int startLocation;
	unsigned int endLocation;
	unsigned int firstVisibleValueLocation;
	unsigned int lastVisibleValueLocation;
	unsigned int readAbove;
	unsigned int lastBufferedOpcodeSize;
	unsigned int firstVisibleOpcodeSize;
};

#endif
