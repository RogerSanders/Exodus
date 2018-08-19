#include "WC_ScrollViewer.h"
#include <set>
#include <cmath>

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* WC_ScrollViewer::WindowClassName = L"EX_ScrollViewer";

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WC_ScrollViewer::WC_ScrollViewer(HINSTANCE moduleHandle, HWND hwnd)
:_moduleHandle(moduleHandle), _hwnd(hwnd)
{
	//Set the initial state of the control to the default settings
	//##TODO## Provide messages to change the auto scrollbar visibility mode. We actually
	//want an enumeration here, with "Hidden", "Visible", and "Auto" modes for each axis.
	_scrollBarAutoHorizontal = true;
	_scrollBarAutoVertical = true;

	//Set the recorded current size of the control to 0
	_currentControlWidth = 0;
	_currentControlHeight = 0;
	_effectiveScrollWidth = 0;
	_effectiveScrollHeight = 0;
	_currentScrollPosX = 0;
	_currentScrollPosY = 0;
}

//----------------------------------------------------------------------------------------
//Class registration
//----------------------------------------------------------------------------------------
bool WC_ScrollViewer::RegisterWindowClass(HINSTANCE moduleHandle)
{
	//Attempt to register the window class for this control, and return the result to the
	//caller.
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = moduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = WindowClassName;
	wc.hIconSm       = NULL;
	ATOM registerClassExReturn = RegisterClassEx(&wc);
	return (registerClassExReturn != 0);
}

//----------------------------------------------------------------------------------------
bool WC_ScrollViewer::UnregisterWindowClass(HINSTANCE moduleHandle)
{
	//Attempt to unregister the window class for this control, and return the result to
	//the caller.
	BOOL unregisterClassReturn = UnregisterClass(WindowClassName, moduleHandle);
	return (unregisterClassReturn != 0);
}

