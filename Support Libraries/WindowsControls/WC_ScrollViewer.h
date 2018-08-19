#ifndef __WC_SCROLLVIEWER_H__
#define __WC_SCROLLVIEWER_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <map>

class WC_ScrollViewer
{
public:
	// Enumerations
	enum class WindowMessages :unsigned int;

	// Constants
	static const wchar_t* WindowClassName;

public:
	// Constructors
	WC_ScrollViewer(HINSTANCE moduleHandle, HWND hwnd);

	// Class registration
	static bool RegisterWindowClass(HINSTANCE moduleHandle);
	static bool UnregisterWindowClass(HINSTANCE moduleHandle);

	// Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	// Structures
	struct HostedWindowInfo;

private:
	// Message handlers
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

	// Hosted window methods
	void AddWindow(HWND windowHandle);
	void RemoveWindow(HWND windowHandle);

	// Sizing methods
	void UpdateScrollbarState(int newControlWidth, int newControlHeight);
	void UpdateHorizontalScrollPos(int newScrollPos);
	void UpdateVerticalScrollPos(int newScrollPos);

private:
	HINSTANCE _moduleHandle;
	HWND _hwnd;
	bool _scrollBarAutoHorizontal;
	bool _scrollBarAutoVertical;
	int _currentControlWidth;
	int _currentControlHeight;
	int _effectiveScrollWidth;
	int _effectiveScrollHeight;
	int _currentScrollPosX;
	int _currentScrollPosY;
	std::map<HWND, HostedWindowInfo> _hostedWindows;
};

#include "WC_ScrollViewer.inl"
#endif
