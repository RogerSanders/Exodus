#ifndef __WC_SCROLLVIEWER_H__
#define __WC_SCROLLVIEWER_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <map>

class WC_ScrollViewer
{
public:
	//Constants
	static const wchar_t* windowClassName;

	//Enumerations
	enum class WindowMessages :unsigned int;

public:
	//Constructors
	WC_ScrollViewer(HINSTANCE amoduleHandle, HWND ahwnd);

	//Class registration
	static bool RegisterWindowClass(HINSTANCE amoduleHandle);
	static bool UnregisterWindowClass(HINSTANCE amoduleHandle);

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//Structures
	struct HostedWindowInfo;

private:
	//Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NCHITTEST(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NOTIFY(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_BOUNCE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_HSCROLL(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_VSCROLL(WPARAM wParam, LPARAM lParam);
	LRESULT msgSCROLL_ADDWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgSCROLL_REMOVEWINDOW(WPARAM wParam, LPARAM lParam);

	//Hosted window methods
	void AddWindow(HWND windowHandle);
	void RemoveWindow(HWND windowHandle);

	//Sizing methods
	void UpdateScrollbarState(int newControlWidth, int newControlHeight);
	void UpdateHorizontalScrollPos(int newScrollPos);
	void UpdateVerticalScrollPos(int newScrollPos);

private:
	HINSTANCE moduleHandle;
	HWND hwnd;
	bool scrollBarAutoHorizontal;
	bool scrollBarAutoVertical;
	int currentControlWidth;
	int currentControlHeight;
	int effectiveScrollWidth;
	int effectiveScrollHeight;
	int currentScrollPosX;
	int currentScrollPosY;
	std::map<HWND, HostedWindowInfo> hostedWindows;
};

#include "WC_ScrollViewer.inl"
#endif
