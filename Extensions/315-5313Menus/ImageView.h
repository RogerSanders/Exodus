#ifndef __IMAGEVIEW_H__
#define __IMAGEVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "ImageViewPresenter.h"
#include "315-5313/IS315_5313.h"

class ImageView :public ViewBase
{
public:
	//Constructors
	ImageView(IUIManager& auiManager, ImageViewPresenter& apresenter, IS315_5313& amodel);

protected:
	//Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PARENTNOTIFY(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//OpenGL window procedure
	static LRESULT CALLBACK WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//OpenGL window event handlers
	LRESULT msgRenderWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_KEYUP(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_KEYDOWN(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgRenderWM_SYSKEYDOWN(HWND hwnd, WPARAM wParam, LPARAM lParam);

	//Image update functions
	void UpdateImage();

	//Screen mode functions
	void UpdateOpenGLViewport();
	void SetFullScreen(HWND hwnd, bool fullScreenModeNew);

private:
	ImageViewPresenter& presenter;
	IS315_5313& model;
	HWND hwndOpenGL;
	HWND hwndStatusBar;
	HGLRC glrc;
	bool fullScreenMode;
	LONG fullScreenSavedWindowStyle;
	LONG fullScreenSavedWindowStyleEx;
	RECT fullScreenSavedWindowRect;
	double imageAspectRatio;
	unsigned int renderWindowWidth;
	unsigned int renderWindowHeight;
	bool videoFixedAspectRatioCached;
	bool videoShowStatusBarCached;
	unsigned int windowPendingClearCount;
	unsigned int renderPosHighlightColorIndex;
	LARGE_INTEGER lastFPSCounterUpdateTickCount;
	LARGE_INTEGER counterFrequency;
	unsigned int framesRenderedSinceLastFPSCounterUpdate;
	unsigned int lastRenderedFrameTokenCached;
};

#endif
