#ifndef __SPRITELISTVIEW_H__
#define __SPRITELISTVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "SpriteListViewPresenter.h"
#include "315-5313/IS315_5313.h"

class SpriteListView :public ViewBase
{
public:
	//Constructors
	SpriteListView(IUIManager& auiManager, SpriteListViewPresenter& apresenter, IS315_5313& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	SpriteListViewPresenter& presenter;
	IS315_5313& model;
};

#endif
