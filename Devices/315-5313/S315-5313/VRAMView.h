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
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Render window procedure
	static LRESULT CALLBACK WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Render window event handlers
	LRESULT msgRenderWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSEMOVE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSELEAVE(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Details dialog window procedure
	static INT_PTR CALLBACK WndProcDetailsStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	INT_PTR WndProcDetails(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Details dialog event handlers
	INT_PTR msgDetailsWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgDetailsWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
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
	struct PaletteIndex
	{
		PaletteIndex()
		:line(0), entry(0), shadow(false), highlight(false)
		{}

		unsigned int line;
		unsigned int entry;
		bool shadow;
		bool highlight;
	};
	struct Color
	{
		unsigned int r;
		unsigned int g;
		unsigned int b;
		bool shadow;
		bool highlight;

		//This is a general conversion function. Takes a number within a given range, and
		//scales it to the corresponding position in a target range. We use it here to do
		//accurate color conversion from Mega Drive 9-bit color codes into any required
		//format.
		template<class S, class T> inline T Convert(S number, S minSource, S maxSource, T minTarget, T maxTarget) const
		{
			return (T)((float)(number - minSource) * ((float)(maxTarget - minTarget) / (float)(maxSource - minSource))) + minTarget;
		}

		inline unsigned char GetR8Bit() const
		{
			unsigned char maxTarget = (!shadow || (highlight && shadow))? 255: 128;
			unsigned char minTarget = (!highlight || (highlight && shadow))? 0: 128;
			return Convert(r, 0u, 7u, minTarget, maxTarget);
		}

		inline unsigned char GetG8Bit() const
		{
			unsigned char maxTarget = (!shadow || (highlight && shadow))? 255: 128;
			unsigned char minTarget = (!highlight || (highlight && shadow))? 0: 128;
			return Convert(g, 0u, 7u, minTarget, maxTarget);
		}

		inline unsigned char GetB8Bit() const
		{
			unsigned char maxTarget = (!shadow || (highlight && shadow))? 255: 128;
			unsigned char minTarget = (!highlight || (highlight && shadow))? 0: 128;
			return Convert(b, 0u, 7u, minTarget, maxTarget);
		}

		inline float GetRFloat() const
		{
			float maxTarget = (!shadow || (highlight && shadow))? 1.0f: 0.5f;
			float minTarget = (!highlight || (highlight && shadow))? 0.0f: 0.5f;
			return Convert(r, 0u, 7u, minTarget, maxTarget);
		}

		inline float GetGFloat() const
		{
			float maxTarget = (!shadow || (highlight && shadow))? 1.0f: 0.5f;
			float minTarget = (!highlight || (highlight && shadow))? 0.0f: 0.5f;
			return Convert(g, 0u, 7u, minTarget, maxTarget);
		}

		inline float GetBFloat() const
		{
			float maxTarget = (!shadow || (highlight && shadow))? 1.0f: 0.5f;
			float minTarget = (!highlight || (highlight && shadow))? 0.0f: 0.5f;
			return Convert(b, 0u, 7u, minTarget, maxTarget);
		}
	};

private:
	S315_5313* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	HGLRC glrc;
	bool shadow;
	bool highlight;
	unsigned char* buffer;
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
