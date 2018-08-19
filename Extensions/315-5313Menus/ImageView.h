#ifndef __IMAGEVIEW_H__
#define __IMAGEVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "ImageViewPresenter.h"
#include "315-5313/IS315_5313.h"

class ImageView :public ViewBase
{
public:
	//Constructors
	ImageView(IUIManager& uiManager, ImageViewPresenter& presenter, IS315_5313& model);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NCHITTEST(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Child window procedure
	LRESULT WndProcChildGroup(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Child event handlers
	LRESULT msgChildWM_KEYUP(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgChildWM_KEYDOWN(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgChildWM_SYSKEYDOWN(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//OpenGL window procedure
	LRESULT WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//OpenGL window event handlers
	LRESULT msgRenderWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSEMOVE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_MOUSELEAVE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void HidePixelInfoWindow();

	//Image update functions
	void UpdateImage();

	//Screen mode functions
	void UpdateOpenGLViewport();
	void UpdateRenderWindowSize();
	void SetFullScreen(bool fullScreenModeNew);

	//Pixel info dialog window procedure
	INT_PTR WndProcPixelInfo(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Pixel info dialog event handlers
	INT_PTR msgPixelInfoWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgPixelInfoWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	ImageViewPresenter& _presenter;
	IS315_5313& _model;
	HWND _hwndMain;
	HWND _hwndChildGroup;
	HWND _hwndOpenGL;
	HWND _hwndStatusBar;
	HWND _savedParent;
	int _savedWindowPosX;
	int _savedWindowPosY;
	int _savedWindowWidth;
	int _savedWindowHeight;

	HGLRC _glrc;
	bool _fullScreenMode;
	LONG _fullScreenSavedWindowStyle;
	LONG _fullScreenSavedWindowStyleEx;
	double _imageAspectRatio;
	int _renderWindowWidth;
	int _renderWindowHeight;
	bool _videoFixedAspectRatioCached;
	bool _videoShowStatusBarCached;
	unsigned int _windowPendingClearCount;
	unsigned int _renderPosHighlightColorIndex;
	LARGE_INTEGER _lastFPSCounterUpdateTickCount;
	LARGE_INTEGER _counterFrequency;
	unsigned int _framesRenderedSinceLastFPSCounterUpdate;
	unsigned int _lastRenderedFrameTokenCached;

	int _imageRegionPosX;
	int _imageRegionPosY;
	int _imageRegionWidth;
	int _imageRegionHeight;
	HWND _hwndPixelInfo;
	bool _pixelInfoVisible;
	int _pixelInfoTargetBufferPosX;
	int _pixelInfoTargetBufferPosY;
};

#endif
