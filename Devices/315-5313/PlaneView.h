#include "S315_5313.h"
#ifndef __S315_5313_PLANEVIEW_H__
#define __S315_5313_PLANEVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::PlaneView :public ViewBase
{
public:
	//Constructors
	PlaneView(S315_5313* adevice);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_HSCROLL(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_VSCROLL(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Render window procedure
	static LRESULT CALLBACK WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Render window event handlers
	LRESULT msgRenderWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_LBUTTONDOWN(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_KEYUP(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_KEYDOWN(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Render helper methods
	void GetScrollPlanePaletteInfo(const std::vector<unsigned char>& vramData, unsigned int mappingBaseAddress, unsigned int patternBaseAddress, unsigned int planeWidth, unsigned int planeHeight, unsigned int xpos, unsigned int ypos, bool interlaceMode2Active, unsigned int& paletteRow, unsigned int& paletteIndex) const;
	void GetScrollPlaneHScrollData(const std::vector<unsigned char>& vramData, unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, bool layerA, unsigned int& layerHscrollPatternDisplacement, unsigned int& layerHscrollMappingDisplacement) const;

private:
	//Enumerations
	enum SelectedLayer
	{
		SELECTEDLAYER_LAYERA,
		SELECTEDLAYER_LAYERB,
		SELECTEDLAYER_WINDOW,
		SELECTEDLAYER_SPRITES
	};

	//Structures
	struct ScreenBoundaryPrimitive
	{
		ScreenBoundaryPrimitive(unsigned int apixelPosXBegin, unsigned int apixelPosXEnd, unsigned int apixelPosYBegin, unsigned int apixelPosYEnd, bool aprimitiveIsPolygon = false, bool aprimitiveIsScreenBoundary = true)
		:pixelPosXBegin(apixelPosXBegin), pixelPosXEnd(apixelPosXEnd), pixelPosYBegin(apixelPosYBegin), pixelPosYEnd(apixelPosYEnd), primitiveIsPolygon(aprimitiveIsPolygon), primitiveIsScreenBoundary(aprimitiveIsScreenBoundary)
		{}

		bool primitiveIsPolygon;
		bool primitiveIsScreenBoundary;
		unsigned int pixelPosXBegin;
		unsigned int pixelPosXEnd;
		unsigned int pixelPosYBegin;
		unsigned int pixelPosYEnd;
	};

private:
	S315_5313* device;
	HGLRC glrc;
	HWND hwndRender;
	unsigned char* buffer;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;

	SelectedLayer selectedLayer;
	bool displayScreen;
	bool spriteBoundaries;

	bool layerAScrollPlaneManual;
	unsigned int layerAScrollPlaneWidth;
	unsigned int layerAScrollPlaneHeight;
	bool layerBScrollPlaneManual;
	unsigned int layerBScrollPlaneWidth;
	unsigned int layerBScrollPlaneHeight;
	bool windowScrollPlaneManual;
	unsigned int windowScrollPlaneWidth;
	unsigned int windowScrollPlaneHeight;
	bool spriteScrollPlaneManual;
	unsigned int spriteScrollPlaneWidth;
	unsigned int spriteScrollPlaneHeight;

	bool layerAMappingBaseManual;
	bool layerBMappingBaseManual;
	bool windowMappingBaseManual;
	bool spriteMappingBaseManual;
	unsigned int layerAMappingBase;
	unsigned int layerBMappingBase;
	unsigned int windowMappingBase;
	unsigned int spriteMappingBase;
	bool layerAPatternBaseManual;
	bool layerBPatternBaseManual;
	bool windowPatternBaseManual;
	bool spritePatternBaseManual;
	unsigned int layerAPatternBase;
	unsigned int layerBPatternBase;
	unsigned int windowPatternBase;
	unsigned int spritePatternBase;
};

#endif
