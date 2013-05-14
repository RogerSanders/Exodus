#include "Processor.h"
#ifndef __PROCESSOR_DISASSEMBLYVIEW_H__
#define __PROCESSOR_DISASSEMBLYVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class Processor::DisassemblyView :public ViewBase
{
public:
	//Constructors
	DisassemblyView(Processor* adevice);

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
		CTL_GRIDLIST = 100
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
	Processor* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	HWND hwndGridList;
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
