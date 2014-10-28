#include "DockingWindow.h"
#include "Resources/AutoHideCursor_Up.cur.h"
#include "Resources/AutoHideCursor_Down.cur.h"
#include "Resources/AutoHideCursor_Left.cur.h"
#include "Resources/AutoHideCursor_Right.cur.h"
#include "Resources/AutoHideCursor_Up_Expand.cur.h"
#include "Resources/AutoHideCursor_Down_Expand.cur.h"
#include "Resources/AutoHideCursor_Left_Expand.cur.h"
#include "Resources/AutoHideCursor_Right_Expand.cur.h"
#include "Stream/Stream.pkg"
//##DEBUG##
#include <iostream>

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* DockingWindow::windowClassName = L"EX_DockingWindow";
const wchar_t* DockingWindow::tabTrayWindowClassName = L"EX_DockingWindow_TabTray";
const wchar_t* DockingWindow::placementTargetWindowClassName = L"EX_DockingWindow_PlacementTarget";
const wchar_t* DockingWindow::placementShadowWindowClassName = L"EX_DockingWindow_PlacementShadow";

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DockingWindow::DockingWindow(HINSTANCE amoduleHandle, HWND ahwnd)
:moduleHandle(amoduleHandle), hwnd(ahwnd)
{
	//Window handles
	tabDockPanel = NULL;
	dockPanel = NULL;

	//Font info
	controlFont = NULL;
	controlFontVertical = NULL;
	controlFontHeight = 1;
	controlFontDescent = 0;

	//Parent docking window info
	parentDockingWindow = 0;

	//Child content info
	contentTabControl = NULL;
	currentSelectedTabIndex = -1;
	dockingPanelTabMarginSize = 2;
	dockingPanelTabGroupSeparatorSize = 5;

	//Autohide child containers
	autoHidePanelVisible = false;
	currentAutoHidePanel = 0;
	currentAutoHidePanelContent = NULL;

	//Drag-info
	windowDragInProgress = false;
	dockingWindowUnderDragPos = 0;
	leftMouseButtonDown = false;
	ignoreNextSizeAndMove = false;
	tabDragActive = false;
	tabWithDeadZoneForDrag = -1;

	//Placement target info
	placementTargetCenterEnabled = false;
	placementTargetCenter = NULL;
	placementShadowVisible = false;
	placementShadow = NULL;
}

//----------------------------------------------------------------------------------------
DockingWindow::~DockingWindow()
{
	//Destroy the placement target windows for this docking window
	for(std::map<WC_DockPanel::DockLocation, HWND>::const_iterator i = placementTargets.begin(); i != placementTargets.end(); ++i)
	{
		DestroyWindow(i->second);
	}
	for(std::map<WC_DockPanel::DockLocation, HWND>::const_iterator i = placementTargetsForceTop.begin(); i != placementTargetsForceTop.end(); ++i)
	{
		DestroyWindow(i->second);
	}
	DestroyWindow(placementTargetCenter);
	DestroyWindow(placementShadow);

	//Destroy each dock and undock custom cursor
	for(std::map<WC_DockPanel::DockLocation, HCURSOR>::const_iterator i = enableAutoHideCursor.begin(); i != enableAutoHideCursor.end(); ++i)
	{
		DestroyCursor(i->second);
	}
	for(std::map<WC_DockPanel::DockLocation, HCURSOR>::const_iterator i = disableAutoHideCursor.begin(); i != disableAutoHideCursor.end(); ++i)
	{
		DestroyCursor(i->second);
	}
}

