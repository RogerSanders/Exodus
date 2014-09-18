#ifndef __MEMORYEDITORVIEW_H__
#define __MEMORYEDITORVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "MemoryEditorViewPresenter.h"
#include "Memory/MemoryRead.h"

class MemoryEditorView :public ViewBase
{
public:
	//Constructors
	MemoryEditorView(IUIManager& auiManager, MemoryEditorViewPresenter& apresenter, IMemory& amodel);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFOCUS(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KILLFOCUS(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	//Constants
	static const long long HEXEDITCONTROL = 100;

private:
	MemoryEditorViewPresenter& presenter;
	IMemory& model;
	HWND hwndMem;
};

#endif
