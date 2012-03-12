#include "S315_5313.h"
#ifndef __S315_5313_PALETTEVIEW_H__
#define __S315_5313_PALETTEVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::PaletteView :public ViewBase
{
public:
	//Constructors
	PaletteView(S315_5313* adevice);

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
	S315_5313* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	HGLRC glrc;
};

#endif