//----------------------------------------------------------------------------------------
//Class registration
//----------------------------------------------------------------------------------------
bool DockingWindow::RegisterWindowClass(HINSTANCE amoduleHandle)
{
	//Register the window class for this control
	bool result = true;
	ATOM registerClassExReturn;
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = amoduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = windowClassName;
	wc.hIconSm       = NULL;
	registerClassExReturn = RegisterClassEx(&wc);
	result &= (registerClassExReturn != 0);

	//Register the TabTray window class
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = TabTrayWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = amoduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = tabTrayWindowClassName;
	wc.hIconSm       = NULL;
	registerClassExReturn = RegisterClassEx(&wc);
	result &= (registerClassExReturn != 0);

	//Register the PlacementTarget window class
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = PlacementTargetWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = amoduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = placementTargetWindowClassName;
	wc.hIconSm       = NULL;
	registerClassExReturn = RegisterClassEx(&wc);
	result &= (registerClassExReturn != 0);

	//Register the PlacementShadow window class
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = PlacementShadowWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = amoduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = placementShadowWindowClassName;
	wc.hIconSm       = NULL;
	registerClassExReturn = RegisterClassEx(&wc);
	result &= (registerClassExReturn != 0);

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
bool DockingWindow::UnregisterWindowClass(HINSTANCE amoduleHandle)
{
	//Unregister each window class registered by this control
	bool result = true;
	result &= (UnregisterClass(windowClassName, amoduleHandle) != 0);
	result &= (UnregisterClass(tabTrayWindowClassName, amoduleHandle) != 0);
	result &= (UnregisterClass(placementTargetWindowClassName, amoduleHandle) != 0);
	result &= (UnregisterClass(placementShadowWindowClassName, amoduleHandle) != 0);

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
//Message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK DockingWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		DockingWindow* object = (DockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->WndProcPrivate(message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		DockingWindow* object = new DockingWindow((HINSTANCE)createStruct->lpCreateParams, hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->WndProcPrivate(message, wParam, lParam);}
	case WM_DESTROY:{
		DockingWindow* object = (DockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
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
LRESULT DockingWindow::WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		return msgWM_CREATE(wParam, lParam);
	case WM_DESTROY:
		return msgWM_DESTROY(wParam, lParam);
	case WM_SIZING:
		return msgWM_SIZING(wParam, lParam);
	case WM_SIZE:
		return msgWM_SIZE(wParam, lParam);
	case WM_MOVING:
		return msgWM_MOVING(wParam, lParam);
	case WM_TIMER:
		return msgWM_TIMER(wParam, lParam);
	case WM_ENTERSIZEMOVE:
		return msgWM_ENTERSIZEMOVE(wParam, lParam);
	case WM_EXITSIZEMOVE:
		return msgWM_EXITSIZEMOVE(wParam, lParam);
	case WM_WINDOWPOSCHANGING:
		return msgWM_WINDOWPOSCHANGING(wParam, lParam);
	case WM_NCHITTEST:
		return msgWM_NCHITTEST(wParam, lParam);
	case WM_NCLBUTTONUP:
		return msgWM_NCLBUTTONUP(wParam, lParam);
	case WM_SETCURSOR:
		return msgWM_SETCURSOR(wParam, lParam);
	case WM_GETFONT:
		return msgWM_GETFONT(wParam, lParam);
	case WM_SETFONT:
		return msgWM_SETFONT(wParam, lParam);
	case WM_BOUNCE:
		return msgWM_BOUNCE(wParam, lParam);
	case WindowMessages::AddContentWindow:
		return msgDOCKWIN_ADDCONTENTWINDOW(wParam, lParam);
	case WindowMessages::RemoveContentWindow:
		return msgDOCKWIN_REMOVECONTENTWINDOW(wParam, lParam);
	case WindowMessages::ModifyContentWindow:
		return msgDOCKWIN_MODIFYCONTENTWINDOW(wParam, lParam);
	case WindowMessages::AddDockedWindow:
		return msgDOCKWIN_ADDDOCKEDWINDOW(wParam, lParam);
	case WindowMessages::RemoveDockedWindow:
		return msgDOCKWIN_REMOVEDOCKEDWINDOW(wParam, lParam);
	case WindowMessages::GetContentWindowIndexFromHandle:
		return msgDOCKWIN_GETCONTENTWINDOWINDEXFROMHANDLE(wParam, lParam);
	case WindowMessages::GetDockedWindowIndexFromHandle:
		return msgDOCKWIN_GETDOCKEDWINDOWINDEXFROMHANDLE(wParam, lParam);
	case WindowMessages::GetContentWindowCount:
		return msgDOCKWIN_GETCONTENTWINDOWCOUNT(wParam, lParam);
	case WindowMessages::GetContentWindowInfo:
		return msgDOCKWIN_GETCONTENTWINDOWINFO(wParam, lParam);
	case WindowMessages::GetDockedWindowCount:
		return msgDOCKWIN_GETDOCKEDWINDOWCOUNT(wParam, lParam);
	case WindowMessages::GetDockedWindowInfo:
		return msgDOCKWIN_GETDOCKEDWINDOWINFO(wParam, lParam);
	case WindowMessages::GetActiveContentWindow:
		return msgDOCKWIN_GETACTIVECONTENTWINDOW(wParam, lParam);
	case WindowMessages::SetActiveContentWindow:
		return msgDOCKWIN_SETACTIVECONTENTWINDOW(wParam, lParam);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
{
	//Read the font metrics for our default font
	HDC hdc = GetDC(hwnd);
	HFONT hfontOld = (HFONT)SelectObject(hdc, controlFont);
	TEXTMETRIC textMetric;
	GetTextMetrics(hdc, &textMetric);
	controlFontHeight = textMetric.tmHeight;
	SelectObject(hdc, hfontOld);
	ReleaseDC(hwnd, hdc);

	//Initialize common controls
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_WIN95_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	//Register the DockPanel window class
	WC_DockPanel::RegisterWindowClass(moduleHandle);

	//Create our tab dock panel control. This dock panel is the only direct child of our
	//window, and displays tabs around the outer frame for child docking windows that are
	//docked in autohide mode. The content region of this window is an inner docking
	//panel, which contains all fully visible docked child panels.
	tabDockPanel = CreateWindowEx(0, WC_DockPanel::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, hwnd, NULL, moduleHandle, NULL);

	//Subclass the tab dock panel control so that it passes messages from its contained
	//tab control on to us
	SetWindowSubclass(tabDockPanel, BounceBackSubclassProc, 0, 0);

	//Create our dock panel control
	dockPanel = CreateWindowEx(0, WC_DockPanel::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, tabDockPanel, NULL, moduleHandle, NULL);

	//Subclass the dock panel control so we can intercept content tab change events
	SetWindowSubclass(dockPanel, BounceBackSubclassProc, 0, 0);

	//Set the dock panel control as the content content for our tab dock panel control
	SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::AddContentWindow, 0, (LPARAM)dockPanel);

	//Set the font for the created dock panel. Note that we don't set the font for the tab
	//dock panel, since we need a different font for horizontal and vertical tab text in
	//this panel. We set the font directly on each of its child elements in this case.
	if(controlFont != NULL)
	{
		SendMessage(dockPanel, WM_SETFONT, (WPARAM)controlFont, FALSE);
	}

	//Create the placement target windows to indicate the drag and drop docking target for
	//this window
	placementTargetCenter = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	placementTargets[WC_DockPanel::DockLocation::Left] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	placementTargets[WC_DockPanel::DockLocation::Right] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	placementTargets[WC_DockPanel::DockLocation::Top] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	placementTargets[WC_DockPanel::DockLocation::Bottom] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	placementTargetsForceTop[WC_DockPanel::DockLocation::Left] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	placementTargetsForceTop[WC_DockPanel::DockLocation::Right] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	placementTargetsForceTop[WC_DockPanel::DockLocation::Top] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	placementTargetsForceTop[WC_DockPanel::DockLocation::Bottom] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);

	//Create the placement shadow window
	placementShadow = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, placementShadowWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	SetLayeredWindowAttributes(placementShadow, 0, 128, LWA_ALPHA);

	//Load each dock and undock custom cursor
	enableAutoHideCursor[WC_DockPanel::DockLocation::Left] = LoadCursorFile(&AutoHideCursor_Left_cur[0], sizeof(AutoHideCursor_Left_cur) / sizeof(AutoHideCursor_Left_cur[0]));
	enableAutoHideCursor[WC_DockPanel::DockLocation::Right] = LoadCursorFile(&AutoHideCursor_Right_cur[0], sizeof(AutoHideCursor_Right_cur) / sizeof(AutoHideCursor_Right_cur[0]));
	enableAutoHideCursor[WC_DockPanel::DockLocation::Top] = LoadCursorFile(&AutoHideCursor_Up_cur[0], sizeof(AutoHideCursor_Up_cur) / sizeof(AutoHideCursor_Up_cur[0]));
	enableAutoHideCursor[WC_DockPanel::DockLocation::Bottom] = LoadCursorFile(&AutoHideCursor_Down_cur[0], sizeof(AutoHideCursor_Down_cur) / sizeof(AutoHideCursor_Down_cur[0]));
	disableAutoHideCursor[WC_DockPanel::DockLocation::Left] = LoadCursorFile(&AutoHideCursor_Left_Expand_cur[0], sizeof(AutoHideCursor_Left_Expand_cur) / sizeof(AutoHideCursor_Left_Expand_cur[0]));
	disableAutoHideCursor[WC_DockPanel::DockLocation::Right] = LoadCursorFile(&AutoHideCursor_Right_Expand_cur[0], sizeof(AutoHideCursor_Right_Expand_cur) / sizeof(AutoHideCursor_Right_Expand_cur[0]));
	disableAutoHideCursor[WC_DockPanel::DockLocation::Top] = LoadCursorFile(&AutoHideCursor_Up_Expand_cur[0], sizeof(AutoHideCursor_Up_Expand_cur) / sizeof(AutoHideCursor_Up_Expand_cur[0]));
	disableAutoHideCursor[WC_DockPanel::DockLocation::Bottom] = LoadCursorFile(&AutoHideCursor_Down_Expand_cur[0], sizeof(AutoHideCursor_Down_Expand_cur) / sizeof(AutoHideCursor_Down_Expand_cur[0]));

	//Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	int newClientWidth = rect.right;
	int newClientHeight = rect.bottom;

	//Process the initial size of the window
	HandleSizeChanged(newClientWidth, newClientHeight);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_DESTROY(WPARAM wParam, LPARAM lParam)
{
	for(std::list<ChildContainerEntry>::const_iterator i = childContainers.begin(); i != childContainers.end(); ++i)
	{
		i->childContainer->NotifyParentDestroyed();
	}
	if(parentDockingWindow != 0)
	{
		parentDockingWindow->RemoveChildContainer(this);
		parentDockingWindow = 0;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_SIZING(WPARAM wParam, LPARAM lParam)
{
	//If a window size operation isn't currently being tracked, or if this docking window
	//isn't docked to a parent, perform the default processing for this window message.
	RECT& rect = *((RECT*)lParam);
	if(!windowDragInProgress || (parentDockingWindow == 0))
	{
		return DefWindowProc(hwnd, WM_SIZING, wParam, lParam);
	}

	//If this window isn't attached as an auto hide docked window to its parent, adjust
	//the desired width or height of this window in its parent docking window to take into
	//account the new requested window size.
	if(!autoHideDockedWindow)
	{
		if(((currentDockLocation == WC_DockPanel::DockLocation::Left) && ((wParam == WMSZ_RIGHT) || (wParam == WMSZ_TOPRIGHT) || (wParam == WMSZ_BOTTOMRIGHT))) || ((currentDockLocation == WC_DockPanel::DockLocation::Right) && ((wParam == WMSZ_LEFT) || (wParam == WMSZ_TOPLEFT) || (wParam == WMSZ_BOTTOMLEFT))))
		{
			SendMessage(parentDockingWindow->GetDockPanelWindowHandle(), (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredWidth, rect.right - rect.left, (LPARAM)hwnd);
		}
		else if(((currentDockLocation == WC_DockPanel::DockLocation::Top) && ((wParam == WMSZ_BOTTOM) || (wParam == WMSZ_BOTTOMLEFT) || (wParam == WMSZ_BOTTOMRIGHT))) || ((currentDockLocation == WC_DockPanel::DockLocation::Bottom) && ((wParam == WMSZ_TOP) || (wParam == WMSZ_TOPLEFT) || (wParam == WMSZ_TOPRIGHT))))
		{
			SendMessage(parentDockingWindow->GetDockPanelWindowHandle(), (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredHeight, rect.bottom - rect.top, (LPARAM)hwnd);
		}
	}

	//Only allow the free side of this docked window to be adjusted. The other three
	//window edges are fully controlled by the parent window.
	if(currentDockLocation != WC_DockPanel::DockLocation::Right)
	{
		rect.left = windowSizeMoveInitialPos.left;
	}
	if(currentDockLocation != WC_DockPanel::DockLocation::Left)
	{
		rect.right = windowSizeMoveInitialPos.right;
	}
	if(currentDockLocation != WC_DockPanel::DockLocation::Bottom)
	{
		rect.top = windowSizeMoveInitialPos.top;
	}
	if(currentDockLocation != WC_DockPanel::DockLocation::Top)
	{
		rect.bottom = windowSizeMoveInitialPos.bottom;
	}

	//Return TRUE to inform windows that we processed this message
	return TRUE;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	int newClientWidth = rect.right;
	int newClientHeight = rect.bottom;

	//Handle this size changed event
	HandleSizeChanged(newClientWidth, newClientHeight);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_MOVING(WPARAM wParam, LPARAM lParam)
{
	//Ensure we're currently tracking a window move operation
	if(!windowDragInProgress)
	{
		return 0;
	}

	//Obtain the current cursor position
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	lastDragCursorPosX = cursorPos.x;
	lastDragCursorPosY = cursorPos.y;

	//If we're currently attached to a docking parent, detach now that a move operation is
	//in progress.
	if(parentDockingWindow != 0)
	{
		//Cancel the current move operation. We need to do this so that we can change the
		//window parent, otherwise the move operation doesn't let the mouse leave the old
		//parent window boundaries.
		SendMessage(hwnd, WM_CANCELMODE, 0, 0);

		//Hide this window until we finish adjusting its position and size
		ShowWindow(hwnd, SW_HIDE);

		//Retrieve the first owner window at the top of the chain of parent windows for
		//our old parent docking window
		HWND newOwnerWindow = GetFirstOwnerWindow(parentDockingWindow->GetWindowHandle());

		//If our old parent docking window had no owner window at the top of the parent
		//chain, set the owner window to be the top-level parent window of the old parent
		//docking window.
		if(newOwnerWindow == NULL)
		{
			newOwnerWindow = GetAncestor(parentDockingWindow->GetWindowHandle(), GA_ROOT);
		}

		//Detach from the parent docking window
		parentDockingWindow->RemoveChildContainer(this);
		parentDockingWindow = 0;

		//Set the owner window for our detached window now that we no longer have a parent
		//docking window
		SetOwnerWindow(hwnd, newOwnerWindow);

		//Now that we've undocked from our parent container, the window size may have
		//changed as a result. We now reposition our window so that the current mouse
		//cursor position is in the center of the window caption bar, as that's where the
		//window would be moved to when we re-initiate the move operation below, so this
		//ensures the window doesn't jump on the screen.
		RECT rect;
		GetWindowRect(hwnd, &rect);
		int newWindowWidth = rect.right - rect.left;
		int newWindowPosX = cursorPos.x - (newWindowWidth / 2);
		int newWindowPosY = cursorPos.y - (GetSystemMetrics(SM_CYSMCAPTION) / 2);
		SetWindowPos(hwnd, NULL, newWindowPosX, newWindowPosY, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

		//Show and activate our window
		ShowWindow(hwnd, SW_SHOW);

		//Since we've just undocked from our parent docking window, and we're aborting
		//this current move operation, ignore the next size and position change event. We
		//need to do this because right now, the correct window size has been restored for
		//our window by the parent docking window when we were removed as a child
		//container, and we've adjusted the window position ourselves to match the correct
		//mouse location after the resize. When we return from this message handler after
		//aborting this move event however, the handler for the move event is going to
		//restore the window position and size back to its original values when the move
		//began, which would override our new correct window position and size. We set
		//this flag here so that we know to filter out the next size and position change
		//events, so that the current window size and position are retained.
		ignoreNextSizeAndMove = true;

		//Begin the move operation again now that we've detached from our parent
		PostMessage(hwnd, WM_SYSCOMMAND, SC_MOVE, 0);
		return 0;
	}

	//Build a list of all docking windows which share this UI thread
	std::list<DockingWindow*> dockingWindowList;
	EnumThreadWindows(GetCurrentThreadId(), EnumDockingWindowsProc, (LPARAM)&dockingWindowList);

	//Attempt to locate a docking window under the target cursor position
	bool foundTargetWindow = false;
	DockingWindow* targetDockingWindow = 0;
	std::list<DockingWindow*>::const_iterator dockingWindowListIterator = dockingWindowList.begin();
	while(!foundTargetWindow && (dockingWindowListIterator != dockingWindowList.end()))
	{
		targetDockingWindow = *dockingWindowListIterator;
		if((targetDockingWindow != this) && (IsChild(hwnd, targetDockingWindow->hwnd) == 0))
		{
			RECT targetDockingWindowRect;
			GetWindowRect(targetDockingWindow->hwnd, &targetDockingWindowRect);
			if((targetDockingWindowRect.left <= lastDragCursorPosX) && (targetDockingWindowRect.right > lastDragCursorPosX) && (targetDockingWindowRect.top <= lastDragCursorPosY) && (targetDockingWindowRect.bottom > lastDragCursorPosY))
			{
				foundTargetWindow = true;
				continue;
			}
		}
		++dockingWindowListIterator;
	}

	//Remove the placement target from the previous target window if required
	if((dockingWindowUnderDragPos != 0) && (!foundTargetWindow || (dockingWindowUnderDragPos != targetDockingWindow)))
	{
		dockingWindowUnderDragPos->HidePlacementTargets();
	}

	//Display a placement target for docking this window into the target docking window if
	//required
	if(foundTargetWindow && ((dockingWindowUnderDragPos == 0) || (dockingWindowUnderDragPos != targetDockingWindow)))
	{
		bool allowCenterDocking = childContainers.empty();
		targetDockingWindow->ShowPlacementTargets(allowCenterDocking);
	}

	//Record the new window that's under the current drag position
	dockingWindowUnderDragPos = (foundTargetWindow)? targetDockingWindow: 0;

	//If a docking window is currently under the cursor, update the display of the
	//placement shadow window for the target docking window.
	if(dockingWindowUnderDragPos != 0)
	{
		//Calculate the current window width and height
		RECT rect;
		GetWindowRect(hwnd, &rect);
		int windowWidth = rect.right - rect.left;
		int windowHeight = rect.bottom - rect.top;

		//Update the window placement shadow for the docking window under the current drag
		//location
		dockingWindowUnderDragPos->UpdatePlacementShadow(lastDragCursorPosX, lastDragCursorPosY, windowWidth, windowHeight);
	}

	//If a docking window is currently under the cursor, perform hit testing for each
	//docking tab in the target docking window, so that tabs can activate while a window
	//is being dragged.
	if(dockingWindowUnderDragPos != 0)
	{
		dockingWindowUnderDragPos->AllDockTabsHitTest(lastDragCursorPosX, lastDragCursorPosY);
	}

	//Restart the timer to bring the window under the current cursor position up in the
	//Z-order
	KillTimer(hwnd, 1);
	if(dockingWindowUnderDragPos != 0)
	{
		SetTimer(hwnd, 1, 500, NULL);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_TIMER(WPARAM wParam, LPARAM lParam)
{
	//If this timer has expired, bring the docking window under the current drag position
	//up in the Z-order just below this window being dragged.
	if(dockingWindowUnderDragPos != 0)
	{
		//Locate the top-level parent of the docking window under the current drag
		//position
		HWND topLevelParentWindow = NULL;
		HWND desktopWindow = GetDesktopWindow();
		HWND searchWindow = dockingWindowUnderDragPos->GetWindowHandle();
		while((searchWindow != NULL) && (searchWindow != desktopWindow))
		{
			topLevelParentWindow = searchWindow;
			searchWindow = GetAncestor(searchWindow, GA_PARENT);
		}

		//Bring the top level parent window of the target docking window up in the Z-order
		//just behind our window. Since our window is currently being dragged and is the
		//active window, this will place the target docking window above all other
		//non-topmost windows. Note that we can't supply the SWP_NOOWNERZORDER flag here,
		//or any owned windows which belong to the top level parent window will appear
		//behind it after the Z-order change instead of above it, like they're always
		//meant to.
		if(topLevelParentWindow != NULL)
		{
			SetWindowPos(topLevelParentWindow, hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_ENTERSIZEMOVE(WPARAM wParam, LPARAM lParam)
{
	//Flag that a window drag operation is in progress
	windowDragInProgress = true;
	dockingWindowUnderDragPos = 0;

	//Capture the initial window size and position
	GetWindowRect(hwnd, &windowSizeMoveInitialPos);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_EXITSIZEMOVE(WPARAM wParam, LPARAM lParam)
{
	//If a window drag operation isn't currently in progress, abort any further
	//processing.
	if(!windowDragInProgress)
	{
		return 0;
	}

	//Kill the timer to bring the window under the current cursor position up in the
	//Z-order
	KillTimer(hwnd, 1);

	//If another docking window is under the current cursor position, dock this window
	//into the target docking window if required.
	if(dockingWindowUnderDragPos != 0)
	{
		//Calculate a docking location within the target window
		bool newDockLocationIsContentRegion;
		WC_DockPanel::DockLocation newDockLocation;
		bool forceTopOfDockingOrder;
		bool foundDockLocation = dockingWindowUnderDragPos->HitTestPlacementTargets(lastDragCursorPosX, lastDragCursorPosY, newDockLocationIsContentRegion, newDockLocation, forceTopOfDockingOrder);

		//If we found a target dock location, dock to the new parent docking window.
		if(foundDockLocation)
		{
			//Dock to the new parent docking window
			if(!newDockLocationIsContentRegion)
			{
				//Dock this container in the target container at the target dock location
				dockingWindowUnderDragPos->AddChildContainer(this, newDockLocation, false, forceTopOfDockingOrder);
				parentDockingWindow = dockingWindowUnderDragPos;
				currentDockLocation = newDockLocation;
				autoHideDockedWindow = false;
			}
			else if(childContainers.empty())
			{
				//Transfer all our content windows to the target docking window
				for(size_t i = 0; i < hostedContent.size(); ++i)
				{
					dockingWindowUnderDragPos->AddHostedContent(hostedContent[i].contentWindow, hostedContent[i].contentTitle);
				}

				//Post a close message to ourself. Note that it's really important that we
				//use PostMessage rather than calling DestroyWindow or using SendMessage here,
				//because both of those approaches will result in WM_DESTROY being processed
				//by this window before that call returns, destroying our class object. Using
				//SendMessage ensures this object remains alive until we've finished
				//processing this message.
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}

			//Activate the top-level parent of the docking window we just docked into
			HWND newActiveWindow = NULL;
			HWND desktopWindow = GetDesktopWindow();
			HWND searchWindow = dockingWindowUnderDragPos->GetWindowHandle();
			while((searchWindow != NULL) && (searchWindow != desktopWindow))
			{
				newActiveWindow = searchWindow;
				searchWindow = GetAncestor(searchWindow, GA_PARENT);
			}
			SetActiveWindow(newActiveWindow);
		}
	}

	//Remove the placement target from the previous target window if required
	if(dockingWindowUnderDragPos != 0)
	{
		dockingWindowUnderDragPos->HidePlacementTargets();
	}

	//Now that this move event is complete, flag that a move is no longer in progress.
	windowDragInProgress = false;

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_WINDOWPOSCHANGING(WPARAM wParam, LPARAM lParam)
{
	//If we need to ignore the next size and move event due to an undocking operation,
	//drop any size and position information from this change event.
	if(ignoreNextSizeAndMove)
	{
		WINDOWPOS& windowPos = *((WINDOWPOS*)lParam);
		windowPos.flags |= SWP_NOMOVE | SWP_NOSIZE;
		ignoreNextSizeAndMove = false;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_NCHITTEST(WPARAM wParam, LPARAM lParam)
{
	//If we're docked in a parent window, ensure we only display a resize icon on the
	//border edge that can actually be resized.
	LRESULT result = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
	if(parentDockingWindow != 0)
	{
		result = ((result == HTLEFT) && (currentDockLocation != WC_DockPanel::DockLocation::Right))? HTBORDER: result;
		result = ((result == HTRIGHT) && (currentDockLocation != WC_DockPanel::DockLocation::Left))? HTBORDER: result;
		result = ((result == HTTOP) && (currentDockLocation != WC_DockPanel::DockLocation::Bottom))? HTBORDER: result;
		result = ((result == HTBOTTOM) && (currentDockLocation != WC_DockPanel::DockLocation::Top))? HTBORDER: result;
	}
	return result;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_NCLBUTTONUP(WPARAM wParam, LPARAM lParam)
{
	//Ensure we're attached to a parent docking window
	if(parentDockingWindow == 0)
	{
		return DefWindowProc(hwnd, WM_NCLBUTTONUP, wParam, lParam);
	}

	//Check if we're currently over the border that activates auto hide toggling
	int windowPosHitRegion = (int)DefWindowProc(hwnd, WM_NCHITTEST, 0, lParam);
	bool overAutoHideToggleBorder = false;
	overAutoHideToggleBorder |= ((windowPosHitRegion == HTLEFT) && (currentDockLocation == WC_DockPanel::DockLocation::Left));
	overAutoHideToggleBorder |= ((windowPosHitRegion == HTRIGHT) && (currentDockLocation == WC_DockPanel::DockLocation::Right));
	overAutoHideToggleBorder |= ((windowPosHitRegion == HTTOP) && (currentDockLocation == WC_DockPanel::DockLocation::Top));
	overAutoHideToggleBorder |= ((windowPosHitRegion == HTBOTTOM) && (currentDockLocation == WC_DockPanel::DockLocation::Bottom));

	//Toggle the auto hide mode if required
	if(overAutoHideToggleBorder)
	{
		//Toggle the auto hide mode for this window
		parentDockingWindow->RemoveChildContainer(this);
		autoHideDockedWindow = !autoHideDockedWindow;
		parentDockingWindow->AddChildContainer(this, currentDockLocation, autoHideDockedWindow, false);
		if(!autoHideDockedWindow)
		{
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
		}

		//Activate the top-level parent of the parent docking window if the auto hide mode
		//was just adjusted
		HWND newActiveWindow = NULL;
		HWND desktopWindow = GetDesktopWindow();
		HWND searchWindow = parentDockingWindow->GetWindowHandle();
		while((searchWindow != NULL) && (searchWindow != desktopWindow))
		{
			newActiveWindow = searchWindow;
			searchWindow = GetAncestor(searchWindow, GA_PARENT);
		}
		SetActiveWindow(newActiveWindow);
	}

	//Pass this message on to the default window procedure
	return DefWindowProc(hwnd, WM_NCLBUTTONUP, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_SETCURSOR(WPARAM wParam, LPARAM lParam)
{
	//If we're not docked to a parent docking window, pass this message on to the default
	//window procedure.
	if(parentDockingWindow == 0)
	{
		return DefWindowProc(hwnd, WM_SETCURSOR, wParam, lParam);
	}

	//Obtain the current cursor position
	POINT point;
	GetCursorPos(&point);
	int cursorPosX = point.x;
	int cursorPosY = point.y;

	//Check if we're currently over the border that activates auto hide toggling
	int windowPosHitRegion = (int)DefWindowProc(hwnd, WM_NCHITTEST, 0, MAKELPARAM(cursorPosX, cursorPosY));
	bool overAutoHideToggleBorder = false;
	overAutoHideToggleBorder |= ((windowPosHitRegion == HTLEFT) && (currentDockLocation == WC_DockPanel::DockLocation::Left));
	overAutoHideToggleBorder |= ((windowPosHitRegion == HTRIGHT) && (currentDockLocation == WC_DockPanel::DockLocation::Right));
	overAutoHideToggleBorder |= ((windowPosHitRegion == HTTOP) && (currentDockLocation == WC_DockPanel::DockLocation::Top));
	overAutoHideToggleBorder |= ((windowPosHitRegion == HTBOTTOM) && (currentDockLocation == WC_DockPanel::DockLocation::Bottom));

	//Display a custom cursor if the mouse is over the auto hide region
	if(overAutoHideToggleBorder)
	{
		//Select the cursor to use for this docking border
		HCURSOR cursorHandle = (!autoHideDockedWindow)? enableAutoHideCursor[currentDockLocation]: disableAutoHideCursor[currentDockLocation];

		//Apply the selected cursor at this location
		SetCursor(cursorHandle);
		return TRUE;
	}

	//Pass this message on to the default window procedure
	return DefWindowProc(hwnd, WM_SETCURSOR, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_GETFONT(WPARAM wParam, LPARAM lParam)
{
	//Return the currently selected font to the caller
	return (LRESULT)controlFont;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_SETFONT(WPARAM wParam, LPARAM lParam)
{
	//Store the font to use for out controls
	controlFont = (HFONT)wParam;

	//Pass this message on to each created child control, except the tab dock panel. We
	//exclude this control from font messages, since we need a different font for vertical
	//and horizontal tab text, so we set the child controls of this tab group
	//individually.
	SendMessage(dockPanel, WM_SETFONT, wParam, lParam);
	if(contentTabControl != NULL)
	{
		SendMessage(contentTabControl, WM_SETFONT, wParam, lParam);
	}

	//Create a vertical version of this font to use for vertical tab text
	controlFontVertical = controlFont;
	if(controlFont != NULL)
	{
		LOGFONT fontAttributes;
		if(GetObject(controlFont, sizeof(fontAttributes), &fontAttributes) != 0)
		{
			fontAttributes.lfOrientation = fontAttributes.lfEscapement = (900 * 3);
			controlFontVertical = CreateFontIndirect(&fontAttributes);
		}
	}

	//Read the font metrics for our new font
	if(controlFont != NULL)
	{
		HDC hdc = GetDC(hwnd);
		HFONT hfontOld = (HFONT)SelectObject(hdc, controlFont);
		TEXTMETRIC textMetric;
		GetTextMetrics(hdc, &textMetric);
		controlFontHeight = textMetric.tmHeight;
		controlFontDescent = textMetric.tmDescent;
		SelectObject(hdc, hfontOld);
		ReleaseDC(hwnd, hdc);
	}

	//Update window sizes now that the size of docked tab elements may have changed
	UpdateAutoHideWindowTabSizes();

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgWM_BOUNCE(WPARAM wParam, LPARAM lParam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lParam;
	if(bounceMessage->hwnd == contentTabControl)
	{
		if(bounceMessage->uMsg == WM_SIZE)
		{
			//Resize the currently selected tab content
			if(currentSelectedTabIndex >= 0)
			{
				const ContentEntry& contentEntry = hostedContent[tabIndexToHostedContentNo[currentSelectedTabIndex]];
				RECT tabControlContentRect;
				GetClientRect(contentTabControl, &tabControlContentRect);
				SendMessage(contentTabControl, TCM_ADJUSTRECT, FALSE, (LPARAM)&tabControlContentRect);
				SetWindowPos(contentEntry.contentWindow, NULL, tabControlContentRect.left, tabControlContentRect.top, tabControlContentRect.right - tabControlContentRect.left, tabControlContentRect.bottom - tabControlContentRect.top, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
			}
		}
		//##TODO## We no longer need this code, but it's being kept around for future
		//reference when we want to do more complex tasks with update region testing.
		//else if(bounceMessage->uMsg == WM_PAINT)
		//{
		//	//Obtain the update rectangle for the tab control
		//	RECT updateRect;
		//	GetUpdateRect(contentTabControl, &updateRect, FALSE);

		//	//Convert the update rectangle for the tab control from logical coordinates to
		//	//device coordinates. By default, the logical coordinate system is simply
		//	//mapped in terms of device units relative to the top left of the client
		//	//region of the window, however the mapping can be changed by the SetMapMode
		//	//function, and the origin can be changed by the SetWindowOrgEx or
		//	//SetViewportOrgEx functions. Although the tab control will probably always
		//	//just use the default mapping, we perform a proper conversion here to be
		//	//safe.
		//	POINT point[2];
		//	point[0].x = updateRect.left;
		//	point[0].y = updateRect.top;
		//	point[1].x = updateRect.right;
		//	point[1].y = updateRect.bottom;
		//	HDC hdc = GetDC(contentTabControl);
		//	LPtoDP(hdc, &point[0], 2);
		//	ReleaseDC(contentTabControl, hdc);
		//	updateRect.left = point[0].x;
		//	updateRect.top = point[0].y;
		//	updateRect.right = point[1].x;
		//	updateRect.bottom = point[1].y;

		//	//Obtain the location and size of the content region of this tab control
		//	RECT tabControlContentRect;
		//	GetClientRect(contentTabControl, &tabControlContentRect);
		//	SendMessage(contentTabControl, TCM_ADJUSTRECT, FALSE, (LPARAM)&tabControlContentRect);

		//	//Determine if the content region of this tab control is within the update
		//	//region of the window
		//	RECT dummyRect;
		//	bool childContentNeedsUpdating = (IntersectRect(&dummyRect, &tabControlContentRect, &updateRect) != 0);

		//	//If the content region of this tab control is in the update region,
		//	//invalidate the portion of the selected content that needs to be redrawn, and
		//	//exclude the content region from the tab control update region.
		//	if(childContentNeedsUpdating)
		//	{
		//		//Locate the content entry for the currently selected tab
		//		std::list<ContentEntry>::const_iterator hostedContentIterator = hostedContent.begin();
		//		bool foundSelectedHostedContent = false;
		//		while(!foundSelectedHostedContent && (hostedContentIterator != hostedContent.end()))
		//		{
		//			if(hostedContentIterator->tabIndex == currentSelectedTabIndex)
		//			{
		//				foundSelectedHostedContent = true;
		//				continue;
		//			}
		//			++hostedContentIterator;
		//		}

		//		//Request the selected content of this tab control to redraw
		//		if(foundSelectedHostedContent)
		//		{
		//			//Redraw the non-client area of the selected content
		//			const ContentEntry& selectedHostedContent = *hostedContentIterator;
		//			RedrawWindow(selectedHostedContent.contentWindow, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);

		//			//Retrieve the location of the selected content window in screen
		//			//coordinates
		//			RECT selectedContentWindowRect;
		//			GetWindowRect(selectedHostedContent.contentWindow, &selectedContentWindowRect);
		//			int windowScreenPosX = selectedContentWindowRect.left;
		//			int windowScreenPosY = selectedContentWindowRect.top;

		//			//Retrieve the position of the client region of the selected content
		//			//in screen coordinates
		//			POINT point;
		//			point.x = 0;
		//			point.y = 0;
		//			ClientToScreen(selectedHostedContent.contentWindow, &point);
		//			int clientScreenPosX = point.x;
		//			int clientScreenPosY = point.y;

		//			//Calculate the left and top border size between the start of the
		//			//window region and the start of the client area for the target
		//			//content window
		//			int windowBorderSizeX = clientScreenPosX - windowScreenPosX;
		//			int windowBorderSizeY = clientScreenPosY - windowScreenPosY;

		//			//Retrieve the size of the client region of the selected content
		//			RECT selectedContentClientRect;
		//			GetClientRect(selectedHostedContent.contentWindow, &selectedContentClientRect);
		//			int clientWidth = selectedContentClientRect.right - selectedContentClientRect.left;
		//			int clientHeight = selectedContentClientRect.bottom - selectedContentClientRect.top;

		//			//Calculate the rectangle which represents the client region of the
		//			//selected content of the tab control, relative to the client area of
		//			//the tab control.
		//			RECT contentClientRectInTabControlClientSpace;
		//			contentClientRectInTabControlClientSpace.left = tabControlContentRect.left + windowBorderSizeX;
		//			contentClientRectInTabControlClientSpace.right = contentClientRectInTabControlClientSpace.left + clientWidth;
		//			contentClientRectInTabControlClientSpace.top = tabControlContentRect.top + windowBorderSizeY;
		//			contentClientRectInTabControlClientSpace.bottom = contentClientRectInTabControlClientSpace.top + clientHeight;

		//			//Calculate the update rectangle for the client region of the selected
		//			//content
		//			RECT childUpdateRect;
		//			BOOL intersectRectReturn = IntersectRect(&childUpdateRect, &contentClientRectInTabControlClientSpace, &updateRect);

		//			//If an update rectangle was successfully calculated for the client
		//			//region of our selected content window, invalidate that region now.
		//			if(intersectRectReturn != 0)
		//			{
		//				//Convert the selected content client update region rectangle from
		//				//coordinates relative to the parent tab control into client
		//				//coordinates
		//				childUpdateRect.left -= (tabControlContentRect.left + windowBorderSizeX);
		//				childUpdateRect.right -= (tabControlContentRect.left + windowBorderSizeX);
		//				childUpdateRect.top -= (tabControlContentRect.top + windowBorderSizeY);
		//				childUpdateRect.bottom -= (tabControlContentRect.top + windowBorderSizeY);

		//				//Invalidate the required region of the client area of the target
		//				//content window
		//				InvalidateRect(selectedHostedContent.contentWindow, &childUpdateRect, FALSE);
		//			}
		//		}

		//		//Ensure the content area of the tab control is excluded from the update
		//		//region of the tab control
		//		ValidateRect(contentTabControl, &tabControlContentRect);
		//	}
		//}
		else if(bounceMessage->uMsg == WM_LBUTTONDOWN)
		{
			//Flag that the left mouse button is currently down
			leftMouseButtonDown = true;

			//Capture the mouse so that we can receive a notification when the left mouse
			//button is released
			SetCapture(contentTabControl);

			//Calculate the index of the tab that the cursor is currently over
			int cursorPosX = (short)LOWORD(bounceMessage->lParam);
			int cursorPosY = (short)HIWORD(bounceMessage->lParam);
			TCHITTESTINFO hitTestInfo;
			hitTestInfo.pt.x = cursorPosX;
			hitTestInfo.pt.y = cursorPosY;
			int currentCursorTabIndex = (int)SendMessage(contentTabControl, TCM_HITTEST, 0, (LPARAM)&hitTestInfo);

			//If the tab under the target cursor position is the same as the currently
			//selected tab, flag that a tab drag event for the tab is currently in
			//progress, otherwise wait for the tab selection change event to fire before
			//we start the tab drag event. We need this check to handle the case where a
			//partially visible tab is selected. In this case, the tab is repositioned
			//when it is selected, and we need to ensure we don't confuse this with a drag
			//event caused by the user.
			if(currentCursorTabIndex == currentSelectedTabIndex)
			{
				tabDragActive = true;
				dragTabIndex = currentCursorTabIndex;
				tabWithDeadZoneForDrag = -1;
				tabDragIgnoreTabIndex = false;
			}
		}
		else if(bounceMessage->uMsg == WM_LBUTTONUP)
		{
			//Flag that the left mouse button is no longer down
			leftMouseButtonDown = false;

			//Release the mouse capture now that the left mouse button has been released
			ReleaseCapture();

			//Stop the tab drag operation if one is in progress
			tabDragActive = false;
		}
		else if(bounceMessage->uMsg == WM_MOUSEMOVE)
		{
			if(tabDragActive)
			{
				//Calculate the tab index that the cursor is currently over
				int cursorPosX = (short)LOWORD(bounceMessage->lParam);
				int cursorPosY = (short)HIWORD(bounceMessage->lParam);
				TCHITTESTINFO hitTestInfo;
				hitTestInfo.pt.x = cursorPosX;
				hitTestInfo.pt.y = cursorPosY;
				int selectedTabIndex = (int)SendMessage(contentTabControl, TCM_HITTEST, 0, (LPARAM)&hitTestInfo);

				//If we're currently set to ignore a particular tab index during tab drag
				//events, and the cursor is currently in a location that's returning that
				//tab index, abort any further processing, otherwise disable tab index
				//ignoring now that the current tab index under the mouse position is
				//different. See the notes under TCN_SELCHANGE for more info.
				if(tabDragIgnoreTabIndex && (tabDragIndexToIgnore == selectedTabIndex))
				{
					return 0;
				}
				tabDragIgnoreTabIndex = false;

				//If the tab the mouse is currently over has a dead zone, abort any
				//further processing if the mouse is within the dead zone.
				if((tabWithDeadZoneForDrag >= 0) && (selectedTabIndex == tabWithDeadZoneForDrag))
				{
					RECT rect;
					SendMessage(contentTabControl, TCM_GETITEMRECT, selectedTabIndex, (LPARAM)&rect);
					int cursorPositionXInTab = cursorPosX - rect.left;
					if((cursorPositionXInTab >= tabDeadZoneStartX) && (cursorPositionXInTab < tabDeadZoneEndX))
					{
						return 0;
					}
				}

				//Clear any dead zone if one is active, since the mouse has now moved
				//outside the tab with the dead zone.
				tabWithDeadZoneForDrag = -1;

				//If the tab has been moved, either before or after another tab in the
				//window, or out of the control, adjust the tab being dragged as required.
				if(selectedTabIndex < 0)
				{
					//Stop the tab drag operation
					tabDragActive = false;

					//Flag that the left mouse button is no longer down, and release the
					//mouse capture. We do this because we're about to undock the target
					//content item from our frame and initiate a move operation on it, but
					//it can't recieve mouse input until we release the mouse capture.
					leftMouseButtonDown = false;
					ReleaseCapture();

					//Retrieve information on the dragged tab. Note that we deliberately
					//copy the content item here rather than reference it, because we're
					//about to remove the content window in order to perform the drag
					//operation, and we need to copy its information before we do.
					ContentEntry contentEntry = hostedContent[tabIndexToHostedContentNo[dragTabIndex]];

					//Determine the window style of our separated docking window
					DWORD separatedDockingWindowStyle = WS_SIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION;
					DWORD separatedDockingWindowStyleEX = WS_EX_TOOLWINDOW;

					//Calculate the width and height of our separated docking window
					RECT rect;
					GetClientRect(contentEntry.contentWindow, &rect);
					AdjustWindowRectEx(&rect, separatedDockingWindowStyle, FALSE, separatedDockingWindowStyleEX);
					int separatedDockingWindowWidth = rect.right - rect.left;
					int separatedDockingWindowHeight = rect.bottom - rect.top;
					int separatedDockingWindowNonClientTopHeight = -rect.top;

					//Calculate the position of our separated docking window
					POINT point;
					point.x = cursorPosX;
					point.y = cursorPosY;
					ClientToScreen(contentTabControl, &point);
					int separatedDockingWindowPosX = point.x - (separatedDockingWindowWidth / 2);
					int separatedDockingWindowPosY = point.y - (separatedDockingWindowNonClientTopHeight / 2);

					//Remove the dragged tab from the list of hosted content for this
					//window
					RemoveHostedContent(contentEntry.contentWindow);

					//Retrieve the first owner window at the top of the chain of parent
					//windows for our docking window
					HWND newOwnerWindow = GetFirstOwnerWindow(hwnd);

					//If we have no owner window at the top of the parent chain, set the
					//new owner window for the detached content window to be the our
					//top-level parent window.
					if(newOwnerWindow == NULL)
					{
						newOwnerWindow = GetAncestor(hwnd, GA_ROOT);
					}

					//Create a separate docking window for the detached content window
					HWND separatedDockingWindowHandle = CreateWindowEx(separatedDockingWindowStyleEX, windowClassName, contentEntry.contentTitle.c_str(), separatedDockingWindowStyle, separatedDockingWindowPosX, separatedDockingWindowPosY, separatedDockingWindowWidth, separatedDockingWindowHeight, newOwnerWindow, NULL, (HINSTANCE)moduleHandle, NULL);

					//Set the font for the created detached content window
					if(controlFont != NULL)
					{
						SendMessage(separatedDockingWindowHandle, WM_SETFONT, (WPARAM)controlFont, FALSE);
					}

					//Add the detached content window to the created docking container
					DockingWindow* separatedDockingWindow = (DockingWindow*)GetWindowLongPtr(separatedDockingWindowHandle, GWLP_USERDATA);
					separatedDockingWindow->AddHostedContent(contentEntry.contentWindow, contentEntry.contentTitle);

					//Show and activate the new docking container
					ShowWindow(separatedDockingWindowHandle, SW_SHOW);

					//Enter move mode for the docking container
					PostMessage(separatedDockingWindowHandle, WM_SYSCOMMAND, SC_MOVE, 0);
				}
				else if(selectedTabIndex != dragTabIndex)
				{
					//Retrieve information on the dragged tab
					const ContentEntry& contentEntry = hostedContent[tabIndexToHostedContentNo[dragTabIndex]];

					//Adjust the index numbers for all the tab entries to account for the
					//tab being moved, and adjust the currently selected tab index.
					tabIndexToHostedContentNo.clear();
					int newCurrentSelectedTabIndex = currentSelectedTabIndex;
					for(size_t i = 0; i < hostedContent.size(); ++i)
					{
						ContentEntry& contentEntry = hostedContent[i];
						int newTabIndex = contentEntry.tabIndex;
						if(contentEntry.tabIndex == dragTabIndex)
						{
							newTabIndex = selectedTabIndex;
						}
						else if((dragTabIndex > selectedTabIndex) && (contentEntry.tabIndex >= selectedTabIndex) && (contentEntry.tabIndex < dragTabIndex))
						{
							newTabIndex = contentEntry.tabIndex + 1;
						}
						else if((dragTabIndex < selectedTabIndex) && (contentEntry.tabIndex > dragTabIndex) && (contentEntry.tabIndex <= selectedTabIndex))
						{
							newTabIndex = contentEntry.tabIndex - 1;
						}
						if(contentEntry.tabIndex == currentSelectedTabIndex)
						{
							newCurrentSelectedTabIndex = newTabIndex;
						}
						contentEntry.tabIndex = newTabIndex;
						tabIndexToHostedContentNo[contentEntry.tabIndex] = i;
					}
					currentSelectedTabIndex = newCurrentSelectedTabIndex;

					//Move the tab being dragged around in the tab control
					SendMessage(contentTabControl, TCM_DELETEITEM, dragTabIndex, 0);
					TCITEM tabControlItem;
					tabControlItem.mask = TCIF_PARAM | TCIF_TEXT;
					tabControlItem.lParam = (LPARAM)contentEntry.contentWindow;
					tabControlItem.pszText = (LPWSTR)contentEntry.contentTitle.c_str();
					SendMessage(contentTabControl, TCM_INSERTITEM, contentEntry.tabIndex, (LPARAM)&tabControlItem);

					//Update the index number of the tab currently being dragged
					dragTabIndex = selectedTabIndex;

					//If we're docked to a parent window in auto hide mode, request the
					//parent window to update its list of tabs for this container now that
					//our list of hosted content windows has changed.
					if((parentDockingWindow != 0) && autoHideDockedWindow)
					{
						parentDockingWindow->UpdateAutoHideChildContainerContent(this);
					}

					//If after the drag operation is complete, the mouse is currently over
					//a different tab than the tab being dragged, create a dead zone in
					//this tab. This deals with the case where a tab is moved before or
					//after another tab, and the other tab has a longer label than the tab
					//being moved. In this case, the mouse will end up over the other tab,
					//not the tab that was being moved, which would trigger a constant
					//loop of moving the tab back and forth. Creating this pixel dead zone
					//allows us to avoid the loop, while still allowing the tab to be
					//moved back to its previous position if it is shifted further along
					//in the tab we ended up over.
					int adjustedSelectedTabIndex = (int)SendMessage(contentTabControl, TCM_HITTEST, 0, (LPARAM)&hitTestInfo);
					if(adjustedSelectedTabIndex != dragTabIndex)
					{
						//Record the tab we ended up on as having a dead zone
						tabWithDeadZoneForDrag = adjustedSelectedTabIndex;

						//Calculate the start and end positions for the dead zone
						RECT rect;
						SendMessage(contentTabControl, TCM_GETITEMRECT, adjustedSelectedTabIndex, (LPARAM)&rect);
						if(adjustedSelectedTabIndex < dragTabIndex)
						{
							tabDeadZoneStartX = cursorPosX - rect.left;
							tabDeadZoneEndX = rect.right - rect.left;
						}
						else
						{
							tabDeadZoneStartX = 0;
							tabDeadZoneEndX = cursorPosX - rect.left;
						}
					}
				}
			}
		}
	}
	else if(bounceMessage->hwnd == dockPanel)
	{
		if(bounceMessage->uMsg == WM_NOTIFY)
		{
			NMHDR* nmhdr = (NMHDR*)bounceMessage->lParam;
			if(nmhdr->hwndFrom == contentTabControl)
			{
				if(nmhdr->code == TCN_SELCHANGE)
				{
					//Handle the tab selection change
					int selectedTabIndex = (int)SendMessage(contentTabControl, TCM_GETCURSEL, 0, 0);
					HandleContentTabChanged(selectedTabIndex);

					//If a tab drag isn't currently flagged as active, and the left mouse
					//button is currently down, and a valid tab was selected, we've
					//deferred the start of a tab drag event until the new tab selection
					//has been completed. In this case, we begin a drag event for the
					//newly selected tab item.
					if(!tabDragActive && leftMouseButtonDown && (selectedTabIndex >= 0))
					{
						//Flag that a tab drag event is currently in progress
						tabDragActive = true;
						dragTabIndex = selectedTabIndex;
						tabWithDeadZoneForDrag = -1;
						tabDragIgnoreTabIndex = false;

						//Calculate the current cursor position in client coordinates
						//relative to the tab control
						POINT point;
						GetCursorPos(&point);
						ScreenToClient(contentTabControl, &point);
						int cursorPosX = point.x;
						int cursorPosY = point.y;

						//Calculate the index of the tab that the mouse is currently over
						TCHITTESTINFO hitTestInfo;
						hitTestInfo.pt.x = cursorPosX;
						hitTestInfo.pt.y = cursorPosY;
						int currentCursorTabIndex = (int)SendMessage(contentTabControl, TCM_HITTEST, 0, (LPARAM)&hitTestInfo);

						//If the index of the tab the mouse is currently over doesn't
						//match the selected tab index, the user may have selected a tab
						//which was partially visible, forcing it to be moved when it was
						//brought into focus. In this case, we flag to ignore tab drag
						//events until the current tab index under the mouse cursor
						//changes. This avoids inadvertent moves or undocks of partially
						//visible tabs when they are just clicked by the user.
						if(currentCursorTabIndex != selectedTabIndex)
						{
							tabDragIgnoreTabIndex = true;
							tabDragIndexToIgnore = currentCursorTabIndex;
						}
					}
				}
			}
		}
		else if(bounceMessage->uMsg == WM_PARENTNOTIFY)
		{
			//If a child window of our dock panel is closing, attempt to locate and remove
			//the corresponding content window for the window that is closing.
			if(LOWORD(bounceMessage->wParam) == WM_DESTROY)
			{
				//Attempt to locate a content window entry for the closing window
				HWND closingChildWindow = (HWND)bounceMessage->lParam;
				bool foundContentEntry = false;
				size_t hostedContentNo = 0;
				while(!foundContentEntry && (hostedContentNo < hostedContent.size()))
				{
					if(hostedContent[hostedContentNo].contentWindow == closingChildWindow)
					{
						foundContentEntry = true;
						continue;
					}
					++hostedContentNo;
				}

				//If we found the content entry for this closing window, remove the
				//content entry from our dock panel, and close our dock panel if no more
				//content windows remain.
				if(foundContentEntry)
				{
					//Remove the target hosted content window
					RemoveHostedContent(hostedContent[hostedContentNo].contentWindow);

					//Close this dock window if no content windows remain
					if(hostedContent.empty())
					{
						SendMessage(hwnd, WM_CLOSE, 0, 0);
					}
				}
			}
		}
		else if(bounceMessage->uMsg == WM_BOUNCE)
		{
			//Tunnel into children of the dock panel that have bounceback enabled. We need
			//this so we can receive content tab control messages.
			return msgWM_BOUNCE(bounceMessage->wParam, bounceMessage->lParam);
		}
	}
	else if(bounceMessage->hwnd == tabDockPanel)
	{
		//Tunnel into children of the tab dock panel that have bounceback enabled. We need
		//this so we can receive the inner dock panel messages.
		if(bounceMessage->uMsg == WM_BOUNCE)
		{
			return msgWM_BOUNCE(bounceMessage->wParam, bounceMessage->lParam);
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_ADDCONTENTWINDOW(WPARAM wParam, LPARAM lParam)
{
	const AddContentWindowParams& params = *((const AddContentWindowParams*)lParam);
	AddHostedContent(params.hwnd, params.windowTitle);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_REMOVECONTENTWINDOW(WPARAM wParam, LPARAM lParam)
{
	HWND hostedContent = (HWND)lParam;
	RemoveHostedContent(hostedContent);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_MODIFYCONTENTWINDOW(WPARAM wParam, LPARAM lParam)
{
	unsigned int contentEntryNo = (unsigned int)wParam;
	const ModifyContentWindowParams& params = *((const ModifyContentWindowParams*)lParam);
	if((params.contentWindowInfo & ContentWindowInfo::Handle) != 0)
	{
		SetHostedContentWindow(contentEntryNo, params.hwnd);
	}
	if((params.contentWindowInfo & ContentWindowInfo::Title) != 0)
	{
		SetHostedContentTitle(contentEntryNo, params.windowTitle);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_ADDDOCKEDWINDOW(WPARAM wParam, LPARAM lParam)
{
	//Retrieve the docking window associated with the supplied window handle
	const AddDockedWindowParams& params = *((const AddDockedWindowParams*)lParam);
	IDockingWindow* newDockedWindow = GetDockingWindowFromHWND(params.hwnd);
	if(newDockedWindow == 0)
	{
		return -1;
	}

	//Add the specified docking window as a child container
	newDockedWindow->DockToNewParent(this, params.dockLocation, params.autoHide, params.forceToTopOfDockingOrder);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_REMOVEDOCKEDWINDOW(WPARAM wParam, LPARAM lParam)
{
	HWND dockedWindowHandle = (HWND)lParam;
	IDockingWindow* dockedWindow = GetDockingWindowFromHWND(dockedWindowHandle);
	if(dockedWindow != 0)
	{
		if(dockedWindow->GetParentDockingWindow() == this)
		{
			dockedWindow->UndockFromParent();
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_GETCONTENTWINDOWINDEXFROMHANDLE(WPARAM wParam, LPARAM lParam)
{
	HWND windowHandle = (HWND)lParam;
	for(unsigned int currentEntryNo = 0; currentEntryNo < hostedContent.size(); ++currentEntryNo)
	{
		if(hostedContent[currentEntryNo].contentWindow == windowHandle)
		{
			return (LRESULT)currentEntryNo;
		}
	}
	return (LRESULT)-1;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_GETDOCKEDWINDOWINDEXFROMHANDLE(WPARAM wParam, LPARAM lParam)
{
	HWND windowHandle = (HWND)lParam;
	unsigned int currentEntryNo = 0;
	for(std::list<ChildContainerEntry>::const_iterator i = childContainers.begin(); i != childContainers.end(); ++i)
	{
		if(i->childContainer->GetWindowHandle() == windowHandle)
		{
			return (LRESULT)currentEntryNo;
		}
		++currentEntryNo;
	}
	return (LRESULT)-1;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_GETCONTENTWINDOWCOUNT(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(unsigned int)hostedContent.size();
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_GETCONTENTWINDOWINFO(WPARAM wParam, LPARAM lParam)
{
	//Retrieve the requested content entry
	size_t targetEntryNo = (size_t)(unsigned int)wParam;
	if(targetEntryNo >= hostedContent.size())
	{
		return 1;
	}
	const ContentEntry& targetEntry = hostedContent[targetEntryNo];

	//Return information on the requested content entry to the caller
	GetContentWindowInfo& params = *((GetContentWindowInfo*)lParam);
	params.hwnd = targetEntry.contentWindow;
	params.windowTitle = targetEntry.contentTitle.c_str();
	params.contentIndex = (unsigned int)targetEntryNo;
	params.tabIndex = targetEntry.tabIndex;
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_GETDOCKEDWINDOWCOUNT(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(unsigned int)childContainers.size();
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_GETDOCKEDWINDOWINFO(WPARAM wParam, LPARAM lParam)
{
	//Retrieve the requested docked child window entry
	size_t targetEntryNo = (size_t)(unsigned int)wParam;
	size_t currentEntryNo = 0;
	std::list<ChildContainerEntry>::const_iterator childContainerIterator = childContainers.begin();
	while((currentEntryNo < targetEntryNo) && (childContainerIterator != childContainers.end()))
	{
		++currentEntryNo;
		++childContainerIterator;
	}
	if((currentEntryNo != targetEntryNo) || (childContainerIterator == childContainers.end()))
	{
		return 1;
	}
	const ChildContainerEntry& targetEntry = *childContainerIterator;

	//Return information on the requested docked child window entry to the caller
	GetDockedWindowInfo& params = *((GetDockedWindowInfo*)lParam);
	params.hwnd = targetEntry.childContainer->GetWindowHandle();
	params.dockLocation = targetEntry.dockLocation;
	params.autoHide = targetEntry.autoHide;
	if(params.autoHide)
	{
		RECT rect;
		GetWindowRect(params.hwnd, &rect);
		params.desiredWidth = (int)(rect.right - rect.left);
		params.desiredHeight = (int)(rect.bottom - rect.top);
	}
	else
	{
		params.desiredWidth = (int)SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::GetDockedWindowDesiredWidth, 0, (LPARAM)params.hwnd);
		params.desiredHeight = (int)SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::GetDockedWindowDesiredHeight, 0, (LPARAM)params.hwnd);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_GETACTIVECONTENTWINDOW(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)GetActiveContent();
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgDOCKWIN_SETACTIVECONTENTWINDOW(WPARAM wParam, LPARAM lParam)
{
	SetActiveContent((unsigned int)wParam);
	return 0;
}

//----------------------------------------------------------------------------------------
//Tab tray message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK DockingWindow::TabTrayWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		DockingWindow* object = (DockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->TabTrayWndProcPrivate(hwnd, message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		DockingWindow* object = (DockingWindow*)createStruct->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->TabTrayWndProcPrivate(hwnd, message, wParam, lParam);}
	case WM_DESTROY:{
		DockingWindow* object = (DockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			LPARAM result = object->TabTrayWndProcPrivate(hwnd, message, wParam, lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
			return result;
		}
		break;}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::TabTrayWndProcPrivate(HWND tabTrayHwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_SIZE:
		return msgTabTrayWM_SIZE(tabTrayHwnd, wParam, lParam);
	case WM_TIMER:
		return msgTabTrayWM_TIMER(tabTrayHwnd, wParam, lParam);
	case WM_MOUSEMOVE:
		return msgTabTrayWM_MOUSEMOVE(tabTrayHwnd, wParam, lParam);
	case WM_PAINT:
		return msgTabTrayWM_PAINT(tabTrayHwnd, wParam, lParam);
	case WM_PRINTCLIENT:
		return msgTabTrayWM_PRINTCLIENT(tabTrayHwnd, wParam, lParam);
	}

	return DefWindowProc(tabTrayHwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgTabTrayWM_SIZE(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam)
{
	//Invalidate the entire tab tray client region whenever its size changes. We need to
	//do this to ensure that changes to tab wrapping force a redraw of the window.
	InvalidateRect(tabTrayHwnd, NULL, FALSE);

	//Pass this size event on to the default message handler
	return DefWindowProc(tabTrayHwnd, WM_SIZE, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgTabTrayWM_TIMER(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam)
{
	//Ensure an auto hide panel is currently visible, and kill this timer if it is not.
	if(!autoHidePanelVisible)
	{
		KillTimer(tabTrayHwnd, 1);
		return 0;
	}

	//Retrieve the current cursor position
	POINT point;
	GetCursorPos(&point);
	int cursorPosX = point.x;
	int cursorPosY = point.y;

	//If the cursor is within the extended hit region of the tab tray that owns the
	//currently visible auto hide dock panel, abort any further processing.
	if((cursorPosX >= tabExtendedHitRegionX) && (cursorPosX < (tabExtendedHitRegionX + tabExtendedHitRegionWidth)) && (cursorPosY >= tabExtendedHitRegionY) && (cursorPosY < (tabExtendedHitRegionY + tabExtendedHitRegionHeight)))
	{
		return 0;
	}

	//If the cursor is within the window region of the docking tab, or in an allowed
	//"grace" region around the currently visible auto hide dock panel, abort any further
	//processing. We need to allow the panel to remain visible here if the mouse is within
	//its boundary, regardless of the actual window that's currently under the mouse
	//cursor, so that a window can be dragged into a tab, even though the window under the
	//mouse cursor will be the window being dragged, not the visible auto hide dock panel.
	//This shouldn't cause any problems with normal use since the auto hide dock panel
	//should always be above all other child windows in the dock panel, and the dock panel
	//must be activated currently in order for the tab panel to process mouse events on
	//the auto hide dock tab panels, meaning it should be above all non-topmost windows in
	//the z-order, and we should ignore topmost windows anyway, as we need to exclude our
	//topmost placement target windows. The grace region makes it easier to move the mouse
	//onto resize borders or the caption bar of the auto hide dock panel without
	//accidentally moving a few pixels off it and triggering the window to immediately
	//hide.
	RECT rect;
	GetWindowRect(currentAutoHidePanel->GetWindowHandle(), &rect);
	int dockPanelBorderGraceSize = 10;
	int dockPanelGraceRegionPosX = (rect.left - dockPanelBorderGraceSize);
	int dockPanelGraceRegionPosY = (rect.top - dockPanelBorderGraceSize);
	int dockPanelGraceRegionWidth = (rect.right - rect.left) + (2 * dockPanelBorderGraceSize);
	int dockPanelGraceRegionHeight = (rect.bottom - rect.top) + (2 * dockPanelBorderGraceSize);
	if((cursorPosX >= dockPanelGraceRegionPosX) && (cursorPosX < (dockPanelGraceRegionPosX + dockPanelGraceRegionWidth)) && (cursorPosY >= dockPanelGraceRegionPosY) && (cursorPosY < (dockPanelGraceRegionPosY + dockPanelGraceRegionHeight)))
	{
		return 0;
	}

	//Ensure the placement targets are hidden for the currently visible auto hide dock
	//panel
	currentAutoHidePanel->HidePlacementTargets();

	//Collapse the dock panel
	CollapseHiddenDockPanel();

	//Kill this timer
	KillTimer(tabTrayHwnd, 1);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgTabTrayWM_MOUSEMOVE(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam)
{
	//If our docking panel isn't currently the active window or within the active window,
	//abort any further processing.
	HWND activeWindow = GetActiveWindow();
	HWND desktopWindow = GetDesktopWindow();
	HWND searchWindow = hwnd;
	while((searchWindow != NULL) && (searchWindow != activeWindow) && (searchWindow != desktopWindow))
	{
		searchWindow = GetAncestor(searchWindow, GA_PARENT);
	}
	if((searchWindow == NULL) || (searchWindow == desktopWindow))
	{
		return 0;
	}

	//Extract the cursor position for this message relative to the control
	int cursorPosX = (short)LOWORD(lParam);
	int cursorPosY = (short)HIWORD(lParam);

	//Perform hit testing to see if any tab is under the cursor
	TabHitTest(tabTrayHwnd, cursorPosX, cursorPosY);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgTabTrayWM_PAINT(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam)
{
	//Attempt to retrieve the target tab dock info
	std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::iterator dockIterator = autoHideDocks.begin();
	bool foundTargetDock = false;
	while(!foundTargetDock && (dockIterator != autoHideDocks.end()))
	{
		if(dockIterator->second.dockWindow == tabTrayHwnd)
		{
			foundTargetDock = true;
			continue;
		}
		++dockIterator;
	}
	if(!foundTargetDock)
	{
		return 0;
	}
	AutoHideDockInfo& dockInfo = dockIterator->second;

	//Save the initial row count of this tab dock panel
	int initialDockRowCount = dockInfo.tabRowCount;

	//Begin the paint operation
	PAINTSTRUCT paintInfo;
	BeginPaint(tabTrayHwnd, &paintInfo);
	HDC hdc = paintInfo.hdc;

	//Calculate the width and height of this control
	RECT rect;
	GetClientRect(tabTrayHwnd, &rect);
	int tabTrayWidth = rect.right - rect.left;
	int tabTrayHeight = rect.bottom - rect.top;

	//Create a bitmap we can render the control onto
	HDC hdcControl = hdc;
	HDC hdcBitmap = CreateCompatibleDC(hdcControl);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdcControl, tabTrayWidth, tabTrayHeight);
	HBITMAP hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

	//Send a WM_PRINTCLIENT message to the native control and get it to render into
	//the bitmap
	SendMessage(tabTrayHwnd, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);

	//Transfer the final bitmap for the control into the screen buffer
	BitBlt(hdcControl, 0, 0, tabTrayWidth, tabTrayHeight, hdcBitmap, 0, 0, SRCCOPY);

	//Validate the entire client area of the control. It's REALLY important that we
	//call this here, otherwise Windows is going to send WM_PAINT messages over and
	//over again waiting for the region to be validated.
	ValidateRect(tabTrayHwnd, NULL);

	//Clean up the allocated handles
	SelectObject(hdcBitmap, hbitmapOriginal);
	DeleteObject(hbitmap);
	DeleteDC(hdcBitmap);

	//End the paint operation
	EndPaint(tabTrayHwnd, &paintInfo);

	//If we calculated that a new size for this dock tab tray is required, resize the
	//control now. Note that we need to do this after we've validated the content region,
	//so that a new WM_PAINT message will be generated after the resize event.
	int tabTrayRowSize = (controlFontHeight + (dockingPanelTabMarginSize * 3));
	if(dockInfo.tabRowCount != initialDockRowCount)
	{
		//Update the size of this tab tray
		if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Top) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom))
		{
			SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredHeight, (dockInfo.tabRowCount * tabTrayRowSize), (LPARAM)tabTrayHwnd);
		}
		else
		{
			SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredWidth, (dockInfo.tabRowCount * tabTrayRowSize), (LPARAM)tabTrayHwnd);
		}

		//If this tab tray is docked to the top or bottom, force any left or right docked
		//tab trays to redraw too, since the top and bottom padding sizes have changed.
		if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Top) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom))
		{
			for(std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::const_iterator i = autoHideDocks.begin(); i != autoHideDocks.end(); ++i)
			{
				if((i->first == WC_DockPanel::DockLocation::Left) || (i->first == WC_DockPanel::DockLocation::Right))
				{
					InvalidateRect(i->second.dockWindow, NULL, FALSE);
				}
			}
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgTabTrayWM_PRINTCLIENT(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam)
{
	//Calculate the width and height of this control
	RECT rect;
	GetClientRect(tabTrayHwnd, &rect);
	int tabTrayWidth = rect.right - rect.left;
	int tabTrayHeight = rect.bottom - rect.top;
	int tabTrayRowSize = (controlFontHeight + (dockingPanelTabMarginSize * 3));

	//Attempt to retrieve the target tab dock info
	std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::iterator dockIterator = autoHideDocks.begin();
	bool foundTargetDock = false;
	while(!foundTargetDock && (dockIterator != autoHideDocks.end()))
	{
		if(dockIterator->second.dockWindow == tabTrayHwnd)
		{
			foundTargetDock = true;
			continue;
		}
		++dockIterator;
	}
	if(!foundTargetDock)
	{
		return 0;
	}
	AutoHideDockInfo& dockInfo = dockIterator->second;

	//Determine the extra padding margin sizes we need to include at the start or end of
	//this dock tab tray, due to corner regions being present from another dock tab tray
	//adjoining this one.
	int verticalTabTrayStartPadding = 0;
	int verticalTabTrayEndPadding = 0;
	if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Left) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Right))
	{
		for(std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::const_iterator i = autoHideDocks.begin(); i != autoHideDocks.end(); ++i)
		{
			if(i->first == WC_DockPanel::DockLocation::Top)
			{
				verticalTabTrayStartPadding += i->second.tabRowCount * tabTrayRowSize;
			}
			else if(i->first == WC_DockPanel::DockLocation::Bottom)
			{
				verticalTabTrayEndPadding += i->second.tabRowCount * tabTrayRowSize;
			}
		}
	}

	//Fill the window with the background colour
	HDC hdc = (HDC)wParam;
	HBRUSH backgroundBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
	PatBlt(hdc, 0, 0, tabTrayWidth, tabTrayHeight, PATCOPY);
	SelectObject(hdc, backgroundBrushOld);
	DeleteObject(backgroundBrush);

	//Switch to our font
	HFONT fontToUseForTab = ((dockInfo.dockLocation == WC_DockPanel::DockLocation::Left) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Right))? controlFontVertical: controlFont;
	HFONT hfontOld = (HFONT)SelectObject(hdc, fontToUseForTab);

	//Set the font colours
	SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
	SetBkMode(hdc, TRANSPARENT);

	//Draw each tab in this tray
	int nextTabGroupWindowPosX = (dockInfo.dockLocation == WC_DockPanel::DockLocation::Right)? dockingPanelTabMarginSize: 0;
	int nextTabGroupWindowPosY = (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom)? dockingPanelTabMarginSize: 0;
	nextTabGroupWindowPosY += verticalTabTrayStartPadding;
	int newTabRowCount = 1;
	for(std::list<AutoHideDockTabGroup>::iterator tabGroupIterator = dockInfo.dockTabGroups.begin(); tabGroupIterator != dockInfo.dockTabGroups.end(); ++tabGroupIterator)
	{
		//Calculate the position and size of each tab entry within this tab group, and
		//calculate the total width and height of the tab group.
		AutoHideDockTabGroup& tabGroupInfo = *tabGroupIterator;
		int nextTabWindowPosX = 0;
		int nextTabWindowPosY = 0;
		std::list<TabRenderInfo> tabRenderInfoList;
		for(std::list<AutoHideDockTab>::iterator tabIterator = tabGroupInfo.dockTabs.begin(); tabIterator != tabGroupInfo.dockTabs.end(); ++tabIterator)
		{
			//Attempt to retrieve the content entry index for the content window
			//associated with this tab
			AutoHideDockTab& dockTab = *tabIterator;
			unsigned int contentEntryNo;
			if(!tabGroupInfo.childContainer->GetHostedContentIndexFromWindow(dockTab.contentWindow, contentEntryNo))
			{
				continue;
			}

			//Retrieve the title for the content window associated with this tab
			std::wstring contentWindowTitle = tabGroupInfo.childContainer->GetHostedContentTitle(contentEntryNo);

			//Calculate the required size of this tab
			int newTabWidth = dockTab.tabWidth;
			int newTabHeight = dockTab.tabHeight;
			SIZE textSize;
			BOOL getTextExtentPoint32Return = GetTextExtentPoint32(hdc, contentWindowTitle.c_str(), (int)contentWindowTitle.size(), &textSize);
			if(getTextExtentPoint32Return != 0)
			{
				if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Left) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Right))
				{
					newTabWidth = controlFontHeight + (dockingPanelTabMarginSize * 2);
					newTabHeight = textSize.cx + (dockingPanelTabMarginSize * 4);
				}
				else
				{
					newTabWidth = textSize.cx + (dockingPanelTabMarginSize * 4);
					newTabHeight = controlFontHeight + (dockingPanelTabMarginSize * 2);
				}
			}

			//Calculate the position and size of the border for this tab
			int borderPosX = nextTabWindowPosX;
			int borderPosY = nextTabWindowPosY;
			int borderWidth = newTabWidth;
			int borderHeight = newTabHeight;
			switch(dockInfo.dockLocation)
			{
			case WC_DockPanel::DockLocation::Left:
				borderPosX -= borderWidth;
				borderWidth *= 2;
				break;
			case WC_DockPanel::DockLocation::Right:
				borderWidth *= 2;
				break;
			case WC_DockPanel::DockLocation::Top:
				borderPosY -= borderHeight;
				borderHeight *= 2;
				break;
			case WC_DockPanel::DockLocation::Bottom:
				borderHeight *= 2;
				break;
			}

			//If another tab just preceded this one within the same tab group, ensure that
			//the borders for the two tabs share a single dividing line between them.
			if(!tabRenderInfoList.empty())
			{
				if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Top) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom))
				{
					borderPosX -= 1;
					borderWidth += 1;
				}
				else
				{
					borderPosY -= 1;
					borderHeight += 1;
				}
			}

			//Build a TabRenderInfo structure for this tab, and add it to the list of tab
			//render info structures.
			TabRenderInfo tabRenderInfo(dockTab, contentWindowTitle);
			tabRenderInfo.tabPosX = nextTabWindowPosX;
			tabRenderInfo.tabPosY = nextTabWindowPosY;
			tabRenderInfo.tabWidth = newTabWidth;
			tabRenderInfo.tabHeight = newTabHeight;
			tabRenderInfo.borderPosX = borderPosX;
			tabRenderInfo.borderPosY = borderPosY;
			tabRenderInfo.borderWidth = borderWidth;
			tabRenderInfo.borderHeight = borderHeight;
			tabRenderInfoList.push_back(tabRenderInfo);

			//Calculate the position of the next tab in this tab group
			if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Top) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom))
			{
				nextTabWindowPosX += newTabWidth;
			}
			else
			{
				nextTabWindowPosY += newTabHeight;
			}
		}
		int tabGroupWidth = nextTabWindowPosX;
		int tabGroupHeight = nextTabWindowPosY;

		//Calculate the position of this tab group
		if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Top) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom))
		{
			if((tabGroupIterator != dockInfo.dockTabGroups.begin()) && ((nextTabGroupWindowPosX + tabGroupWidth) > tabTrayWidth))
			{
				nextTabGroupWindowPosX = 0;
				nextTabGroupWindowPosY += tabTrayRowSize;
				++newTabRowCount;
			}
		}
		else
		{
			if((tabGroupIterator != dockInfo.dockTabGroups.begin()) && ((nextTabGroupWindowPosY + tabGroupHeight) > (tabTrayHeight - verticalTabTrayEndPadding)))
			{
				nextTabGroupWindowPosX += tabTrayRowSize;
				nextTabGroupWindowPosY = verticalTabTrayStartPadding;
				++newTabRowCount;
			}
		}
		int tabGroupWindowPosX = nextTabGroupWindowPosX;
		int tabGroupWindowPosY = nextTabGroupWindowPosY;

		//Draw each tab in this group
		for(std::list<TabRenderInfo>::const_iterator tabRenderInfoIterator = tabRenderInfoList.begin(); tabRenderInfoIterator != tabRenderInfoList.end(); ++tabRenderInfoIterator)
		{
			//Create a new pen to draw the tab border
			const TabRenderInfo& tabRenderInfo = *tabRenderInfoIterator;
			HPEN hpen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
			HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);

			//Setup a clipping region to restrict the border drawing area to the allocated
			//region for this tab. We deliberately pass outside the target tab region when
			//drawing the border, in order to create the cut-off edge of the tab border,
			//so we need a clipping region when drawing more than one row of tabs to
			//ensure we don't overwrite the content of other tabs.
			RECT rect;
			rect.left = tabGroupWindowPosX + tabRenderInfo.tabPosX;
			rect.right = tabGroupWindowPosX + tabRenderInfo.tabPosX + tabRenderInfo.tabWidth;
			rect.top = tabGroupWindowPosY + tabRenderInfo.tabPosY;
			rect.bottom = tabGroupWindowPosY + tabRenderInfo.tabPosY + tabRenderInfo.tabHeight;
			HRGN borderClippingRegion = CreateRectRgnIndirect(&rect);
			SelectClipRgn(hdc, borderClippingRegion);

			//Draw the border for this tab
			RoundRect(hdc, tabGroupWindowPosX + tabRenderInfo.borderPosX, tabGroupWindowPosY + tabRenderInfo.borderPosY, tabGroupWindowPosX + tabRenderInfo.borderPosX + tabRenderInfo.borderWidth, tabGroupWindowPosY + tabRenderInfo.borderPosY + tabRenderInfo.borderHeight, 5, 5);

			//Remove the border clipping region for this tab
			SelectClipRgn(hdc, NULL);
			DeleteObject(borderClippingRegion);

			//Destroy the pen object used to draw the tab border
			SelectObject(hdc, hpenOld);
			DeleteObject(hpen);

			//Calculate the position for the text in this tab
			int textPosX = tabGroupWindowPosX + tabRenderInfo.tabPosX + dockingPanelTabMarginSize;
			int textPosY = tabGroupWindowPosY + tabRenderInfo.tabPosY + dockingPanelTabMarginSize;
			if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Left) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Right))
			{
				SetTextAlign(hdc, TA_BASELINE);
				textPosX += controlFontDescent;
				textPosY += dockingPanelTabMarginSize;
			}
			else
			{
				textPosX += dockingPanelTabMarginSize;
			}

			//Draw the text for this tab
			TextOut(hdc, textPosX, textPosY, tabRenderInfo.windowTitle.c_str(), (int)tabRenderInfo.windowTitle.size());

			//Save the position and size of this tab for hit testing purposes
			tabRenderInfo.dockTabInfo.tabPosX = tabGroupWindowPosX + tabRenderInfo.tabPosX;
			tabRenderInfo.dockTabInfo.tabPosY = tabGroupWindowPosY + tabRenderInfo.tabPosY;
			tabRenderInfo.dockTabInfo.tabWidth = tabRenderInfo.tabWidth;
			tabRenderInfo.dockTabInfo.tabHeight = tabRenderInfo.tabHeight;
		}

		//Calculate the initial position for the next docking tab group
		if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Top) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom))
		{
			nextTabGroupWindowPosX = tabGroupWindowPosX + tabGroupWidth + dockingPanelTabGroupSeparatorSize;
			nextTabGroupWindowPosY = tabGroupWindowPosY;
		}
		else
		{
			nextTabGroupWindowPosX = tabGroupWindowPosX;
			nextTabGroupWindowPosY = tabGroupWindowPosY + tabGroupHeight + dockingPanelTabGroupSeparatorSize;
		}
	}

	//Update the current tab row count for this tab tray
	dockInfo.tabRowCount = newTabRowCount;

	//Restore the state of the device context, and free any allocated handles
	SelectObject(hdc, hfontOld);

	return 0;
}

//----------------------------------------------------------------------------------------
//Placement target message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK DockingWindow::PlacementTargetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		DockingWindow* object = (DockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->PlacementTargetWndProcPrivate(hwnd, message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		DockingWindow* object = (DockingWindow*)createStruct->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->PlacementTargetWndProcPrivate(hwnd, message, wParam, lParam);}
	case WM_DESTROY:{
		DockingWindow* object = (DockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			LPARAM result = object->PlacementTargetWndProcPrivate(hwnd, message, wParam, lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
			return result;
		}
		break;}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::PlacementTargetWndProcPrivate(HWND placementTargetHwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		return msgPlacementTargetWM_PAINT(placementTargetHwnd, wParam, lParam);
	case WM_PRINTCLIENT:
		return msgPlacementTargetWM_PRINTCLIENT(placementTargetHwnd, wParam, lParam);
	}

	return DefWindowProc(placementTargetHwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgPlacementTargetWM_PAINT(HWND placementTargetHwnd, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintInfo;
	BeginPaint(placementTargetHwnd, &paintInfo);
	HDC hdc = paintInfo.hdc;

	//Calculate the width and height of this control
	RECT rect;
	GetClientRect(placementTargetHwnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	//Create a bitmap we can render the control onto
	HDC hdcControl = hdc;
	HDC hdcBitmap = CreateCompatibleDC(hdcControl);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdcControl, windowWidth, windowHeight);
	HBITMAP hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

	//Send a WM_PRINTCLIENT message to the native control and get it to render into
	//the bitmap.
	SendMessage(placementTargetHwnd, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);

	//Transfer the final bitmap for the control into the screen buffer
	BitBlt(hdcControl, 0, 0, windowWidth, windowHeight, hdcBitmap, 0, 0, SRCCOPY);

	//Validate the entire client area of the control. It's REALLY important that we
	//call this here, otherwise Windows is going to send WM_PAINT messages over and
	//over again waiting for the region to be validated.
	ValidateRect(placementTargetHwnd, NULL);

	//Clean up the allocated handles
	SelectObject(hdcBitmap, hbitmapOriginal);
	DeleteObject(hbitmap);
	DeleteDC(hdcBitmap);

	EndPaint(placementTargetHwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgPlacementTargetWM_PRINTCLIENT(HWND placementTargetHwnd, WPARAM wParam, LPARAM lParam)
{
	//Set some measurement constants for this window
	int borderWidth = 1;
	int marginSize = 3;
	int windowImageBorderWidth = 1;
	int dockingRegionTopInsertMarginSize = 2;

	//Calculate the width and height of this control
	RECT rect;
	GetClientRect(placementTargetHwnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	//Attempt to determine which location we're targeting
	WC_DockPanel::DockLocation targetLocation;
	bool targetLocationIsCenter = (placementTargetCenter == placementTargetHwnd);
	bool targetLocationIsForceTop = false;
	bool foundTargetLocation = targetLocationIsCenter;
	std::map<WC_DockPanel::DockLocation, HWND>::iterator placementTargetIterator = placementTargets.begin();
	while(!foundTargetLocation && (placementTargetIterator != placementTargets.end()))
	{
		if(placementTargetIterator->second == placementTargetHwnd)
		{
			foundTargetLocation = true;
			targetLocation = placementTargetIterator->first;
			continue;
		}
		++placementTargetIterator;
	}
	std::map<WC_DockPanel::DockLocation, HWND>::iterator placementTargetForceTopIterator = placementTargetsForceTop.begin();
	while(!foundTargetLocation && (placementTargetForceTopIterator != placementTargetsForceTop.end()))
	{
		if(placementTargetForceTopIterator->second == placementTargetHwnd)
		{
			foundTargetLocation = true;
			targetLocationIsForceTop = true;
			targetLocation = placementTargetForceTopIterator->first;
			continue;
		}
		++placementTargetForceTopIterator;
	}
	if(!foundTargetLocation)
	{
		return 0;
	}

	//Select the colours to use for this window
	WinColor backgroundColor(240, 240, 240);
	WinColor borderColor(176, 176, 176);
	WinColor windowBorderColor(56, 80, 128);
	WinColor windowBackgroundColor(224, 224, 224);
	WinColor windowDockTargetColor(255, 215, 135);
	if(targetLocationIsCenter && !placementTargetCenterEnabled)
	{
		windowBorderColor = WinColor(128, 128, 128);
		windowDockTargetColor = WinColor(255, 245, 225);
	}

	//Calculate the size of the window image inside this window
	int windowImageHeight;
	int windowImageWidth;
	if(targetLocationIsCenter)
	{
		windowImageWidth = windowWidth - (marginSize * 2);
		windowImageHeight = windowHeight - (marginSize * 2);
	}
	else if(!targetLocationIsForceTop)
	{
		if((targetLocation == WC_DockPanel::DockLocation::Left) || (targetLocation == WC_DockPanel::DockLocation::Right))
		{
			windowImageWidth = windowWidth - dockingRegionTopInsertMarginSize;
			windowImageHeight = windowHeight - ((borderWidth + marginSize) * 2);
		}
		else
		{
			windowImageWidth = windowWidth - ((borderWidth + marginSize) * 2);
			windowImageHeight = windowHeight - dockingRegionTopInsertMarginSize;
		}
	}
	else
	{
		if((targetLocation == WC_DockPanel::DockLocation::Left) || (targetLocation == WC_DockPanel::DockLocation::Right))
		{
			windowImageWidth = windowWidth - (borderWidth + marginSize);
			windowImageHeight = windowHeight - ((borderWidth + marginSize) * 2);
		}
		else
		{
			windowImageWidth = windowWidth - ((borderWidth + marginSize) * 2);
			windowImageHeight = windowHeight - (borderWidth + marginSize);
		}
	}

	//Calculate the size of the forced top insertion arrow icon
	int dockingRegionTopInsertIconArrowWidth = 0;
	int dockingRegionTopInsertIconArrowHeight = 0;
	if(targetLocationIsForceTop)
	{
		if((targetLocation == WC_DockPanel::DockLocation::Left) || (targetLocation == WC_DockPanel::DockLocation::Right))
		{
			dockingRegionTopInsertIconArrowWidth = windowImageWidth - (borderWidth * 4);
			dockingRegionTopInsertIconArrowHeight = dockingRegionTopInsertIconArrowWidth * 2;
		}
		else
		{
			dockingRegionTopInsertIconArrowHeight = windowImageHeight - (borderWidth * 4);
			dockingRegionTopInsertIconArrowWidth = dockingRegionTopInsertIconArrowHeight * 2;
		}
	}

	//Calculate the position of the window image inside this window
	int windowImagePosX;
	int windowImagePosY;
	if(targetLocationIsCenter)
	{
		windowImagePosX = marginSize;
		windowImagePosY = marginSize;
	}
	else if(!targetLocationIsForceTop)
	{
		if(targetLocation == WC_DockPanel::DockLocation::Left)
		{
			windowImagePosX = dockingRegionTopInsertMarginSize;
			windowImagePosY = borderWidth + marginSize;
		}
		else if(targetLocation == WC_DockPanel::DockLocation::Right)
		{
			windowImagePosX = 0;
			windowImagePosY = borderWidth + marginSize;
		}
		else if(targetLocation == WC_DockPanel::DockLocation::Top)
		{
			windowImagePosX = borderWidth + marginSize;
			windowImagePosY = dockingRegionTopInsertMarginSize;
		}
		else if(targetLocation == WC_DockPanel::DockLocation::Bottom)
		{
			windowImagePosX = borderWidth + marginSize;
			windowImagePosY = 0;
		}
	}
	else
	{
		windowImagePosX = (targetLocation == WC_DockPanel::DockLocation::Right)? 0: borderWidth + marginSize;
		windowImagePosY = (targetLocation == WC_DockPanel::DockLocation::Bottom)? 0: borderWidth + marginSize;
	}

	//Calculate the position of the forced top insertion arrow icon
	int dockingRegionTopInsertIconArrowPosX = 0;
	int dockingRegionTopInsertIconArrowPosY = 0;
	if(targetLocationIsForceTop)
	{
		dockingRegionTopInsertIconArrowPosX = windowImagePosX + ((windowImageWidth - dockingRegionTopInsertIconArrowWidth) / 2);
		dockingRegionTopInsertIconArrowPosY = windowImagePosY + ((windowImageHeight - dockingRegionTopInsertIconArrowHeight) / 2);
	}

	//Calculate the height of the window image caption
	int windowImageCaptionHeight = (!targetLocationIsForceTop)? ((windowImageHeight + 4) / 8): windowImageBorderWidth;

	//Calculate the size and position of the client region within the window image
	int windowImageClientPosX = windowImagePosX + windowImageBorderWidth;
	int windowImageClientPosY = windowImagePosY + windowImageCaptionHeight;
	int windowImageClientWidth = windowImageWidth - (windowImageBorderWidth * 2);
	int windowImageClientHeight = windowImageHeight - (windowImageCaptionHeight + windowImageBorderWidth);

	//Calculate the size and position of the dock target region within the window image
	int dockTargetImagePosX;
	int dockTargetImagePosY;
	int dockTargetImageWidth;
	int dockTargetImageHeight;
	if(targetLocationIsCenter)
	{
		dockTargetImagePosX = windowImageClientPosX;
		dockTargetImagePosY = windowImageClientPosY;
		dockTargetImageWidth = windowImageClientWidth;
		dockTargetImageHeight = windowImageClientHeight;
	}
	else if(!targetLocationIsForceTop)
	{
		dockTargetImagePosX = (targetLocation == WC_DockPanel::DockLocation::Right)? windowImageClientPosX + (windowImageClientWidth / 2): windowImageClientPosX;
		dockTargetImagePosY = (targetLocation == WC_DockPanel::DockLocation::Bottom)? windowImageClientPosY + (windowImageClientHeight / 2): windowImageClientPosY;
		if((targetLocation == WC_DockPanel::DockLocation::Left) || (targetLocation == WC_DockPanel::DockLocation::Right))
		{
			dockTargetImageWidth = (windowImageClientWidth / 2);
			dockTargetImageHeight = windowImageClientHeight;
		}
		else
		{
			dockTargetImageWidth = windowImageClientWidth;
			dockTargetImageHeight = (windowImageClientHeight / 2);
		}
	}

	//Create each pen and brush object we require for this draw process
	HBRUSH backgroundBrush = CreateSolidBrush(backgroundColor.GetColorREF());
	HBRUSH windowBorderBrush = CreateSolidBrush(windowBorderColor.GetColorREF());
	HBRUSH windowBackgroundBrush = CreateSolidBrush(windowBackgroundColor.GetColorREF());
	HBRUSH windowDockTargetBrush = CreateSolidBrush(windowDockTargetColor.GetColorREF());

	//Fill the window with the background colour
	HDC hdc = (HDC)wParam;
	HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
	PatBlt(hdc, 0, 0, windowWidth, windowHeight, PATCOPY);
	SelectObject(hdc, backgroundBrushOld);

	//Draw the border for this window
	if(!targetLocationIsCenter)
	{
		HPEN hpen = CreatePen(PS_SOLID, 0, borderColor.GetColorREF());
		HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
		if((targetLocation == WC_DockPanel::DockLocation::Left) || (targetLocation == WC_DockPanel::DockLocation::Right))
		{
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, windowWidth, 0);
			MoveToEx(hdc, 0, windowHeight-1, NULL);
			LineTo(hdc, windowWidth, windowHeight-1);
		}
		else
		{
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, 0, windowHeight);
			MoveToEx(hdc, windowWidth-1, 0, NULL);
			LineTo(hdc, windowWidth-1, windowHeight);
		}
		if(targetLocationIsForceTop)
		{
			if(targetLocation == WC_DockPanel::DockLocation::Left)
			{
				MoveToEx(hdc, 0, 0, NULL);
				LineTo(hdc, 0, windowHeight);
			}
			else if(targetLocation == WC_DockPanel::DockLocation::Right)
			{
				MoveToEx(hdc, windowWidth-1, 0, NULL);
				LineTo(hdc, windowWidth-1, windowHeight);
			}
			else if(targetLocation == WC_DockPanel::DockLocation::Top)
			{
				MoveToEx(hdc, 0, 0, NULL);
				LineTo(hdc, windowWidth, 0);
			}
			else if(targetLocation == WC_DockPanel::DockLocation::Bottom)
			{
				MoveToEx(hdc, 0, windowHeight-1, NULL);
				LineTo(hdc, windowWidth, windowHeight-1);
			}
		}
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);
	}

	//Draw the window image border
	RECT windowBorderRect;
	windowBorderRect.left = windowImagePosX;
	windowBorderRect.right = windowImagePosX + windowImageWidth;
	windowBorderRect.top = windowImagePosY;
	windowBorderRect.bottom = windowImagePosY + windowImageHeight;
	FrameRect(hdc, &windowBorderRect, windowBorderBrush);

	//Draw the window image caption
	if(!targetLocationIsForceTop)
	{
		RECT windowCaptionRect;
		windowCaptionRect.left = windowImagePosX;
		windowCaptionRect.right = windowImagePosX + windowImageWidth;
		windowCaptionRect.top = windowImagePosY;
		windowCaptionRect.bottom = windowImageClientPosY;
		FillRect(hdc, &windowCaptionRect, windowBorderBrush);
	}

	//Draw the window image background
	RECT windowBackgroundRect;
	windowBackgroundRect.left = windowImageClientPosX;
	windowBackgroundRect.right = windowImageClientPosX + windowImageClientWidth;
	windowBackgroundRect.top = windowImageClientPosY;
	windowBackgroundRect.bottom = windowImageClientPosY + windowImageClientHeight;
	FillRect(hdc, &windowBackgroundRect, windowBackgroundBrush);

	//Draw the window image dock target background
	if(!targetLocationIsForceTop)
	{
		RECT windowDockTargetRect;
		windowDockTargetRect.left = dockTargetImagePosX;
		windowDockTargetRect.right = dockTargetImagePosX + dockTargetImageWidth;
		windowDockTargetRect.top = dockTargetImagePosY;
		windowDockTargetRect.bottom = dockTargetImagePosY + dockTargetImageHeight;
		FillRect(hdc, &windowDockTargetRect, windowDockTargetBrush);
	}

	//Draw a stippled line over the edge of the dock target region
	if(!targetLocationIsForceTop && !targetLocationIsCenter)
	{
		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = windowBorderColor.GetColorREF();
		HPEN hpen = ExtCreatePen(PS_COSMETIC | PS_ALTERNATE, 1, &logBrush, 0, NULL);
		HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
		if(targetLocation == WC_DockPanel::DockLocation::Left)
		{
			MoveToEx(hdc, dockTargetImagePosX + (dockTargetImageWidth - 1), dockTargetImagePosY, NULL);
			LineTo(hdc, dockTargetImagePosX + (dockTargetImageWidth - 1), dockTargetImagePosY + dockTargetImageHeight);
		}
		else if(targetLocation == WC_DockPanel::DockLocation::Right)
		{
			MoveToEx(hdc, dockTargetImagePosX, dockTargetImagePosY, NULL);
			LineTo(hdc, dockTargetImagePosX, dockTargetImagePosY + dockTargetImageHeight);
		}
		else if(targetLocation == WC_DockPanel::DockLocation::Top)
		{
			MoveToEx(hdc, dockTargetImagePosX, dockTargetImagePosY + (dockTargetImageHeight - 1), NULL);
			LineTo(hdc, dockTargetImagePosX + dockTargetImageWidth, dockTargetImagePosY + (dockTargetImageHeight - 1));
		}
		else if(targetLocation == WC_DockPanel::DockLocation::Bottom)
		{
			MoveToEx(hdc, dockTargetImagePosX, dockTargetImagePosY, NULL);
			LineTo(hdc, dockTargetImagePosX + dockTargetImageWidth, dockTargetImagePosY);
		}
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);
	}

	//Draw the forced top insertion icon arrow
	if(targetLocationIsForceTop)
	{
		HPEN dockingRegionTopInsertIconArrowPen = CreatePen(PS_SOLID, 0, windowBorderColor.GetColorREF());
		HPEN dockingRegionTopInsertIconArrowPenOld = (HPEN)SelectObject(hdc, dockingRegionTopInsertIconArrowPen);
		HBRUSH dockingRegionTopInsertIconArrowBrushOld = (HBRUSH)SelectObject(hdc, windowBorderBrush);
		POINT dockingRegionTopInsertIconArrowPoints[4];
		if(targetLocation == WC_DockPanel::DockLocation::Left)
		{
			dockingRegionTopInsertIconArrowPoints[0].x = dockingRegionTopInsertIconArrowPosX + (dockingRegionTopInsertIconArrowWidth - 1);
			dockingRegionTopInsertIconArrowPoints[0].y = dockingRegionTopInsertIconArrowPosY;
			dockingRegionTopInsertIconArrowPoints[1].x = dockingRegionTopInsertIconArrowPosX;
			dockingRegionTopInsertIconArrowPoints[1].y = dockingRegionTopInsertIconArrowPosY + ((dockingRegionTopInsertIconArrowHeight - 1) / 2);
			dockingRegionTopInsertIconArrowPoints[2].x = dockingRegionTopInsertIconArrowPosX;
			dockingRegionTopInsertIconArrowPoints[2].y = dockingRegionTopInsertIconArrowPosY + (dockingRegionTopInsertIconArrowHeight / 2);
			dockingRegionTopInsertIconArrowPoints[3].x = dockingRegionTopInsertIconArrowPosX + (dockingRegionTopInsertIconArrowWidth - 1);
			dockingRegionTopInsertIconArrowPoints[3].y = dockingRegionTopInsertIconArrowPosY + (dockingRegionTopInsertIconArrowHeight - 1);
		}
		else if(targetLocation == WC_DockPanel::DockLocation::Right)
		{
			dockingRegionTopInsertIconArrowPoints[0].x = dockingRegionTopInsertIconArrowPosX;
			dockingRegionTopInsertIconArrowPoints[0].y = dockingRegionTopInsertIconArrowPosY;
			dockingRegionTopInsertIconArrowPoints[1].x = dockingRegionTopInsertIconArrowPosX + (dockingRegionTopInsertIconArrowWidth - 1);
			dockingRegionTopInsertIconArrowPoints[1].y = dockingRegionTopInsertIconArrowPosY + ((dockingRegionTopInsertIconArrowHeight - 1) / 2);
			dockingRegionTopInsertIconArrowPoints[2].x = dockingRegionTopInsertIconArrowPosX + (dockingRegionTopInsertIconArrowWidth - 1);
			dockingRegionTopInsertIconArrowPoints[2].y = dockingRegionTopInsertIconArrowPosY + (dockingRegionTopInsertIconArrowHeight / 2);
			dockingRegionTopInsertIconArrowPoints[3].x = dockingRegionTopInsertIconArrowPosX;
			dockingRegionTopInsertIconArrowPoints[3].y = dockingRegionTopInsertIconArrowPosY + (dockingRegionTopInsertIconArrowHeight - 1);
		}
		else if(targetLocation == WC_DockPanel::DockLocation::Top)
		{
			dockingRegionTopInsertIconArrowPoints[0].x = dockingRegionTopInsertIconArrowPosX;
			dockingRegionTopInsertIconArrowPoints[0].y = dockingRegionTopInsertIconArrowPosY + (dockingRegionTopInsertIconArrowHeight - 1);
			dockingRegionTopInsertIconArrowPoints[1].x = dockingRegionTopInsertIconArrowPosX + ((dockingRegionTopInsertIconArrowWidth - 1) / 2);
			dockingRegionTopInsertIconArrowPoints[1].y = dockingRegionTopInsertIconArrowPosY;
			dockingRegionTopInsertIconArrowPoints[2].x = dockingRegionTopInsertIconArrowPosX + (dockingRegionTopInsertIconArrowWidth / 2);
			dockingRegionTopInsertIconArrowPoints[2].y = dockingRegionTopInsertIconArrowPosY;
			dockingRegionTopInsertIconArrowPoints[3].x = dockingRegionTopInsertIconArrowPosX + (dockingRegionTopInsertIconArrowWidth - 1);
			dockingRegionTopInsertIconArrowPoints[3].y = dockingRegionTopInsertIconArrowPosY + (dockingRegionTopInsertIconArrowHeight - 1);
		}
		else if(targetLocation == WC_DockPanel::DockLocation::Bottom)
		{
			dockingRegionTopInsertIconArrowPoints[0].x = dockingRegionTopInsertIconArrowPosX;
			dockingRegionTopInsertIconArrowPoints[0].y = dockingRegionTopInsertIconArrowPosY;
			dockingRegionTopInsertIconArrowPoints[1].x = dockingRegionTopInsertIconArrowPosX + ((dockingRegionTopInsertIconArrowWidth - 1) / 2);
			dockingRegionTopInsertIconArrowPoints[1].y = dockingRegionTopInsertIconArrowPosY + (dockingRegionTopInsertIconArrowHeight - 1);
			dockingRegionTopInsertIconArrowPoints[2].x = dockingRegionTopInsertIconArrowPosX + (dockingRegionTopInsertIconArrowWidth / 2);
			dockingRegionTopInsertIconArrowPoints[2].y = dockingRegionTopInsertIconArrowPosY + (dockingRegionTopInsertIconArrowHeight - 1);
			dockingRegionTopInsertIconArrowPoints[3].x = dockingRegionTopInsertIconArrowPosX + (dockingRegionTopInsertIconArrowWidth - 1);
			dockingRegionTopInsertIconArrowPoints[3].y = dockingRegionTopInsertIconArrowPosY;
		}
		Polygon(hdc, &dockingRegionTopInsertIconArrowPoints[0], sizeof(dockingRegionTopInsertIconArrowPoints) / sizeof(dockingRegionTopInsertIconArrowPoints[0]));
		SelectObject(hdc, dockingRegionTopInsertIconArrowBrushOld);
		SelectObject(hdc, dockingRegionTopInsertIconArrowPenOld);
		DeleteObject(dockingRegionTopInsertIconArrowPen);
	}

	//Delete each pen and brush object which was created for this draw process
	DeleteObject(backgroundBrush);
	DeleteObject(windowBorderBrush);
	DeleteObject(windowBackgroundBrush);
	DeleteObject(windowDockTargetBrush);

	return 0;
}

//----------------------------------------------------------------------------------------
//Placement shadow message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK DockingWindow::PlacementShadowWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		DockingWindow* object = (DockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->PlacementShadowWndProcPrivate(hwnd, message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		DockingWindow* object = (DockingWindow*)createStruct->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->PlacementShadowWndProcPrivate(hwnd, message, wParam, lParam);}
	case WM_DESTROY:{
		DockingWindow* object = (DockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			LPARAM result = object->PlacementShadowWndProcPrivate(hwnd, message, wParam, lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
			return result;
		}
		break;}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::PlacementShadowWndProcPrivate(HWND placementShadowHwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		return msgPlacementShadowWM_PAINT(placementShadowHwnd, wParam, lParam);
	}

	return DefWindowProc(placementShadowHwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DockingWindow::msgPlacementShadowWM_PAINT(HWND placementShadowHwnd, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintInfo;
	BeginPaint(placementShadowHwnd, &paintInfo);
	HDC hdc = paintInfo.hdc;

	//Calculate the width and height of this control
	RECT rect;
	GetClientRect(placementShadowHwnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	//Fill the window with the background colour
	WinColor backgroundColor(32, 64, 255);
	HBRUSH backgroundBrush = CreateSolidBrush(backgroundColor.GetColorREF());
	HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
	PatBlt(hdc, 0, 0, windowWidth, windowHeight, PATCOPY);
	SelectObject(hdc, backgroundBrushOld);
	DeleteObject(backgroundBrush);

	//Validate the entire client area of the control. It's REALLY important that we
	//call this here, otherwise Windows is going to send WM_PAINT messages over and
	//over again waiting for the region to be validated.
	ValidateRect(placementShadowHwnd, NULL);

	EndPaint(placementShadowHwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
//Helper methods
//----------------------------------------------------------------------------------------
IDockingWindow* DockingWindow::GetDockingWindowFromHWND(HWND hwnd)
{
	//Retrieve the class name of the target window
	size_t windowClassNameLength = wcslen(windowClassName);
	size_t classNameBufferSize = windowClassNameLength + 2;
	std::vector<wchar_t> classNameBuffer(classNameBufferSize);
	int getClassNameReturn = GetClassName(hwnd, &classNameBuffer[0], (int)classNameBufferSize);
	if(getClassNameReturn == 0)
	{
		return 0;
	}

	//Ensure the class name of the target window matches our window class name
	int stringCompareReturn = wcscmp(windowClassName, &classNameBuffer[0]);
	if(stringCompareReturn != 0)
	{
		return 0;
	}

	//Retrieve the IDockingWindow object for the target window, and return it to the
	//caller.
	IDockingWindow* targetDockingWindow = (IDockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	return targetDockingWindow;
}

//----------------------------------------------------------------------------------------
BOOL CALLBACK DockingWindow::EnumDockingWindowsProc(HWND hwnd, LPARAM lParam)
{
	std::list<IDockingWindow*>& dockingWindowList = *((std::list<IDockingWindow*>*)lParam);
	AddDockingWindowToWindowList(hwnd, dockingWindowList);
	return TRUE;
}

//----------------------------------------------------------------------------------------
void DockingWindow::AddDockingWindowToWindowList(HWND hwnd, std::list<IDockingWindow*>& dockingWindowList)
{
	//If this window is visible, attempt to add it or its child windows to the docking
	//window list. Note that although this search seems heavy handed, it seems to be the
	//only reliable way to detect other docking windows that may be nested within a main
	//frame that is not a docking window, without relying on some kind of shared global
	//static buffer of current windows. Also note that performance tests have shown this
	//to be quite quick, with no perceivable stalls in the interface from evaluating this
	//on each mouse move event for 1000 windows iterated.
	if(IsWindowVisible(hwnd) != 0)
	{
		//If this window is a docking window, it and its child docking containers to the
		//list of docking windows, otherwise search into the children of this window
		//looking for docking windows.
		IDockingWindow* dockingWindow = GetDockingWindowFromHWND(hwnd);
		if(dockingWindow != 0)
		{
			//Add all children of this docking window to the window list, then add this
			//window itself. Note that the ordering is important here, as the list is
			//sorted in Z-order, from highest to lowest, and we need child frames of a
			//docking window to appear above their parent in the list in order to not be
			//obscured by it.
			std::list<IDockingWindow*> childDockingWindowList = dockingWindow->GetNestedChildDockingWindowList();
			dockingWindowList.insert(dockingWindowList.end(), childDockingWindowList.begin(), childDockingWindowList.end());
			dockingWindowList.push_back(dockingWindow);
		}
		else
		{
			//Iterate through each child window of this window and attempt to add each one
			//as a docking window. Note that we don't use EnumChildWindows here because
			//real-world cases were observed where it didn't appear to be returning
			//windows in Z-order, and indeed, the official documentation gives no
			//guarantee that it should. The official documentation also gives no guarantee
			//on the ordering of EnumThreadWindows for that matter, but observation shows
			//it does indeed seem to always return in Z-order, and its widely relied upon
			//to do this.
			HWND nextWindow = GetWindow(hwnd, GW_CHILD);
			while(nextWindow != NULL)
			{
				AddDockingWindowToWindowList(nextWindow, dockingWindowList);
				nextWindow = GetWindow(nextWindow, GW_HWNDNEXT);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
HCURSOR DockingWindow::LoadCursorFile(const unsigned char* cursorData, size_t cursorDataSize)
{
	//Load the selected cursor data into a stream buffer
	Stream::Buffer iconData;
	iconData.WriteData(cursorData, cursorDataSize);
	iconData.SetStreamPos(0);

	//Allocate stream buffers for each icon contained within the selected cursor file
	unsigned int iconEntryCount = GetIconFileEntryCount(iconData);
	std::vector<Stream::Buffer> iconResourceData;
	iconResourceData.resize(iconEntryCount);
	std::map<int, Stream::IStream*> iconResourceDataMap;
	for(unsigned int i = 0; i < iconEntryCount; ++i)
	{
		iconResourceDataMap[(int)i] = &iconResourceData[i];
	}

	//Attempt to convert the specified cursor file into cursor resource structures
	Stream::Buffer iconResourceDirectory;
	if(!ConvertIconFileToIconResource(iconData, iconResourceDirectory, iconResourceDataMap))
	{
		return NULL;
	}

	//Select the most appropriate cursor resource to use within the cursor file
	int selectedIconID = LookupIconIdFromDirectory((PBYTE)&iconResourceDirectory[0], FALSE);
	if(iconResourceDataMap.find(selectedIconID) == iconResourceDataMap.end())
	{
		return NULL;
	}

	//Load the selected cursor as a resource
	std::vector<unsigned char> selectedIconResourceData;
	iconResourceDataMap[selectedIconID]->ReadData(selectedIconResourceData, iconResourceDataMap[selectedIconID]->Size());
	HICON iconHandle = CreateIconFromResourceEx((PBYTE)&selectedIconResourceData[0], (int)selectedIconResourceData.size(), FALSE, 0x30000, 0, 0, 0);

	//Cast the icon handle to a cursor handle, and return it to the caller.
	HCURSOR iconHandleAsCursorHandle = (HCURSOR)iconHandle;
	return iconHandleAsCursorHandle;
}

//----------------------------------------------------------------------------------------
//Window handle methods
//----------------------------------------------------------------------------------------
HWND DockingWindow::GetWindowHandle() const
{
	return hwnd;
}

//----------------------------------------------------------------------------------------
HWND DockingWindow::GetDockPanelWindowHandle() const
{
	return dockPanel;
}

//----------------------------------------------------------------------------------------
//Placement content methods
//----------------------------------------------------------------------------------------
void DockingWindow::HidePlacementTargets()
{
	//Begin a session for processing this batch of window size changes. Processing all the
	//window size and position changes in a single operation in this manner gives the best
	//performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos(10);

	//Hide the placement shadow window
	DeferWindowPos(deferWindowPosSession, placementShadow, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	placementShadowVisible = false;

	//Hide all the placement target windows
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Left], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Right], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Top], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Bottom], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Left], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Right], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Top], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Bottom], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetCenter, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);

	//Process all the window size and position changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);
}

//----------------------------------------------------------------------------------------
void DockingWindow::ShowPlacementTargets(bool allowCenterDocking)
{
	//Record whether the center placement target is enabled
	placementTargetCenterEnabled = allowCenterDocking;

	//Calculate the size of the docking region and its position in client coordinates
	//relative to the tab docking region
	RECT dockingRegionRect;
	SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::GetContentRect, 0, (LPARAM)&dockingRegionRect);
	int dockingRegionPosX = dockingRegionRect.left;
	int dockingRegionPosY = dockingRegionRect.top;
	int dockingRegionWidth = dockingRegionRect.right - dockingRegionRect.left;
	int dockingRegionHeight = dockingRegionRect.bottom - dockingRegionRect.top;

	//Calculate the position of the docking region in screen coordinates
	POINT point;
	point.x = dockingRegionPosX;
	point.y = dockingRegionPosY;
	ClientToScreen(dockPanel, &point);
	int dockingRegionScreenPosX = point.x;
	int dockingRegionScreenPosY = point.y;

	//Calculate the center position of the docking region in screen coordinates
	int dockingRegionCenterScreenPosX = dockingRegionScreenPosX + (dockingRegionWidth / 2);
	int dockingRegionCenterScreenPosY = dockingRegionScreenPosY + (dockingRegionHeight / 2);

	//Calculate the sizes of each docking region
	int borderWidth = 1;
	int marginSize = 3;
	int dockingRegionWindowContentSize = 24;
	int dockingRegionTopInsertMarginSize = 2;
	int dockingRegionTopInsertIconWidth = dockingRegionWindowContentSize / 2;
	int centerDockingIconWidth = dockingRegionWindowContentSize + (2 * marginSize);
	int centerDockingIconHeight = centerDockingIconWidth;
	std::map<WC_DockPanel::DockLocation, int> dockingIconWidth;
	std::map<WC_DockPanel::DockLocation, int> dockingIconHeight;
	std::map<WC_DockPanel::DockLocation, int> dockingIconForceTopWidth;
	std::map<WC_DockPanel::DockLocation, int> dockingIconForceTopHeight;
	dockingIconWidth[WC_DockPanel::DockLocation::Left] = dockingRegionWindowContentSize + dockingRegionTopInsertMarginSize;
	dockingIconHeight[WC_DockPanel::DockLocation::Left] = dockingRegionWindowContentSize + ((marginSize + borderWidth) * 2);
	dockingIconWidth[WC_DockPanel::DockLocation::Right] = dockingRegionWindowContentSize + dockingRegionTopInsertMarginSize;
	dockingIconHeight[WC_DockPanel::DockLocation::Right] = dockingRegionWindowContentSize + ((marginSize + borderWidth) * 2);
	dockingIconWidth[WC_DockPanel::DockLocation::Top] = dockingRegionWindowContentSize + ((marginSize + borderWidth) * 2);
	dockingIconHeight[WC_DockPanel::DockLocation::Top] = dockingRegionWindowContentSize + dockingRegionTopInsertMarginSize;
	dockingIconWidth[WC_DockPanel::DockLocation::Bottom] = dockingRegionWindowContentSize + ((marginSize + borderWidth) * 2);
	dockingIconHeight[WC_DockPanel::DockLocation::Bottom] = dockingRegionWindowContentSize + dockingRegionTopInsertMarginSize;
	dockingIconForceTopWidth[WC_DockPanel::DockLocation::Left] = dockingRegionTopInsertIconWidth + marginSize + borderWidth;
	dockingIconForceTopHeight[WC_DockPanel::DockLocation::Left] = dockingRegionWindowContentSize + ((marginSize + borderWidth) * 2);
	dockingIconForceTopWidth[WC_DockPanel::DockLocation::Right] = dockingRegionTopInsertIconWidth + marginSize + borderWidth;
	dockingIconForceTopHeight[WC_DockPanel::DockLocation::Right] = dockingRegionWindowContentSize + ((marginSize + borderWidth) * 2);
	dockingIconForceTopWidth[WC_DockPanel::DockLocation::Top] = dockingRegionWindowContentSize + ((marginSize + borderWidth) * 2);
	dockingIconForceTopHeight[WC_DockPanel::DockLocation::Top] = dockingRegionTopInsertIconWidth + marginSize + borderWidth;
	dockingIconForceTopWidth[WC_DockPanel::DockLocation::Bottom] = dockingRegionWindowContentSize + ((marginSize + borderWidth) * 2);
	dockingIconForceTopHeight[WC_DockPanel::DockLocation::Bottom] = dockingRegionTopInsertIconWidth + marginSize + borderWidth;

	//Calculate the position of each docking region
	int centerDockingIconPosX = dockingRegionCenterScreenPosX - (centerDockingIconWidth / 2);
	int centerDockingIconPosY = dockingRegionCenterScreenPosY - (centerDockingIconHeight / 2);
	std::map<WC_DockPanel::DockLocation, int> dockingIconPosX;
	std::map<WC_DockPanel::DockLocation, int> dockingIconPosY;
	std::map<WC_DockPanel::DockLocation, int> dockingIconForceTopPosX;
	std::map<WC_DockPanel::DockLocation, int> dockingIconForceTopPosY;
	dockingIconPosX[WC_DockPanel::DockLocation::Left] = centerDockingIconPosX - dockingIconWidth[WC_DockPanel::DockLocation::Left];
	dockingIconPosY[WC_DockPanel::DockLocation::Left] = centerDockingIconPosY - borderWidth;
	dockingIconPosX[WC_DockPanel::DockLocation::Right] = centerDockingIconPosX + centerDockingIconWidth;
	dockingIconPosY[WC_DockPanel::DockLocation::Right] = centerDockingIconPosY - borderWidth;
	dockingIconPosX[WC_DockPanel::DockLocation::Top] = centerDockingIconPosX - borderWidth;
	dockingIconPosY[WC_DockPanel::DockLocation::Top] = centerDockingIconPosY - dockingIconHeight[WC_DockPanel::DockLocation::Top];
	dockingIconPosX[WC_DockPanel::DockLocation::Bottom] = centerDockingIconPosX - borderWidth;
	dockingIconPosY[WC_DockPanel::DockLocation::Bottom] = centerDockingIconPosY + centerDockingIconHeight;
	dockingIconForceTopPosX[WC_DockPanel::DockLocation::Left] = dockingIconPosX[WC_DockPanel::DockLocation::Left] - dockingIconForceTopWidth[WC_DockPanel::DockLocation::Left];
	dockingIconForceTopPosY[WC_DockPanel::DockLocation::Left] = dockingIconPosY[WC_DockPanel::DockLocation::Left];
	dockingIconForceTopPosX[WC_DockPanel::DockLocation::Right] = dockingIconPosX[WC_DockPanel::DockLocation::Right] + dockingIconWidth[WC_DockPanel::DockLocation::Right];
	dockingIconForceTopPosY[WC_DockPanel::DockLocation::Right] = dockingIconPosY[WC_DockPanel::DockLocation::Right];
	dockingIconForceTopPosX[WC_DockPanel::DockLocation::Top] = dockingIconPosX[WC_DockPanel::DockLocation::Top];
	dockingIconForceTopPosY[WC_DockPanel::DockLocation::Top] = dockingIconPosY[WC_DockPanel::DockLocation::Top] - dockingIconForceTopHeight[WC_DockPanel::DockLocation::Top];
	dockingIconForceTopPosX[WC_DockPanel::DockLocation::Bottom] = dockingIconPosX[WC_DockPanel::DockLocation::Bottom];
	dockingIconForceTopPosY[WC_DockPanel::DockLocation::Bottom] = dockingIconPosY[WC_DockPanel::DockLocation::Bottom] + dockingIconHeight[WC_DockPanel::DockLocation::Bottom];

	//Begin a session for processing this batch of window size changes. Processing all the
	//window size and position changes in a single operation in this manner gives the best
	//performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos(18);

	//Position each placement target over the center content region of the target docking
	//window
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Left], NULL, dockingIconPosX[WC_DockPanel::DockLocation::Left], dockingIconPosY[WC_DockPanel::DockLocation::Left], dockingIconWidth[WC_DockPanel::DockLocation::Left], dockingIconHeight[WC_DockPanel::DockLocation::Left], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Right], NULL, dockingIconPosX[WC_DockPanel::DockLocation::Right], dockingIconPosY[WC_DockPanel::DockLocation::Right], dockingIconWidth[WC_DockPanel::DockLocation::Right], dockingIconHeight[WC_DockPanel::DockLocation::Right], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Top], NULL, dockingIconPosX[WC_DockPanel::DockLocation::Top], dockingIconPosY[WC_DockPanel::DockLocation::Top], dockingIconWidth[WC_DockPanel::DockLocation::Top], dockingIconHeight[WC_DockPanel::DockLocation::Top], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Bottom], NULL, dockingIconPosX[WC_DockPanel::DockLocation::Bottom], dockingIconPosY[WC_DockPanel::DockLocation::Bottom], dockingIconWidth[WC_DockPanel::DockLocation::Bottom], dockingIconHeight[WC_DockPanel::DockLocation::Bottom], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Left], NULL, dockingIconForceTopPosX[WC_DockPanel::DockLocation::Left], dockingIconForceTopPosY[WC_DockPanel::DockLocation::Left], dockingIconForceTopWidth[WC_DockPanel::DockLocation::Left], dockingIconForceTopHeight[WC_DockPanel::DockLocation::Left], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Right], NULL, dockingIconForceTopPosX[WC_DockPanel::DockLocation::Right], dockingIconForceTopPosY[WC_DockPanel::DockLocation::Right], dockingIconForceTopWidth[WC_DockPanel::DockLocation::Right], dockingIconForceTopHeight[WC_DockPanel::DockLocation::Right], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Top], NULL, dockingIconForceTopPosX[WC_DockPanel::DockLocation::Top], dockingIconForceTopPosY[WC_DockPanel::DockLocation::Top], dockingIconForceTopWidth[WC_DockPanel::DockLocation::Top], dockingIconForceTopHeight[WC_DockPanel::DockLocation::Top], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Bottom], NULL, dockingIconForceTopPosX[WC_DockPanel::DockLocation::Bottom], dockingIconForceTopPosY[WC_DockPanel::DockLocation::Bottom], dockingIconForceTopWidth[WC_DockPanel::DockLocation::Bottom], dockingIconForceTopHeight[WC_DockPanel::DockLocation::Bottom], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(deferWindowPosSession, placementTargetCenter, NULL, centerDockingIconPosX, centerDockingIconPosY, centerDockingIconWidth, centerDockingIconHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

	//Show the placement target over the target docking window
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Left], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Right], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Top], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargets[WC_DockPanel::DockLocation::Bottom], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Left], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Right], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Top], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetsForceTop[WC_DockPanel::DockLocation::Bottom], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	DeferWindowPos(deferWindowPosSession, placementTargetCenter, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

	//Process all the window size and position changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);
}

//----------------------------------------------------------------------------------------
void DockingWindow::UpdatePlacementShadow(int cursorPosX, int cursorPosY, int dockWindowWidth, int dockWindowHeight)
{
	//Determine the current docking target based on the supplied cursor position
	bool foundDockLocation = true;
	bool newDockLocationIsContentRegion = false;
	WC_DockPanel::DockLocation newDockLocation;
	bool forceTopOfDockingOrder = false;
	POINT cursorPos;
	cursorPos.x = cursorPosX;
	cursorPos.y = cursorPosY;
	RECT rect;
	if(placementTargetCenterEnabled && (GetWindowRect(placementTargetCenter, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocationIsContentRegion = true;
	}
	else if((GetWindowRect(placementTargets[WC_DockPanel::DockLocation::Left], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Left;
	}
	else if((GetWindowRect(placementTargets[WC_DockPanel::DockLocation::Right], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Right;
	}
	else if((GetWindowRect(placementTargets[WC_DockPanel::DockLocation::Top], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Top;
	}
	else if((GetWindowRect(placementTargets[WC_DockPanel::DockLocation::Bottom], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Bottom;
	}
	else if((GetWindowRect(placementTargetsForceTop[WC_DockPanel::DockLocation::Left], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Left;
		forceTopOfDockingOrder = true;
	}
	else if((GetWindowRect(placementTargetsForceTop[WC_DockPanel::DockLocation::Right], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Right;
		forceTopOfDockingOrder = true;
	}
	else if((GetWindowRect(placementTargetsForceTop[WC_DockPanel::DockLocation::Top], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Top;
		forceTopOfDockingOrder = true;
	}
	else if((GetWindowRect(placementTargetsForceTop[WC_DockPanel::DockLocation::Bottom], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Bottom;
		forceTopOfDockingOrder = true;
	}
	else
	{
		foundDockLocation = false;
	}

	//If we found a target dock location, update the placement shadow window position and
	//size.
	if(foundDockLocation)
	{
		//Obtain a rectangle representing the target position of the dock window into the
		//dock panel at the requested location, in dock panel client coordinates.
		RECT newDockRect;
		if(newDockLocationIsContentRegion)
		{
			//Since the content region is the target of this dock panel, obtain the
			//content rectangle as the rectangle for this dock window.
			SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::GetContentRect, 0, (LPARAM)&newDockRect);
		}
		else
		{
			//Calculate the maximum docking width and height to use when adding a new
			//docked window to the dock panel. We always limit new dock windows to take up
			//no more than half the available content region automatically.
			RECT contentRect;
			SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::GetContentRect, 0, (LPARAM)&contentRect);
			int maxInitialDockedWidth = (contentRect.right - contentRect.left) / 2;
			int maxInitialDockedHeight = (contentRect.bottom - contentRect.top) / 2;

			//Limit the dock window and height by our maximum initial dock window and
			//height
			dockWindowWidth = (dockWindowWidth > maxInitialDockedWidth)? maxInitialDockedWidth: dockWindowWidth;
			dockWindowHeight = (dockWindowHeight > maxInitialDockedHeight)? maxInitialDockedHeight: dockWindowHeight;

			//Obtain a rectangle defining the calculated target location for this new dock
			//panel entry
			WC_DockPanel::CalculateNewDockedWindowRectParams params;
			params.dockLocation = newDockLocation;
			params.addToFront = forceTopOfDockingOrder;
			params.preferredWidth = dockWindowWidth;
			params.preferredHeight = dockWindowHeight;
			SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::CalculateNewDockedWindowRect, 0, (LPARAM)&params);
			newDockRect = params.rect;
		}

		//Convert the target position into screen coordinates
		POINT point;
		point.x = newDockRect.left;
		point.y = newDockRect.top;
		ClientToScreen(dockPanel, &point);
		int placementShadowPosX = point.x;
		int placementShadowPosY = point.y;
		int placementShadowWidth = newDockRect.right - newDockRect.left;
		int placementShadowHeight = newDockRect.bottom - newDockRect.top;

		//Update the placement shadow position and size
		SetWindowPos(placementShadow, NULL, placementShadowPosX, placementShadowPosY, placementShadowWidth, placementShadowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	}

	//Show or hide the placement shadow window if required
	if(foundDockLocation && !placementShadowVisible)
	{
		SetWindowPos(placementShadow, placementTargetCenter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
		placementShadowVisible = true;
	}
	else if(!foundDockLocation && placementShadowVisible)
	{
		SetWindowPos(placementShadow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
		placementShadowVisible = false;
	}
}

//----------------------------------------------------------------------------------------
bool DockingWindow::HitTestPlacementTargets(int cursorPosX, int cursorPosY, bool& dockLocationIsContentRegion, WC_DockPanel::DockLocation& dockLocation, bool& forceTop) const
{
	//Calculate a docking location within the target window
	bool foundDockLocation = true;
	bool newDockLocationIsContentRegion = false;
	WC_DockPanel::DockLocation newDockLocation = WC_DockPanel::DockLocation::Left;
	bool forceTopOfDockingOrder = false;
	POINT cursorPos;
	cursorPos.x = cursorPosX;
	cursorPos.y = cursorPosY;
	RECT rect;
	if((GetWindowRect(placementTargetCenter, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocationIsContentRegion = true;
	}
	else if((GetWindowRect(placementTargets.find(WC_DockPanel::DockLocation::Left)->second, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Left;
	}
	else if((GetWindowRect(placementTargets.find(WC_DockPanel::DockLocation::Right)->second, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Right;
	}
	else if((GetWindowRect(placementTargets.find(WC_DockPanel::DockLocation::Top)->second, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Top;
	}
	else if((GetWindowRect(placementTargets.find(WC_DockPanel::DockLocation::Bottom)->second, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Bottom;
	}
	else if((GetWindowRect(placementTargetsForceTop.find(WC_DockPanel::DockLocation::Left)->second, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Left;
		forceTopOfDockingOrder = true;
	}
	else if((GetWindowRect(placementTargetsForceTop.find(WC_DockPanel::DockLocation::Right)->second, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Right;
		forceTopOfDockingOrder = true;
	}
	else if((GetWindowRect(placementTargetsForceTop.find(WC_DockPanel::DockLocation::Top)->second, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Top;
		forceTopOfDockingOrder = true;
	}
	else if((GetWindowRect(placementTargetsForceTop.find(WC_DockPanel::DockLocation::Bottom)->second, &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = WC_DockPanel::DockLocation::Bottom;
		forceTopOfDockingOrder = true;
	}
	else
	{
		foundDockLocation = false;
	}

	//If we didn't find a target dock location, return false.
	if(!foundDockLocation)
	{
		return false;
	}

	//Return the target docking location to the caller
	dockLocationIsContentRegion = newDockLocationIsContentRegion;
	dockLocation = newDockLocation;
	forceTop = forceTopOfDockingOrder;
	return true;
}

//----------------------------------------------------------------------------------------
//Tab control update methods
//----------------------------------------------------------------------------------------
void DockingWindow::HandleContentTabChanged(int newTabIndex)
{
	//Hide any currently visible content for the currently selected tab
	if(currentSelectedTabIndex >= 0)
	{
		ShowWindow(hostedContent[tabIndexToHostedContentNo[currentSelectedTabIndex]].contentWindow, SW_HIDE);
	}

	//Resize and reposition the new selected content window
	const ContentEntry& contentEntry = hostedContent[tabIndexToHostedContentNo[newTabIndex]];
	RECT tabControlRect;
	GetClientRect(contentTabControl, &tabControlRect);
	SendMessage(contentTabControl, TCM_ADJUSTRECT, FALSE, (LPARAM)&tabControlRect);
	SetWindowPos(contentEntry.contentWindow, NULL, tabControlRect.left, tabControlRect.top, tabControlRect.right - tabControlRect.left, tabControlRect.bottom - tabControlRect.top, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

	//Make the selected content window visible
	ShowWindow(contentEntry.contentWindow, SW_SHOWNA);

	//Update the title of our docking window to match the selected tab
	SetWindowText(hwnd, contentEntry.contentTitle.c_str());

	//Update the currently selected tab index
	currentSelectedTabIndex = newTabIndex;
}

//----------------------------------------------------------------------------------------
void DockingWindow::TabHitTest(HWND tabTrayHwnd, int cursorPosX, int cursorPosY)
{
	//Attempt to retrieve information for the tab the cursor is currently over, if any.
	bool foundTargetDockWindow = false;
	HWND targetContentWindow;
	IDockingWindow* targetDockedWindow;
	int targetTabExtendedHitRegionX;
	int targetTabExtendedHitRegionY;
	int targetTabExtendedHitRegionWidth;
	int targetTabExtendedHitRegionHeight;
	std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::const_iterator dockIterator = autoHideDocks.begin();
	while(!foundTargetDockWindow && (dockIterator != autoHideDocks.end()))
	{
		//Ensure this dock is the target tab dock
		const AutoHideDockInfo& dockInfo = dockIterator->second;
		if(dockInfo.dockWindow != tabTrayHwnd)
		{
			++dockIterator;
			continue;
		}

		//Search for the tab the cursor is currently over in this tab dock
		std::list<AutoHideDockTabGroup>::const_iterator dockTabGroupIterator = dockInfo.dockTabGroups.begin();
		while(!foundTargetDockWindow && (dockTabGroupIterator != dockInfo.dockTabGroups.end()))
		{
			const AutoHideDockTabGroup& dockTabGroup = *dockTabGroupIterator;
			int dockGroupLength = 0;
			int dockGroupPosScreenX;
			int dockGroupPosScreenY;
			for(std::list<AutoHideDockTab>::const_iterator dockTabIterator = dockTabGroup.dockTabs.begin(); dockTabIterator != dockTabGroup.dockTabs.end(); ++dockTabIterator)
			{
				//If this is the first tab in this docking group, calculate the position
				//of this tab in screen coordinates.
				const AutoHideDockTab& dockTab = *dockTabIterator;
				if(dockTabIterator == dockTabGroup.dockTabs.begin())
				{
					POINT point;
					point.x = dockTab.tabPosX;
					point.y = dockTab.tabPosY;
					ClientToScreen(dockInfo.dockWindow, &point);
					dockGroupPosScreenX = point.x;
					dockGroupPosScreenY = point.y;
				}

				//Add the "length" of this tab item to the total length of the tab group
				dockGroupLength += ((dockInfo.dockLocation == WC_DockPanel::DockLocation::Left) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Right))? dockTab.tabHeight: dockTab.tabWidth;

				//If the cursor is currently located within this tab, latch the associated
				//docking window and content item as the target.
				if((cursorPosX >= dockTab.tabPosX) && (cursorPosX < (dockTab.tabPosX + dockTab.tabWidth)) && (cursorPosY >= dockTab.tabPosY) && (cursorPosY < (dockTab.tabPosY + dockTab.tabHeight)))
				{
					//Calculate the current position of the tab tray that hosts this tab
					RECT rect;
					GetWindowRect(dockInfo.dockWindow, &rect);
					int tabTrayPosX = rect.left;
					int tabTrayPosY = rect.top;

					//Save the target of this tab as the target dock window for the
					//current mouse position
					foundTargetDockWindow = true;
					targetDockedWindow = dockTabGroup.childContainer;
					targetContentWindow = dockTab.contentWindow;

					//Calculate the position and size of the extended hit region for the
					//tab of the autohide dock panel currently being shown
					if(dockInfo.dockLocation == WC_DockPanel::DockLocation::Left)
					{
						targetTabExtendedHitRegionWidth = dockTab.tabWidth + dockingPanelTabMarginSize;
						targetTabExtendedHitRegionX = tabTrayPosX + dockTab.tabPosX;
					}
					else if(dockInfo.dockLocation == WC_DockPanel::DockLocation::Right)
					{
						targetTabExtendedHitRegionWidth = dockTab.tabWidth + dockingPanelTabMarginSize;
						targetTabExtendedHitRegionX = tabTrayPosX + (dockTab.tabPosX - dockingPanelTabMarginSize);
					}
					else if(dockInfo.dockLocation == WC_DockPanel::DockLocation::Top)
					{
						targetTabExtendedHitRegionHeight = dockTab.tabHeight + dockingPanelTabMarginSize;
						targetTabExtendedHitRegionY = tabTrayPosY + dockTab.tabPosY;
					}
					else if(dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom)
					{
						targetTabExtendedHitRegionHeight = dockTab.tabHeight + dockingPanelTabMarginSize;
						targetTabExtendedHitRegionY = tabTrayPosY + (dockTab.tabPosY - dockingPanelTabMarginSize);
					}
				}
			}

			//If we found a target dock tab while iterating this docking group, make the
			//hit dead zone "length" equal to the length of the docking group, and set the
			//dead zone position based on the start location of the docking group.
			if(foundTargetDockWindow)
			{
				if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Left) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Right))
				{
					targetTabExtendedHitRegionHeight = dockGroupLength;
					targetTabExtendedHitRegionY = dockGroupPosScreenY;
				}
				else
				{
					targetTabExtendedHitRegionWidth = dockGroupLength;
					targetTabExtendedHitRegionX = dockGroupPosScreenX;
				}
			}

			++dockTabGroupIterator;
		}
		++dockIterator;
	}

	//Either show or hide an autohide content panel if required
	if(!foundTargetDockWindow && autoHidePanelVisible)
	{
		//Calculate the cursor position in screen coordinates
		POINT point;
		point.x = cursorPosX;
		point.y = cursorPosY;
		ClientToScreen(tabTrayHwnd, &point);
		int screenCursorPosX = point.x;
		int screenCursorPosY = point.y;

		//If the cursor is currently outside the extended hit region for the autohide dock
		//panel that's being shown, hide the panel.
		if((screenCursorPosX < tabExtendedHitRegionX) || (screenCursorPosX >= (tabExtendedHitRegionX + tabExtendedHitRegionWidth)) || (screenCursorPosY < tabExtendedHitRegionY) || (screenCursorPosY >= (tabExtendedHitRegionY + tabExtendedHitRegionHeight)))
		{
			CollapseHiddenDockPanel();
		}
	}
	else
	{
		//Hide any currently visible auto hide dock panel if it's different to the new tab
		//the cursor is located over.
		if(autoHidePanelVisible && ((currentAutoHidePanel != targetDockedWindow) || (currentAutoHidePanelContent != targetContentWindow)))
		{
			CollapseHiddenDockPanel();
		}

		//Show the auto hide dock panel corresponding with the tab the cursor is currently
		//over if required
		if(foundTargetDockWindow && !autoHidePanelVisible)
		{
			//Calculate the current width and height of the hidden dock panel
			RECT rect;
			GetWindowRect(targetDockedWindow->GetWindowHandle(), &rect);
			int hiddenPanelWidth = rect.right - rect.left;
			int hiddenPanelHeight = rect.bottom - rect.top;

			//Calculate the position and size of the tab tray that contains this tab
			GetWindowRect(tabTrayHwnd, &rect);
			int tabTrayPosX = rect.left;
			int tabTrayPosY = rect.top;
			int tabTrayWidth = rect.right - rect.left;
			int tabTrayHeight = rect.bottom - rect.top;

			//Calculate the new position and size of the hidden dock panel
			int newPanelPosX;
			int newPanelPosY;
			int newPanelWidth;
			int newPanelHeight;
			switch(targetDockedWindow->GetDockLocation())
			{
			case WC_DockPanel::DockLocation::Left:
				newPanelPosX = targetTabExtendedHitRegionX + targetTabExtendedHitRegionWidth;
				newPanelPosY = tabTrayPosY;
				newPanelWidth = hiddenPanelWidth;
				newPanelHeight = tabTrayHeight;
				break;
			case WC_DockPanel::DockLocation::Right:
				newPanelPosX = targetTabExtendedHitRegionX - hiddenPanelWidth;
				newPanelPosY = tabTrayPosY;
				newPanelWidth = hiddenPanelWidth;
				newPanelHeight = tabTrayHeight;
				break;
			case WC_DockPanel::DockLocation::Top:
				newPanelPosX = tabTrayPosX;
				newPanelPosY = targetTabExtendedHitRegionY + targetTabExtendedHitRegionHeight;
				newPanelWidth = tabTrayWidth;
				newPanelHeight = hiddenPanelHeight;
				break;
			case WC_DockPanel::DockLocation::Bottom:
				newPanelPosX = tabTrayPosX;
				newPanelPosY = targetTabExtendedHitRegionY - hiddenPanelHeight;
				newPanelWidth = tabTrayWidth;
				newPanelHeight = hiddenPanelHeight;
				break;
			}

			//Determine the extra padding margin sizes we need to include at the start or
			//end of this dock tab tray, due to corner regions being present from another
			//dock tab tray adjoining this one.
			int verticalTabTrayStartPadding = 0;
			int verticalTabTrayEndPadding = 0;
			int tabTrayRowSize = (controlFontHeight + (dockingPanelTabMarginSize * 3));
			if((targetDockedWindow->GetDockLocation() == WC_DockPanel::DockLocation::Left) || (targetDockedWindow->GetDockLocation() == WC_DockPanel::DockLocation::Right))
			{
				for(std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::const_iterator i = autoHideDocks.begin(); i != autoHideDocks.end(); ++i)
				{
					if(i->first == WC_DockPanel::DockLocation::Top)
					{
						verticalTabTrayStartPadding += i->second.tabRowCount * tabTrayRowSize;
					}
					else if(i->first == WC_DockPanel::DockLocation::Bottom)
					{
						verticalTabTrayEndPadding += i->second.tabRowCount * tabTrayRowSize;
					}
				}
			}

			//Adjust the position and size of the hidden dock panel as required to account
			//for the corner padding margin
			newPanelPosY += verticalTabTrayStartPadding;
			newPanelHeight -= verticalTabTrayStartPadding + verticalTabTrayEndPadding;

			//Convert the new hidden dock panel position from screen coordinates to client
			//coordinates of our window
			POINT point;
			point.x = newPanelPosX;
			point.y = newPanelPosY;
			ScreenToClient(hwnd, &point);
			newPanelPosX = point.x;
			newPanelPosY = point.y;

			//Show the hidden dock panel
			ExpandHiddenDockPanel(targetDockedWindow, targetContentWindow, newPanelPosX, newPanelPosY, newPanelWidth, newPanelHeight);
			tabExtendedHitRegionX = targetTabExtendedHitRegionX;
			tabExtendedHitRegionY = targetTabExtendedHitRegionY;
			tabExtendedHitRegionWidth = targetTabExtendedHitRegionWidth;
			tabExtendedHitRegionHeight = targetTabExtendedHitRegionHeight;

			//Start a timer to poll for when we need to hide this tab again. Note that a
			//variety of methods have been tried to manage auto-hiding. It appears the
			//only reliable way we can track the mouse without polling is to capture mouse
			//input, but this stops mouse message reaching the docked window when it is
			//over it. While we can in theory directly forward mouse messages to the
			//window under the cursor while we're tracking it, there are a lot of messages
			//to handle and forward, and we'd break any new or unknown messages that we
			//didn't explicitly handle, and we'd also break mouse tracking if any of the
			//windows we forwarded messages onto tried to capture the mouse itself.
			//Polling with a timer appears to be the simplest and most reliable way to
			//achieve the required behaviour.
			SetTimer(tabTrayHwnd, 1, 50, NULL);
		}
	}
}

//----------------------------------------------------------------------------------------
void DockingWindow::AllDockTabsHitTest(int cursorPosX, int cursorPosY)
{
	//Perform hit testing for each tab dock window on the specified cursor position
	for(std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::const_iterator i = autoHideDocks.begin(); i != autoHideDocks.end(); ++i)
	{
		//Calculate the cursor position in client coordinates for the target dock
		//window
		const AutoHideDockInfo& autoHideDockInfo = i->second;
		POINT point;
		point.x = cursorPosX;
		point.y = cursorPosY;
		ScreenToClient(autoHideDockInfo.dockWindow, &point);
		int cursorPosForDockTabX = point.x;
		int cursorPosForDockTabY = point.y;

		//Perform hit testing for the target tab dock
		TabHitTest(autoHideDockInfo.dockWindow, cursorPosForDockTabX, cursorPosForDockTabY);
	}
}

//----------------------------------------------------------------------------------------
void DockingWindow::UpdateAutoHideWindowTabSizes()
{
	//Update the size of each tab window
	for(std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::iterator dockIterator = autoHideDocks.begin(); dockIterator != autoHideDocks.end(); ++dockIterator)
	{
		//Set the size for this tab tray
		AutoHideDockInfo& dockInfo = dockIterator->second;
		if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Top) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom))
		{
			SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredHeight, controlFontHeight + (3 * dockingPanelTabMarginSize), (LPARAM)dockInfo.dockWindow);
		}
		else
		{
			SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredWidth, controlFontHeight + (3 * dockingPanelTabMarginSize), (LPARAM)dockInfo.dockWindow);
		}

		//Force the tab group to redraw using the new size information
		InvalidateRect(dockInfo.dockWindow, NULL, FALSE);
	}
}

//----------------------------------------------------------------------------------------
//Child container methods
//----------------------------------------------------------------------------------------
void DockingWindow::AddChildContainer(IDockingWindow* childContainer, WC_DockPanel::DockLocation dockLocation, bool autoHide, bool forceTop)
{
	//Create a new entry object for this child container
	ChildContainerEntry entry;
	entry.childContainer = childContainer;
	entry.dockLocation = dockLocation;
	entry.autoHide = autoHide;

	//Add this entry object to the list of child container entries
	childContainers.push_back(entry);

	//Either add this window to the docking container, or the autohide region, depending
	//on whether autohide has been selected for this docked panel.
	if(!entry.autoHide)
	{
		//Calculate the maximum initial width or height for this new child docking window
		//depending on the target dock location. We do this to ensure that a new docked
		//window is never made to cover the content region when it is initially docked.
		RECT rect;
		SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::GetContentRect, 0, (LPARAM)&rect);
		int maxInitialDockedWidth = (rect.right - rect.left) / 2;
		int maxInitialDockedHeight = (rect.bottom - rect.top) / 2;

		//Calculate window dimensions of the new child container
		GetWindowRect(entry.childContainer->GetWindowHandle(), &rect);
		int childWindowWidth = rect.right - rect.left;
		int childWindowHeight = rect.bottom - rect.top;

		//Add this child container to our internal docking panel
		if(forceTop)
		{
			SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::AddDockedWindowToFront, (WPARAM)entry.dockLocation, (LPARAM)childContainer->GetWindowHandle());
		}
		else
		{
			SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::AddDockedWindow, (WPARAM)entry.dockLocation, (LPARAM)childContainer->GetWindowHandle());
		}

		//Limit the new docked child window by its maximum width or height if required
		if((entry.dockLocation == WC_DockPanel::DockLocation::Top) || (entry.dockLocation == WC_DockPanel::DockLocation::Bottom))
		{
			if(childWindowHeight > maxInitialDockedHeight)
			{
				SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredHeight, maxInitialDockedHeight, (LPARAM)childContainer->GetWindowHandle());
			}
		}
		else
		{
			if(childWindowWidth > maxInitialDockedWidth)
			{
				SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredWidth, maxInitialDockedWidth, (LPARAM)childContainer->GetWindowHandle());
			}
		}
	}
	else
	{
		//Hide the new child container since it's been added in autohide mode
		ShowWindow(entry.childContainer->GetWindowHandle(), SW_HIDE);

		//Set the child container as a child window of our control
		SetWindowParent(entry.childContainer->GetWindowHandle(), hwnd);

		//Retrieve the tab dock window info for the target dock location, creating it if
		//it doesn't already exist.
		AutoHideDockInfo& dockInfo = autoHideDocks[entry.dockLocation];
		dockInfo.dockLocation = entry.dockLocation;

		//Create the tab dock window for this dock location if it doesn't already exist
		if(dockInfo.dockWindow == NULL)
		{
			//Create the tab dock window
			dockInfo.dockWindow = CreateWindowEx(0, tabTrayWindowClassName, L"", WS_CHILD, 0, 0, 0, 0, tabDockPanel, NULL, moduleHandle, (LPVOID)this);

			//Add this auto hide dock window tab tray to the tab dock panel. We force a
			//particular ordering of vertical tab trays vs horizontal tab trays so that we
			//can deal with the corner regions which appear where a horizontal and
			//vertical tab tray meet.
			if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Top) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom))
			{
				SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::AddDockedWindow, (WPARAM)dockInfo.dockLocation, (LPARAM)dockInfo.dockWindow);
			}
			else
			{
				SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::AddDockedWindowToFront, (WPARAM)dockInfo.dockLocation, (LPARAM)dockInfo.dockWindow);
			}

			//Set the size for this tab tray
			if((dockInfo.dockLocation == WC_DockPanel::DockLocation::Top) || (dockInfo.dockLocation == WC_DockPanel::DockLocation::Bottom))
			{
				SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredHeight, controlFontHeight + (3 * dockingPanelTabMarginSize), (LPARAM)dockInfo.dockWindow);
			}
			else
			{
				SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::SetDockedWindowDesiredWidth, controlFontHeight + (3 * dockingPanelTabMarginSize), (LPARAM)dockInfo.dockWindow);
			}

			//Show the tab dock tray
			ShowWindow(dockInfo.dockWindow, SW_SHOWNA);

			//Force each tab tray to redraw now that a new tab tray has been added. This
			//allows existing tab trays to add in padding margins where required to ensure
			//that tabs don't get drawn in the corner region where two tab trays meet.
			for(std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::const_iterator i = autoHideDocks.begin(); i != autoHideDocks.end(); ++i)
			{
				InvalidateRect(i->second.dockWindow, NULL, FALSE);
			}
		}

		//Create a tab group to define all the entries under this new autohide docked
		//content window
		AutoHideDockTabGroup dockTabGroup;
		dockTabGroup.childContainer = entry.childContainer;
		unsigned int childContainerContentWindowCount = dockTabGroup.childContainer->GetHostedContentCount();
		for(unsigned int i = 0; i < childContainerContentWindowCount; ++i)
		{
			AutoHideDockTab dockTab;
			unsigned int sortedContentEntryNo = dockTabGroup.childContainer->GetSortedContentEntryNo(i);
			dockTab.contentWindow = dockTabGroup.childContainer->GetHostedContentWindow(sortedContentEntryNo);
			dockTabGroup.dockTabs.push_back(dockTab);
		}

		//Add this new tab group to the list of tab groups
		dockInfo.dockTabGroups.push_back(dockTabGroup);

		//Force the tab group to redraw now that its content has changed
		InvalidateRect(dockInfo.dockWindow, NULL, FALSE);
	}
}

//----------------------------------------------------------------------------------------
void DockingWindow::RemoveChildContainer(IDockingWindow* childContainer)
{
	//Attempt to locate the target child container in our list of child containers
	bool foundTargetItem = false;
	std::list<ChildContainerEntry>::iterator childContainerIterator = childContainers.begin();
	while(!foundTargetItem && (childContainerIterator != childContainers.end()))
	{
		if(childContainerIterator->childContainer == childContainer)
		{
			foundTargetItem = true;
			continue;
		}
		++childContainerIterator;
	}
	if(!foundTargetItem)
	{
		return;
	}

	//Remove the target item
	childContainers.erase(childContainerIterator);

	//Remove the container from our internal docking windows
	if(!childContainer->IsAutoHideEnabled())
	{
		//Remove this child container from our internal docking panel
		SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::RemoveDockedWindow, 0, (LPARAM)childContainer->GetWindowHandle());
	}
	else
	{
		//If the child container being removed is the currently visible auto hide dock
		//panel, flag that no auto hide dock panel is currently visible.
		if(autoHidePanelVisible && (currentAutoHidePanel == childContainer))
		{
			autoHidePanelVisible = false;
			currentAutoHidePanel = 0;
			currentAutoHidePanelContent = NULL;
		}

		//Remove the tab group for this container from the list of tab groups for the
		//target dock location
		AutoHideDockInfo& dockInfo = autoHideDocks[childContainer->GetDockLocation()];
		std::list<AutoHideDockTabGroup>::iterator dockTabGroupIterator = dockInfo.dockTabGroups.begin();
		bool removedTargetDockGroup = false;
		while(!removedTargetDockGroup && (dockTabGroupIterator != dockInfo.dockTabGroups.end()))
		{
			if(dockTabGroupIterator->childContainer == childContainer)
			{
				dockInfo.dockTabGroups.erase(dockTabGroupIterator);
				removedTargetDockGroup = true;
				continue;
			}
			++dockTabGroupIterator;
		}

		//Remove the child container as a child window of our control
		SetWindowParent(childContainer->GetWindowHandle(), NULL);

		//If there are no more tabs present in the dock tab group at the target location,
		//remove the tab group, otherwise force it to redraw now that its content has
		//changed.
		if(dockInfo.dockTabGroups.empty())
		{
			//Destroy the tab tray window
			ShowWindow(dockInfo.dockWindow, SW_HIDE);
			SendMessage(tabDockPanel, (UINT)WC_DockPanel::WindowMessages::RemoveDockedWindow, 0, (LPARAM)dockInfo.dockWindow);
			DestroyWindow(dockInfo.dockWindow);

			//Remove this tab tray from the list of tab trays
			autoHideDocks.erase(childContainer->GetDockLocation());

			//Force each tab tray to redraw now that a tab tray has been removed. This
			//allows other tab trays to remove padding margins where they are no longer
			//required to ensure that tabs don't get drawn in the corner region where two
			//tab trays meet.
			for(std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::const_iterator i = autoHideDocks.begin(); i != autoHideDocks.end(); ++i)
			{
				InvalidateRect(i->second.dockWindow, NULL, FALSE);
			}
		}
		else
		{
			InvalidateRect(dockInfo.dockWindow, NULL, FALSE);
		}
	}
}

//----------------------------------------------------------------------------------------
void DockingWindow::UpdateAutoHideChildContainerContent(IDockingWindow* childContainer)
{
	//Retrieve the tab dock window info for the target dock location
	std::map<WC_DockPanel::DockLocation, AutoHideDockInfo>::iterator dockInfoIterator = autoHideDocks.find(childContainer->GetDockLocation());
	if(dockInfoIterator == autoHideDocks.end())
	{
		return;
	}
	AutoHideDockInfo& dockInfo = dockInfoIterator->second;

	//Retrieve the associated tab docking group for the target child container
	std::list<AutoHideDockTabGroup>::iterator tabGroupIterator = dockInfo.dockTabGroups.begin();
	bool foundDockTabGroup = false;
	while(!foundDockTabGroup && (tabGroupIterator != dockInfo.dockTabGroups.end()))
	{
		if(tabGroupIterator->childContainer == childContainer)
		{
			foundDockTabGroup = true;
			continue;
		}
		++tabGroupIterator;
	}
	if(!foundDockTabGroup)
	{
		return;
	}
	AutoHideDockTabGroup& dockTabGroup = *tabGroupIterator;

	//Rebuild the list of tabs for this tab group
	dockTabGroup.dockTabs.clear();
	unsigned int childContainerContentWindowCount = childContainer->GetHostedContentCount();
	for(unsigned int i = 0; i < childContainerContentWindowCount; ++i)
	{
		AutoHideDockTab dockTab;
		unsigned int sortedContentEntryNo = childContainer->GetSortedContentEntryNo(i);
		dockTab.contentWindow = childContainer->GetHostedContentWindow(sortedContentEntryNo);
		dockTabGroup.dockTabs.push_back(dockTab);
	}

	//Force the tab group to redraw now that its content has changed
	InvalidateRect(dockInfo.dockWindow, NULL, FALSE);
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<IDockingWindow*>> DockingWindow::GetNestedChildDockingWindowList() const
{
	//Create a list object to hold our list of nested child windows
	std::list<IDockingWindow*> dockingWindowList;

	//If an auto hide panel is currently visible for this docking window, add it to the
	//window list first because it's overlapping child windows and at the top of the
	//Z-order.
	if(autoHidePanelVisible)
	{
		std::list<IDockingWindow*> childDockingWindowList = currentAutoHidePanel->GetNestedChildDockingWindowList();
		dockingWindowList.insert(dockingWindowList.end(), childDockingWindowList.begin(), childDockingWindowList.end());
		dockingWindowList.push_back(currentAutoHidePanel);
	}

	//Add each docked non-auto hide child container to the window list. Note that Z-order
	//doesn't matter here, since non auto hide docked windows don't overlap.
	for(std::list<ChildContainerEntry>::const_iterator i = childContainers.begin(); i != childContainers.end(); ++i)
	{
		const ChildContainerEntry& containerEntry = *i;
		if(!containerEntry.autoHide)
		{
			std::list<IDockingWindow*> childDockingWindowList = containerEntry.childContainer->GetNestedChildDockingWindowList();
			dockingWindowList.insert(dockingWindowList.end(), childDockingWindowList.begin(), childDockingWindowList.end());
			dockingWindowList.push_back(containerEntry.childContainer);
		}
	}

	//Return the list of nested docking windows to the caller
	return dockingWindowList;
}

//----------------------------------------------------------------------------------------
void DockingWindow::ExpandHiddenDockPanel(IDockingWindow* targetDockedWindow, HWND targetContentWindow, int newWindowPosX, int newWindowPosY, int newWindowWidth, int newWindowHeight)
{
	//Hide any other currently visible hidden child dock panel
	if(autoHidePanelVisible)
	{
		CollapseHiddenDockPanel();
	}

	//Set the selected content tab as the currently active content tab for the target
	//docking panel
	unsigned int contentEntryNo;
	if(targetDockedWindow->GetHostedContentIndexFromWindow(targetContentWindow, contentEntryNo))
	{
		targetDockedWindow->SetActiveContent(contentEntryNo);
	}

	//Position and size the dock panel
	SetWindowPos(targetDockedWindow->GetWindowHandle(), NULL, newWindowPosX, newWindowPosY, newWindowWidth, newWindowHeight, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	//Make the dock panel visible
	ShowWindow(targetDockedWindow->GetWindowHandle(), SW_SHOWNA);

	//Flag that this auto hide panel is now visible
	autoHidePanelVisible = true;
	currentAutoHidePanel = targetDockedWindow;
	currentAutoHidePanelContent = targetContentWindow;
}

//----------------------------------------------------------------------------------------
void DockingWindow::CollapseHiddenDockPanel()
{
	//Ensure a dock panel is currently visible
	if(!autoHidePanelVisible)
	{
		return;
	}

	//Hide the target child dock panel
	ShowWindow(currentAutoHidePanel->GetWindowHandle(), SW_HIDE);

	//Flag that no auto hide panels are now visible
	autoHidePanelVisible = false;
	currentAutoHidePanel = 0;
	currentAutoHidePanelContent = NULL;
}

//----------------------------------------------------------------------------------------
//Hosted content methods
//----------------------------------------------------------------------------------------
void DockingWindow::AddHostedContent(HWND contentWindow, const MarshalSupport::Marshal::In<std::wstring>& contentTitle)
{
	//Create a new entry object for this hosted content window
	ContentEntry entry;
	entry.tabIndex = 0;
	entry.contentWindow = contentWindow;
	entry.contentTitle = contentTitle;

	//Add the target content window
	if(hostedContent.empty())
	{
		//Add this entry object to the list of hosted content entries
		tabIndexToHostedContentNo[entry.tabIndex] = hostedContent.size();
		hostedContent.push_back(entry);

		//Set this content window as the content for our internal docking panel
		SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::AddContentWindow, 0, (LPARAM)entry.contentWindow);

		//Update the title of our docking window to match the content title
		SetWindowText(hwnd, entry.contentTitle.c_str());
	}
	else
	{
		bool createNewTabControl = hostedContent.size() == 1;
		if(createNewTabControl)
		{
			//Hide the existing content window
			ContentEntry& existingContentEntry = *hostedContent.begin();
			ShowWindow(existingContentEntry.contentWindow, SW_HIDE);

			//Remove the content window from our internal docking panel
			SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::RemoveContentWindow, 0, (LPARAM)existingContentEntry.contentWindow);

			//Create a new tab control to manage the content
			contentTabControl = CreateWindowEx(0, WC_TABCONTROL, L"", WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 0, 0, hwnd, NULL, moduleHandle, NULL);

			//Subclass the tab control so we can intercept size change events
			SetWindowSubclass(contentTabControl, BounceBackSubclassProc, 0, 0);

			//Set the font for the created tab control
			if(controlFont != NULL)
			{
				SendMessage(contentTabControl, WM_SETFONT, (WPARAM)controlFont, FALSE);
			}

			//Set no tab as currently active in the tab control
			currentSelectedTabIndex = -1;

			//Remove the existing content entry from the tab index to content number map
			size_t existingContentIndex = tabIndexToHostedContentNo[existingContentEntry.tabIndex];
			tabIndexToHostedContentNo.erase(existingContentEntry.tabIndex);

			//Insert the existing content item into the content tab control
			TCITEM tabControlItem;
			tabControlItem.mask = TCIF_PARAM | TCIF_TEXT;
			tabControlItem.lParam = (LPARAM)existingContentEntry.contentWindow;
			tabControlItem.pszText = (LPWSTR)existingContentEntry.contentTitle.c_str();
			existingContentEntry.tabIndex = (int)SendMessage(contentTabControl, TCM_INSERTITEM, 9999, (LPARAM)&tabControlItem);

			//Add the existing content entry back into the tab index to content number map
			//with the new tab index number. We preserve and reuse the original content
			//index number here just for some paranoid safety against future changes, even
			//though with the current logic it should always be 0.
			tabIndexToHostedContentNo[existingContentEntry.tabIndex] = existingContentIndex;

			//Clear the current selection in the content tab control. Without this, the
			//tab control seems to always latch the first tab item added as the currently
			//active tab item, which will cause problems if we query it in a message
			//handler before we set it properly. We clear it here to ensure we're always
			//in sync with the selected tab index.
			SendMessage(contentTabControl, TCM_SETCURSEL, (WPARAM)-1, 0);

			//Set the existing content item as a child window of the tab control
			SetWindowParent(existingContentEntry.contentWindow, contentTabControl);

			//Hide the existing content window now that we've added it to our tab control.
			//We're going to set the newly added content item as the active tab item, so
			//we don't want the existing content window still visible.
			ShowWindow(existingContentEntry.contentWindow, SW_HIDE);
		}

		//Hide the specified content window until we finish assigning its parent and
		//positioning it
		ShowWindow(entry.contentWindow, SW_HIDE);

		//Insert this content item into the content tab control
		TCITEM tabControlItem;
		tabControlItem.mask = TCIF_PARAM | TCIF_TEXT;
		tabControlItem.lParam = (LPARAM)entry.contentWindow;
		tabControlItem.pszText = (LPWSTR)entry.contentTitle.c_str();
		entry.tabIndex = (int)SendMessage(contentTabControl, TCM_INSERTITEM, 9999, (LPARAM)&tabControlItem);

		//Set this content item as a child window of the tab control
		SetWindowParent(entry.contentWindow, contentTabControl);

		if(createNewTabControl)
		{
			//Set the content tab control as the content for our internal docking panel
			SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::AddContentWindow, 0, (LPARAM)contentTabControl);

			//Show the tab control
			ShowWindow(contentTabControl, SW_SHOWNA);
		}

		//Add this entry object to the list of hosted content entries
		tabIndexToHostedContentNo[entry.tabIndex] = hostedContent.size();
		hostedContent.push_back(entry);

		//Select the new content item as the active item in the tab control
		SendMessage(contentTabControl, TCM_SETCURSEL, entry.tabIndex, 0);
		HandleContentTabChanged(entry.tabIndex);
	}

	//If we're docked to a parent window in auto hide mode, request the parent window to
	//update its list of tabs for this container now that our list of hosted content
	//windows has changed.
	if((parentDockingWindow != 0) && autoHideDockedWindow)
	{
		parentDockingWindow->UpdateAutoHideChildContainerContent(this);
	}
}

//----------------------------------------------------------------------------------------
void DockingWindow::RemoveHostedContent(HWND contentWindow)
{
	//Attempt to locate the target window in our list of hosted content windows
	bool foundTargetItem = false;
	size_t hostedContentIndex = 0;
	while(!foundTargetItem && (hostedContentIndex < hostedContent.size()))
	{
		if(hostedContent[hostedContentIndex].contentWindow == contentWindow)
		{
			foundTargetItem = true;
			continue;
		}
		++hostedContentIndex;
	}
	if(!foundTargetItem)
	{
		return;
	}

	//Retrieve a copy of the target item information, and remove it from our list of
	//hosted content.
	ContentEntry entry = hostedContent[hostedContentIndex];
	hostedContent.erase(hostedContent.begin() + hostedContentIndex);
	tabIndexToHostedContentNo.erase(entry.tabIndex);

	//Remove the target content window
	if(hostedContent.size() > 1)
	{
		//Remove the content window from our tab control
		SendMessage(contentTabControl, TCM_DELETEITEM, entry.tabIndex, 0);

		//Adjust the tab indexes for the remaining tab items now that we've removed a tab,
		//and calculate the tab index of the tab nearest to this one.
		tabIndexToHostedContentNo.clear();
		int nextTabIndexFromRemovedTab = -1;
		for(size_t i = 0; i < hostedContent.size(); ++i)
		{
			hostedContent[i].tabIndex = (hostedContent[i].tabIndex >= entry.tabIndex)? hostedContent[i].tabIndex - 1: hostedContent[i].tabIndex;
			tabIndexToHostedContentNo[hostedContent[i].tabIndex] = i;
			nextTabIndexFromRemovedTab = ((nextTabIndexFromRemovedTab < 0) || (hostedContent[i].tabIndex <= entry.tabIndex))? hostedContent[i].tabIndex: nextTabIndexFromRemovedTab;
		}

		//If the content item that was removed was the currently selected tab, or appears
		//before the currently selected tab in the list, update the current tab selection.
		if(currentSelectedTabIndex >= entry.tabIndex)
		{
			int newSelectedTabIndex = (currentSelectedTabIndex == entry.tabIndex)? nextTabIndexFromRemovedTab: currentSelectedTabIndex - 1;
			currentSelectedTabIndex = -1;
			SendMessage(contentTabControl, TCM_SETCURSEL, newSelectedTabIndex, 0);
			HandleContentTabChanged(newSelectedTabIndex);
		}

		//Remove the specified window as a child window of the tab control
		SetWindowParent(entry.contentWindow, NULL);
	}
	else if(hostedContent.size() == 1)
	{
		//Remove both the specified window and the remaining content window as child
		//windows of the tab control
		ContentEntry& remainingContentEntry = *hostedContent.begin();
		SetWindowParent(entry.contentWindow, NULL);
		SetWindowParent(remainingContentEntry.contentWindow, NULL);

		//Reset the tab index of the remaining item to 0, and update the content index
		//number for the remaining item in the index.
		tabIndexToHostedContentNo.erase(remainingContentEntry.tabIndex);
		remainingContentEntry.tabIndex = 0;
		tabIndexToHostedContentNo[remainingContentEntry.tabIndex] = 0;

		//Remove the tab control from our internal docking panel
		SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::RemoveContentWindow, 0, (LPARAM)contentTabControl);

		//Destroy the content tab control window now that only a single item is going to
		//be displayed in the content region
		DestroyWindow(contentTabControl);
		contentTabControl = NULL;

		//Clear the currently selected tab index now that the tab control has been
		//destroyed
		currentSelectedTabIndex = -1;

		//Set the remaining content window as the content for our internal docking panel
		SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::AddContentWindow, 0, (LPARAM)remainingContentEntry.contentWindow);

		//Ensure the remaining content window is visible
		SetWindowPos(remainingContentEntry.contentWindow, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

		//Update the title of our docking window to match the content title
		SetWindowText(hwnd, remainingContentEntry.contentTitle.c_str());
	}
	else
	{
		//Remove the content window from our internal docking panel
		SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::RemoveContentWindow, 0, (LPARAM)entry.contentWindow);

		//Clear the title of our docking window now that there's no hosted content
		SetWindowText(hwnd, L"");
	}

	//If we're docked to a parent window in auto hide mode, request the parent window to
	//update its list of tabs for this container now that our list of hosted content
	//windows has changed.
	if((parentDockingWindow != 0) && autoHideDockedWindow)
	{
		parentDockingWindow->UpdateAutoHideChildContainerContent(this);
	}
}

//----------------------------------------------------------------------------------------
unsigned int DockingWindow::GetHostedContentCount() const
{
	return (unsigned int)hostedContent.size();
}

//----------------------------------------------------------------------------------------
unsigned int DockingWindow::GetSortedContentEntryNo(unsigned int sortedContentEntryIndex) const
{
	//Calculate the sorted index number for the target content entry
	std::map<int, size_t>::const_iterator tabIndexIterator = tabIndexToHostedContentNo.begin();
	std::advance(tabIndexIterator, sortedContentEntryIndex);
	size_t sortedContentEntryNo = tabIndexIterator->second;

	//Return the sorted content entry index to the caller
	return (unsigned int)sortedContentEntryNo;
}

//----------------------------------------------------------------------------------------
bool DockingWindow::GetHostedContentIndexFromWindow(HWND contentWindow, unsigned int& contentEntryNo) const
{
	//Attempt to locate the content entry for the target content window
	size_t contentEntryIndex = 0;
	bool foundContentEntry = false;
	while(!foundContentEntry && (contentEntryIndex < hostedContent.size()))
	{
		if(hostedContent[contentEntryIndex].contentWindow == contentWindow)
		{
			foundContentEntry = true;
			continue;
		}
		++contentEntryIndex;
	}
	if(!foundContentEntry)
	{
		return false;
	}

	//Return the index for the target content entry to the caller
	contentEntryNo = (unsigned int)contentEntryIndex;
	return true;
}

//----------------------------------------------------------------------------------------
unsigned int DockingWindow::GetActiveContent() const
{
	//Return the index number of the currently active content entry
	unsigned int activeContentNo = 0;
	if(hostedContent.size() > 1)
	{
		std::map<int, size_t>::const_iterator tabIndexToHostedContentNoIterator = tabIndexToHostedContentNo.find(currentSelectedTabIndex);
		if(tabIndexToHostedContentNoIterator != tabIndexToHostedContentNo.end())
		{
			activeContentNo = (unsigned int)tabIndexToHostedContentNoIterator->second;
		}
	}
	return activeContentNo;
}

//----------------------------------------------------------------------------------------
void DockingWindow::SetActiveContent(unsigned int contentEntryNo)
{
	//Ensure that our content is currently hosted within a tab control. If no tab control
	//is present, we don't need to do anything here.
	if(contentTabControl == NULL)
	{
		return;
	}

	//Set the target content window as the new active content window if required
	const ContentEntry& contentEntry = hostedContent[contentEntryNo];
	if(currentSelectedTabIndex != contentEntry.tabIndex)
	{
		SendMessage(contentTabControl, TCM_SETCURSEL, contentEntry.tabIndex, 0);
		HandleContentTabChanged(contentEntry.tabIndex);
	}
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> DockingWindow::GetHostedContentTitle(unsigned int contentEntryNo) const
{
	//Return the title of the target content window to the caller
	return hostedContent[contentEntryNo].contentTitle;
}

//----------------------------------------------------------------------------------------
void DockingWindow::SetHostedContentTitle(unsigned int contentEntryNo, const std::wstring& newContentTitle)
{
	//Update the title of the target content entry
	ContentEntry& contentEntry = hostedContent[contentEntryNo];
	contentEntry.contentTitle = newContentTitle;

	//If we're currently displaying a tab control for our hosted content items, update the
	//text for the tab item for the target content entry now that the title has been
	//modified.
	if(hostedContent.size() > 1)
	{
		TCITEM tabControlItem;
		tabControlItem.mask = TCIF_TEXT;
		tabControlItem.pszText = (LPWSTR)contentEntry.contentTitle.c_str();
		SendMessage(contentTabControl, TCM_SETITEM, (WPARAM)contentEntry.tabIndex, (LPARAM)&tabControlItem);
	}

	//If the hosted content entry having its title modified is the currently selected
	//content entry, update the title of our docking window to match the new content
	//title.
	if((hostedContent.size() <= 1) || (currentSelectedTabIndex == contentEntry.tabIndex))
	{
		SetWindowText(hwnd, contentEntry.contentTitle.c_str());
	}

	//If we're docked to a parent window in auto hide mode, request the parent window to
	//update its list of tabs for this container now that the title of one of our hosted
	//content windows has changed.
	if((parentDockingWindow != 0) && autoHideDockedWindow)
	{
		parentDockingWindow->UpdateAutoHideChildContainerContent(this);
	}
}

//----------------------------------------------------------------------------------------
HWND DockingWindow::GetHostedContentWindow(unsigned int contentEntryNo) const
{
	//Return the window handle for the target hosted content entry to the caller
	return hostedContent[contentEntryNo].contentWindow;
}

//----------------------------------------------------------------------------------------
void DockingWindow::SetHostedContentWindow(unsigned int contentEntryNo, HWND newContentWindow)
{
	//Retrieve the old content window handle for the target content entry, and update the
	//target window handle to the new window.
	ContentEntry& contentEntry = hostedContent[contentEntryNo];
	HWND oldContentWindow = contentEntry.contentWindow;
	contentEntry.contentWindow = newContentWindow;

	//Hide the old content window
	ShowWindow(oldContentWindow, SW_HIDE);

	//Remove the old content window as a child window
	SetWindowParent(oldContentWindow, NULL);

	//Replace the target content window in the main docking window or the docked tab
	//control as required
	if(hostedContent.size() > 1)
	{
		//Set the new content window as a child window of the tab control
		SetWindowParent(newContentWindow, contentTabControl);

		//Replace the associated window in our tab control
		TCITEM tabControlItem;
		tabControlItem.mask = TCIF_PARAM;
		tabControlItem.lParam = (LPARAM)newContentWindow;
		SendMessage(contentTabControl, TCM_SETITEM, (WPARAM)contentEntry.tabIndex, (LPARAM)&tabControlItem);

		//If the hosted content entry having its associated window modified is the
		//currently selected content entry, handle its tab selection event again to update
		//the visibility, position, and size of the window, otherwise hide the window.
		if(currentSelectedTabIndex == contentEntry.tabIndex)
		{
			HandleContentTabChanged(currentSelectedTabIndex);
		}
		else
		{
			ShowWindow(newContentWindow, SW_HIDE);
		}
	}
	else
	{
		//Replace the old content window with the new content window in our dock panel
		//control
		SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::RemoveContentWindow, 0, (LPARAM)oldContentWindow);
		SendMessage(dockPanel, (UINT)WC_DockPanel::WindowMessages::AddContentWindow, 0, (LPARAM)newContentWindow);
	}

	//If we're docked to a parent window in auto hide mode, request the parent window to
	//update its list of tabs for this container now that the window handle for one of our
	//hosted content windows has changed.
	if((parentDockingWindow != 0) && autoHideDockedWindow)
	{
		parentDockingWindow->UpdateAutoHideChildContainerContent(this);
	}
}

//----------------------------------------------------------------------------------------
//Parent docking window methods
//----------------------------------------------------------------------------------------
IDockingWindow* DockingWindow::GetParentDockingWindow() const
{
	return parentDockingWindow;
}

//----------------------------------------------------------------------------------------
void DockingWindow::DockToNewParent(IDockingWindow* parentContainer, WC_DockPanel::DockLocation dockLocation, bool autoHide, bool forceTop)
{
	//Remove our docking window from any current parent docking window
	if(parentDockingWindow != 0)
	{
		parentDockingWindow->RemoveChildContainer(this);
	}

	//Add our docking window as a child docking window of the specified parent
	parentDockingWindow = parentContainer;
	currentDockLocation = dockLocation;
	autoHideDockedWindow = autoHide;
	parentDockingWindow->AddChildContainer(this, currentDockLocation, autoHideDockedWindow, forceTop);
}

//----------------------------------------------------------------------------------------
void DockingWindow::UndockFromParent()
{
	//Remove our docking window from any current parent docking window
	if(parentDockingWindow != 0)
	{
		parentDockingWindow->RemoveChildContainer(this);
		parentDockingWindow = 0;
	}
}

//----------------------------------------------------------------------------------------
WC_DockPanel::DockLocation DockingWindow::GetDockLocation() const
{
	return currentDockLocation;
}

//----------------------------------------------------------------------------------------
bool DockingWindow::IsAutoHideEnabled() const
{
	return autoHideDockedWindow;
}

//----------------------------------------------------------------------------------------
void DockingWindow::NotifyParentDestroyed()
{
	parentDockingWindow = 0;
}

//----------------------------------------------------------------------------------------
//Sizing methods
//----------------------------------------------------------------------------------------
void DockingWindow::HandleSizeChanged(int newWidth, int newHeight)
{
	//Resize the contained tab dock panel control. This will resize the tab docking
	//regions for autohide child panels, and the contained dock panel for visible docked
	//child panels.
	SetWindowPos(tabDockPanel, NULL, 0, 0, newWidth, newHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
}
