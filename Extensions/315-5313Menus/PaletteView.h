#ifndef __PALETTEVIEW_H__
#define __PALETTEVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Image/Image.pkg"
#include "PaletteViewPresenter.h"
#include "315-5313/IS315_5313.h"

class PaletteView :public ViewBase
{
public:
	//Constructors
	PaletteView(IUIManager& auiManager, PaletteViewPresenter& apresenter, IS315_5313& amodel);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	PaletteViewPresenter& presenter;
	IS315_5313& model;
	Image paletteImage;
};

#endif
