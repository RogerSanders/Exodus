#ifndef __VRAMVIEW_H__
#define __VRAMVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "Image/Image.pkg"
#include "VRAMViewPresenter.h"
#include "315-5313/IS315_5313.h"

class VRAMView :public ViewBase
{
public:
	// Constructors
	VRAMView(IUIManager& uiManager, VRAMViewPresenter& presenter, IS315_5313& model);

protected:
	// Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	// Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ERASEBKGND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam);

	// Control dialog window procedure
	INT_PTR WndProcControlDialog1(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcControlDialog2(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// Control dialog event handlers
	INT_PTR msgControlDialog1WM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam);
	INT_PTR msgControlDialog1WM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgControlDialog2WM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam);
	INT_PTR msgControlDialog2WM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	// Render window procedure
	LRESULT WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// Render window event handlers
	LRESULT msgRenderWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSEMOVE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSELEAVE(HWND hwnd, WPARAM wParam, LPARAM lParam);

	// Render window helper methods
	void UpdateRenderWindowSizeIfRequired();

	// Details dialog window procedure
	INT_PTR WndProcDetails(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// Details dialog event handlers
	INT_PTR msgDetailsWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgDetailsWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	// Enumerations
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

	// Structures
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
	VRAMViewPresenter& _presenter;
	IS315_5313& _model;
	bool _shadow;
	bool _highlight;
	bool _initializedDialog;
	std::wstring _previousText;
	unsigned int _currentControlFocus;
	Image _vramImage;

	HWND _hwndLayoutGrid;
	HWND _hwndScrollViewer;
	HWND _hwndControlDialog1;
	HWND _hwndControlDialog2;
	HWND _hwndRender;
	HWND _hwndDetails;
	HWND _hwndDetails16;
	bool _detailsBlock16;
	bool _detailsVisible;
	unsigned int _blocksPerRenderRowSetting;
	unsigned int _blocksPerRenderRow;
	unsigned int _blockMagnificationFactorSetting;
	unsigned int _blockMagnificationFactor;
	int _renderWindowAvailableWidth;
	int _renderWindowAvailableHeight;

	BlockSize _blockSize;
	SelectedPalette _selectedPalette;
	TileEntry _tileDetails[16][8];
	unsigned int _tileAddress;
	unsigned int _tileNumber;
};

#endif
