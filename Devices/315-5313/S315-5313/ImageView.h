#include "S315_5313.h"
#ifndef __S315_5313_IMAGEVIEW_H__
#define __S315_5313_IMAGEVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::ImageView :public ViewBase
{
public:
	//Constructors
	ImageView(S315_5313* adevice);

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
	S315_5313* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	HGLRC glrc;
};

#endif
