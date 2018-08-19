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
	SpriteListView(IUIManager& uiManager, SpriteListViewPresenter& presenter, IS315_5313& model);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Enumerations
	enum Columns
	{
		COLUMN_NO,
		COLUMN_XPOS,
		COLUMN_YPOS,
		COLUMN_TILES,
		COLUMN_LINK,
		COLUMN_HSIZE,
		COLUMN_VSIZE,
		COLUMN_P,
		COLUMN_HFLIP,
		COLUMN_VFLIP,
		COLUMN_C,
		COLUMN_RAW0,
		COLUMN_RAW1,
		COLUMN_RAW2,
		COLUMN_RAW3
	};

	//Constants
	static const long long DataListControlID = 100;

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	SpriteListViewPresenter& _presenter;
	IS315_5313& _model;
	HFONT _fontHandle;
	HWND _hwndLayoutGrid;
	HWND _hwndDataGrid;
};

#endif
