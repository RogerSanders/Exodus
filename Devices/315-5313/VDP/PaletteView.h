#include "VDP.h"
#ifndef __VDP_PALETTEVIEW_H__
#define __VDP_PALETTEVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class VDP::PaletteView :public ViewBase
{
public:
	//Constructors
	PaletteView(VDP* adevice);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Palette update functions
	void UpdatePalette();

private:
	VDP* device;
	HGLRC glrc;
};

#endif
