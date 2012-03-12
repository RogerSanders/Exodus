#include "VDP.h"
#ifndef __VDP_IMAGEVIEW_H__
#define __VDP_IMAGEVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class VDP::ImageView :public ViewBase
{
public:
	//Constructors
	ImageView(VDP* adevice);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KEYUP(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KEYDOWN(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Image update functions
	void UpdateImage();

private:
	VDP* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	HGLRC glrc;
};

#endif
