#include "Processor.h"
#ifndef __PROCESSOR_WATCHPOINTVIEW_H__
#define __PROCESSOR_WATCHPOINTVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "Watchpoint.h"

class Processor::WatchpointView :public ViewBase
{
public:
	//Constructors
	WatchpointView(Processor* adevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Watchpoint functions
	void UpdateWatchpoint(HWND hwnd, const Watchpoint& watchpoint, unsigned int addressCharWidth, unsigned int dataCharWidth);

private:
	Processor* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	int watchpointListIndex;
	Watchpoint watchpoint;
	WatchpointList watchpointsCopy;
};

#endif
