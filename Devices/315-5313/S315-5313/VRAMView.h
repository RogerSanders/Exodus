#include "S315_5313.h"
#ifndef __S315_5313_VRAMVIEW_H__
#define __S315_5313_VRAMVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::VRAMView :public ViewBase
{
public:
	//Constructors
	VRAMView(S315_5313* adevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Render window procedure
	static LRESULT CALLBACK WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Render window event handlers
	LRESULT msgRenderWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSEMOVE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSELEAVE(HWND hwnd, WPARAM wParam, LPARAM lParam);

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
		PALETTE_LINE1	= 0,
		PALETTE_LINE2	= 1,
		PALETTE_LINE3	= 2,
		PALETTE_LINE4	= 3,
		PALETTE_LOWHIGH	= 4,
		PALETTE_HIGHLOW	= 5
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
	S315_5313* device;
	HGLRC glrc;
	bool shadow;
	bool highlight;
	unsigned char* buffer;
	HWND hwndRender;
	HWND hwndDetails;
	HWND hwndDetails16;
	bool detailsBlock16;
	bool detailsVisible;

	BlockSize blockSize;
	SelectedPalette selectedPalette;
	TileEntry tileDetails[16][8];
	unsigned int tileAddress;
	unsigned int tileNumber;
};

#endif
