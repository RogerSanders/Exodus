#ifndef __PLANEVIEW_H__
#define __PLANEVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "PlaneViewPresenter.h"
#include "315-5313/IS315_5313.h"

class PlaneView :public ViewBase
{
public:
	//Constructors
	PlaneView(IUIManager& uiManager, PlaneViewPresenter& presenter, IS315_5313& model);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_HSCROLL(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_VSCROLL(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void UpdateControlPositions(HWND hwnd);
	void UpdateScrollbar(HWND scrollWindow, WPARAM wParam);

	//Render window procedure
	static LRESULT CALLBACK WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Render window event handlers
	LRESULT msgRenderWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_LBUTTONDOWN(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_KEYUP(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_KEYDOWN(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void UpdateRenderWindowSize(HWND hwnd, int renderWindowWidth, int renderWindowHeight);

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
	PlaneViewPresenter& _presenter;
	IS315_5313& _model;
	HGLRC _glrc;
	HWND _hwndRender;
	unsigned char* _buffer;
	bool _initializedDialog;
	std::wstring _previousText;
	unsigned int _currentControlFocus;
	int _currentRenderWindowWidth;
	int _currentRenderWindowHeight;
	float _imageZoomFactor;
	unsigned int _bufferWidth;
	unsigned int _bufferHeight;
	std::vector<float> _zoomSettings;

	SelectedLayer _selectedLayer;
	bool _displayScreen;
	bool _spriteBoundaries;

	bool _layerAScrollPlaneManual;
	unsigned int _layerAScrollPlaneWidth;
	unsigned int _layerAScrollPlaneHeight;
	bool _layerBScrollPlaneManual;
	unsigned int _layerBScrollPlaneWidth;
	unsigned int _layerBScrollPlaneHeight;
	bool _windowScrollPlaneManual;
	unsigned int _windowScrollPlaneWidth;
	unsigned int _windowScrollPlaneHeight;
	bool _spriteScrollPlaneManual;
	unsigned int _spriteScrollPlaneWidth;
	unsigned int _spriteScrollPlaneHeight;

	bool _layerAMappingBaseManual;
	bool _layerBMappingBaseManual;
	bool _windowMappingBaseManual;
	bool _spriteMappingBaseManual;
	unsigned int _layerAMappingBase;
	unsigned int _layerBMappingBase;
	unsigned int _windowMappingBase;
	unsigned int _spriteMappingBase;
	bool _layerAPatternBaseManual;
	bool _layerBPatternBaseManual;
	bool _windowPatternBaseManual;
	bool _spritePatternBaseManual;
	unsigned int _layerAPatternBase;
	unsigned int _layerBPatternBase;
	unsigned int _windowPatternBase;
	unsigned int _spritePatternBase;
};

#endif
