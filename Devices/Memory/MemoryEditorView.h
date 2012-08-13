#include "MemoryRead.h"
#ifndef __MEMORYREAD_MEMORYEDITORVIEW_H__
#define __MEMORYREAD_MEMORYEDITORVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class MemoryRead::MemoryEditorView :public ViewBase
{
public:
	//Constructors
	MemoryEditorView(MemoryRead* adevice);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFOCUS(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KILLFOCUS(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	static const long long HEXEDITCONTROL = 100;

	MemoryRead* device;
	HWND hwndMem;
};

#endif
