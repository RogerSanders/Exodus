#ifndef __VRAMVIEW_H__
#define __VRAMVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "VRAMViewPresenter.h"
#include "315-5313/IS315_5313.h"

class VRAMView :public ViewBase
{
public:
	//Constructors
	VRAMView(IUIManager& auiManager, VRAMViewPresenter& apresenter, IS315_5313& amodel);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ERASEBKGND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Control dialog window procedure
	static INT_PTR CALLBACK WndProcControlDialogStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcControlDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Control dialog event handlers
	INT_PTR msgControlDialogWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam);
	INT_PTR msgControlDialogWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Render window procedure
	static LRESULT CALLBACK WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Render window event handlers
	LRESULT msgRenderWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSEMOVE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSELEAVE(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Render window helper methods
	bool UpdateRenderWindowSizeIfRequired();

	//Details dialog window procedure
	static INT_PTR CALLBACK WndProcDetailsStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcDetails(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Details dialog event handlers
	INT_PTR msgDetailsWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgDetailsWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgDetailsWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	//Enumerations
	enum BlockSize
	{
		BLOCKSIZE_AUTO,
		BLOCKSIZE_8X8,
		BLOCKSIZE_8X16
	};
	enum SelectedPalette
	{
		PALETTE_LINE1   = 0,
		PALETTE_LINE2   = 1,
		PALETTE_LINE3   = 2,
		PALETTE_LINE4   = 3,
		PALETTE_LOWHIGH = 4,
		PALETTE_HIGHLOW = 5
	};

	//Structures
	struct TileEntry
	{
		TileEntry()
		:value(0), r(0), g(0), b(0)
		{}
		unsigned int value;
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};

private:
	VRAMViewPresenter& presenter;
	IS315_5313& model;
	HGLRC glrc;
	bool shadow;
	bool highlight;
	unsigned char* buffer;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	HWND hwndLayoutGrid;
	HWND hwndControlDialog;
	HWND hwndRender;
	HWND hwndDetails;
	HWND hwndDetails16;
	bool detailsBlock16;
	bool detailsVisible;
	unsigned int blocksPerRenderRowSetting;
	unsigned int blocksPerRenderRow;
	unsigned int blockMagnificationFactorSetting;
	unsigned int blockMagnificationFactor;

	BlockSize blockSize;
	SelectedPalette selectedPalette;
	TileEntry tileDetails[16][8];
	unsigned int tileAddress;
	unsigned int tileNumber;
};

#endif
