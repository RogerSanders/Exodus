#include "MemoryRead.h"
#ifndef __MEMORYREAD_MEMORYVIEWEROLDVIEW_H__
#define __MEMORYREAD_MEMORYVIEWEROLDVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class MemoryRead::MemoryViewerOldView :public ViewBase
{
public:
	//Constructors
	MemoryViewerOldView(MemoryRead* adevice);

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
	MemoryRead* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	unsigned int memoryLocation;
	unsigned int memorySize;
	unsigned int addressWidth;
	unsigned int displayWidth;
};

#endif