//----------------------------------------------------------------------------------------
//Message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK WC_ScrollViewer::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	default:{
		WC_ScrollViewer* object = (WC_ScrollViewer*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (object != 0)
		{
			return object->WndProcPrivate(message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		WC_ScrollViewer* object = new WC_ScrollViewer((HINSTANCE)createStruct->lpCreateParams, hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->WndProcPrivate(message, wParam, lParam);}
	case WM_DESTROY:{
		WC_ScrollViewer* object = (WC_ScrollViewer*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (object != 0)
		{
			LPARAM result = object->WndProcPrivate(message, wParam, lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
			delete object;
			return result;
		}
		break;}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return msgWM_CREATE(wParam, lParam);
	case WM_SIZE:
		return msgWM_SIZE(wParam, lParam);
	case WM_ERASEBKGND:
		return msgWM_ERASEBKGND(wParam, lParam);
	case WM_NCHITTEST:
		return msgWM_NCHITTEST(wParam, lParam);
	case WM_COMMAND:
		return msgWM_COMMAND(wParam, lParam);
	case WM_NOTIFY:
		return msgWM_NOTIFY(wParam, lParam);
	case WM_BOUNCE:
		return msgWM_BOUNCE(wParam, lParam);
	case WM_HSCROLL:
		return msgWM_HSCROLL(wParam, lParam);
	case WM_VSCROLL:
		return msgWM_VSCROLL(wParam, lParam);
	case (unsigned int)WindowMessages::AddWindow:
		return msgSCROLL_ADDWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::RemoveWindow:
		return msgSCROLL_REMOVEWINDOW(wParam, lParam);
	}

	return DefWindowProc(_hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(_hwnd, &rect);
	int newClientWidth = (int)(rect.right - rect.left);
	int newClientHeight = (int)(rect.bottom - rect.top);

	//Process the initial size of the window
	UpdateScrollbarState(newClientWidth, newClientHeight);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(_hwnd, &rect);
	int newClientWidth = (int)(rect.right - rect.left);
	int newClientHeight = (int)(rect.bottom - rect.top);

	//Handle this size changed event
	if ((_currentControlWidth != newClientWidth) || (_currentControlHeight != newClientHeight))
	{
		UpdateScrollbarState(newClientWidth, newClientHeight);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam)
{
	//If the WS_EX_TRANSPARENT window style has been applied to our control, since we want
	//this control to essentially have a transparent background, we don't perform any
	//operation when a background erase is requested. Note that this requires the
	//containing window to use the WS_EX_COMPOSITED style in order to achieve the desired
	//effect. If the WS_EX_TRANSPARENT style has not been specified, we pass this message
	//on to DefWindowProc.
	if (((unsigned int)GetWindowLongPtr(_hwnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT) != 0)
	{
		return TRUE;
	}
	return DefWindowProc(_hwnd, WM_ERASEBKGND, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgWM_NCHITTEST(WPARAM wParam, LPARAM lParam)
{
	//Make this control transparent in the client area for hit testing
	LRESULT result = DefWindowProc(_hwnd, WM_NCHITTEST, wParam, lParam);
	return (result == HTCLIENT)? HTTRANSPARENT: result;
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgWM_COMMAND(WPARAM wParam, LPARAM lParam)
{
	//Forward this message directly to the parent window
	return SendMessage(GetAncestor(_hwnd, GA_PARENT), WM_COMMAND, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgWM_NOTIFY(WPARAM wParam, LPARAM lParam)
{
	//Forward this message directly to the parent window
	return SendMessage(GetAncestor(_hwnd, GA_PARENT), WM_NOTIFY, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgWM_BOUNCE(WPARAM wParam, LPARAM lParam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lParam;
	if (bounceMessage->uMsg == WM_SIZE)
	{
		//If the size of a hosted window has changed, update the scrollbar state.
		std::map<HWND, HostedWindowInfo>::iterator hostedWindowsIterator = _hostedWindows.find(bounceMessage->hwnd);
		if (hostedWindowsIterator != _hostedWindows.end())
		{
			HostedWindowInfo& windowInfo = hostedWindowsIterator->second;
			RECT rect;
			GetClientRect(windowInfo.windowHandle, &rect);
			if ((windowInfo.currentSizeX  != rect.right) || (windowInfo.currentSizeY != rect.bottom))
			{
				windowInfo.currentSizeX = rect.right;
				windowInfo.currentSizeY = rect.bottom;
				UpdateScrollbarState(_currentControlWidth, _currentControlHeight);
			}
		}
	}
	return SendMessage(GetAncestor(_hwnd, GA_PARENT), WM_BOUNCE, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgWM_HSCROLL(WPARAM wParam, LPARAM lParam)
{
	static const unsigned int arrowStepCount = 25;
	switch (LOWORD(wParam))
	{
	case SB_THUMBTRACK:{
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(_hwnd, SB_HORZ, &scrollInfo);
		UpdateHorizontalScrollPos(scrollInfo.nTrackPos);
		break;}
	case SB_TOP:
		UpdateHorizontalScrollPos(0);
		break;
	case SB_BOTTOM:
		UpdateHorizontalScrollPos(_effectiveScrollWidth);
		break;
	case SB_PAGEUP:
		UpdateHorizontalScrollPos(_currentScrollPosX - _currentControlWidth);
		break;
	case SB_PAGEDOWN:
		UpdateHorizontalScrollPos(_currentScrollPosX + _currentControlWidth);
		break;
	case SB_LINEUP:
		UpdateHorizontalScrollPos(_currentScrollPosX - ((_currentControlWidth + (arrowStepCount - 1)) / arrowStepCount));
		break;
	case SB_LINEDOWN:
		UpdateHorizontalScrollPos(_currentScrollPosX + ((_currentControlWidth + (arrowStepCount - 1)) / arrowStepCount));
		break;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgWM_VSCROLL(WPARAM wParam, LPARAM lParam)
{
	static const unsigned int arrowStepCount = 25;
	switch (LOWORD(wParam))
	{
	case SB_THUMBTRACK:{
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(_hwnd, SB_VERT, &scrollInfo);
		UpdateVerticalScrollPos(scrollInfo.nTrackPos);
		break;}
	case SB_TOP:
		UpdateVerticalScrollPos(0);
		break;
	case SB_BOTTOM:
		UpdateVerticalScrollPos(_effectiveScrollHeight);
		break;
	case SB_PAGEUP:
		UpdateVerticalScrollPos(_currentScrollPosY - _currentControlHeight);
		break;
	case SB_PAGEDOWN:
		UpdateVerticalScrollPos(_currentScrollPosY + _currentControlHeight);
		break;
	case SB_LINEUP:
		UpdateVerticalScrollPos(_currentScrollPosY - ((_currentControlHeight + (arrowStepCount - 1)) / arrowStepCount));
		break;
	case SB_LINEDOWN:
		UpdateVerticalScrollPos(_currentScrollPosY + ((_currentControlHeight + (arrowStepCount - 1)) / arrowStepCount));
		break;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgSCROLL_ADDWINDOW(WPARAM wParam, LPARAM lParam)
{
	HWND windowHandle = (HWND)lParam;
	AddWindow(windowHandle);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_ScrollViewer::msgSCROLL_REMOVEWINDOW(WPARAM wParam, LPARAM lParam)
{
	HWND windowHandle = (HWND)lParam;
	RemoveWindow(windowHandle);
	return 0;
}

//----------------------------------------------------------------------------------------
//Hosted window methods
//----------------------------------------------------------------------------------------
void WC_ScrollViewer::AddWindow(HWND windowHandle)
{
	//Ensure this window isn't currently hosted in our control
	std::map<HWND, HostedWindowInfo>::iterator hostedWindowsIterator = _hostedWindows.find(windowHandle);
	if (hostedWindowsIterator != _hostedWindows.end())
	{
		return;
	}

	//Calculate the actual width and height of the target window
	RECT rect;
	GetClientRect(windowHandle, &rect);
	int actualWindowWidth = (int)(rect.right - rect.left);
	int actualWindowHeight = (int)(rect.bottom - rect.top);

	//Build a HostedWindowInfo object to record information on this new hosted window
	HostedWindowInfo hostedWindowInfo;
	hostedWindowInfo.windowHandle = windowHandle;
	hostedWindowInfo.currentSizeX = actualWindowWidth;
	hostedWindowInfo.currentSizeY = actualWindowHeight;

	//Record information on this new hosted window
	_hostedWindows[windowHandle] = hostedWindowInfo;

	//Set the target window as a child window of our control
	SetWindowParent(windowHandle, _hwnd);

	//Subclass the target window so we can intercept size change events
	SetWindowSubclass(windowHandle, BounceBackSubclassProc, 0, 0);

	//Update the scrollbar state for the control
	UpdateScrollbarState(_currentControlWidth, _currentControlHeight);
}

//----------------------------------------------------------------------------------------
void WC_ScrollViewer::RemoveWindow(HWND windowHandle)
{
	//Attempt to retrieve info on the target hosted window
	std::map<HWND, HostedWindowInfo>::iterator hostedWindowsIterator = _hostedWindows.find(windowHandle);
	if (hostedWindowsIterator == _hostedWindows.end())
	{
		return;
	}
	const HostedWindowInfo& hostedWindowInfo = hostedWindowsIterator->second;

	//Remove the target window as a child window of our control
	SetWindowParent(hostedWindowInfo.windowHandle, NULL);

	//Remove the window from the list of hosted windows
	_hostedWindows.erase(hostedWindowsIterator);

	//Update the scrollbar state for the control
	UpdateScrollbarState(_currentControlWidth, _currentControlHeight);
}

//----------------------------------------------------------------------------------------
//Sizing methods
//----------------------------------------------------------------------------------------
void WC_ScrollViewer::UpdateScrollbarState(int newControlWidth, int newControlHeight)
{
	//Calculate the total required width and height of the scroll region
	int totalScrollWidth = 0;
	int totalScrollHeight = 0;
	for (std::map<HWND, HostedWindowInfo>::const_iterator i = _hostedWindows.begin(); i != _hostedWindows.end(); ++i)
	{
		const HostedWindowInfo& windowInfo = i->second;
		totalScrollWidth = (windowInfo.currentSizeX > totalScrollWidth)? windowInfo.currentSizeX: totalScrollWidth;
		totalScrollHeight = (windowInfo.currentSizeY > totalScrollHeight)? windowInfo.currentSizeY: totalScrollHeight;
	}

	//Calculate the effective width and height of the scroll region, taking the current
	//scroll position into account. This code ensures that if a hosted window reduces in
	//size, the scroll position isn't shifted automatically, which might be disconcerting
	//for the user. The additional scroll width and height will be reclaimed when the user
	//scrolls back towards the origin.
	int newEffectiveScrollWidth = (_currentScrollPosX > totalScrollWidth)? _currentScrollPosX: totalScrollWidth;
	int newEffectiveScrollHeight = (_currentScrollPosY > totalScrollHeight)? _currentScrollPosY: totalScrollHeight;

	//Update the horizontal scrollbar settings if required
	if ((_currentControlWidth != newControlWidth) || (_effectiveScrollWidth != newEffectiveScrollWidth))
	{
		//Update the stored horizontal scroll info
		_currentControlWidth = newControlWidth;
		_effectiveScrollWidth = newEffectiveScrollWidth;

		//Apply the new horizontal scrollbar settings
		unsigned int maxScrollSetting = ((_currentScrollPosX + (_currentControlWidth - 1)) > (_effectiveScrollWidth - 1))? (_currentScrollPosX + (_currentControlWidth - 1)): (_effectiveScrollWidth - 1);
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.nMin = 0;
		scrollInfo.nMax = maxScrollSetting;
		scrollInfo.nPos = _currentScrollPosX;
		scrollInfo.nPage = _currentControlWidth;
		scrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		SetScrollInfo(_hwnd, SB_HORZ, &scrollInfo, TRUE);
	}

	//Update the vertical scrollbar settings if required
	if ((_currentControlHeight != newControlHeight) || (_effectiveScrollHeight != newEffectiveScrollHeight))
	{
		//Update the stored vertical scroll info
		_currentControlHeight = newControlHeight;
		_effectiveScrollHeight = newEffectiveScrollHeight;

		//Apply the new vertical scrollbar settings
		unsigned int maxScrollSetting = ((_currentScrollPosY + (_currentControlHeight - 1)) > (_effectiveScrollHeight - 1))? (_currentScrollPosY + (_currentControlHeight - 1)): (_effectiveScrollHeight - 1);
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.nMin = 0;
		scrollInfo.nMax = maxScrollSetting;
		scrollInfo.nPos = _currentScrollPosY;
		scrollInfo.nPage = _currentControlHeight;
		scrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		SetScrollInfo(_hwnd, SB_VERT, &scrollInfo, TRUE);
	}
}

//----------------------------------------------------------------------------------------
void WC_ScrollViewer::UpdateHorizontalScrollPos(int newScrollPos)
{
	//Limit the new scroll position based on the allowed minimum and maximum range
	int maxScrollPos = ((_effectiveScrollWidth - _currentControlWidth) < _currentScrollPosX)? _currentScrollPosX: (_effectiveScrollWidth - _currentControlWidth);
	newScrollPos = (newScrollPos < 0)? 0: ((newScrollPos > maxScrollPos)? maxScrollPos: newScrollPos);

	//Update the stored scroll position
	_currentScrollPosX = newScrollPos;

	//Calculate the total required width of the scroll region
	int totalScrollWidth = 0;
	for (std::map<HWND, HostedWindowInfo>::const_iterator i = _hostedWindows.begin(); i != _hostedWindows.end(); ++i)
	{
		const HostedWindowInfo& windowInfo = i->second;
		totalScrollWidth = (windowInfo.currentSizeX > totalScrollWidth)? windowInfo.currentSizeX: totalScrollWidth;
	}

	//Calculate the effective width of the scroll region, taking the current scroll
	//position into account. This code ensures that if a hosted window reduces in size,
	//the scroll position isn't shifted automatically, which might be disconcerting for
	//the user. The additional scroll width and height will be reclaimed when the user
	//scrolls back towards the origin.
	_effectiveScrollWidth = (_currentScrollPosX > totalScrollWidth)? _currentScrollPosX: totalScrollWidth;

	//Update the current scroll position
	unsigned int maxScrollSetting = ((_currentScrollPosX + (_currentControlWidth - 1)) > (_effectiveScrollWidth - 1))? (_currentScrollPosX + (_currentControlWidth - 1)): (_effectiveScrollWidth - 1);
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.nMin = 0;
	scrollInfo.nMax = maxScrollSetting;
	scrollInfo.nPos = _currentScrollPosX;
	scrollInfo.fMask = SIF_POS | SIF_RANGE;
	SetScrollInfo(_hwnd, SB_HORZ, &scrollInfo, TRUE);

	//Begin a session for processing this batch of window position changes. Processing all
	//the position changes in a single operation in this manner gives the best performance
	//and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos((int)_hostedWindows.size());

	//Reposition all our child windows to take the new scroll position into account
	for (std::map<HWND, HostedWindowInfo>::const_iterator i = _hostedWindows.begin(); i != _hostedWindows.end(); ++i)
	{
		const HostedWindowInfo& windowInfo = i->second;
		DeferWindowPos(deferWindowPosSession, windowInfo.windowHandle, NULL, -_currentScrollPosX, -_currentScrollPosY, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	//Process all the window position changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);
}

//----------------------------------------------------------------------------------------
void WC_ScrollViewer::UpdateVerticalScrollPos(int newScrollPos)
{
	//Limit the new scroll position based on the allowed minimum and maximum range
	int maxScrollPos = ((_effectiveScrollHeight - _currentControlHeight) < _currentScrollPosY)? _currentScrollPosY: (_effectiveScrollHeight - _currentControlHeight);
	newScrollPos = (newScrollPos < 0)? 0: ((newScrollPos > maxScrollPos)? maxScrollPos: newScrollPos);

	//Update the stored scroll position
	_currentScrollPosY = newScrollPos;

	//Calculate the total required height of the scroll region
	int totalScrollHeight = 0;
	for (std::map<HWND, HostedWindowInfo>::const_iterator i = _hostedWindows.begin(); i != _hostedWindows.end(); ++i)
	{
		const HostedWindowInfo& windowInfo = i->second;
		totalScrollHeight = (windowInfo.currentSizeY > totalScrollHeight)? windowInfo.currentSizeY: totalScrollHeight;
	}

	//Calculate the effective height of the scroll region, taking the current scroll
	//position into account. This code ensures that if a hosted window reduces in size,
	//the scroll position isn't shifted automatically, which might be disconcerting for
	//the user. The additional scroll width and height will be reclaimed when the user
	//scrolls back towards the origin.
	_effectiveScrollHeight = (_currentScrollPosY > totalScrollHeight)? _currentScrollPosY: totalScrollHeight;

	//Update the current scroll position
	unsigned int maxScrollSetting = ((_currentScrollPosY + (_currentControlHeight - 1)) > (_effectiveScrollHeight - 1))? (_currentScrollPosY + (_currentControlHeight - 1)): (_effectiveScrollHeight - 1);
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.nMin = 0;
	scrollInfo.nMax = maxScrollSetting;
	scrollInfo.nPos = _currentScrollPosY;
	scrollInfo.fMask = SIF_POS | SIF_RANGE;
	SetScrollInfo(_hwnd, SB_VERT, &scrollInfo, TRUE);

	//Begin a session for processing this batch of window position changes. Processing all
	//the position changes in a single operation in this manner gives the best performance
	//and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos((int)_hostedWindows.size());

	//Reposition all our child windows to take the new scroll position into account
	for (std::map<HWND, HostedWindowInfo>::const_iterator i = _hostedWindows.begin(); i != _hostedWindows.end(); ++i)
	{
		const HostedWindowInfo& windowInfo = i->second;
		DeferWindowPos(deferWindowPosSession, windowInfo.windowHandle, NULL, -_currentScrollPosX, -_currentScrollPosY, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	//Process all the window position changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);
}
