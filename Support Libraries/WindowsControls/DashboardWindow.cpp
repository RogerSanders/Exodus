#include "DashboardWindow.h"
#include <limits>

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* DashboardWindow::windowClassName = L"EX_DockWin_DashboardWindow";
const wchar_t* DashboardWindow::placementTargetWindowClassName = L"EX_DashboardWindow_PlacementTarget";
const wchar_t* DashboardWindow::placementShadowWindowClassName = L"EX_DashboardWindow_PlacementShadow";
const wchar_t* DashboardWindow::dividerHighlightWindowClassName = L"EX_DashboardWindow_DividerHighlight";

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DashboardWindow::DashboardWindow(HINSTANCE amoduleHandle, HWND ahwnd)
:moduleHandle(amoduleHandle), hwnd(ahwnd)
{
	//Font info
	controlFont = NULL;
	controlFontHeight = 1;

	//Window metrics
	dividerSize = 8;
	controlWidth = 0;
	controlHeight = 0;

	//Parent docking window info
	parentDockingWindow = 0;

	//Divider drag info
	dividerDragActive = false;
	dragLastPosX = 0;
	dragLastPosY = 0;
	dragPosOverLimitX = 0;
	dragPosOverLimitY = 0;
	dragPosSnapDisplacementX = 0;
	dragPosSnapDisplacementY = 0;

	//Drag-info
	windowDragInProgress = false;
	dockingWindowUnderDragPos = 0;
	leftMouseButtonDown = false;
	ignoreNextSizeAndMove = false;

	//Create the default region
	ContentRegion* contentRegion = new ContentRegion();
	regions.push_back(contentRegion);
	topLevelDividersFromTop.push_back(DividerContentEntry(contentRegion, 0));
	topLevelDividersFromLeft.push_back(DividerContentEntry(contentRegion, 0));

	//Drop target info
	dropTargetsChildDockingWindow = 0;
	dropTargetsVisible = false;
	currentRegionDropTarget = 0;
	dropShadowVisible = false;
	dropShadow = NULL;

	//Divider highlight info
	dividerHighlightVisibleWindowCount = 0;
	for(unsigned int i = 0; i < dividerHighlightWindowCount; ++i)
	{
		dividerHighlightWindows[i] = NULL;
	}
}

//----------------------------------------------------------------------------------------
DashboardWindow::~DashboardWindow()
{
	//Destroy the placement target windows for this docking window
	for(std::map<DockTargetPos, HWND>::const_iterator i = dropTargets.begin(); i != dropTargets.end(); ++i)
	{
		DestroyWindow(i->second);
	}
	DestroyWindow(dropShadow);

	//Destroy the divider highlight windows
	for(unsigned int i = 0; i < dividerHighlightWindowCount; ++i)
	{
		DestroyWindow(dividerHighlightWindows[i]);
	}

	//Destroy each allocated region and divider
	for(std::list<ContentRegion*>::const_iterator i = regions.begin(); i != regions.end(); ++i)
	{
		delete *i;
	}
	for(std::list<Divider*>::const_iterator i = dividers.begin(); i != dividers.end(); ++i)
	{
		delete *i;
	}
}

//----------------------------------------------------------------------------------------
//Class registration
//----------------------------------------------------------------------------------------
bool DashboardWindow::RegisterWindowClass(HINSTANCE amoduleHandle)
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

	//Register the DividerHighlight window class
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = DividerHighlightWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = amoduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = dividerHighlightWindowClassName;
	wc.hIconSm       = NULL;
	registerClassExReturn = RegisterClassEx(&wc);
	result &= (registerClassExReturn != 0);

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
bool DashboardWindow::UnregisterWindowClass(HINSTANCE amoduleHandle)
{
	//Unregister each window class registered by this control
	bool result = true;
	result &= (UnregisterClass(windowClassName, amoduleHandle) != 0);
	result &= (UnregisterClass(placementTargetWindowClassName, amoduleHandle) != 0);
	result &= (UnregisterClass(placementShadowWindowClassName, amoduleHandle) != 0);
	result &= (UnregisterClass(dividerHighlightWindowClassName, amoduleHandle) != 0);

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
//Message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK DashboardWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		DashboardWindow* object = (DashboardWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->WndProcPrivate(message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		DashboardWindow* object = new DashboardWindow((HINSTANCE)createStruct->lpCreateParams, hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->WndProcPrivate(message, wParam, lParam);}
	case WM_DESTROY:{
		DashboardWindow* object = (DashboardWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
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
LRESULT DashboardWindow::WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_LBUTTONUP:
		return msgWM_LBUTTONUP(wParam, lParam);
	case WM_LBUTTONDOWN:
		return msgWM_LBUTTONDOWN(wParam, lParam);
	case WM_MOUSEMOVE:
		return msgWM_MOUSEMOVE(wParam, lParam);
	case WM_MOUSELEAVE:
		return msgWM_MOUSELEAVE(wParam, lParam);
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
	case WM_ERASEBKGND:
		return msgWM_ERASEBKGND(wParam, lParam);
	case WM_PAINT:
		return msgWM_PAINT(wParam, lParam);
	case WM_PRINTCLIENT:
		return msgWM_PRINTCLIENT(wParam, lParam);
	case (unsigned int)WindowMessages::AddContentWindow:
		return msgDASHWIN_ADDCONTENTWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::RemoveContentWindow:
		return msgDASHWIN_REMOVECONTENTWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::GetRegionCount:
		return msgDASHWIN_GETREGIONCOUNT(wParam, lParam);
	case (unsigned int)WindowMessages::GetRegionWindow:
		return msgDASHWIN_GETREGIONWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::SetRegionWindow:
		return msgDASHWIN_SETREGIONWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::SaveLayoutToDividerList:
		return msgDASHWIN_SAVELAYOUTTODIVIDERLIST(wParam, lParam);
	case (unsigned int)WindowMessages::LoadLayoutFromDividerList:
		return msgDASHWIN_LOADLAYOUTFROMDIVIDERLIST(wParam, lParam);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
{
	//Read the font metrics for our default font
	HDC hdc = GetDC(hwnd);
	HFONT hfontOld = (HFONT)SelectObject(hdc, controlFont);
	TEXTMETRIC textMetric;
	GetTextMetrics(hdc, &textMetric);
	controlFontHeight = textMetric.tmHeight;
	SelectObject(hdc, hfontOld);
	ReleaseDC(hwnd, hdc);

	//Cursor info
	cursorSizeHorizontal = LoadCursor(NULL, IDC_SIZEWE);
	cursorSizeVertical = LoadCursor(NULL, IDC_SIZENS);
	cursorSizeAll = LoadCursor(NULL, IDC_SIZEALL);

	//Create the placement target windows to indicate the drag and drop docking target for
	//this window
	dropTargets[DockTargetPos::Left] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	dropTargets[DockTargetPos::Right] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	dropTargets[DockTargetPos::Top] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	dropTargets[DockTargetPos::Bottom] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	dropTargets[DockTargetPos::Center] = CreateWindowEx(WS_EX_TOPMOST, placementTargetWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);

	//Create the placement shadow window
	dropShadow = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, placementShadowWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
	SetLayeredWindowAttributes(dropShadow, 0, 128, LWA_ALPHA);

	//Create the divider highlight windows
	for(unsigned int i = 0; i < dividerHighlightWindowCount; ++i)
	{
		dividerHighlightWindows[i] = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, dividerHighlightWindowClassName, L"", WS_POPUP, 0, 0, 0, 0, hwnd, NULL, moduleHandle, (LPVOID)this);
		SetLayeredWindowAttributes(dividerHighlightWindows[i], 0, 128, LWA_ALPHA);
	}

	//Set the background colour of the dividers
	dividerBackgroundColor = WinColor(120, 120, 150);

	//Define pen objects to draw the marked and shadow components of the icons on our
	//dividers. Note that we're building images here for use with the AlphaBlend win32 GDI
	//function, and as per the requirements of that function, we pre-multiply our colour
	//components here by the alpha value.
	//##TODO## Once we have proper canvas and pen classes, use real pen objects here.
	unsigned char markerAlphaComponent = 180;
	unsigned char markerColorComponent = 255;
	unsigned char markerScaledColorComponent = (unsigned char)(((float)markerAlphaComponent * (1.0f / 255.0f)) * (float)markerColorComponent);
	unsigned char shadowAlphaComponent = 60;
	unsigned char shadowScaledColorComponent = 0;
	std::map<int, unsigned char> markerPenColorComponents;
	markerPenColorComponents[0] = markerScaledColorComponent;
	markerPenColorComponents[1] = markerScaledColorComponent;
	markerPenColorComponents[2] = markerScaledColorComponent;
	markerPenColorComponents[3] = markerAlphaComponent;
	std::map<int, unsigned char> shadowPenColorComponents;
	shadowPenColorComponents[0] = shadowScaledColorComponent;
	shadowPenColorComponents[1] = shadowScaledColorComponent;
	shadowPenColorComponents[2] = shadowScaledColorComponent;
	shadowPenColorComponents[3] = shadowAlphaComponent;

	//Draw the image used for our grabbers on the dividers
	unsigned int grabberImageWidth = 4;
	unsigned int grabberImageHeight = 4;
	grabberImage.SetImageFormat(grabberImageWidth, grabberImageHeight, IImage::PIXELFORMAT_RGBA);
	DrawImageSquare(grabberImage, 2, 2, 2, 2, shadowPenColorComponents);
	DrawImageSquare(grabberImage, 1, 1, 2, 2, markerPenColorComponents);

	//Draw the images used for the extend buttons on our dividers
	unsigned int arrowImageWidth = 8;
	unsigned int arrowImageHeight = 8;
	arrowImageTopLeft.SetImageFormat(arrowImageWidth, arrowImageHeight, IImage::PIXELFORMAT_RGBA);
	DrawImageLine(arrowImageTopLeft, 2, 2, 7, 2, shadowPenColorComponents);
	DrawImageLine(arrowImageTopLeft, 2, 2, 2, 7, shadowPenColorComponents);
	DrawImageLine(arrowImageTopLeft, 2, 2, 7, 7, shadowPenColorComponents);
	DrawImageLine(arrowImageTopLeft, 1, 1, 6, 1, markerPenColorComponents);
	DrawImageLine(arrowImageTopLeft, 1, 1, 1, 6, markerPenColorComponents);
	DrawImageLine(arrowImageTopLeft, 1, 1, 6, 6, markerPenColorComponents);
	arrowImageTopRight.SetImageFormat(arrowImageWidth, arrowImageHeight, IImage::PIXELFORMAT_RGBA);
	DrawImageLine(arrowImageTopRight, 2, 2, 7, 2, shadowPenColorComponents);
	DrawImageLine(arrowImageTopRight, 7, 2, 7, 7, shadowPenColorComponents);
	DrawImageLine(arrowImageTopRight, 2, 7, 7, 2, shadowPenColorComponents);
	DrawImageLine(arrowImageTopRight, 1, 1, 6, 1, markerPenColorComponents);
	DrawImageLine(arrowImageTopRight, 6, 1, 6, 6, markerPenColorComponents);
	DrawImageLine(arrowImageTopRight, 1, 6, 6, 1, markerPenColorComponents);
	arrowImageBottomLeft.SetImageFormat(arrowImageWidth, arrowImageHeight, IImage::PIXELFORMAT_RGBA);
	DrawImageLine(arrowImageBottomLeft, 2, 2, 2, 7, shadowPenColorComponents);
	DrawImageLine(arrowImageBottomLeft, 2, 7, 7, 7, shadowPenColorComponents);
	DrawImageLine(arrowImageBottomLeft, 7, 2, 2, 7, shadowPenColorComponents);
	DrawImageLine(arrowImageBottomLeft, 1, 1, 1, 6, markerPenColorComponents);
	DrawImageLine(arrowImageBottomLeft, 1, 6, 6, 6, markerPenColorComponents);
	DrawImageLine(arrowImageBottomLeft, 6, 1, 1, 6, markerPenColorComponents);
	arrowImageBottomRight.SetImageFormat(arrowImageWidth, arrowImageHeight, IImage::PIXELFORMAT_RGBA);
	DrawImageLine(arrowImageBottomRight, 7, 2, 7, 7, shadowPenColorComponents);
	DrawImageLine(arrowImageBottomRight, 2, 7, 7, 7, shadowPenColorComponents);
	DrawImageLine(arrowImageBottomRight, 2, 2, 7, 7, shadowPenColorComponents);
	DrawImageLine(arrowImageBottomRight, 6, 1, 6, 6, markerPenColorComponents);
	DrawImageLine(arrowImageBottomRight, 1, 6, 6, 6, markerPenColorComponents);
	DrawImageLine(arrowImageBottomRight, 1, 1, 6, 6, markerPenColorComponents);

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
LRESULT DashboardWindow::msgWM_DESTROY(WPARAM wParam, LPARAM lParam)
{
	for(std::list<ContentRegion*>::const_iterator i = regions.begin(); i != regions.end(); ++i)
	{
		ContentRegion& contentRegion = *(*i);
		if(contentRegion.windowAsDockingWindow != 0)
		{
			contentRegion.windowAsDockingWindow->NotifyRemovedFromParent();
			contentRegion.windowAsDockingWindow = 0;
		}
	}
	if(parentDockingWindow != 0)
	{
		parentDockingWindow->RemoveChildContainer(this);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_SIZING(WPARAM wParam, LPARAM lParam)
{
	//If a window size operation isn't currently being tracked, or if this docking window
	//isn't docked to a parent, perform the default processing for this window message.
	RECT& rect = *((RECT*)lParam);
	if(!windowDragInProgress || (parentDockingWindow == 0))
	{
		return DefWindowProc(hwnd, WM_SIZING, wParam, lParam);
	}

	//Ensure that we only allow the edges of this window to be resized that the parent
	//docking window allows to be resized.
	if(!parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Left))
	{
		rect.left = windowSizeMoveInitialPos.left;
	}
	if(!parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Right))
	{
		rect.right = windowSizeMoveInitialPos.right;
	}
	if(!parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Top))
	{
		rect.top = windowSizeMoveInitialPos.top;
	}
	if(!parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Bottom))
	{
		rect.bottom = windowSizeMoveInitialPos.bottom;
	}

	//Notify the parent docking window of our new desired with and height
	int newDesiredWidth = rect.right - rect.left;
	int newDesiredHeight = rect.bottom - rect.top;
	parentDockingWindow->UpdateDesiredChildContainerSize(this, newDesiredWidth, newDesiredHeight);

	//Return TRUE to inform windows that we processed this message
	return TRUE;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	int newClientWidth = rect.right;
	int newClientHeight = rect.bottom;

	//If this control has changed in size, process the size change event.
	if((controlWidth != newClientWidth) || (controlHeight != newClientHeight))
	{
		HandleSizeChanged(newClientWidth, newClientHeight);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_LBUTTONUP(WPARAM wParam, LPARAM lParam)
{
	//Terminate a drag operation if one is in progress
	if(dividerDragActive)
	{
		dividerDragActive = false;
		verticalDividersBeingDragged.clear();
		horizontalDividersBeingDragged.clear();
		ReleaseCapture();
		return 0;
	}
	return DefWindowProc(hwnd, WM_LBUTTONUP, wParam, lParam);;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	//Retrieve the current cursor location
	int cursorPosX = (int)(short)LOWORD(lParam);
	int cursorPosY = (int)(short)HIWORD(lParam);

	//Attempt to locate any dividers which are under the cursor
	std::list<Divider*> dividersUnderCursor = GetDividersAtPosition(cursorPosX, cursorPosY);

	//If no dividers are under the current cursor position, abort any further processing.
	if(dividersUnderCursor.empty())
	{
		return DefWindowProc(hwnd, WM_LBUTTONDOWN, wParam, lParam);
	}

	//If there are currently any visible divider highlight windows, hide them now.
	if(dividerHighlightVisibleWindowCount > 0)
	{
		dividerHighlightVisibleWindowCount = 0;
		HDWP deferWindowPosSession = BeginDeferWindowPos(dividerHighlightWindowCount);
		for(unsigned int i = 0; i < dividerHighlightWindowCount; ++i)
		{
			DeferWindowPos(deferWindowPosSession, dividerHighlightWindows[i], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | ((i < dividerHighlightVisibleWindowCount)? SWP_SHOWWINDOW: SWP_HIDEWINDOW));
		}
		EndDeferWindowPos(deferWindowPosSession);
	}

	//If this click needs to trigger a divider extend operation, perform it now, and abort
	//any further processing.
	if(dividersUnderCursor.size() == 1)
	{
		Divider* targetDivider = dividersUnderCursor.front();
		std::list<DividerExtendButtonPosition> extendButtonPositions = GetExtendButtonPositionsForDivider(targetDivider);
		for(std::list<DividerExtendButtonPosition>::const_iterator i = extendButtonPositions.begin(); i != extendButtonPositions.end(); ++i)
		{
			const DividerExtendButtonPosition& extendButtonInfo = *i;
			if((targetDivider->vertical && (cursorPosY >= (targetDivider->cachedPosY + extendButtonInfo.dividerExtendButtonStartPos)) && (cursorPosY < (targetDivider->cachedPosY + extendButtonInfo.dividerExtendButtonStartPos + extendButtonInfo.dividerExtendButtonLength)))
			|| (!targetDivider->vertical && (cursorPosX >= (targetDivider->cachedPosX + extendButtonInfo.dividerExtendButtonStartPos)) && (cursorPosX < (targetDivider->cachedPosX + extendButtonInfo.dividerExtendButtonStartPos + extendButtonInfo.dividerExtendButtonLength))))
			{
				ExtendDivider(targetDivider, extendButtonInfo.extendFromStart, extendButtonInfo.extendPrecedingContent);
				return 0;
			}
		}
	}

	//Record information on this drag operation
	dividerDragActive = true;
	dragLastPosX = cursorPosX;
	dragLastPosY = cursorPosY;
	dragPosOverLimitX = 0;
	dragPosOverLimitY = 0;
	dragPosSnapDisplacementX = 0;
	dragPosSnapDisplacementY = 0;

	//Determine if a split grabber is being dragged, and prepare the dividers to make them
	//draggable if one is.
	bool draggingSplitDivider = false;
	if(dividersUnderCursor.size() == 1)
	{
		Divider* divider = dividersUnderCursor.front();
		std::list<DividerSplitPosition> splitPositions = GetSplitPositionsForDivider(dividersUnderCursor.front());
		std::list<DividerSplitPosition>::const_iterator splitPositionIterator = splitPositions.begin();
		while(!draggingSplitDivider && (splitPositionIterator != splitPositions.end()))
		{
			//If the mouse is within the area of this divider split grabber, flag that
			//we're performing a split drag operation.
			const DividerSplitPosition& splitPositionInfo = *splitPositionIterator;
			if(divider->vertical && (cursorPosY >= (divider->cachedPosY + splitPositionInfo.dividerSplitterStartPos)) && (cursorPosY < (divider->cachedPosY + splitPositionInfo.dividerSplitterStartPos + splitPositionInfo.dividerSplitterLength)))
			{
				draggingSplitDivider = true;
			}
			else if(!divider->vertical && (cursorPosX >= (divider->cachedPosX + splitPositionInfo.dividerSplitterStartPos)) && (cursorPosX < (divider->cachedPosX + splitPositionInfo.dividerSplitterStartPos + splitPositionInfo.dividerSplitterLength)))
			{
				draggingSplitDivider = true;
			}

			//If we need to swap which divider continues through at the target divider
			//junction in order to perform this drag operation, change the dividers now.
			if(draggingSplitDivider && splitPositionInfo.mergeRequired)
			{
				//Split the target divider, and merge surrounding dividers.
				Divider* splitDivider = divider;
				if((splitPositionInfo.startPrecedingMergeDivider != 0) && (splitPositionInfo.startFollowingMergeDivider != 0) && (splitPositionInfo.startPrecedingMergeDivider != splitPositionInfo.startFollowingMergeDivider))
				{
					splitDivider = SwapContinuingDividerAtJunction(splitPositionInfo.startPrecedingMergeDivider, splitPositionInfo.startFollowingMergeDivider, divider);
				}
				if((splitPositionInfo.endPrecedingMergeDivider != 0) && (splitPositionInfo.endFollowingMergeDivider != 0) && (splitPositionInfo.endPrecedingMergeDivider != splitPositionInfo.endFollowingMergeDivider))
				{
					SwapContinuingDividerAtJunction(splitPositionInfo.endPrecedingMergeDivider, splitPositionInfo.endFollowingMergeDivider, splitDivider);
				}

				//Replace the divider under the cursor with the new split divider
				dividersUnderCursor.clear();
				dividersUnderCursor.push_back(splitDivider);
			}

			//Advance to the next split grabber
			++splitPositionIterator;
		}
	}

	//Populate the lists of vertical and horizontal dividers being dragged
	verticalDividersBeingDragged.clear();
	horizontalDividersBeingDragged.clear();
	for(std::list<Divider*>::const_iterator i = dividersUnderCursor.begin(); i != dividersUnderCursor.end(); ++i)
	{
		//Add the target divider to the list of dividers being dragged
		Divider* divider = *i;
		std::set<Divider*>& targetDividerList = (divider->vertical)? verticalDividersBeingDragged: horizontalDividersBeingDragged;
		targetDividerList.insert(divider);

		//If we're not dragging a split grabber, add all merge candidates of the target
		//divider to the list of dividers being dragged.
		if(!draggingSplitDivider)
		{
			std::set<Divider*> mergeCandidates = GetMergeCandidates(divider, false);
			targetDividerList.insert(mergeCandidates.begin(), mergeCandidates.end());
		}
	}

	//Capture the mouse to ensure we continue to get mouse notifications if the cursor
	//moves outside the window
	SetCapture(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_MOUSEMOVE(WPARAM wParam, LPARAM lParam)
{
	//Retrieve the current cursor location
	int cursorPosX = (int)(short)LOWORD(lParam);
	int cursorPosY = (int)(short)HIWORD(lParam);

	//If a drag operation isn't currently in progress for a divider, update the divider
	//highlight windows as required, and abort any further processing.
	if(!dividerDragActive)
	{
		bool dropShadowRequired = false;
		unsigned int previousDividerHighlightVisibleWindowCount = dividerHighlightVisibleWindowCount;
		dividerHighlightVisibleWindowCount = 0;
		std::list<Divider*> dividersUnderCursor = GetDividersAtPosition(cursorPosX, cursorPosY);
		if(dividersUnderCursor.size() == 1)
		{
			//If the cursor is over a divider split grabber, position the divider
			//highlight window.
			Divider* targetDivider = dividersUnderCursor.front();
			std::list<DividerSplitPosition> splitPositions = GetSplitPositionsForDivider(targetDivider);
			std::list<DividerSplitPosition>::const_iterator splitPositionIterator = splitPositions.begin();
			while(splitPositionIterator != splitPositions.end())
			{
				//Calculate the position and size of the splitter grabber for this split
				//position
				const DividerSplitPosition& splitPosition = *splitPositionIterator;
				int splitterScreenPosX = targetDivider->cachedPosX + ((targetDivider->vertical)? 0: splitPosition.dividerSplitterStartPos);
				int splitterScreenPosY = targetDivider->cachedPosY + ((targetDivider->vertical)? splitPosition.dividerSplitterStartPos: 0);
				int splitterWidth = (targetDivider->vertical)? targetDivider->cachedWidth: splitPosition.dividerSplitterLength;
				int splitterHeight = (targetDivider->vertical)? splitPosition.dividerSplitterLength: targetDivider->cachedHeight;

				//If the cursor isn't over this splitter grabber, advance to the next
				//split position.
				if((cursorPosX < splitterScreenPosX) || (cursorPosX >= (splitterScreenPosX + splitterWidth)) || (cursorPosY < splitterScreenPosY) || (cursorPosY >= (splitterScreenPosY + splitterHeight)))
				{
					++splitPositionIterator;
					continue;
				}

				//Calculate the position and size of the divider highlight window
				int dividerHighlightPosX;
				int dividerHighlightPosY;
				int dividerHighlightWidth;
				int dividerHighlightHeight;
				if(targetDivider->vertical)
				{
					dividerHighlightPosX = targetDivider->cachedPosX;
					dividerHighlightPosY = (splitPosition.startPrecedingMergeDivider != 0)? (splitPosition.startPrecedingMergeDivider->cachedPosY + splitPosition.startPrecedingMergeDivider->cachedHeight): targetDivider->cachedPosY;
					dividerHighlightWidth = targetDivider->cachedWidth;
					dividerHighlightHeight = (splitPosition.endPrecedingMergeDivider != 0)? (splitPosition.endPrecedingMergeDivider->cachedPosY - dividerHighlightPosY): ((targetDivider->cachedPosY + targetDivider->cachedHeight) - dividerHighlightPosY);
				}
				else
				{
					dividerHighlightPosX = (splitPosition.startPrecedingMergeDivider != 0)? (splitPosition.startPrecedingMergeDivider->cachedPosX + splitPosition.startPrecedingMergeDivider->cachedWidth): targetDivider->cachedPosX;
					dividerHighlightPosY = targetDivider->cachedPosY;
					dividerHighlightWidth = (splitPosition.endPrecedingMergeDivider != 0)? (splitPosition.endPrecedingMergeDivider->cachedPosX - dividerHighlightPosX): ((targetDivider->cachedPosX + targetDivider->cachedWidth) - dividerHighlightPosX);
					dividerHighlightHeight = targetDivider->cachedHeight;
				}

				//Calculate the position of the target divider in screen coordinates
				POINT point;
				point.x = dividerHighlightPosX;
				point.y = dividerHighlightPosY;
				ClientToScreen(hwnd, &point);
				int dividerHighlightScreenPosX = point.x;
				int dividerHighlightScreenPosY = point.y;

				//Position and size the divider highlight window
				SetWindowPos(dividerHighlightWindows[0], NULL, dividerHighlightScreenPosX, dividerHighlightScreenPosY, dividerHighlightWidth, dividerHighlightHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

				//Flag that one divider highlight window is visible, and stop looking
				//through the list of split positions.
				dividerHighlightVisibleWindowCount = 1;
				break;
			}

			//If the cursor is over a divider extend button, position the divider
			//highlight window.
			std::list<DividerExtendButtonPosition> extendButtonPositions = GetExtendButtonPositionsForDivider(targetDivider);
			std::list<DividerExtendButtonPosition>::const_iterator extendButtonPositionsIterator = extendButtonPositions.begin();
			while(extendButtonPositionsIterator != extendButtonPositions.end())
			{
				//If the cursor isn't over this extend button, advance to the next extend
				//button.
				const DividerExtendButtonPosition& extendButtonInfo = *extendButtonPositionsIterator;
				if(!(targetDivider->vertical && (cursorPosY >= (targetDivider->cachedPosY + extendButtonInfo.dividerExtendButtonStartPos)) && (cursorPosY < (targetDivider->cachedPosY + extendButtonInfo.dividerExtendButtonStartPos + extendButtonInfo.dividerExtendButtonLength)))
				&& !(!targetDivider->vertical && (cursorPosX >= (targetDivider->cachedPosX + extendButtonInfo.dividerExtendButtonStartPos)) && (cursorPosX < (targetDivider->cachedPosX + extendButtonInfo.dividerExtendButtonStartPos + extendButtonInfo.dividerExtendButtonLength))))
				{
					++extendButtonPositionsIterator;
					continue;
				}

				//Calculate the position and size of the drop shadow window
				DockTargetPos dropShadowDividerEdge;
				int dropShadowPosX;
				int dropShadowPosY;
				int dropShadowWidth;
				int dropShadowHeight;
				if(targetDivider->vertical)
				{
					dropShadowDividerEdge = (extendButtonInfo.extendPrecedingContent)? DockTargetPos::Left: DockTargetPos::Right;
					dropShadowPosX = (extendButtonInfo.extendPrecedingContent)? extendButtonInfo.regionToTruncate->cachedPosX: targetDivider->cachedPosX;
					dropShadowPosY = (extendButtonInfo.extendFromStart)? extendButtonInfo.regionToTruncate->cachedPosY: targetDivider->cachedPosY;
					dropShadowWidth = (extendButtonInfo.extendPrecedingContent)? ((targetDivider->cachedPosX + targetDivider->cachedWidth) - extendButtonInfo.regionToTruncate->cachedPosX): ((extendButtonInfo.regionToTruncate->cachedPosX + extendButtonInfo.regionToTruncate->width) - targetDivider->cachedPosX);
					dropShadowHeight = (extendButtonInfo.extendFromStart)? ((targetDivider->cachedPosY + targetDivider->cachedHeight) - extendButtonInfo.regionToTruncate->cachedPosY): ((extendButtonInfo.regionToTruncate->cachedPosY + extendButtonInfo.regionToTruncate->height) - targetDivider->cachedPosY);
				}
				else
				{
					dropShadowDividerEdge = (extendButtonInfo.extendPrecedingContent)? DockTargetPos::Top: DockTargetPos::Bottom;
					dropShadowPosX = (extendButtonInfo.extendFromStart)? extendButtonInfo.regionToTruncate->cachedPosX: targetDivider->cachedPosX;
					dropShadowPosY = (extendButtonInfo.extendPrecedingContent)? extendButtonInfo.regionToTruncate->cachedPosY: targetDivider->cachedPosY;
					dropShadowWidth = (extendButtonInfo.extendFromStart)? ((targetDivider->cachedPosX + targetDivider->cachedWidth) - extendButtonInfo.regionToTruncate->cachedPosX): ((extendButtonInfo.regionToTruncate->cachedPosX + extendButtonInfo.regionToTruncate->width) - targetDivider->cachedPosX);
					dropShadowHeight = (extendButtonInfo.extendPrecedingContent)? ((targetDivider->cachedPosY + targetDivider->cachedHeight) - extendButtonInfo.regionToTruncate->cachedPosY): ((extendButtonInfo.regionToTruncate->cachedPosY + extendButtonInfo.regionToTruncate->height) - targetDivider->cachedPosY);
				}

				//Record the edge to anchor the drop shadow against
				dropShadowCurrentPos = dropShadowDividerEdge;

				//Calculate the position of the drop shadow window in screen coordinates
				POINT point;
				point.x = dropShadowPosX;
				point.y = dropShadowPosY;
				ClientToScreen(hwnd, &point);
				int dropShadowScreenPosX = point.x;
				int dropShadowScreenPosY = point.y;

				//Update the drop shadow position and size
				SetWindowPos(dropShadow, NULL, dropShadowScreenPosX, dropShadowScreenPosY, dropShadowWidth, dropShadowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

				//Show the drop shadow window
				if(!dropShadowVisible)
				{
					SetWindowPos(dropShadow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
					dropShadowVisible = true;
				}

				//Flag that a drop shadow window is required, and stop looking through the
				//list of divider extend buttons.
				dropShadowRequired = true;
				break;
			}
		}

		//Hide the drop shadow window if it is currently visible and no longer required
		if(!dropShadowRequired && dropShadowVisible)
		{
			SetWindowPos(dropShadow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
			dropShadowVisible = false;
		}

		//If the number of visible divider highlight windows has changed, update the
		//window visibility now.
		if(dividerHighlightVisibleWindowCount != previousDividerHighlightVisibleWindowCount)
		{
			HDWP deferWindowPosSession = BeginDeferWindowPos(dividerHighlightWindowCount);
			for(unsigned int i = 0; i < dividerHighlightWindowCount; ++i)
			{
				DeferWindowPos(deferWindowPosSession, dividerHighlightWindows[i], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | ((i < dividerHighlightVisibleWindowCount)? SWP_SHOWWINDOW: SWP_HIDEWINDOW));
			}
			EndDeferWindowPos(deferWindowPosSession);
		}

		//Request WM_MOUSELEAVE messages to be generated if we're currently showing
		//divider highlight or drop shadow windows
		if(dropShadowVisible || (dividerHighlightVisibleWindowCount > 0))
		{
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof(trackMouseEvent);
			trackMouseEvent.dwFlags = TME_LEAVE;
			trackMouseEvent.hwndTrack = hwnd;
			TrackMouseEvent(&trackMouseEvent);
		}

		//Since a divider isn't currently being dragged, abort any further processing.
		return DefWindowProc(hwnd, WM_MOUSEMOVE, wParam, lParam);
	}

	//Calculate the displacement from the previous processed cursor location to the new
	//cursor location
	int cursorDisplacementX = cursorPosX - dragLastPosX;
	int cursorDisplacementY = cursorPosY - dragLastPosY;

	//Combine displacement from the last drag operation which was outside the maximum
	//bounds of movement
	cursorDisplacementX += dragPosOverLimitX;
	cursorDisplacementY += dragPosOverLimitY;
	dragPosOverLimitX = 0;
	dragPosOverLimitY = 0;

	//Subtract displacement from the last drag operation that was added due to a snapping
	//operation
	cursorDisplacementX -= dragPosSnapDisplacementX;
	cursorDisplacementY -= dragPosSnapDisplacementY;
	dragPosSnapDisplacementX = 0;
	dragPosSnapDisplacementY = 0;

	//If a vertical divider is being dragged, adjust any content regions affected by this
	//divider drag.
	int maxNegativeDisplacementX = std::numeric_limits<int>::max();
	int maxPositiveDisplacementX = std::numeric_limits<int>::max();
	if(!verticalDividersBeingDragged.empty())
	{
		//Calculate the maximum and minimum horizontal displacement value
		for(std::set<Divider*>::const_iterator dragDividerIterator = verticalDividersBeingDragged.begin(); dragDividerIterator != verticalDividersBeingDragged.end(); ++dragDividerIterator)
		{
			Divider* divider = *dragDividerIterator;
			for(std::list<DividerContentEntry>::const_iterator i = divider->precedingContent.begin(); i != divider->precedingContent.end(); ++i)
			{
				maxNegativeDisplacementX = (i->contentRegion->width < maxNegativeDisplacementX)? i->contentRegion->width: maxNegativeDisplacementX;
			}
			for(std::list<DividerContentEntry>::const_iterator i = divider->followingContent.begin(); i != divider->followingContent.end(); ++i)
			{
				maxPositiveDisplacementX = (i->contentRegion->width < maxPositiveDisplacementX)? i->contentRegion->width: maxPositiveDisplacementX;
			}
		}

		//Limit the horizontal displacement by the minimum and maximum values, and store
		//any excess displacement to factor into the next drag operation.
		if(cursorDisplacementX > maxPositiveDisplacementX)
		{
			dragPosOverLimitX = (cursorDisplacementX - maxPositiveDisplacementX);
			cursorDisplacementX = maxPositiveDisplacementX;
		}
		else if(cursorDisplacementX < -maxNegativeDisplacementX)
		{
			dragPosOverLimitX = (cursorDisplacementX + maxNegativeDisplacementX);
			cursorDisplacementX = -maxNegativeDisplacementX;
		}

		//Update the width of each affected content region
		for(std::set<Divider*>::const_iterator dragDividerIterator = verticalDividersBeingDragged.begin(); dragDividerIterator != verticalDividersBeingDragged.end(); ++dragDividerIterator)
		{
			Divider* divider = *dragDividerIterator;
			for(std::list<DividerContentEntry>::const_iterator i = divider->precedingContent.begin(); i != divider->precedingContent.end(); ++i)
			{
				i->contentRegion->width += cursorDisplacementX;
			}
			for(std::list<DividerContentEntry>::const_iterator i = divider->followingContent.begin(); i != divider->followingContent.end(); ++i)
			{
				i->contentRegion->width -= cursorDisplacementX;
			}
		}
	}

	//If a horizontal divider is being dragged, adjust any content regions affected by
	//this divider drag.
	int maxNegativeDisplacementY = std::numeric_limits<int>::max();
	int maxPositiveDisplacementY = std::numeric_limits<int>::max();
	if(!horizontalDividersBeingDragged.empty())
	{
		//Calculate the maximum and minimum vertical displacement value
		for(std::set<Divider*>::const_iterator dragDividerIterator = horizontalDividersBeingDragged.begin(); dragDividerIterator != horizontalDividersBeingDragged.end(); ++dragDividerIterator)
		{
			Divider* divider = *dragDividerIterator;
			for(std::list<DividerContentEntry>::const_iterator i = divider->precedingContent.begin(); i != divider->precedingContent.end(); ++i)
			{
				maxNegativeDisplacementY = (i->contentRegion->height < maxNegativeDisplacementY)? i->contentRegion->height: maxNegativeDisplacementY;
			}
			for(std::list<DividerContentEntry>::const_iterator i = divider->followingContent.begin(); i != divider->followingContent.end(); ++i)
			{
				maxPositiveDisplacementY = (i->contentRegion->height < maxPositiveDisplacementY)? i->contentRegion->height: maxPositiveDisplacementY;
			}
		}

		//Limit the vertical displacement by the minimum and maximum values, and store any
		//excess displacement to factor into the next drag operation.
		if(cursorDisplacementY > maxPositiveDisplacementY)
		{
			dragPosOverLimitY = (cursorDisplacementY - maxPositiveDisplacementY);
			cursorDisplacementY = maxPositiveDisplacementY;
		}
		else if(cursorDisplacementY < -maxNegativeDisplacementY)
		{
			dragPosOverLimitY = (cursorDisplacementY + maxNegativeDisplacementY);
			cursorDisplacementY = -maxNegativeDisplacementY;
		}

		//Update the height of each affected content region
		for(std::set<Divider*>::const_iterator dragDividerIterator = horizontalDividersBeingDragged.begin(); dragDividerIterator != horizontalDividersBeingDragged.end(); ++dragDividerIterator)
		{
			Divider* divider = *dragDividerIterator;
			for(std::list<DividerContentEntry>::const_iterator i = divider->precedingContent.begin(); i != divider->precedingContent.end(); ++i)
			{
				i->contentRegion->height += cursorDisplacementY;
			}
			for(std::list<DividerContentEntry>::const_iterator i = divider->followingContent.begin(); i != divider->followingContent.end(); ++i)
			{
				i->contentRegion->height -= cursorDisplacementY;
			}
		}
	}

	//Update the last drag positions
	dragLastPosX = cursorPosX;
	dragLastPosY = cursorPosY;
	
	//Update our cached divider locations using the new region sizes. Note that we need to
	//do this before performing snapping, since we need to test against the new divider
	//positions. We defer window updates here since we don't want to trigger two size
	//operations as a result.
	UpdateCachedLocations(true);

	//Apply vertical snapping for this drag operation
	std::set<Divider*> verticalDragDividersSnapped;
	if(!verticalDividersBeingDragged.empty())
	{
		//Build sorted sets of dividers based on their distances from any dividers being
		//dragged
		std::map<int, std::set<Divider*>> dividerSnapPositionsNegativeSorted;
		std::map<int, std::set<Divider*>> dividerSnapPositionsPositiveSorted;
		for(std::set<Divider*>::const_iterator dragDividerIterator = verticalDividersBeingDragged.begin(); dragDividerIterator != verticalDividersBeingDragged.end(); ++dragDividerIterator)
		{
			Divider* divider = *dragDividerIterator;
			std::set<Divider*> mergeCandidatesForDivider = GetMergeCandidates(divider, true);
			for(std::set<Divider*>::const_iterator i = mergeCandidatesForDivider.begin(); i != mergeCandidatesForDivider.end(); ++i)
			{
				//If this divider merge candidate is being dragged, skip it.
				Divider* mergeCandidate = *i;
				if(verticalDividersBeingDragged.find(mergeCandidate) != verticalDividersBeingDragged.end())
				{
					continue;
				}

				//Calculate the distance to this merge candidate, and add it to the list
				//of divider snap positions.
				int distanceToMergeCandidate = mergeCandidate->cachedPosX - divider->cachedPosX;
				if(distanceToMergeCandidate < 0)
				{
					dividerSnapPositionsNegativeSorted[-distanceToMergeCandidate].insert(divider);
				}
				else
				{
					dividerSnapPositionsPositiveSorted[distanceToMergeCandidate].insert(divider);
				}
			}
		}

		//Calculate the snap displacement value based on the identified snap positions
		bool positiveSnapPosValid = (!dividerSnapPositionsPositiveSorted.empty() && (dividerSnapPositionsPositiveSorted.begin()->first <= maxPositiveDisplacementX));
		bool negativeSnapPosValid = (!dividerSnapPositionsNegativeSorted.empty() && (dividerSnapPositionsNegativeSorted.begin()->first <= maxNegativeDisplacementX));
		int snapPosDisplacement = 0;
		std::set<Divider*> dividerSnapSet;
		if(positiveSnapPosValid && (!negativeSnapPosValid || (dividerSnapPositionsPositiveSorted.begin()->first >= dividerSnapPositionsNegativeSorted.begin()->first)))
		{
			snapPosDisplacement = dividerSnapPositionsPositiveSorted.begin()->first;
			verticalDragDividersSnapped = dividerSnapPositionsPositiveSorted.begin()->second;
		}
		else if(negativeSnapPosValid)
		{
			snapPosDisplacement = -dividerSnapPositionsNegativeSorted.begin()->first;
			verticalDragDividersSnapped = dividerSnapPositionsNegativeSorted.begin()->second;
		}

		//Update the width of each affected content region
		for(std::set<Divider*>::const_iterator dragDividerIterator = verticalDividersBeingDragged.begin(); dragDividerIterator != verticalDividersBeingDragged.end(); ++dragDividerIterator)
		{
			Divider* divider = *dragDividerIterator;
			for(std::list<DividerContentEntry>::const_iterator i = divider->precedingContent.begin(); i != divider->precedingContent.end(); ++i)
			{
				i->contentRegion->width += snapPosDisplacement;
			}
			for(std::list<DividerContentEntry>::const_iterator i = divider->followingContent.begin(); i != divider->followingContent.end(); ++i)
			{
				i->contentRegion->width -= snapPosDisplacement;
			}
		}

		//Record the applied snap displacement value, so that it can be removed in the
		//next drag operation.
		dragPosSnapDisplacementX = snapPosDisplacement;
	}

	//Apply horizontal snapping for this drag operation
	std::set<Divider*> horizontalDragDividersSnapped;
	if(!horizontalDividersBeingDragged.empty())
	{
		//Build a set of all snap targets for the set of horizontal dividers being dragged
		std::set<Divider*> dividerSnapPositions;
		for(std::set<Divider*>::const_iterator dragDividerIterator = horizontalDividersBeingDragged.begin(); dragDividerIterator != horizontalDividersBeingDragged.end(); ++dragDividerIterator)
		{
			std::set<Divider*> mergeCandidatesForDivider = GetMergeCandidates(*dragDividerIterator, true);
			dividerSnapPositions.insert(mergeCandidatesForDivider.begin(), mergeCandidatesForDivider.end());
		}

		//Build sorted sets of dividers based on their distances from any dividers being
		//dragged
		std::map<int, std::set<Divider*>> dividerSnapPositionsNegativeSorted;
		std::map<int, std::set<Divider*>> dividerSnapPositionsPositiveSorted;
		for(std::set<Divider*>::const_iterator dragDividerIterator = horizontalDividersBeingDragged.begin(); dragDividerIterator != horizontalDividersBeingDragged.end(); ++dragDividerIterator)
		{
			Divider* divider = *dragDividerIterator;
			for(std::set<Divider*>::const_iterator i = dividerSnapPositions.begin(); i != dividerSnapPositions.end(); ++i)
			{
				//If this divider merge candidate is being dragged, skip it.
				Divider* mergeCandidate = *i;
				if(horizontalDividersBeingDragged.find(mergeCandidate) != horizontalDividersBeingDragged.end())
				{
					continue;
				}

				//Calculate the distance to this merge candidate, and add it to the list
				//of divider snap positions.
				int distanceToMergeCandidate = mergeCandidate->cachedPosY - divider->cachedPosY;
				if(distanceToMergeCandidate < 0)
				{
					dividerSnapPositionsNegativeSorted[-distanceToMergeCandidate].insert(divider);
				}
				else
				{
					dividerSnapPositionsPositiveSorted[distanceToMergeCandidate].insert(divider);
				}
			}
		}

		//Calculate the snap displacement value based on the identified snap positions
		bool positiveSnapPosValid = (!dividerSnapPositionsPositiveSorted.empty() && (dividerSnapPositionsPositiveSorted.begin()->first <= maxPositiveDisplacementY));
		bool negativeSnapPosValid = (!dividerSnapPositionsNegativeSorted.empty() && (dividerSnapPositionsNegativeSorted.begin()->first <= maxNegativeDisplacementY));
		int snapPosDisplacement = 0;
		std::set<Divider*> dividerSnapSet;
		if(positiveSnapPosValid && (!negativeSnapPosValid || (dividerSnapPositionsPositiveSorted.begin()->first >= dividerSnapPositionsNegativeSorted.begin()->first)))
		{
			snapPosDisplacement = dividerSnapPositionsPositiveSorted.begin()->first;
			horizontalDragDividersSnapped = dividerSnapPositionsPositiveSorted.begin()->second;
		}
		else if(negativeSnapPosValid)
		{
			snapPosDisplacement = -dividerSnapPositionsNegativeSorted.begin()->first;
			horizontalDragDividersSnapped = dividerSnapPositionsNegativeSorted.begin()->second;
		}

		//Update the height of each affected content region
		for(std::set<Divider*>::const_iterator dragDividerIterator = horizontalDividersBeingDragged.begin(); dragDividerIterator != horizontalDividersBeingDragged.end(); ++dragDividerIterator)
		{
			Divider* divider = *dragDividerIterator;
			for(std::list<DividerContentEntry>::const_iterator i = divider->precedingContent.begin(); i != divider->precedingContent.end(); ++i)
			{
				i->contentRegion->height += snapPosDisplacement;
			}
			for(std::list<DividerContentEntry>::const_iterator i = divider->followingContent.begin(); i != divider->followingContent.end(); ++i)
			{
				i->contentRegion->height -= snapPosDisplacement;
			}
		}

		//Record the applied snap displacement value, so that it can be removed in the
		//next drag operation.
		dragPosSnapDisplacementY = snapPosDisplacement;
	}

	//Retrieve the number of divider highlight windows that are currently visible, and
	//reset the count to 0.
	unsigned int previousDividerHighlightVisibleWindowCount = dividerHighlightVisibleWindowCount;
	dividerHighlightVisibleWindowCount = 0;

	//If we applied divider snapping, update our cached divider locations again, since the
	//region sizes have changed again, and position the divider highlight windows as
	//required.
	if((dragPosSnapDisplacementX != 0) || (dragPosSnapDisplacementY != 0))
	{
		//Update the cached locations of all regions and dividers, but defer the actual
		//window position changes at this time.
		UpdateCachedLocations(true);

		//Position any required divider highlight windows for this snap operation
		std::set<Divider*> dragDividersSnappedProcessed;
		std::set<Divider*> dragDividersSnapped;
		dragDividersSnapped.insert(verticalDragDividersSnapped.begin(), verticalDragDividersSnapped.end());
		dragDividersSnapped.insert(horizontalDragDividersSnapped.begin(), horizontalDragDividersSnapped.end());
		for(std::set<Divider*>::const_iterator dragDividersSnappedIterator = dragDividersSnapped.begin(); dragDividersSnappedIterator != dragDividersSnapped.end(); ++dragDividersSnappedIterator)
		{
			//If the target divider has already been included in a snap group, skip it.
			Divider* divider = *dragDividersSnappedIterator;
			if(dragDividersSnappedProcessed.find(divider) != dragDividersSnappedProcessed.end())
			{
				continue;
			}

			//Calculate the bounding client rectangle for the highlight window to show for
			//this snap group
			bool setHighlightWindowInfo = false;
			int highlightWindowStartPosX;
			int highlightWindowStartPosY;
			int highlightWindowEndPosX;
			int highlightWindowEndPosY;
			std::set<Divider*> mergeCandidates = GetAllMergeCandidates(divider);
			for(std::set<Divider*>::const_iterator i = mergeCandidates.begin(); i != mergeCandidates.end(); ++i)
			{
				Divider* mergeCandidate = *i;
				highlightWindowStartPosX = (!setHighlightWindowInfo || (mergeCandidate->cachedPosX < highlightWindowStartPosX))? mergeCandidate->cachedPosX: highlightWindowStartPosX;
				highlightWindowStartPosY = (!setHighlightWindowInfo || (mergeCandidate->cachedPosY < highlightWindowStartPosY))? mergeCandidate->cachedPosY: highlightWindowStartPosY;
				highlightWindowEndPosX = (!setHighlightWindowInfo || ((mergeCandidate->cachedPosX + mergeCandidate->cachedWidth) > highlightWindowEndPosX))? (mergeCandidate->cachedPosX + mergeCandidate->cachedWidth): highlightWindowEndPosX;
				highlightWindowEndPosY = (!setHighlightWindowInfo || ((mergeCandidate->cachedPosY + mergeCandidate->cachedHeight) > highlightWindowEndPosY))? (mergeCandidate->cachedPosY + mergeCandidate->cachedHeight): highlightWindowEndPosY;
				setHighlightWindowInfo = true;
				dragDividersSnappedProcessed.insert(mergeCandidate);
			}

			//Position and size the divider highlight window
			if(setHighlightWindowInfo)
			{
				//Calculate the position of the highlight window in screen coordinates
				POINT point;
				point.x = highlightWindowStartPosX;
				point.y = highlightWindowStartPosY;
				ClientToScreen(hwnd, &point);
				int highlightWindowScreenPosX = point.x;
				int highlightWindowScreenPosY = point.y;

				//Calculate size of the highlight window
				int highlightWindowWidth = highlightWindowEndPosX - highlightWindowStartPosX;
				int highlightWindowHeight = highlightWindowEndPosY - highlightWindowStartPosY;

				//Position and size the highlight window
				SetWindowPos(dividerHighlightWindows[dividerHighlightVisibleWindowCount++], NULL, highlightWindowScreenPosX, highlightWindowScreenPosY, highlightWindowWidth, highlightWindowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
			}

			//Add all the dividers in this snap group to the list of processed snap
			//dividers
			dragDividersSnappedProcessed.insert(mergeCandidates.begin(), mergeCandidates.end());
		}
	}

	//If the number of visible divider highlight windows has changed, update the
	//window visibility now.
	if(dividerHighlightVisibleWindowCount != previousDividerHighlightVisibleWindowCount)
	{
		//Update the visibility state of each divider highlight window
		HDWP deferWindowPosSession = BeginDeferWindowPos(dividerHighlightWindowCount);
		for(unsigned int i = 0; i < dividerHighlightWindowCount; ++i)
		{
			DeferWindowPos(deferWindowPosSession, dividerHighlightWindows[i], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | ((i < dividerHighlightVisibleWindowCount)? SWP_SHOWWINDOW: SWP_HIDEWINDOW));
		}
		EndDeferWindowPos(deferWindowPosSession);

		//Request WM_MOUSELEAVE messages to be generated if we're currently showing
		//divider highlight windows
		if(dividerHighlightVisibleWindowCount > 0)
		{
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof(trackMouseEvent);
			trackMouseEvent.dwFlags = TME_LEAVE;
			trackMouseEvent.hwndTrack = hwnd;
			TrackMouseEvent(&trackMouseEvent);
		}
	}

	//Since we deferred window updates in order to perform snapping, we apply them here
	//now, so that our hosted windows are sized and positioned correctly.
	ApplyCachedLocations();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	//Redraw our window immediately. This minimizes tearing and improves the appearance of
	//the window during a divider drag operation.
	UpdateWindow(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_MOUSELEAVE(WPARAM wParam, LPARAM lParam)
{
	//Hide the drop shadow window if it is currently visible
	if(dropShadowVisible)
	{
		SetWindowPos(dropShadow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
		dropShadowVisible = false;
	}

	//Hide all the divider highlight windows
	if(dividerHighlightVisibleWindowCount > 0)
	{
		dividerHighlightVisibleWindowCount = 0;
		HDWP deferWindowPosSession = BeginDeferWindowPos(dividerHighlightWindowCount);
		for(unsigned int i = 0; i < dividerHighlightWindowCount; ++i)
		{
			DeferWindowPos(deferWindowPosSession, dividerHighlightWindows[i], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
		}
		EndDeferWindowPos(deferWindowPosSession);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_MOVING(WPARAM wParam, LPARAM lParam)
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
	std::list<IDockingWindow*> dockingWindowList;
	EnumThreadWindows(GetCurrentThreadId(), EnumDockingWindowsProc, (LPARAM)&dockingWindowList);

	//Attempt to locate a docking window under the target cursor position
	bool foundTargetWindow = false;
	IDockingWindow* targetDockingWindow = 0;
	std::list<IDockingWindow*>::const_iterator dockingWindowListIterator = dockingWindowList.begin();
	while(!foundTargetWindow && (dockingWindowListIterator != dockingWindowList.end()))
	{
		targetDockingWindow = *dockingWindowListIterator;
		if((targetDockingWindow != this) && (IsChild(hwnd, targetDockingWindow->GetWindowHandle()) == 0))
		{
			RECT targetDockingWindowRect;
			GetWindowRect(targetDockingWindow->GetWindowHandle(), &targetDockingWindowRect);
			if((targetDockingWindowRect.left <= lastDragCursorPosX) && (targetDockingWindowRect.right > lastDragCursorPosX) && (targetDockingWindowRect.top <= lastDragCursorPosY) && (targetDockingWindowRect.bottom > lastDragCursorPosY))
			{
				foundTargetWindow = true;
				continue;
			}
		}
		++dockingWindowListIterator;
	}

	//If the target docking window has a parent docking window, set the highest level
	//parent docking window as our target docking window.
	if(foundTargetWindow)
	{
		IDockingWindow* parentOfTargetDockingWindow = targetDockingWindow->GetParentDockingWindow();
		while(parentOfTargetDockingWindow != 0)
		{
			targetDockingWindow = parentOfTargetDockingWindow;
			parentOfTargetDockingWindow = targetDockingWindow->GetParentDockingWindow();
		}
	}

	//Remove the placement target from the previous target window if required
	if((dockingWindowUnderDragPos != 0) && (!foundTargetWindow || (dockingWindowUnderDragPos != targetDockingWindow)))
	{
		dockingWindowUnderDragPos->HideDropTargets(this);
	}

	//Record the new window that's under the current drag position
	dockingWindowUnderDragPos = (foundTargetWindow)? targetDockingWindow: 0;

	//If a docking window is currently under the cursor, update the display of any drop
	//targets for the target docking window.
	if(dockingWindowUnderDragPos != 0)
	{
		//Calculate the current window width and height
		RECT rect;
		GetWindowRect(hwnd, &rect);
		int windowWidth = rect.right - rect.left;
		int windowHeight = rect.bottom - rect.top;

		//Update the drop targets for the target window
		dockingWindowUnderDragPos->ShowDropTargets(this, windowWidth, windowHeight, lastDragCursorPosX, lastDragCursorPosY);
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
LRESULT DashboardWindow::msgWM_TIMER(WPARAM wParam, LPARAM lParam)
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
LRESULT DashboardWindow::msgWM_ENTERSIZEMOVE(WPARAM wParam, LPARAM lParam)
{
	//Flag that a window drag operation is in progress
	windowDragInProgress = true;
	dockingWindowUnderDragPos = 0;

	//Capture the initial window size and position
	GetWindowRect(hwnd, &windowSizeMoveInitialPos);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_EXITSIZEMOVE(WPARAM wParam, LPARAM lParam)
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
		IDockingWindowDropTargetInfo* dropTargetInfo;
		bool foundDockLocation = dockingWindowUnderDragPos->HitTestDropTargets(this, lastDragCursorPosX, lastDragCursorPosY, dropTargetInfo);

		//If we found a target dock location, dock to the new parent docking window.
		if(foundDockLocation)
		{
			//Dock to the new parent docking window
			HWND dropTargetOwningWindow = dropTargetInfo->GetOwningDockingWindow();
			IDockingWindow* dropTargetDockingWindow = GetDockingWindowFromHWND(dropTargetOwningWindow);
			dropTargetDockingWindow->AddChildContainer(this, dropTargetInfo);

			//Activate the top-level parent of the docking window we just docked into
			HWND newActiveWindow = NULL;
			HWND desktopWindow = GetDesktopWindow();
			HWND searchWindow = dropTargetDockingWindow->GetWindowHandle();
			while((searchWindow != NULL) && (searchWindow != desktopWindow))
			{
				newActiveWindow = searchWindow;
				searchWindow = GetAncestor(searchWindow, GA_PARENT);
			}
			SetActiveWindow(newActiveWindow);

			//Delete the allocated drop target info
			dropTargetInfo->Destroy();
		}
	}

	//Remove the placement target from the previous target window if required
	if(dockingWindowUnderDragPos != 0)
	{
		dockingWindowUnderDragPos->HideDropTargets(this);
	}

	//Now that this move event is complete, flag that a move is no longer in progress.
	windowDragInProgress = false;

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_WINDOWPOSCHANGING(WPARAM wParam, LPARAM lParam)
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
LRESULT DashboardWindow::msgWM_NCHITTEST(WPARAM wParam, LPARAM lParam)
{
	//If we're docked in a parent window, ensure we only display resize icons for the
	//border edges that can actually be resized.
	LRESULT result = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
	if(parentDockingWindow != 0)
	{
		switch(result)
		{
		case HTLEFT:
			result = (!parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Left))? HTBORDER: result;
			break;
		case HTRIGHT:
			result = (!parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Right))? HTBORDER: result;
			break;
		case HTTOP:
			result = (!parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Top))? HTBORDER: result;
			break;
		case HTBOTTOM:
			result = (!parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Bottom))? HTBORDER: result;
			break;
		case HTTOPLEFT:
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
		case HTBOTTOMRIGHT:{
			bool resizeLeftBorder = ((result == HTTOPLEFT) || (result == HTBOTTOMLEFT)) && parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Left);
			bool resizeRightBorder = ((result == HTTOPRIGHT) || (result == HTBOTTOMRIGHT)) && parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Right);
			bool resizeTopBorder = ((result == HTTOPLEFT) || (result == HTTOPRIGHT)) && parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Top);
			bool resizeBottomBorder = ((result == HTBOTTOMLEFT) || (result == HTBOTTOMRIGHT)) && parentDockingWindow->CanResizeChildContainerWindowEdge(this, WindowEdge::Bottom);
			if(resizeLeftBorder && resizeTopBorder)
			{
				result = HTTOPLEFT;
			}
			else if(resizeLeftBorder && resizeBottomBorder)
			{
				result = HTBOTTOMLEFT;
			}
			else if(resizeRightBorder && resizeTopBorder)
			{
				result = HTTOPRIGHT;
			}
			else if(resizeRightBorder && resizeBottomBorder)
			{
				result = HTBOTTOMRIGHT;
			}
			else if(resizeLeftBorder)
			{
				result = HTLEFT;
			}
			else if(resizeRightBorder)
			{
				result = HTRIGHT;
			}
			else if(resizeTopBorder)
			{
				result = HTTOP;
			}
			else if(resizeBottomBorder)
			{
				result = HTBOTTOM;
			}
			else
			{
				result = HTBORDER;
			}
			break;}
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_NCLBUTTONUP(WPARAM wParam, LPARAM lParam)
{
	//Ensure we're attached to a parent docking window
	if(parentDockingWindow == 0)
	{
		return DefWindowProc(hwnd, WM_NCLBUTTONUP, wParam, lParam);
	}

	//Allow the parent docking window to perform any processing it wants to in response to
	//this click event
	int cursorPosX = (int)(short)LOWORD(lParam);
	int cursorPosY = (int)(short)HIWORD(lParam);
	parentDockingWindow->ParentBorderClickForChildContainer(this, cursorPosX, cursorPosY);

	//Pass this message on to the default window procedure
	return DefWindowProc(hwnd, WM_NCLBUTTONUP, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_SETCURSOR(WPARAM wParam, LPARAM lParam)
{
	//Obtain the current cursor position
	POINT point;
	GetCursorPos(&point);
	int cursorPosX = point.x;
	int cursorPosY = point.y;

	//If we're docked to a parent docking window, allow the parent window to override the
	//cursor for the target mouse position, and apply it if the cursor is overridden.
	if(parentDockingWindow != 0)
	{
		HCURSOR parentOverrideCursor = parentDockingWindow->ParentOverrideCursorForChildContainer(this, cursorPosX, cursorPosY);
		if(parentOverrideCursor != NULL)
		{
			SetCursor(parentOverrideCursor);
			return TRUE;
		}
	}

	//If the current cursor position is outside the client region of our control, pass
	//this message on to the default window procedure.
	LRESULT hitTestResult = DefWindowProc(hwnd, WM_NCHITTEST, 0, MAKELPARAM(cursorPosX, cursorPosY));
	if(hitTestResult != HTCLIENT)
	{
		return DefWindowProc(hwnd, WM_SETCURSOR, wParam, lParam);
	}

	//Convert the cursor position into client coordinates
	ScreenToClient(hwnd, &point);
	int cursorClientPosX = point.x;
	int cursorClientPosY = point.y;

	//If a child window is under the current cursor position, pass this message on to the
	//default window procedure.
	POINT clientPosPoint;
	clientPosPoint.x = cursorClientPosX;
	clientPosPoint.y = cursorClientPosY;
	HWND childWindowUnderCursor = ChildWindowFromPoint(hwnd, clientPosPoint);
	if((childWindowUnderCursor != NULL) && (childWindowUnderCursor != hwnd))
	{
		return DefWindowProc(hwnd, WM_SETCURSOR, wParam, lParam);
	}

	//Attempt to locate any dividers which are under the cursor
	std::list<Divider*> dividersUnderCursor = GetDividersAtPosition(cursorClientPosX, cursorClientPosY);

	//If no dividers are under the current cursor position, pass this message on to the
	//default window procedure.
	if(dividersUnderCursor.empty())
	{
		return DefWindowProc(hwnd, WM_SETCURSOR, wParam, lParam);
	}

	//Determine the cursor to show at this location
	HCURSOR cursor = (dividersUnderCursor.size() > 1)? cursorSizeAll: ((dividersUnderCursor.front()->vertical)? cursorSizeHorizontal: cursorSizeVertical);

	//Apply the selected cursor at this location
	SetCursor(cursor);
	return TRUE;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_GETFONT(WPARAM wParam, LPARAM lParam)
{
	//Return the currently selected font to the caller
	return (LRESULT)controlFont;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_SETFONT(WPARAM wParam, LPARAM lParam)
{
	//Store the font to use for out controls
	controlFont = (HFONT)wParam;

	//Read the font metrics for our new font
	if(controlFont != NULL)
	{
		HDC hdc = GetDC(hwnd);
		HFONT hfontOld = (HFONT)SelectObject(hdc, controlFont);
		TEXTMETRIC textMetric;
		GetTextMetrics(hdc, &textMetric);
		controlFontHeight = textMetric.tmHeight;
		SelectObject(hdc, hfontOld);
		ReleaseDC(hwnd, hdc);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam)
{
	//We handle the WM_ERASEBKGND message here and return 0 to prevent the background
	//being erased by the default window procedure. Returning zero here will leave the
	//existing window content intact, and instead leave an erase pending for our WM_PAINT
	//message handler to process, which is what we want. Without processing this message,
	//there's a noticeable flicker when redrawing the control where the background is
	//erased before the WM_PAINT message is issued, such as when the control is resized.
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_PAINT(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintInfo;
	BeginPaint(hwnd, &paintInfo);
	HDC hdc = paintInfo.hdc;

	//Create a bitmap we can render the control onto
	HDC hdcControl = hdc;
	HDC hdcBitmap = CreateCompatibleDC(hdcControl);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdcControl, controlWidth, controlHeight);
	HBITMAP hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

	//Send a WM_PRINTCLIENT message to the native control and get it to render into
	//the bitmap.
	SendMessage(hwnd, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);

	//Transfer the final bitmap for the control into the screen buffer
	BitBlt(hdcControl, 0, 0, controlWidth, controlHeight, hdcBitmap, 0, 0, SRCCOPY);

	//Clean up the allocated handles
	SelectObject(hdcBitmap, hbitmapOriginal);
	DeleteObject(hbitmap);
	DeleteDC(hdcBitmap);

	EndPaint(hwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_PRINTCLIENT(WPARAM wParam, LPARAM lParam)
{
	//Fill the window with the background colour
	HDC hdc = (HDC)wParam;
	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, GetSysColorBrush(COLOR_WINDOW));

	//Create a GDI bitmap from our grabber image
	HDC hdcGrabberBitmap = CreateCompatibleDC(hdc);
	HBITMAP grabberBitmap = ImageToLoadedDIB(grabberImage, hdcGrabberBitmap);
	HBITMAP grabberBitmapOld = (HBITMAP)SelectObject(hdcGrabberBitmap, grabberBitmap);

	//Create GDI bitmaps from our arrow images
	HDC hdcArrowTopLeftBitmap = CreateCompatibleDC(hdc);
	HBITMAP arrowTopLeftBitmap = ImageToLoadedDIB(arrowImageTopLeft, hdcArrowTopLeftBitmap);
	HBITMAP arrowTopLeftBitmapOld = (HBITMAP)SelectObject(hdcArrowTopLeftBitmap, arrowTopLeftBitmap);
	HDC hdcArrowTopRightBitmap = CreateCompatibleDC(hdc);
	HBITMAP arrowTopRightBitmap = ImageToLoadedDIB(arrowImageTopRight, hdcArrowTopRightBitmap);
	HBITMAP arrowTopRightBitmapOld = (HBITMAP)SelectObject(hdcArrowTopRightBitmap, arrowTopRightBitmap);
	HDC hdcArrowBottomLeftBitmap = CreateCompatibleDC(hdc);
	HBITMAP arrowBottomLeftBitmap = ImageToLoadedDIB(arrowImageBottomLeft, hdcArrowBottomLeftBitmap);
	HBITMAP arrowBottomLeftBitmapOld = (HBITMAP)SelectObject(hdcArrowBottomLeftBitmap, arrowBottomLeftBitmap);
	HDC hdcArrowBottomRightBitmap = CreateCompatibleDC(hdc);
	HBITMAP arrowBottomRightBitmap = ImageToLoadedDIB(arrowImageBottomRight, hdcArrowBottomRightBitmap);
	HBITMAP arrowBottomRightBitmapOld = (HBITMAP)SelectObject(hdcArrowBottomRightBitmap, arrowBottomRightBitmap);

	//Create a brush for the background of the divider
	HBRUSH dividerBackgroundBrush = CreateSolidBrush(dividerBackgroundColor.GetColorREF());

	//Draw each divider
	for(std::list<Divider*>::const_iterator i = dividers.begin(); i != dividers.end(); ++i)
	{
		//Draw the divider
		Divider* divider = *i;
		rect.left = divider->cachedPosX;
		rect.right = divider->cachedPosX + divider->cachedWidth;
		rect.top = divider->cachedPosY;
		rect.bottom = divider->cachedPosY + divider->cachedHeight;
		FillRect(hdc, &rect, dividerBackgroundBrush);

		//Draw separator grabbers for each divider split position
		std::list<DividerSplitPosition> splitPositions = GetSplitPositionsForDivider(divider);
		for(std::list<DividerSplitPosition>::const_iterator i = splitPositions.begin(); i != splitPositions.end(); ++i)
		{
			//Define the blend parameters used for the alpha blend operation
			BLENDFUNCTION blendFunction;
			blendFunction.BlendOp = AC_SRC_OVER;
			blendFunction.BlendFlags = 0;
			blendFunction.SourceConstantAlpha = 255;
			blendFunction.AlphaFormat = AC_SRC_ALPHA;

			//Tile the grabber icon to fill in the region at which the grabber is present
			const DividerSplitPosition& splitPositionInfo = *i;
			int grabberImageWidth = (int)grabberImage.GetImageWidth();
			int grabberImageHeight = (int)grabberImage.GetImageHeight();
			int grabberStartPosX = (divider->vertical)? divider->cachedPosX: (divider->cachedPosX + splitPositionInfo.dividerSplitterStartPos);
			int grabberStartPosY = (divider->vertical)? (divider->cachedPosY + splitPositionInfo.dividerSplitterStartPos): divider->cachedPosY;
			int grabberEndPosX = (divider->vertical)? (divider->cachedPosX + divider->cachedWidth): (divider->cachedPosX + splitPositionInfo.dividerSplitterStartPos + splitPositionInfo.dividerSplitterLength);
			int grabberEndPosY = (divider->vertical)? (divider->cachedPosY + splitPositionInfo.dividerSplitterStartPos + splitPositionInfo.dividerSplitterLength): (divider->cachedPosY + divider->cachedHeight);
			for(int grabberPosX = grabberStartPosX; (grabberPosX + grabberImageWidth) <= grabberEndPosX; grabberPosX += grabberImageWidth)
			{
				for(int grabberPosY = grabberStartPosY; (grabberPosY + grabberImageHeight) <= grabberEndPosY; grabberPosY += grabberImageHeight)
				{
					AlphaBlend(hdc, grabberPosX, grabberPosY, grabberImageWidth, grabberImageHeight, hdcGrabberBitmap, 0, 0, grabberImageWidth, grabberImageHeight, blendFunction);
				}
			}
		}

		//Draw extend buttons for this divider where required
		std::list<DividerExtendButtonPosition> extendButtonPositions = GetExtendButtonPositionsForDivider(divider);
		for(std::list<DividerExtendButtonPosition>::const_iterator i = extendButtonPositions.begin(); i != extendButtonPositions.end(); ++i)
		{
			//Determine which icon to use for this extend button
			HDC arrowBitmapDC;
			const DividerExtendButtonPosition& extendButtonInfo = *i;
			if(extendButtonInfo.extendPrecedingContent)
			{
				if(extendButtonInfo.extendFromStart)
				{
					arrowBitmapDC = (divider->vertical)? hdcArrowTopLeftBitmap: hdcArrowTopLeftBitmap;
				}
				else
				{
					arrowBitmapDC = (divider->vertical)? hdcArrowBottomLeftBitmap: hdcArrowTopRightBitmap;
				}
			}
			else
			{
				if(extendButtonInfo.extendFromStart)
				{
					arrowBitmapDC = (divider->vertical)? hdcArrowTopRightBitmap: hdcArrowBottomLeftBitmap;
				}
				else
				{
					arrowBitmapDC = (divider->vertical)? hdcArrowBottomRightBitmap: hdcArrowBottomRightBitmap;
				}
			}

			//Determine the position and size of the extend button
			int buttonWidth = (!divider->vertical)? extendButtonInfo.dividerExtendButtonLength: divider->cachedWidth;
			int buttonHeight = (divider->vertical)? extendButtonInfo.dividerExtendButtonLength: divider->cachedHeight;
			int buttonStartPosX = (!divider->vertical)? extendButtonInfo.dividerExtendButtonStartPos: 0;
			int buttonStartPosY = (divider->vertical)? extendButtonInfo.dividerExtendButtonStartPos: 0;
			int arrowStartPosX = divider->cachedPosX + buttonStartPosX;
			int arrowStartPosY = divider->cachedPosY + buttonStartPosY;
			int arrowImageWidth = arrowImageTopLeft.GetImageWidth();
			int arrowImageHeight = arrowImageTopLeft.GetImageHeight();

			//Draw the extend button
			BLENDFUNCTION blendFunction;
			blendFunction.BlendOp = AC_SRC_OVER;
			blendFunction.BlendFlags = 0;
			blendFunction.SourceConstantAlpha = 255;
			blendFunction.AlphaFormat = AC_SRC_ALPHA;
			AlphaBlend(hdc, arrowStartPosX, arrowStartPosY, buttonWidth, buttonHeight, arrowBitmapDC, 0, 0, arrowImageWidth, arrowImageHeight, blendFunction);
		}
	}

	//Free the brush for the background of the divider
	DeleteObject(dividerBackgroundBrush);

	//Free the grabber bitmap resources
	SelectObject(hdcGrabberBitmap, grabberBitmapOld);
	DeleteObject(grabberBitmap);
	DeleteDC(hdcGrabberBitmap);

	//Free the arrow bitmap resources
	SelectObject(hdcArrowTopLeftBitmap, arrowTopLeftBitmapOld);
	DeleteObject(arrowTopLeftBitmap);
	DeleteDC(hdcArrowTopLeftBitmap);
	SelectObject(hdcArrowTopRightBitmap, arrowTopRightBitmapOld);
	DeleteObject(arrowTopRightBitmap);
	DeleteDC(hdcArrowTopRightBitmap);
	SelectObject(hdcArrowBottomLeftBitmap, arrowBottomLeftBitmapOld);
	DeleteObject(arrowBottomLeftBitmap);
	DeleteDC(hdcArrowBottomLeftBitmap);
	SelectObject(hdcArrowBottomRightBitmap, arrowBottomRightBitmapOld);
	DeleteObject(arrowBottomRightBitmap);
	DeleteDC(hdcArrowBottomRightBitmap);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgDASHWIN_ADDCONTENTWINDOW(WPARAM wParam, LPARAM lParam)
{
	//Attempt to locate an existing content region which corresponds with the target
	//window handle, if present.
	const AddWindowParams& params = *((const AddWindowParams*)lParam);
	ContentRegion* existingContentRegion = 0;
	if(params.existingWindow != NULL)
	{
		std::list<ContentRegion*>::const_iterator contentRegionIterator = regions.begin();
		while(contentRegionIterator != regions.end())
		{
			if((*contentRegionIterator)->windowHandle == params.existingWindow)
			{
				existingContentRegion = *contentRegionIterator;
				break;
			}
			++contentRegionIterator;
		}
	}

	//If no existing content region was identified, default to the first region.
	existingContentRegion = (existingContentRegion == 0)? regions.back(): existingContentRegion;

	//If the target region has no associated content window, associate it with the
	//specified window, otherwise insert it next to the target content region.
	if(existingContentRegion->windowHandle == NULL)
	{
		UpdateRegionContent(*existingContentRegion, params.windowHandle);
	}
	else
	{
		InsertRegion(*existingContentRegion, params.insertDirection, params.windowHandle, params.regionProportion);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgDASHWIN_REMOVECONTENTWINDOW(WPARAM wParam, LPARAM lParam)
{
	//Attempt to locate an existing content region which corresponds with the target
	//window handle, if present.
	HWND windowHandle = (HWND)lParam;
	ContentRegion* existingContentRegion = 0;
	std::list<ContentRegion*>::const_iterator contentRegionIterator = regions.begin();
	while(contentRegionIterator != regions.end())
	{
		if((*contentRegionIterator)->windowHandle == windowHandle)
		{
			existingContentRegion = *contentRegionIterator;
			break;
		}
		++contentRegionIterator;
	}

	//If the target content region was found, remove it.
	if(existingContentRegion != 0)
	{
		RemoveRegion(*existingContentRegion);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgDASHWIN_GETREGIONCOUNT(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(unsigned int)regions.size();
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgDASHWIN_GETREGIONWINDOW(WPARAM wParam, LPARAM lParam)
{
	//Obtain a list of the regions sorted in a manner based on their structure rather than
	//creation order
	std::list<ContentRegion*> sortedRegionList;
	BuildSortedRegionList(sortedRegionList);

	//Retrieve the requested region info
	size_t targetEntryNo = (size_t)(unsigned int)wParam;
	if(targetEntryNo >= sortedRegionList.size())
	{
		return 0;
	}
	std::list<ContentRegion*>::const_iterator regionIterator = sortedRegionList.begin();
	for(size_t i = 0; i < targetEntryNo; ++i)
	{
		++regionIterator;
	}
	ContentRegion* targetEntry = *regionIterator;

	//Return the window handle of the target region to the caller
	return (LRESULT)targetEntry->windowHandle;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgDASHWIN_SETREGIONWINDOW(WPARAM wParam, LPARAM lParam)
{
	//Obtain a list of the regions sorted in a manner based on their structure rather than
	//creation order
	std::list<ContentRegion*> sortedRegionList;
	BuildSortedRegionList(sortedRegionList);

	//Retrieve the requested region info
	size_t targetEntryNo = (size_t)(unsigned int)wParam;
	if(targetEntryNo >= sortedRegionList.size())
	{
		return 1;
	}
	std::list<ContentRegion*>::const_iterator regionIterator = sortedRegionList.begin();
	for(size_t i = 0; i < targetEntryNo; ++i)
	{
		++regionIterator;
	}
	ContentRegion* targetEntry = *regionIterator;

	//Replace the hosted window for the target region
	HWND newHostedWindow = (HWND)lParam;
	UpdateRegionContent(*targetEntry, newHostedWindow);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgDASHWIN_REMOVEALLREGIONS(WPARAM wParam, LPARAM lParam)
{
	RemoveAllRegions();
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgDASHWIN_SAVELAYOUTTODIVIDERLIST(WPARAM wParam, LPARAM lParam)
{
	std::list<DividerListEntry>& dividerList = *((std::list<DividerListEntry>*)lParam);
	SaveLayoutInfo(dividerList);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgDASHWIN_LOADLAYOUTFROMDIVIDERLIST(WPARAM wParam, LPARAM lParam)
{
	const std::list<DividerListEntry>& dividerList = *((std::list<DividerListEntry>*)lParam);
	LoadLayoutInfo(dividerList);
	return 0;
}

//----------------------------------------------------------------------------------------
//Placement target message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK DashboardWindow::PlacementTargetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		DashboardWindow* object = (DashboardWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->PlacementTargetWndProcPrivate(hwnd, message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		DashboardWindow* object = (DashboardWindow*)createStruct->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->PlacementTargetWndProcPrivate(hwnd, message, wParam, lParam);}
	case WM_DESTROY:{
		DashboardWindow* object = (DashboardWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
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
LRESULT DashboardWindow::PlacementTargetWndProcPrivate(HWND placementTargetHwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
LRESULT DashboardWindow::msgPlacementTargetWM_PAINT(HWND placementTargetHwnd, WPARAM wParam, LPARAM lParam)
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
LRESULT DashboardWindow::msgPlacementTargetWM_PRINTCLIENT(HWND placementTargetHwnd, WPARAM wParam, LPARAM lParam)
{
	//Set some measurement constants for this window
	int borderWidth = 1;
	int marginSize = 3;
	int windowImageBorderWidth = 1;

	//Calculate the width and height of this control
	RECT rect;
	GetClientRect(placementTargetHwnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	//Attempt to determine which location we're targeting
	DockTargetPos targetLocation;
	bool foundTargetLocation = false;
	std::map<DockTargetPos, HWND>::iterator placementTargetIterator = dropTargets.begin();
	while(!foundTargetLocation && (placementTargetIterator != dropTargets.end()))
	{
		if(placementTargetIterator->second == placementTargetHwnd)
		{
			foundTargetLocation = true;
			targetLocation = placementTargetIterator->first;
			continue;
		}
		++placementTargetIterator;
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
	WinColor newDividerColor(255, 0, 0);

	//Calculate the size of the window image inside this window
	int windowImageWidth = windowWidth - ((borderWidth + marginSize) * 2);
	int windowImageHeight = windowHeight - ((borderWidth + marginSize) * 2);

	//Calculate the position of the window image inside this window
	int windowImagePosX = borderWidth + marginSize;
	int windowImagePosY = borderWidth + marginSize;

	//Calculate the size and position of the client region within the window image
	int windowImageClientPosX = windowImagePosX + windowImageBorderWidth;
	int windowImageClientPosY = windowImagePosY + windowImageBorderWidth;
	int windowImageClientWidth = windowImageWidth - (windowImageBorderWidth * 2);
	int windowImageClientHeight = windowImageHeight - (windowImageBorderWidth * 2);

	//Calculate the size and position of the dock target region within the window image
	int dockTargetImagePosX = (targetLocation == DockTargetPos::Right)? windowImageClientPosX + (windowImageClientWidth / 3): windowImageClientPosX;
	int dockTargetImagePosY = (targetLocation == DockTargetPos::Bottom)? windowImageClientPosY + (windowImageClientHeight / 3): windowImageClientPosY;
	int dockTargetImageWidth = ((targetLocation == DockTargetPos::Left) || (targetLocation == DockTargetPos::Right))? (windowImageClientWidth - (windowImageClientWidth / 3)): windowImageClientWidth;
	int dockTargetImageHeight = ((targetLocation == DockTargetPos::Top) || (targetLocation == DockTargetPos::Bottom))? (windowImageClientHeight - (windowImageClientHeight / 3)): windowImageClientHeight;

	//Create each pen and brush object we require for this draw process
	HBRUSH borderBrush = CreateSolidBrush(borderColor.GetColorREF());
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
	RECT borderRect;
	GetClientRect(placementTargetHwnd, &borderRect);
	FrameRect(hdc, &borderRect, borderBrush);

	//Draw the window image border
	RECT windowBorderRect;
	windowBorderRect.left = dockTargetImagePosX - windowImageBorderWidth;
	windowBorderRect.right = dockTargetImagePosX + dockTargetImageWidth + windowImageBorderWidth;
	windowBorderRect.top = dockTargetImagePosY - windowImageBorderWidth;
	windowBorderRect.bottom = dockTargetImagePosY + dockTargetImageHeight + windowImageBorderWidth;
	FrameRect(hdc, &windowBorderRect, windowBorderBrush);

	//Draw the window image background
	RECT windowBackgroundRect;
	windowBackgroundRect.left = windowImageClientPosX;
	windowBackgroundRect.right = windowImageClientPosX + windowImageClientWidth;
	windowBackgroundRect.top = windowImageClientPosY;
	windowBackgroundRect.bottom = windowImageClientPosY + windowImageClientHeight;
	FillRect(hdc, &windowBackgroundRect, windowBackgroundBrush);

	//Draw the window image dock target background
	RECT windowDockTargetRect;
	windowDockTargetRect.left = dockTargetImagePosX;
	windowDockTargetRect.right = dockTargetImagePosX + dockTargetImageWidth;
	windowDockTargetRect.top = dockTargetImagePosY;
	windowDockTargetRect.bottom = dockTargetImagePosY + dockTargetImageHeight;
	FillRect(hdc, &windowDockTargetRect, windowDockTargetBrush);

	//Draw the divider line on the edge of the dock target
	if(targetLocation != DockTargetPos::Center)
	{
		HPEN dividerPen = CreatePen(PS_SOLID, 0, newDividerColor.GetColorREF());
		HPEN dividerPenOld = (HPEN)SelectObject(hdc, dividerPen);
		if((targetLocation == DockTargetPos::Left) || (targetLocation == DockTargetPos::Right))
		{
			int dividerPosX = (targetLocation == DockTargetPos::Left)? (dockTargetImagePosX + (dockTargetImageWidth - 1)): dockTargetImagePosX;
			MoveToEx(hdc, dividerPosX, dockTargetImagePosY, NULL);
			LineTo(hdc, dividerPosX, dockTargetImagePosY + dockTargetImageHeight);
		}
		else
		{
			int dividerPosY = (targetLocation == DockTargetPos::Top)? (dockTargetImagePosY + (dockTargetImageHeight - 1)): dockTargetImagePosY;
			MoveToEx(hdc, dockTargetImagePosX, dividerPosY, NULL);
			LineTo(hdc, dockTargetImagePosX + dockTargetImageWidth, dividerPosY);
		}
		SelectObject(hdc, dividerPenOld);
		DeleteObject(dividerPen);
	}

	//Draw a stippled line for the dithered edge of the window background
	if(targetLocation != DockTargetPos::Center)
	{
		LOGBRUSH windowBackgroundLogBrush;
		windowBackgroundLogBrush.lbStyle = BS_SOLID;
		windowBackgroundLogBrush.lbColor = windowBackgroundColor.GetColorREF();
		HPEN windowBackgroundPen = ExtCreatePen(PS_COSMETIC | PS_ALTERNATE, 1, &windowBackgroundLogBrush, 0, NULL);
		HPEN windowBackgroundPenOld = (HPEN)SelectObject(hdc, windowBackgroundPen);
		if((targetLocation == DockTargetPos::Left) || (targetLocation == DockTargetPos::Right))
		{
			int ditheredBorderPosX = (targetLocation == DockTargetPos::Left)? (windowImageClientPosX + windowImageClientWidth): windowImagePosX;
			MoveToEx(hdc, ditheredBorderPosX, windowImagePosY, NULL);
			LineTo(hdc, ditheredBorderPosX, windowImagePosY + windowImageHeight);
		}
		else
		{
			int ditheredBorderPosY = (targetLocation == DockTargetPos::Top)? (windowImageClientPosY + windowImageClientHeight): windowImagePosY;
			MoveToEx(hdc, windowImagePosX, ditheredBorderPosY, NULL);
			LineTo(hdc, windowImagePosX + windowImageWidth, ditheredBorderPosY);
		}
		SelectObject(hdc, windowBackgroundPenOld);
		DeleteObject(windowBackgroundPen);
	}

	//Draw stippled lines for the stippled portion of the window border
	if(targetLocation != DockTargetPos::Center)
	{
		LOGBRUSH borderLogBrush;
		borderLogBrush.lbStyle = BS_SOLID;
		borderLogBrush.lbColor = windowBorderColor.GetColorREF();
		HPEN borderPen = ExtCreatePen(PS_COSMETIC | PS_ALTERNATE, 1, &borderLogBrush, 0, NULL);
		HPEN borderPenOld = (HPEN)SelectObject(hdc, borderPen);
		if((targetLocation == DockTargetPos::Left) || (targetLocation == DockTargetPos::Right))
		{
			int ditheredBorderStartPosX = (targetLocation == DockTargetPos::Left)? ((windowImagePosX + windowImageWidth) - 1): windowImagePosX;
			int ditheredBorderEndPosX = (targetLocation == DockTargetPos::Left)? (windowImagePosX - 1): (windowImagePosX + windowImageWidth);
			MoveToEx(hdc, ditheredBorderStartPosX, windowImagePosY, NULL);
			LineTo(hdc, ditheredBorderEndPosX, windowImagePosY);
			MoveToEx(hdc, ditheredBorderStartPosX, windowImagePosY + (windowImageHeight - windowImageBorderWidth), NULL);
			LineTo(hdc, ditheredBorderEndPosX, windowImagePosY + (windowImageHeight - windowImageBorderWidth));
		}
		else
		{
			int ditheredBorderStartPosY = (targetLocation == DockTargetPos::Top)? ((windowImagePosY + windowImageHeight) - 1): windowImagePosY;
			int ditheredBorderEndPosY = (targetLocation == DockTargetPos::Top)? (windowImagePosY - 1): (windowImagePosY + windowImageHeight);
			MoveToEx(hdc, windowImagePosX, ditheredBorderStartPosY, NULL);
			LineTo(hdc, windowImagePosX, ditheredBorderEndPosY);
			MoveToEx(hdc, windowImagePosX + (windowImageWidth - windowImageBorderWidth), ditheredBorderStartPosY, NULL);
			LineTo(hdc, windowImagePosX + (windowImageWidth - windowImageBorderWidth), ditheredBorderEndPosY);
		}
		SelectObject(hdc, borderPenOld);
		DeleteObject(borderPen);
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
LRESULT CALLBACK DashboardWindow::PlacementShadowWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		DashboardWindow* object = (DashboardWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->PlacementShadowWndProcPrivate(hwnd, message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		DashboardWindow* object = (DashboardWindow*)createStruct->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->PlacementShadowWndProcPrivate(hwnd, message, wParam, lParam);}
	case WM_DESTROY:{
		DashboardWindow* object = (DashboardWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
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
LRESULT DashboardWindow::PlacementShadowWndProcPrivate(HWND placementShadowHwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		return msgPlacementShadowWM_PAINT(placementShadowHwnd, wParam, lParam);
	}
	return DefWindowProc(placementShadowHwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgPlacementShadowWM_PAINT(HWND placementShadowHwnd, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintInfo;
	BeginPaint(placementShadowHwnd, &paintInfo);
	HDC hdc = paintInfo.hdc;

	//Calculate the width and height of this control
	RECT rect;
	GetClientRect(placementShadowHwnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	//Select the colours to use for this window
	WinColor backgroundColor(32, 64, 255);
	WinColor dividerColor(255, 0, 0);

	//Create each pen and brush object we require for this draw process
	HBRUSH backgroundBrush = CreateSolidBrush(backgroundColor.GetColorREF());
	HBRUSH dividerBrush = CreateSolidBrush(dividerColor.GetColorREF());

	//Fill the window with the background colour
	HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
	PatBlt(hdc, 0, 0, windowWidth, windowHeight, PATCOPY);
	SelectObject(hdc, backgroundBrushOld);

	//Draw a rectangle indicating the position at which the new divider will be inserted
	if(dropShadowCurrentPos != DockTargetPos::Center)
	{
		RECT dividerRect;
		dividerRect.left = (dropShadowCurrentPos == DockTargetPos::Left)? (windowWidth - dividerSize): 0;
		dividerRect.right = ((dropShadowCurrentPos == DockTargetPos::Left) || (dropShadowCurrentPos == DockTargetPos::Right))? dividerRect.left + dividerSize: windowWidth;
		dividerRect.top = (dropShadowCurrentPos == DockTargetPos::Top)? (windowHeight - dividerSize): 0;
		dividerRect.bottom = ((dropShadowCurrentPos == DockTargetPos::Top) || (dropShadowCurrentPos == DockTargetPos::Bottom))? dividerRect.top + dividerSize: windowHeight;
		FillRect(hdc, &dividerRect, dividerBrush);
	}

	//Delete each pen and brush object which was created for this draw process
	DeleteObject(backgroundBrush);
	DeleteObject(dividerBrush);

	//Validate the entire client area of the control. It's REALLY important that we
	//call this here, otherwise Windows is going to send WM_PAINT messages over and
	//over again waiting for the region to be validated.
	ValidateRect(placementShadowHwnd, NULL);

	EndPaint(placementShadowHwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
//Placement shadow message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK DashboardWindow::DividerHighlightWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		DashboardWindow* object = (DashboardWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->DividerHighlightWndProcPrivate(hwnd, message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		DashboardWindow* object = (DashboardWindow*)createStruct->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->DividerHighlightWndProcPrivate(hwnd, message, wParam, lParam);}
	case WM_DESTROY:{
		DashboardWindow* object = (DashboardWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			LPARAM result = object->DividerHighlightWndProcPrivate(hwnd, message, wParam, lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
			return result;
		}
		break;}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::DividerHighlightWndProcPrivate(HWND placementShadowHwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		return msgDividerHighlightWM_PAINT(placementShadowHwnd, wParam, lParam);
	}
	return DefWindowProc(placementShadowHwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgDividerHighlightWM_PAINT(HWND placementShadowHwnd, WPARAM wParam, LPARAM lParam)
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
	WinColor backgroundColor(255, 0, 0);
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
IDockingWindow* DashboardWindow::GetDockingWindowFromHWND(HWND hwnd)
{
	//Ensure the class name of the target window begins with our docking window prefix
	const std::wstring dockWinPrefix = L"EX_DockWin";
	std::wstring targetWindowClassName = GetClassName(hwnd);
	if((targetWindowClassName.size() < dockWinPrefix.size()) || (targetWindowClassName.substr(0, dockWinPrefix.size()) != dockWinPrefix))
	{
		return 0;
	}

	//Retrieve the IDockingWindow object for the target window, and return it to the
	//caller.
	IDockingWindow* targetDockingWindow = (IDockingWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	return targetDockingWindow;
}

//----------------------------------------------------------------------------------------
BOOL CALLBACK DashboardWindow::EnumDockingWindowsProc(HWND hwnd, LPARAM lParam)
{
	std::list<IDockingWindow*>& dockingWindowList = *((std::list<IDockingWindow*>*)lParam);
	AddDockingWindowToWindowList(hwnd, dockingWindowList);
	return TRUE;
}

//----------------------------------------------------------------------------------------
void DashboardWindow::AddDockingWindowToWindowList(HWND hwnd, std::list<IDockingWindow*>& dockingWindowList)
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
			//it does indeed seem to always return in Z-order, and it's widely relied upon
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
//Window handle methods
//----------------------------------------------------------------------------------------
HWND DashboardWindow::GetWindowHandle() const
{
	return hwnd;
}

//----------------------------------------------------------------------------------------
//Placement content methods
//----------------------------------------------------------------------------------------
void DashboardWindow::HideDropTargets(IDockingWindow* callingDockingWindow)
{
	HideDropTargets(callingDockingWindow, true);
}

//----------------------------------------------------------------------------------------
void DashboardWindow::HideDropTargets(IDockingWindow* callingDockingWindow, bool hideChildWindowDropTargets)
{
	//If drop targets have been shown for a child docking window, hide them now.
	if(hideChildWindowDropTargets && (dropTargetsChildDockingWindow != 0))
	{
		dropTargetsChildDockingWindow->HideDropTargets(callingDockingWindow);
		dropTargetsChildDockingWindow = 0;
	}

	//If drop targets aren't currently visible, abort any further processing.
	if(!dropTargetsVisible)
	{
		return;
	}

	//Begin a session for processing this batch of window size changes. Processing all the
	//window size and position changes in a single operation in this manner gives the best
	//performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos(5);

	//Hide the drop shadow window
	DeferWindowPos(deferWindowPosSession, dropShadow, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	dropShadowVisible = false;

	//Hide all the drop target windows
	DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Left], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Right], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Top], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Bottom], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
	DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Center], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);

	//Process all the window size and position changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);

	//Flag that drop targets are no longer visible
	dropTargetsVisible = false;
	currentRegionDropTarget = 0;
}

//----------------------------------------------------------------------------------------
void DashboardWindow::ShowDropTargets(IDockingWindow* callingDockingWindow, int dockWindowWidth, int dockWindowHeight, int cursorPosX, int cursorPosY)
{
	//Convert the cursor position into client coordinates
	POINT cursorPos;
	cursorPos.x = cursorPosX;
	cursorPos.y = cursorPosY;
	ScreenToClient(hwnd, &cursorPos);
	int cursorClientPosX = cursorPos.x;
	int cursorClientPosY = cursorPos.y;

	//Determine what region lies under the supplied cursor position
	ContentRegion* targetRegion = GetRegionAtPosition(cursorClientPosX, cursorClientPosY);

	//Determine what child window lies under the supplied cursor position. Note that if a
	//docked child window has been maximized, this may be different to what we're
	//expecting based on the region positions.
	POINT clientPosPoint;
	clientPosPoint.x = cursorClientPosX;
	clientPosPoint.y = cursorClientPosY;
	HWND childWindowUnderCursor = ChildWindowFromPointEx(hwnd, clientPosPoint, CWP_SKIPINVISIBLE);
	childWindowUnderCursor = (childWindowUnderCursor == hwnd)? NULL: childWindowUnderCursor;

	//Calculate the position and size of the child window under the supplied cursor
	//position
	int childWindowUnderCursorPosX = -1;
	int childWindowUnderCursorPosY = -1;
	int childWindowUnderCursorWidth = -1;
	int childWindowUnderCursorHeight = -1;
	if(childWindowUnderCursor != NULL)
	{
		RECT childWindowUnderCursorRect;
		GetWindowRect(childWindowUnderCursor, &childWindowUnderCursorRect);
		POINT childWindowUnderCursorScreenPos;
		childWindowUnderCursorScreenPos.x = childWindowUnderCursorRect.left;
		childWindowUnderCursorScreenPos.y = childWindowUnderCursorRect.top;
		ScreenToClient(hwnd, &childWindowUnderCursorScreenPos);
		childWindowUnderCursorPosX = childWindowUnderCursorScreenPos.x;
		childWindowUnderCursorPosY = childWindowUnderCursorScreenPos.y;
		childWindowUnderCursorWidth = childWindowUnderCursorRect.right - childWindowUnderCursorRect.left;
		childWindowUnderCursorHeight = childWindowUnderCursorRect.bottom - childWindowUnderCursorRect.top;
	}

	//If no target region was under the cursor position, or the window under the cursor
	//position was different or a different position and size to what we were expecting
	//for the target region, hide placement targets for this window, and show placement
	//targets for the child window the cursor is over if it is a docking window, then
	//abort any further processing. This allows us to support child windows which have
	//been maximized. Note that we also need to allow the case where no window is
	//associated with the target region, as it's possible to have regions that currently
	//have no windows bound to them.
	if((targetRegion == 0) || (targetRegion->windowHandle != childWindowUnderCursor) || ((targetRegion->windowHandle != NULL) && ((targetRegion->windowPosX != childWindowUnderCursorPosX) || (targetRegion->windowPosY != childWindowUnderCursorPosY) || (targetRegion->windowWidth != childWindowUnderCursorWidth) || (targetRegion->windowHeight != childWindowUnderCursorHeight))))
	{
		//If the target child window is a docking window, show drop targets for it.
		bool hideChildWindowDropTargets = true;
		IDockingWindow* childDockingWindowUnderCursor = GetDockingWindowFromHWND(childWindowUnderCursor);
		if(childDockingWindowUnderCursor != 0)
		{
			//If a different child docking window had drop targets being shown, hide them
			//now.
			if((dropTargetsChildDockingWindow != 0) && (dropTargetsChildDockingWindow != childDockingWindowUnderCursor))
			{
				dropTargetsChildDockingWindow->HideDropTargets(callingDockingWindow);
			}

			//Set the target child docking window as the child window we're currently
			//showing drop targets for, and request it to show drop targets.
			dropTargetsChildDockingWindow = childDockingWindowUnderCursor;
			dropTargetsChildDockingWindow->ShowDropTargets(callingDockingWindow, dockWindowWidth, dockWindowHeight, cursorPosX, cursorPosY);

			//Since we're showing drop targets for a child docking window now, flag that
			//we don't want to hide the child window drop targets when we hide our own
			//drop targets.
			hideChildWindowDropTargets = false;
		}

		//Hide the drop targets for this window, and abort any further processing.
		HideDropTargets(this, hideChildWindowDropTargets);
		return;
	}

	//If the region under the supplied cursor position is different from the last update,
	//change our placement target windows to target the new region.
	if(currentRegionDropTarget != targetRegion)
	{
		//If drop targets have been shown for a child docking window in the previous
		//target region, hide them now.
		if(dropTargetsChildDockingWindow != 0)
		{
			dropTargetsChildDockingWindow->HideDropTargets(callingDockingWindow);
			dropTargetsChildDockingWindow = 0;
		}

		//Update the current target region for the drop targets
		currentRegionDropTarget = targetRegion;

		//Calculate the position of the target region in screen coordinates
		POINT targetRegionPos;
		targetRegionPos.x = targetRegion->cachedPosX;
		targetRegionPos.y = targetRegion->cachedPosY;
		ClientToScreen(hwnd, &targetRegionPos);
		int targetRegionScreenPosX = targetRegionPos.x;
		int targetRegionScreenPosY = targetRegionPos.y;

		//Show drop target windows for the target region
		const int borderWidth = 1;
		const int marginSize = 3;
		const int dropTargetWindowIconWidth = 14;
		const int dropTargetWindowIconHeight = 22;
		if(targetRegion->windowHandle == NULL)
		{
			//Calculate the position and size of the center drop target
			int dropTargetIconWidth = dropTargetWindowIconHeight + ((marginSize + borderWidth) * 2);
			int dropTargetIconHeight = dropTargetIconWidth;
			int dropTargetIconPosX = targetRegionScreenPosX + ((targetRegion->width / 2) - (dropTargetIconWidth / 2));
			int dropTargetIconPosY = targetRegionScreenPosY + ((targetRegion->height / 2) - (dropTargetIconHeight / 2));

			//Begin a session for processing this batch of window size changes. Processing
			//all the window size and position changes in a single operation in this
			//manner gives the best performance and appearance.
			HDWP deferWindowPosSession = BeginDeferWindowPos(6);

			//Position and size all the drop target windows
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Center], NULL, dropTargetIconPosX, dropTargetIconPosY, dropTargetIconWidth, dropTargetIconHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

			//Show the drop target windows
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Left], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Right], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Top], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Bottom], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Center], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

			//Flag that drop targets are now visible
			dropTargetsVisible = true;

			//Process all the window size and position changes involved in this update
			EndDeferWindowPos(deferWindowPosSession);
		}
		else
		{
			//Calculate the sizes of each drop target
			std::map<DockTargetPos, int> dropTargetIconWidth;
			std::map<DockTargetPos, int> dropTargetIconHeight;
			dropTargetIconWidth[DockTargetPos::Left] = dropTargetWindowIconWidth + ((marginSize + borderWidth) * 2);
			dropTargetIconHeight[DockTargetPos::Left] = dropTargetWindowIconHeight + ((marginSize + borderWidth) * 2);
			dropTargetIconWidth[DockTargetPos::Right] = dropTargetWindowIconWidth + ((marginSize + borderWidth) * 2);
			dropTargetIconHeight[DockTargetPos::Right] = dropTargetWindowIconHeight + ((marginSize + borderWidth) * 2);
			dropTargetIconWidth[DockTargetPos::Top] = dropTargetWindowIconHeight + ((marginSize + borderWidth) * 2);
			dropTargetIconHeight[DockTargetPos::Top] = dropTargetWindowIconWidth + ((marginSize + borderWidth) * 2);
			dropTargetIconWidth[DockTargetPos::Bottom] = dropTargetWindowIconHeight + ((marginSize + borderWidth) * 2);
			dropTargetIconHeight[DockTargetPos::Bottom] = dropTargetWindowIconWidth + ((marginSize + borderWidth) * 2);

			//Calculate the position of each docking region
			std::map<DockTargetPos, int> dropTargetIconPosX;
			std::map<DockTargetPos, int> dropTargetIconPosY;
			dropTargetIconPosX[DockTargetPos::Left] = targetRegionScreenPosX;
			dropTargetIconPosY[DockTargetPos::Left] = targetRegionScreenPosY + ((targetRegion->height / 2) - (dropTargetIconHeight[DockTargetPos::Left] / 2));
			dropTargetIconPosX[DockTargetPos::Right] = targetRegionScreenPosX + (targetRegion->width - dropTargetIconWidth[DockTargetPos::Right]);
			dropTargetIconPosY[DockTargetPos::Right] = targetRegionScreenPosY + ((targetRegion->height / 2) - (dropTargetIconHeight[DockTargetPos::Right] / 2));
			dropTargetIconPosX[DockTargetPos::Top] = targetRegionScreenPosX + ((targetRegion->width / 2) - (dropTargetIconWidth[DockTargetPos::Top] / 2));
			dropTargetIconPosY[DockTargetPos::Top] = targetRegionScreenPosY;
			dropTargetIconPosX[DockTargetPos::Bottom] = targetRegionScreenPosX + ((targetRegion->width / 2) - (dropTargetIconWidth[DockTargetPos::Top] / 2));
			dropTargetIconPosY[DockTargetPos::Bottom] = targetRegionScreenPosY + (targetRegion->height - dropTargetIconHeight[DockTargetPos::Bottom]);

			//Begin a session for processing this batch of window size changes. Processing
			//all the window size and position changes in a single operation in this
			//manner gives the best performance and appearance.
			HDWP deferWindowPosSession = BeginDeferWindowPos(9);

			//Position and size all the drop target windows
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Left], NULL, dropTargetIconPosX[DockTargetPos::Left], dropTargetIconPosY[DockTargetPos::Left], dropTargetIconWidth[DockTargetPos::Left], dropTargetIconHeight[DockTargetPos::Left], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Right], NULL, dropTargetIconPosX[DockTargetPos::Right], dropTargetIconPosY[DockTargetPos::Right], dropTargetIconWidth[DockTargetPos::Right], dropTargetIconHeight[DockTargetPos::Right], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Top], NULL, dropTargetIconPosX[DockTargetPos::Top], dropTargetIconPosY[DockTargetPos::Top], dropTargetIconWidth[DockTargetPos::Top], dropTargetIconHeight[DockTargetPos::Top], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Bottom], NULL, dropTargetIconPosX[DockTargetPos::Bottom], dropTargetIconPosY[DockTargetPos::Bottom], dropTargetIconWidth[DockTargetPos::Bottom], dropTargetIconHeight[DockTargetPos::Bottom], SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

			//Show the drop target windows
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Left], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Right], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Top], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Bottom], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
			DeferWindowPos(deferWindowPosSession, dropTargets[DockTargetPos::Center], NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);

			//Flag that drop targets are now visible
			dropTargetsVisible = true;

			//Process all the window size and position changes involved in this update
			EndDeferWindowPos(deferWindowPosSession);
		}
	}

	//If the target region is hosting a docking window, instruct it to show its drop
	//targets too.
	if(targetRegion->windowAsDockingWindow != 0)
	{
		dropTargetsChildDockingWindow = targetRegion->windowAsDockingWindow;
		dropTargetsChildDockingWindow->ShowDropTargets(callingDockingWindow, dockWindowWidth, dockWindowHeight, cursorPosX, cursorPosY);
	}

	//Determine the current drop target based on the supplied cursor position
	bool foundDockLocation = true;
	DockTargetPos newDockLocation;
	RECT rect;
	cursorPos.x = cursorPosX;
	cursorPos.y = cursorPosY;
	if((GetWindowRect(dropTargets[DockTargetPos::Left], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = DockTargetPos::Left;
	}
	else if((GetWindowRect(dropTargets[DockTargetPos::Right], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = DockTargetPos::Right;
	}
	else if((GetWindowRect(dropTargets[DockTargetPos::Top], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = DockTargetPos::Top;
	}
	else if((GetWindowRect(dropTargets[DockTargetPos::Bottom], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = DockTargetPos::Bottom;
	}
	else if((GetWindowRect(dropTargets[DockTargetPos::Center], &rect) != 0) && (PtInRect(&rect, cursorPos) != 0))
	{
		newDockLocation = DockTargetPos::Center;
	}
	else
	{
		foundDockLocation = false;
	}

	//If we found a target dock location, update the placement shadow window position and
	//size.
	if(foundDockLocation && (!dropShadowVisible || (dropShadowCurrentPos != newDockLocation)))
	{
		//Record the edge to anchor the placement shadow against
		dropShadowCurrentPos = newDockLocation;

		//Calculate the width and height of the placement shadow window
		int placementShadowWidth = (newDockLocation == DockTargetPos::Center)? targetRegion->width: (((newDockLocation == DockTargetPos::Left) || (newDockLocation == DockTargetPos::Right))? ((targetRegion->width + dividerSize) / 2): targetRegion->width);
		int placementShadowHeight = (newDockLocation == DockTargetPos::Center)? targetRegion->height: (((newDockLocation == DockTargetPos::Top) || (newDockLocation == DockTargetPos::Bottom))? ((targetRegion->height + dividerSize) / 2): targetRegion->height);

		//Calculate the position of the placement shadow window in screen coordinates
		POINT point;
		point.x = (newDockLocation == DockTargetPos::Center)? targetRegion->cachedPosX: ((newDockLocation == DockTargetPos::Right)? ((targetRegion->cachedPosX + targetRegion->width) - placementShadowWidth): targetRegion->cachedPosX);
		point.y = (newDockLocation == DockTargetPos::Center)? targetRegion->cachedPosY: ((newDockLocation == DockTargetPos::Bottom)? ((targetRegion->cachedPosY + targetRegion->height) - placementShadowHeight): targetRegion->cachedPosY);
		ClientToScreen(hwnd, &point);
		int placementShadowPosX = point.x;
		int placementShadowPosY = point.y;

		//Update the placement shadow position and size
		SetWindowPos(dropShadow, NULL, placementShadowPosX, placementShadowPosY, placementShadowWidth, placementShadowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	}

	//Show or hide the placement shadow window if required
	if(foundDockLocation && !dropShadowVisible)
	{
		SetWindowPos(dropShadow, dropTargets[DockTargetPos::Left], 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
		dropShadowVisible = true;
	}
	else if(!foundDockLocation && dropShadowVisible)
	{
		SetWindowPos(dropShadow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
		dropShadowVisible = false;
	}
}

//----------------------------------------------------------------------------------------
bool DashboardWindow::HitTestDropTargets(IDockingWindow* callingDockingWindow, int cursorPosX, int cursorPosY, IDockingWindowDropTargetInfo*& dropTargetInfo) const
{
	//If no valid placement target is currently selected, pass the call onto the nested
	//docking window within the current target region if present, otherwise return false.
	if(!dropShadowVisible && (dropTargetsChildDockingWindow != 0))
	{
		return dropTargetsChildDockingWindow->HitTestDropTargets(callingDockingWindow, cursorPosX, cursorPosY, dropTargetInfo);
	}
	else if(!dropTargetsVisible || !dropShadowVisible)
	{
		return false;
	}

	//Calculate the insert direction to use based on the currently selected drop target.
	//Note that the insert direction is ignored if the target region has no current
	//content window, as the window is used to set the content of the target region
	//instead. Since the only time a center drop target is allowed is if the target region
	//has no content window, we process a center dock position with a left insert
	//direction, because it doesn't matter which one is used.
	InsertDirection insertDirection = InsertDirection::Left;
	switch(dropShadowCurrentPos)
	{
	case DockTargetPos::Center:
	case DockTargetPos::Left:
		insertDirection = InsertDirection::Left;
		break;
	case DockTargetPos::Right:
		insertDirection = InsertDirection::Right;
		break;
	case DockTargetPos::Top:
		insertDirection = InsertDirection::Top;
		break;
	case DockTargetPos::Bottom:
		insertDirection = InsertDirection::Bottom;
		break;
	}

	//If the target region has no associated window, calculate the index number for the
	//region in order to uniquely identify the target.
	int targetRegionNo = -1;
	if(currentRegionDropTarget->windowHandle == NULL)
	{
		//Obtain a list of the regions sorted in a manner based on their structure rather
		//than creation order
		std::list<ContentRegion*> sortedRegionList;
		BuildSortedRegionList(sortedRegionList);

		//Determine the index of the target region
		targetRegionNo = 0;
		std::list<ContentRegion*>::const_iterator regionIterator = sortedRegionList.begin();
		while(regionIterator != sortedRegionList.end())
		{
			if(*regionIterator == currentRegionDropTarget)
			{
				break;
			}
			++targetRegionNo;
			++regionIterator;
		}
	}

	//Return the target docking location to the caller
	DashboardWindowDropTargetInfo* dropTargetInfoResolved = new DashboardWindowDropTargetInfo();
	dropTargetInfoResolved->existingWindow = currentRegionDropTarget->windowHandle;
	dropTargetInfoResolved->targetRegionNo = targetRegionNo;
	dropTargetInfoResolved->insertDirection = insertDirection;
	dropTargetInfoResolved->regionProportion = 0.5;
	dropTargetInfoResolved->owningDockingWindow = hwnd;
	dropTargetInfo = dropTargetInfoResolved;
	return true;
}

//----------------------------------------------------------------------------------------
//Child container methods
//----------------------------------------------------------------------------------------
void DashboardWindow::AddChildContainer(IDockingWindow* childContainer, const IDockingWindowDropTargetInfo* dropTargetInfo)
{
	//Convert the drop target info into the known correct type
	const DashboardWindowDropTargetInfo& dropTargetInfoResolved = *((const DashboardWindowDropTargetInfo*)dropTargetInfo);

	//Attempt to locate an existing content region which corresponds with the target
	//window region to add next to, if present.
	ContentRegion* existingContentRegion = 0;
	if(dropTargetInfoResolved.existingWindow != NULL)
	{
		std::list<ContentRegion*>::const_iterator contentRegionIterator = regions.begin();
		while(contentRegionIterator != regions.end())
		{
			if((*contentRegionIterator)->windowHandle == dropTargetInfoResolved.existingWindow)
			{
				existingContentRegion = *contentRegionIterator;
				break;
			}
			++contentRegionIterator;
		}
	}
	else if(dropTargetInfoResolved.targetRegionNo >= 0)
	{
		//Obtain a list of the regions sorted in a manner based on their structure rather
		//than creation order
		std::list<ContentRegion*> sortedRegionList;
		BuildSortedRegionList(sortedRegionList);

		//Retrieve the requested region info
		if(dropTargetInfoResolved.targetRegionNo < (int)sortedRegionList.size())
		{
			std::list<ContentRegion*>::const_iterator regionIterator = sortedRegionList.begin();
			for(int i = 0; i < dropTargetInfoResolved.targetRegionNo; ++i)
			{
				++regionIterator;
			}
			existingContentRegion = *regionIterator;
		}
	}

	//If no existing content region was identified, default to the first region.
	existingContentRegion = (existingContentRegion == 0)? regions.back(): existingContentRegion;

	//If the target region has no associated content window, associate it with the
	//specified window, otherwise insert it next to the target content region.
	if(existingContentRegion->windowHandle == NULL)
	{
		UpdateRegionContent(*existingContentRegion, childContainer->GetWindowHandle());
	}
	else
	{
		InsertRegion(*existingContentRegion, dropTargetInfoResolved.insertDirection, childContainer->GetWindowHandle(), dropTargetInfoResolved.regionProportion);
	}
}

//----------------------------------------------------------------------------------------
void DashboardWindow::RemoveChildContainer(IDockingWindow* childContainer)
{
	HWND windowHandle = childContainer->GetWindowHandle();
	ContentRegion* existingContentRegion = 0;
	std::list<ContentRegion*>::const_iterator contentRegionIterator = regions.begin();
	while(contentRegionIterator != regions.end())
	{
		if((*contentRegionIterator)->windowHandle == windowHandle)
		{
			existingContentRegion = *contentRegionIterator;
			break;
		}
		++contentRegionIterator;
	}
	if(existingContentRegion != 0)
	{
		RemoveRegion(*existingContentRegion);
	}
}

//----------------------------------------------------------------------------------------
void DashboardWindow::NotifyChildContainerContentChanged(IDockingWindow* childContainer)
{ }

//----------------------------------------------------------------------------------------
bool DashboardWindow::HasNestedChildDockingWindows() const
{
	//Return true if we have at least one hosted docking window
	for(std::list<ContentRegion*>::const_iterator contentRegionIterator = regions.begin(); contentRegionIterator != regions.end(); ++contentRegionIterator)
	{
		const ContentRegion& contentRegion = *(*contentRegionIterator);
		if(contentRegion.windowAsDockingWindow != 0)
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::list<IDockingWindow*>> DashboardWindow::GetNestedChildDockingWindowList() const
{
	//Create a list object to hold our list of nested child windows
	std::list<IDockingWindow*> dockingWindowList;

	//Build a list of any hosted docking windows
	for(std::list<ContentRegion*>::const_iterator contentRegionIterator = regions.begin(); contentRegionIterator != regions.end(); ++contentRegionIterator)
	{
		const ContentRegion& contentRegion = *(*contentRegionIterator);
		if(contentRegion.windowAsDockingWindow != 0)
		{
			dockingWindowList.push_back(contentRegion.windowAsDockingWindow);
		}
	}

	//Return the list of nested docking windows to the caller
	return dockingWindowList;
}

//----------------------------------------------------------------------------------------
bool DashboardWindow::CanResizeChildContainerWindowEdge(IDockingWindow* childContainer, WindowEdge windowEdge) const
{
	return false;
}

//----------------------------------------------------------------------------------------
void DashboardWindow::UpdateDesiredChildContainerSize(IDockingWindow* childContainer, int desiredWidth, int desiredHeight)
{ }

//----------------------------------------------------------------------------------------
HCURSOR DashboardWindow::ParentOverrideCursorForChildContainer(IDockingWindow* childContainer, int cursorPosX, int cursorPosY) const
{
	return NULL;
}

//----------------------------------------------------------------------------------------
bool DashboardWindow::ParentBorderClickForChildContainer(IDockingWindow* childContainer, int cursorPosX, int cursorPosY)
{
	return false;
}

//----------------------------------------------------------------------------------------
//Parent docking window methods
//----------------------------------------------------------------------------------------
IDockingWindow* DashboardWindow::GetParentDockingWindow() const
{
	return parentDockingWindow;
}

//----------------------------------------------------------------------------------------
void DashboardWindow::NotifyAddedToParent(IDockingWindow* newParentDockingWindow)
{
	parentDockingWindow = newParentDockingWindow;
}

//----------------------------------------------------------------------------------------
void DashboardWindow::NotifyRemovedFromParent()
{
	parentDockingWindow = 0;
}

//----------------------------------------------------------------------------------------
//Cached data
//----------------------------------------------------------------------------------------
void DashboardWindow::UpdateCachedLocations(bool deferWindowUpdate)
{
	//Update the cached position and size of each vertical divider branching off the top
	//edge of the control
	int currentPosX = 0;
	for(std::list<DividerContentEntry>::iterator topLevelDividerListIterator = topLevelDividersFromTop.begin(); topLevelDividerListIterator != topLevelDividersFromTop.end(); ++topLevelDividerListIterator)
	{
		DividerContentEntry& entry = *topLevelDividerListIterator;
		if(entry.followingContentDivider != 0)
		{
			UpdateCachedDividerLocation(currentPosX, 0, *entry.contentRegion, *entry.followingContentDivider);
			currentPosX = entry.followingContentDivider->cachedPosX + entry.followingContentDivider->cachedWidth;
		}
	}

	//Update the cached position and size of each horizontal divider branching off the
	//left edge of the control
	int currentPosY = 0;
	for(std::list<DividerContentEntry>::iterator topLevelDividerListIterator = topLevelDividersFromLeft.begin(); topLevelDividerListIterator != topLevelDividersFromLeft.end(); ++topLevelDividerListIterator)
	{
		DividerContentEntry& entry = *topLevelDividerListIterator;
		if(entry.followingContentDivider != 0)
		{
			UpdateCachedDividerLocation(0, currentPosY, *entry.contentRegion, *entry.followingContentDivider);
			currentPosY = entry.followingContentDivider->cachedPosY + entry.followingContentDivider->cachedHeight;
		}
	}

	//Update the cached position of each region
	for(std::list<ContentRegion*>::iterator contentRegionIterator = regions.begin(); contentRegionIterator != regions.end(); ++contentRegionIterator)
	{
		ContentRegion& contentRegion = *(*contentRegionIterator);
		contentRegion.cachedPosX = (contentRegion.leftDivider != 0)? contentRegion.leftDivider->cachedPosX + dividerSize: 0;
		contentRegion.cachedPosY = (contentRegion.topDivider != 0)? contentRegion.topDivider->cachedPosY + dividerSize: 0;
	}

	//If window size and position updates have been requested, perform them now.
	if(!deferWindowUpdate)
	{
		ApplyCachedLocations();
	}
}

//----------------------------------------------------------------------------------------
void DashboardWindow::ApplyCachedLocations()
{
	//Begin a session for processing this batch of window size changes. Processing all the
	//window size and position changes in a single operation in this manner gives the best
	//performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos((int)regions.size());

	//Resize any hosted content windows which need to be resized
	for(std::list<ContentRegion*>::iterator contentRegionIterator = regions.begin(); contentRegionIterator != regions.end(); ++contentRegionIterator)
	{
		ContentRegion& contentRegion = *(*contentRegionIterator);
		if((contentRegion.windowPosX != contentRegion.cachedPosX) || (contentRegion.windowPosY != contentRegion.cachedPosY) || (contentRegion.windowWidth != contentRegion.width) || (contentRegion.windowHeight != contentRegion.height))
		{
			contentRegion.windowPosX = contentRegion.cachedPosX;
			contentRegion.windowPosY = contentRegion.cachedPosY;
			contentRegion.windowWidth = contentRegion.width;
			contentRegion.windowHeight = contentRegion.height;
			DeferWindowPos(deferWindowPosSession, contentRegion.windowHandle, NULL, contentRegion.windowPosX, contentRegion.windowPosY, contentRegion.windowWidth, contentRegion.windowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
		}
	}

	//Process all the window size changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);
}

//----------------------------------------------------------------------------------------
void DashboardWindow::UpdateCachedDividerLocation(int regionPosX, int regionPosY, const ContentRegion& precedingRegion, Divider& divider)
{
	//Calculate the total length of this divider
	int dividerLength = 0;
	bool firstContentEntry = true;
	for(std::list<DividerContentEntry>::const_iterator i = divider.followingContent.begin(); i != divider.followingContent.end(); ++i)
	{
		const ContentRegion& contentRegion = *(i->contentRegion);
		if(!firstContentEntry)
		{
			dividerLength += dividerSize;
		}
		dividerLength += (divider.vertical)? contentRegion.height: contentRegion.width;
		firstContentEntry = false;
	}

	//Update the cached position and size of the target divider, and advance the region
	//position to the next region after this divider along the same axis.
	if(divider.vertical)
	{
		divider.cachedPosX = regionPosX + precedingRegion.width;
		divider.cachedPosY = regionPosY;
		divider.cachedWidth = dividerSize;
		divider.cachedHeight = dividerLength;
		regionPosX = divider.cachedPosX + divider.cachedWidth;
	}
	else
	{
		divider.cachedPosX = regionPosX;
		divider.cachedPosY = regionPosY + precedingRegion.height;
		divider.cachedWidth = dividerLength;
		divider.cachedHeight = dividerSize;
		regionPosY = divider.cachedPosY + divider.cachedHeight;
	}

	//Update the cached position and size of each child divider branching off this divider
	for(std::list<DividerContentEntry>::iterator childDividerIterator = divider.followingContent.begin(); childDividerIterator != divider.followingContent.end(); ++childDividerIterator)
	{
		DividerContentEntry& entry = *childDividerIterator;
		if(entry.followingContentDivider != 0)
		{
			UpdateCachedDividerLocation(regionPosX, regionPosY, *entry.contentRegion, *entry.followingContentDivider);
			if(divider.vertical)
			{
				regionPosY = entry.followingContentDivider->cachedPosY + entry.followingContentDivider->cachedHeight;
			}
			else
			{
				regionPosX = entry.followingContentDivider->cachedPosX + entry.followingContentDivider->cachedWidth;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//Region insertion functions
//----------------------------------------------------------------------------------------
void DashboardWindow::InsertRegion(ContentRegion& existingRegion, InsertDirection insertDirection, HWND hostedWindow, double regionProportion)
{
	//Calculate the actual distance along the existing region at which to insert the
	//divider for the new region
	int distanceAlongRegion;
	if((insertDirection == InsertDirection::Top) || (insertDirection == InsertDirection::Bottom))
	{
		distanceAlongRegion = (existingRegion.height > dividerSize)? (int)((double)(existingRegion.height - dividerSize) * regionProportion): 0;
	}
	else
	{
		distanceAlongRegion = (existingRegion.width > dividerSize)? (int)((double)(existingRegion.width - dividerSize) * regionProportion): 0;
	}

	//Insert the new region at the calculated position within the existing region
	InsertRegion(existingRegion, insertDirection, hostedWindow, distanceAlongRegion);
}

//----------------------------------------------------------------------------------------
DashboardWindow::Divider* DashboardWindow::InsertRegion(ContentRegion& existingRegion, InsertDirection insertDirection, HWND hostedWindow, int distanceAlongRegion)
{
	//Subclass any child edit controls to fix the focus issue outlined in KB230587. This
	//bug affects us because we use the native window caption bar for docked windows.
	if(hostedWindow != NULL)
	{
		std::list<HWND> descendantWindows = GetDescendantWindows(hostedWindow);
		for(std::list<HWND>::const_iterator i = descendantWindows.begin(); i != descendantWindows.end(); ++i)
		{
			HWND childWindow = *i;
			if(GetClassName(childWindow) == WC_EDIT)
			{
				SetWindowSubclass(childWindow, EditBoxFocusFixSubclassProc, 0, 0);
			}
		}
	}

	//Create a new content region, and add it to the list of content regions.
	ContentRegion* newRegionPointer = new ContentRegion();
	regions.push_back(newRegionPointer);
	ContentRegion& newRegion = *newRegionPointer;

	//Attach the specified window to the new content region
	newRegion.windowHandle = hostedWindow;

	//Set this new hosted window as a child window of our control
	if(hostedWindow!= NULL)
	{
		SetWindowParent(hostedWindow, hwnd);
	}

	//Attempt to retrieve a docking window interface for the hosted window
	if(hostedWindow != NULL)
	{
		newRegion.windowAsDockingWindow = GetDockingWindowFromHWND(hostedWindow);
	}

	//If the hosted window is a docking window, notify the child docking window that it's
	//been added to our docking window as a child.
	if(newRegion.windowAsDockingWindow)
	{
		newRegion.windowAsDockingWindow->NotifyAddedToParent(this);
	}

	//Create a new divider, and add it to the list of dividers.
	Divider* newDivider = new Divider();
	if((insertDirection == InsertDirection::Top) || (insertDirection == InsertDirection::Bottom))
	{
		newDivider->vertical = false;
		newDivider->startAnchorDivider = existingRegion.leftDivider;
		newDivider->endAnchorDivider = existingRegion.rightDivider;
	}
	else
	{
		newDivider->vertical = true;
		newDivider->startAnchorDivider = existingRegion.topDivider;
		newDivider->endAnchorDivider = existingRegion.bottomDivider;
	}
	dividers.push_back(newDivider);

	//Populate the following and preceding content lists for our new divider
	if((insertDirection == InsertDirection::Right) || (insertDirection == InsertDirection::Bottom))
	{
		newDivider->followingContent.push_back(DividerContentEntry(&newRegion, 0));
		newDivider->precedingContent.push_back(DividerContentEntry(&existingRegion, 0));
	}
	else
	{
		newDivider->followingContent.push_back(DividerContentEntry(&existingRegion, 0));
		newDivider->precedingContent.push_back(DividerContentEntry(&newRegion, 0));
	}

	//Build a list of all divider content lists that need to have the new region and
	//divider inserted into them. These lists are maintained by separators running
	//perpendicular to the divider insertion direction.
	std::list<std::list<DividerContentEntry>*> listOfDividerListsToInsertRegion;
	if((insertDirection == InsertDirection::Top) || (insertDirection == InsertDirection::Bottom))
	{
		listOfDividerListsToInsertRegion.push_back((existingRegion.leftDivider != 0)? &existingRegion.leftDivider->followingContent: &topLevelDividersFromLeft);
		if(existingRegion.rightDivider != 0)
		{
			listOfDividerListsToInsertRegion.push_back(&existingRegion.rightDivider->precedingContent);
		}
	}
	else
	{
		listOfDividerListsToInsertRegion.push_back((existingRegion.topDivider != 0)? &existingRegion.topDivider->followingContent: &topLevelDividersFromTop);
		if(existingRegion.bottomDivider != 0)
		{
			listOfDividerListsToInsertRegion.push_back(&existingRegion.bottomDivider->precedingContent);
		}
	}

	//Insert a divider between the new and existing content regions
	for(std::list<std::list<DividerContentEntry>*>::const_iterator i = listOfDividerListsToInsertRegion.begin(); i != listOfDividerListsToInsertRegion.end(); ++i)
	{
		std::list<DividerContentEntry>& dividerList = *(*i);
		std::list<DividerContentEntry>::iterator dividerIterator = dividerList.begin();
		while(dividerIterator != dividerList.end())
		{
			//If this divider content list entry doesn't refer to the existing content
			//region, advance to the next entry in the list.
			DividerContentEntry& searchContentEntry = *dividerIterator;
			if(searchContentEntry.contentRegion != &existingRegion)
			{
				++dividerIterator;
				continue;
			}

			//Determine what divider follows our new content entry
			Divider* followingContentDivider = ((insertDirection == InsertDirection::Right) || (insertDirection == InsertDirection::Bottom))? searchContentEntry.followingContentDivider: newDivider;

			//If our new content region is being added after the existing content region,
			//update the following content divider for the existing content entry, and
			//advance past the existing content region in the divider content list.
			if((insertDirection == InsertDirection::Right) || (insertDirection == InsertDirection::Bottom))
			{
				searchContentEntry.followingContentDivider = newDivider;
				++dividerIterator;
			}

			//Add the new content region into this divider content list, and exit the loop
			//now that we've finished processing this list.
			dividerList.insert(dividerIterator, DividerContentEntry(&newRegion, followingContentDivider));
			break;
		}
	}

	//Build a list of all divider content lists that need to have the existing region
	//replaced with the new region. These lists are maintained by separators running
	//parallel to the divider insertion direction.
	std::list<DividerContentEntry>* dividerListsToReplaceRegion = 0;
	switch(insertDirection)
	{
	case InsertDirection::Top:
		dividerListsToReplaceRegion = (existingRegion.topDivider != 0)? &existingRegion.topDivider->followingContent: &topLevelDividersFromTop;
		break;
	case InsertDirection::Bottom:
		if(existingRegion.bottomDivider != 0)
		{
			dividerListsToReplaceRegion = &existingRegion.bottomDivider->precedingContent;
		}
		break;
	case InsertDirection::Left:
		dividerListsToReplaceRegion = (existingRegion.leftDivider != 0)? &existingRegion.leftDivider->followingContent: &topLevelDividersFromLeft;
		break;
	case InsertDirection::Right:
		if(existingRegion.rightDivider != 0)
		{
			dividerListsToReplaceRegion = &existingRegion.rightDivider->precedingContent;
		}
		break;
	}

	//Replace references to the existing content region with the new content region where
	//required
	if(dividerListsToReplaceRegion != 0)
	{
		std::list<DividerContentEntry>::iterator dividerListsToReplaceRegionIterator = dividerListsToReplaceRegion->begin();
		while(dividerListsToReplaceRegionIterator != dividerListsToReplaceRegion->end())
		{
			//If this divider content list entry doesn't refer to the existing content
			//region, advance to the next entry in the list.
			DividerContentEntry& searchContentEntry = *dividerListsToReplaceRegionIterator;
			if(searchContentEntry.contentRegion != &existingRegion)
			{
				++dividerListsToReplaceRegionIterator;
				continue;
			}

			//Replace this reference to the existing content region with the new content
			//region, and exit the loop now that we've finished processing this list.
			searchContentEntry.contentRegion = &newRegion;
			break;
		}
	}

	//Update the associated dividers for the existing and new content regions
	switch(insertDirection)
	{
	case InsertDirection::Top:
		newRegion.topDivider = existingRegion.topDivider;
		newRegion.bottomDivider = newDivider;
		newRegion.leftDivider = existingRegion.leftDivider;
		newRegion.rightDivider = existingRegion.rightDivider;
		existingRegion.topDivider = newDivider;
		break;
	case InsertDirection::Bottom:
		newRegion.topDivider = newDivider;
		newRegion.bottomDivider = existingRegion.bottomDivider;
		newRegion.leftDivider = existingRegion.leftDivider;
		newRegion.rightDivider = existingRegion.rightDivider;
		existingRegion.bottomDivider = newDivider;
		break;
	case InsertDirection::Left:
		newRegion.topDivider = existingRegion.topDivider;
		newRegion.bottomDivider = existingRegion.bottomDivider;
		newRegion.leftDivider = existingRegion.leftDivider;
		newRegion.rightDivider = newDivider;
		existingRegion.leftDivider = newDivider;
		break;
	case InsertDirection::Right:
		newRegion.topDivider = existingRegion.topDivider;
		newRegion.bottomDivider = existingRegion.bottomDivider;
		newRegion.leftDivider = newDivider;
		newRegion.rightDivider = existingRegion.rightDivider;
		existingRegion.rightDivider = newDivider;
		break;
	}

	//Update the width and height of the new content region, and all other content regions
	//affected by the newly inserted region.
	if((insertDirection == InsertDirection::Top) || (insertDirection == InsertDirection::Bottom))
	{
		newRegion.width = existingRegion.width;
		if((distanceAlongRegion + dividerSize) <= existingRegion.height)
		{
			//Split the existing region to insert the new region
			newRegion.height = (existingRegion.height - (distanceAlongRegion + dividerSize));
			existingRegion.height -= (newRegion.height + dividerSize);
		}
		else
		{
			//If we're extending the existing region in order to add the new region,
			//adjust the size of all affected regions. Note that in order to correctly
			//handle this case, we need to increase the size of all regions running across
			//the control, otherwise gaps would be created next to the existing region.
			newRegion.height = 0;
			int otherRegionsSizeAdjustment = (distanceAlongRegion + dividerSize) - existingRegion.height;
			int existingRegionSizeAdjustment = distanceAlongRegion - existingRegion.height;
			for(std::list<ContentRegion*>::const_iterator i = regions.begin(); i != regions.end(); ++i)
			{
				ContentRegion& region = *(*i);
				if((&region != &existingRegion) && (region.cachedPosY <= (existingRegion.cachedPosY + existingRegion.height)) && ((region.cachedPosY + region.height) >= (existingRegion.cachedPosY + existingRegion.height)))
				{
					region.height += otherRegionsSizeAdjustment;
				}
			}
			existingRegion.height += existingRegionSizeAdjustment;
		}
	}
	else
	{
		newRegion.height = existingRegion.height;
		if((distanceAlongRegion + dividerSize) <= existingRegion.width)
		{
			//Split the existing region to insert the new region
			newRegion.width = (existingRegion.width - (distanceAlongRegion + dividerSize));
			existingRegion.width -= (newRegion.width + dividerSize);
		}
		else
		{
			//If we're extending the existing region in order to add the new region,
			//adjust the size of all affected regions. Note that in order to correctly
			//handle this case, we need to increase the size of all regions running across
			//the control, otherwise gaps would be created next to the existing region.
			newRegion.width = 0;
			int otherRegionsSizeAdjustment = (distanceAlongRegion + dividerSize) - existingRegion.width;
			int existingRegionSizeAdjustment = distanceAlongRegion - existingRegion.width;
			for(std::list<ContentRegion*>::const_iterator i = regions.begin(); i != regions.end(); ++i)
			{
				ContentRegion& region = *(*i);
				if((&region != &existingRegion) && (region.cachedPosX <= (existingRegion.cachedPosX + existingRegion.width)) && ((region.cachedPosX + region.width) >= (existingRegion.cachedPosX + existingRegion.width)))
				{
					region.width += otherRegionsSizeAdjustment;
				}
			}
			existingRegion.width += existingRegionSizeAdjustment;
		}
	}

	//Update our cached divider locations using the new region sizes
	UpdateCachedLocations();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	//If we're docked to a parent window, notify the parent docking window that our list
	//of hosted content windows has changed.
	if(parentDockingWindow != 0)
	{
		parentDockingWindow->NotifyChildContainerContentChanged(this);
	}

	//Return the newly created divider to the caller
	return newDivider;
}

//----------------------------------------------------------------------------------------
void DashboardWindow::RemoveRegion(ContentRegion& existingRegion)
{
	//Determine which divider around the target region we can remove, to extend other
	//regions into the space allocated to the target region, if any.
	bool foundDividerToRemove = false;
	Divider* dividerToRemove = 0;
	WindowEdge dividerToRemoveLocation;
	if((existingRegion.leftDivider != 0) && ((existingRegion.topDivider == 0) && (existingRegion.bottomDivider == 0) || ((existingRegion.leftDivider->startAnchorDivider == existingRegion.topDivider) && (existingRegion.leftDivider->endAnchorDivider == existingRegion.bottomDivider))))
	{
		foundDividerToRemove = true;
		dividerToRemoveLocation = IDockingWindow::WindowEdge::Left;
		dividerToRemove = existingRegion.leftDivider;
	}
	else if((existingRegion.topDivider != 0) && ((existingRegion.leftDivider == 0) && (existingRegion.rightDivider == 0) || ((existingRegion.topDivider->startAnchorDivider == existingRegion.leftDivider) && (existingRegion.topDivider->endAnchorDivider == existingRegion.rightDivider))))
	{
		foundDividerToRemove = true;
		dividerToRemoveLocation = IDockingWindow::WindowEdge::Top;
		dividerToRemove = existingRegion.topDivider;
	}
	else if((existingRegion.rightDivider != 0) && ((existingRegion.topDivider == 0) && (existingRegion.bottomDivider == 0) || ((existingRegion.rightDivider->startAnchorDivider == existingRegion.topDivider) && (existingRegion.rightDivider->endAnchorDivider == existingRegion.bottomDivider))))
	{
		foundDividerToRemove = true;
		dividerToRemoveLocation = IDockingWindow::WindowEdge::Right;
		dividerToRemove = existingRegion.rightDivider;
	}
	else if((existingRegion.bottomDivider != 0) && ((existingRegion.leftDivider == 0) && (existingRegion.rightDivider == 0) || ((existingRegion.bottomDivider->startAnchorDivider == existingRegion.leftDivider) && (existingRegion.bottomDivider->endAnchorDivider == existingRegion.rightDivider))))
	{
		foundDividerToRemove = true;
		dividerToRemoveLocation = IDockingWindow::WindowEdge::Bottom;
		dividerToRemove = existingRegion.bottomDivider;
	}

	//If we couldn't find a suitable existing divider to remove in order to merge the
	//target content region into an existing one, remove the associated window with the
	//target region, but retain the region itself, and abort any further processing.
	if(!foundDividerToRemove)
	{
		UpdateRegionContent(existingRegion, NULL);
		return;
	}

	//If the target region has drop targets currently visible, hide the drop targets.
	if(dropTargetsVisible && (currentRegionDropTarget == &existingRegion))
	{
		HideDropTargets(this);
	}

	//Build a list of all divider content lists that need to have the new region removed
	//from them
	std::list<std::list<DividerContentEntry>*> listOfDividerListsToRemoveRegion;
	if((dividerToRemoveLocation == IDockingWindow::WindowEdge::Left) || (dividerToRemoveLocation == IDockingWindow::WindowEdge::Right))
	{
		listOfDividerListsToRemoveRegion.push_back((existingRegion.topDivider != 0)? &existingRegion.topDivider->followingContent: &topLevelDividersFromTop);
		if(existingRegion.bottomDivider != 0)
		{
			listOfDividerListsToRemoveRegion.push_back(&existingRegion.bottomDivider->precedingContent);
		}
	}
	else
	{
		listOfDividerListsToRemoveRegion.push_back((existingRegion.leftDivider != 0)? &existingRegion.leftDivider->followingContent: &topLevelDividersFromLeft);
		if(existingRegion.rightDivider != 0)
		{
			listOfDividerListsToRemoveRegion.push_back(&existingRegion.rightDivider->precedingContent);
		}
	}

	//Remove the content region from all required content lists
	for(std::list<std::list<DividerContentEntry>*>::const_iterator i = listOfDividerListsToRemoveRegion.begin(); i != listOfDividerListsToRemoveRegion.end(); ++i)
	{
		bool updatedContentList = false;
		std::list<DividerContentEntry>& dividerList = *(*i);
		std::list<DividerContentEntry>::iterator previousContentIterator = dividerList.end();
		std::list<DividerContentEntry>::iterator contentIterator = dividerList.begin();
		while(!updatedContentList && (contentIterator != dividerList.end()))
		{
			DividerContentEntry& contentEntry = *contentIterator;
			if(contentEntry.contentRegion == &existingRegion)
			{
				if((previousContentIterator != dividerList.end()) && (previousContentIterator->followingContentDivider == dividerToRemove))
				{
					previousContentIterator->followingContentDivider = contentEntry.followingContentDivider;
				}
				contentIterator = dividerList.erase(contentIterator);
				updatedContentList = true;
				continue;
			}
			previousContentIterator = contentIterator;
			++contentIterator;
		}
	}

	//Determine which divider list, if any, needs to have the shifted regions that are
	//replacing the existing content region inserted into it.
	std::list<DividerContentEntry>* dividerContentListToUpdate = 0;
	std::list<DividerContentEntry>* dividerContentListToInsert = 0;
	switch(dividerToRemoveLocation)
	{
	case IDockingWindow::WindowEdge::Left:
		if(existingRegion.rightDivider != 0)
		{
			dividerContentListToUpdate = &existingRegion.rightDivider->precedingContent;
			dividerContentListToInsert = &existingRegion.leftDivider->precedingContent;
		}
		break;
	case IDockingWindow::WindowEdge::Top:
		if(existingRegion.bottomDivider != 0)
		{
			dividerContentListToUpdate = &existingRegion.bottomDivider->precedingContent;
			dividerContentListToInsert = &existingRegion.topDivider->precedingContent;
		}
		break;
	case IDockingWindow::WindowEdge::Right:
		dividerContentListToUpdate = (existingRegion.leftDivider != 0)? &existingRegion.leftDivider->followingContent: &topLevelDividersFromLeft;
		dividerContentListToInsert = &existingRegion.rightDivider->followingContent;
		break;
	case IDockingWindow::WindowEdge::Bottom:
		dividerContentListToUpdate = (existingRegion.topDivider != 0)? &existingRegion.topDivider->followingContent: &topLevelDividersFromTop;
		dividerContentListToInsert = &existingRegion.bottomDivider->followingContent;
		break;
	}

	//If we identified a divider list which needs to have our shifted regions inserted
	//into it, insert the entries now.
	if(dividerContentListToUpdate != 0)
	{
		bool updatedContentList = false;
		std::list<DividerContentEntry>& dividerList = *dividerContentListToUpdate;
		std::list<DividerContentEntry>::iterator contentIterator = dividerList.begin();
		while(!updatedContentList && (contentIterator != dividerList.end()))
		{
			if(contentIterator->contentRegion == &existingRegion)
			{
				std::list<DividerContentEntry> dividerContentListToInsertModified = *dividerContentListToInsert;
				dividerContentListToInsertModified.back().followingContentDivider = contentIterator->followingContentDivider;
				contentIterator = dividerList.erase(contentIterator);
				dividerList.splice(contentIterator, dividerContentListToInsertModified);
				updatedContentList = true;
				continue;
			}
			++contentIterator;
		}
	}

	//Update the size and bounding list of dividers for the region that is being extended
	//into the space that was allocated to our target region
	switch(dividerToRemoveLocation)
	{
	case IDockingWindow::WindowEdge::Left:{
		int sizeToAdd = dividerSize + existingRegion.width;
		for(std::list<DividerContentEntry>::iterator i = existingRegion.leftDivider->precedingContent.begin(); i != existingRegion.leftDivider->precedingContent.end(); ++i)
		{
			i->contentRegion->width += sizeToAdd;
			i->contentRegion->rightDivider = existingRegion.rightDivider;
			if(i->followingContentDivider != 0)
			{
				i->followingContentDivider->endAnchorDivider = existingRegion.rightDivider;
			}
		}
		break;}
	case IDockingWindow::WindowEdge::Top:{
		int sizeToAdd = dividerSize + existingRegion.height;
		for(std::list<DividerContentEntry>::iterator i = existingRegion.topDivider->precedingContent.begin(); i != existingRegion.topDivider->precedingContent.end(); ++i)
		{
			i->contentRegion->height += sizeToAdd;
			i->contentRegion->bottomDivider = existingRegion.bottomDivider;
			if(i->followingContentDivider != 0)
			{
				i->followingContentDivider->endAnchorDivider = existingRegion.bottomDivider;
			}
		}
		break;}
	case IDockingWindow::WindowEdge::Right:{
		int sizeToAdd = dividerSize + existingRegion.width;
		for(std::list<DividerContentEntry>::iterator i = existingRegion.rightDivider->followingContent.begin(); i != existingRegion.rightDivider->followingContent.end(); ++i)
		{
			i->contentRegion->width += sizeToAdd;
			i->contentRegion->leftDivider = existingRegion.leftDivider;
			if(i->followingContentDivider != 0)
			{
				i->followingContentDivider->startAnchorDivider = existingRegion.leftDivider;
			}
		}
		break;}
	case IDockingWindow::WindowEdge::Bottom:{
		int sizeToAdd = dividerSize + existingRegion.height;
		for(std::list<DividerContentEntry>::iterator i = existingRegion.bottomDivider->followingContent.begin(); i != existingRegion.bottomDivider->followingContent.end(); ++i)
		{
			i->contentRegion->height += sizeToAdd;
			i->contentRegion->topDivider = existingRegion.topDivider;
			if(i->followingContentDivider != 0)
			{
				i->followingContentDivider->startAnchorDivider = existingRegion.topDivider;
			}
		}
		break;}
	}

	//Remove this hosted window as a child window of our control
	if(existingRegion.windowHandle != NULL)
	{
		SetWindowParent(existingRegion.windowHandle, NULL);
	}

	//If the hosted window is a docking window, notify the child docking window that it's
	//been removed from our docking window as a child.
	if(existingRegion.windowAsDockingWindow != 0)
	{
		existingRegion.windowAsDockingWindow->NotifyRemovedFromParent();
	}

	//Remove the target divider from the list of dividers
	bool removedDivider = false;
	std::list<Divider*>::iterator dividerIterator = dividers.begin();
	while(!removedDivider && (dividerIterator != dividers.end()))
	{
		if(*dividerIterator == dividerToRemove)
		{
			dividers.erase(dividerIterator);
			removedDivider = true;
			continue;
		}
		++dividerIterator;
	}

	//Delete the target divider object
	delete dividerToRemove;

	//Remove the target region from the list of regions
	bool removedRegion = false;
	std::list<ContentRegion*>::iterator regionIterator = regions.begin();
	while(!removedRegion && (regionIterator != regions.end()))
	{
		if(*regionIterator == &existingRegion)
		{
			regions.erase(regionIterator);
			removedRegion = true;
			continue;
		}
		++regionIterator;
	}

	//Delete the target region object
	delete &existingRegion;

	//Update our cached divider locations using the new region sizes
	UpdateCachedLocations();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	//If we're docked to a parent window, notify the parent docking window that our list
	//of hosted content windows has changed.
	if(parentDockingWindow != 0)
	{
		parentDockingWindow->NotifyChildContainerContentChanged(this);
	}
}

//----------------------------------------------------------------------------------------
void DashboardWindow::RemoveAllRegions()
{
	//Cancel any divider drag operation that may be in progress
	if(dividerDragActive)
	{
		dividerDragActive = false;
		verticalDividersBeingDragged.clear();
		horizontalDividersBeingDragged.clear();
	}

	//Delete all regions
	for(std::list<ContentRegion*>::const_iterator i = regions.begin(); i != regions.end(); ++i)
	{
		//Remove this hosted window as a child window of our control
		ContentRegion* region = *i;
		if(region->windowHandle != NULL)
		{
			SetWindowParent(region->windowHandle, NULL);
		}

		//If the hosted window is a docking window, notify the child docking window that it's
		//been removed from our docking window as a child.
		if(region->windowAsDockingWindow != 0)
		{
			region->windowAsDockingWindow->NotifyRemovedFromParent();
		}

		//Delete the region object
		delete region;
	}
	regions.clear();

	//Delete all dividers
	for(std::list<Divider*>::const_iterator i = dividers.begin(); i != dividers.end(); ++i)
	{
		//Delete the divider object
		delete *i;
	}
	dividers.clear();
	topLevelDividersFromTop.clear();
	topLevelDividersFromLeft.clear();

	//Re-create the default region
	ContentRegion* contentRegion = new ContentRegion();
	regions.push_back(contentRegion);
	topLevelDividersFromTop.push_back(DividerContentEntry(contentRegion, 0));
	topLevelDividersFromLeft.push_back(DividerContentEntry(contentRegion, 0));

	//Update our cached divider locations using the new region sizes
	UpdateCachedLocations();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	//If we're docked to a parent window, notify the parent docking window that our list
	//of hosted content windows has changed.
	if(parentDockingWindow != 0)
	{
		parentDockingWindow->NotifyChildContainerContentChanged(this);
	}
}

//----------------------------------------------------------------------------------------
void DashboardWindow::UpdateRegionContent(ContentRegion& existingRegion, HWND newHostedWindow)
{
	if(existingRegion.windowHandle != NULL)
	{
		//If the existing hosted window is currently visible, hide it.
		ShowWindow(existingRegion.windowHandle, SW_HIDE);

		//Remove this hosted window as a child window of our control
		SetWindowParent(existingRegion.windowHandle, NULL);

		//If the hosted window is a docking window, notify the child docking window that
		//it's been removed from our docking window as a child.
		if(existingRegion.windowAsDockingWindow != 0)
		{
			existingRegion.windowAsDockingWindow->NotifyRemovedFromParent();
		}
	}

	//Subclass any child edit controls to fix the focus issue outlined in KB230587. This
	//bug affects us because we use the native window caption bar for docked windows.
	std::list<HWND> descendantWindows = GetDescendantWindows(newHostedWindow);
	for(std::list<HWND>::const_iterator i = descendantWindows.begin(); i != descendantWindows.end(); ++i)
	{
		HWND childWindow = *i;
		if(GetClassName(childWindow) == WC_EDIT)
		{
			SetWindowSubclass(childWindow, EditBoxFocusFixSubclassProc, 0, 0);
		}
	}

	//Record this new hosted window as the window associated with the target region
	existingRegion.windowHandle = newHostedWindow;
	existingRegion.windowAsDockingWindow = GetDockingWindowFromHWND(newHostedWindow);

	if(newHostedWindow != NULL)
	{
		//Set this new hosted window as a child window of our control
		SetWindowParent(newHostedWindow, hwnd);

		//Update the position and size of the new hosted window
		SetWindowPos(newHostedWindow, NULL, existingRegion.windowPosX, existingRegion.windowPosY, existingRegion.windowWidth, existingRegion.windowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

		//If the hosted window is a docking window, notify the child docking window that
		//it's been added to our docking window as a child.
		if(existingRegion.windowAsDockingWindow)
		{
			existingRegion.windowAsDockingWindow->NotifyAddedToParent(this);
		}
	}

	//If we're docked to a parent window, notify the parent docking window that our list
	//of hosted content windows has changed.
	if(parentDockingWindow != 0)
	{
		parentDockingWindow->NotifyChildContainerContentChanged(this);
	}
}

//----------------------------------------------------------------------------------------
DashboardWindow::ContentRegion* DashboardWindow::GetRegionAtPosition(int posX, int posY) const
{
	//Return the region which lies under the target position, if any.
	ContentRegion* locatedRegion = 0;
	std::list<ContentRegion*>::const_iterator regionIterator = regions.begin();
	while((locatedRegion == 0) && (regionIterator != regions.end()))
	{
		ContentRegion* region = *regionIterator;
		if((posX >= region->windowPosX) && (posX < (region->windowPosX + region->width)) && (posY >= region->windowPosY) && (posY < (region->windowPosY + region->height)))
		{
			locatedRegion = region;
			continue;
		}
		++regionIterator;
	}
	return locatedRegion;
}

//----------------------------------------------------------------------------------------
void DashboardWindow::BuildSortedRegionList(std::list<ContentRegion*>& sortedRegionList) const
{
	std::set<ContentRegion*> processedRegions;
	BuildSortedRegionList(topLevelDividersFromTop, processedRegions, sortedRegionList);
	BuildSortedRegionList(topLevelDividersFromLeft, processedRegions, sortedRegionList);
}

//----------------------------------------------------------------------------------------
void DashboardWindow::BuildSortedRegionList(const std::list<DividerContentEntry>& targetContentList, std::set<ContentRegion*>& processedRegions, std::list<ContentRegion*>& sortedRegionList) const
{
	for(std::list<DividerContentEntry>::const_iterator i = targetContentList.begin(); i != targetContentList.end(); ++i)
	{
		//If we haven't already added this region to the sorted region list, add it now.
		ContentRegion* region = i->contentRegion;
		if(processedRegions.find(region) == processedRegions.end())
		{
			processedRegions.insert(region);
			sortedRegionList.push_back(region);
		}

		//If a divider branches off after the processed region, recurse into the branching
		//divider to add its regions to the sorted region list.
		if(i->followingContentDivider != 0)
		{
			BuildSortedRegionList(i->followingContentDivider->followingContent, processedRegions, sortedRegionList);
		}
	}
}

//----------------------------------------------------------------------------------------
//Divider functions
//----------------------------------------------------------------------------------------
std::set<DashboardWindow::Divider*> DashboardWindow::GetMergeCandidates(Divider* targetDivider, bool getPartialOverlappingMatches) const
{
	//Create a set to hold any identified merge candidates for the target divider
	std::set<Divider*> mergeCandidates;

	//Build the list of content lists to search for merge candidates for the target
	//divider
	std::list<std::list<DividerContentEntry>*> contentLists;
	if(targetDivider->startAnchorDivider != 0)
	{
		contentLists.push_back(&targetDivider->startAnchorDivider->precedingContent);
	}
	if(targetDivider->endAnchorDivider != 0)
	{
		contentLists.push_back(&targetDivider->endAnchorDivider->followingContent);
	}

	//Build the set of merge candidates
	for(std::list<std::list<DividerContentEntry>*>::const_iterator i = contentLists.begin(); i != contentLists.end(); ++i)
	{
		const std::list<DividerContentEntry>& contentList = *(*i);
		std::list<DividerContentEntry>::const_iterator contentIterator = contentList.begin();
		while(contentIterator != contentList.end())
		{
			//If there are no more dividers in the content list which are eligible for
			//merging, abort any more processing of the preceding content list.
			if((contentIterator->followingContentDivider == 0) || (!getPartialOverlappingMatches && ((targetDivider->vertical && (contentIterator->followingContentDivider->cachedPosX > targetDivider->cachedPosX)) || (!targetDivider->vertical && (contentIterator->followingContentDivider->cachedPosY > targetDivider->cachedPosY)))))
			{
				break;
			}

			//If we've found a preceding divider which is eligible for merging, add it to
			//the set of merge candidates.
			if(targetDivider->vertical)
			{
				if(getPartialOverlappingMatches && (((contentIterator->followingContentDivider->cachedPosX >= targetDivider->cachedPosX) && (contentIterator->followingContentDivider->cachedPosX < (targetDivider->cachedPosX + targetDivider->cachedWidth))) || (((contentIterator->followingContentDivider->cachedPosX + (contentIterator->followingContentDivider->cachedWidth - 1)) >= targetDivider->cachedPosX) && ((contentIterator->followingContentDivider->cachedPosX + (contentIterator->followingContentDivider->cachedWidth - 1)) < (targetDivider->cachedPosX + targetDivider->cachedWidth)))))
				{
					mergeCandidates.insert(contentIterator->followingContentDivider);
				}
				else if(contentIterator->followingContentDivider->cachedPosX == targetDivider->cachedPosX)
				{
					mergeCandidates.insert(contentIterator->followingContentDivider);
				}
			}
			else
			{
				if(getPartialOverlappingMatches && (((contentIterator->followingContentDivider->cachedPosY >= targetDivider->cachedPosY) && (contentIterator->followingContentDivider->cachedPosY < (targetDivider->cachedPosY + targetDivider->cachedHeight))) || (((contentIterator->followingContentDivider->cachedPosY + (contentIterator->followingContentDivider->cachedHeight - 1)) >= targetDivider->cachedPosY) && ((contentIterator->followingContentDivider->cachedPosY + (contentIterator->followingContentDivider->cachedHeight - 1)) < (targetDivider->cachedPosY + targetDivider->cachedHeight)))))
				{
					mergeCandidates.insert(contentIterator->followingContentDivider);
				}
				else if(contentIterator->followingContentDivider->cachedPosY == targetDivider->cachedPosY)
				{
					mergeCandidates.insert(contentIterator->followingContentDivider);
				}
			}

			//Advance to the next entry in the list
			++contentIterator;
		}
	}

	//Return the set of merge candidates to the caller
	return mergeCandidates;
}

//----------------------------------------------------------------------------------------
std::set<DashboardWindow::Divider*> DashboardWindow::GetAllMergeCandidates(Divider* targetDivider) const
{
	//Create a set to store our resulting list of merge candidates, and add the target
	//divider to the list.
	std::set<Divider*> allMergeCandidates;
	allMergeCandidates.insert(targetDivider);

	//Add the list of merge candidates for the target divider, and all merge candidates of
	//those dividers, into the resulting set recursively.
	GetAllMergeCandidatesInternal(targetDivider, allMergeCandidates);

	//Return the result to the caller
	return allMergeCandidates;
}

//----------------------------------------------------------------------------------------
void DashboardWindow::GetAllMergeCandidatesInternal(Divider* targetDivider, std::set<Divider*>& resultSet) const
{
	std::set<Divider*> mergeCandidatesForDivider = GetMergeCandidates(targetDivider, false);
	for(std::set<Divider*>::const_iterator i = mergeCandidatesForDivider.begin(); i != mergeCandidatesForDivider.end(); ++i)
	{
		//If we've already processed this merge candidate, skip it.
		Divider* mergeCandidate = *i;
		if(resultSet.find(mergeCandidate) != resultSet.end())
		{
			continue;
		}

		//Add this divider to the set of results
		resultSet.insert(mergeCandidate);

		//Add all merge candidates of this merge candidate to the set of results
		GetAllMergeCandidatesInternal(mergeCandidate, resultSet);
	}
}

//----------------------------------------------------------------------------------------
std::list<DashboardWindow::DividerSplitPosition> DashboardWindow::GetSplitPositionsForDivider(Divider* targetDivider) const
{
	//Build a list of all possible split positions for the target divider
	std::list<DividerSplitPosition> splitPositions;
	int precedingRegionCurrentStartPos = 0;
	int followingRegionCurrentStartPos = 0;
	int synchronizedStartPos = 0;
	Divider* synchronizedStartPosPrecedingDivider = 0;
	Divider* synchronizedStartPosFollowingDivider = 0;
	std::list<DividerContentEntry>::const_iterator precedingContentIterator = targetDivider->precedingContent.begin();
	std::list<DividerContentEntry>::const_iterator followingContentIterator = targetDivider->followingContent.begin();
	while((precedingContentIterator != targetDivider->precedingContent.end()) && (followingContentIterator != targetDivider->followingContent.end()))
	{
		//If the start position for the current content regions on both sides of the
		//divider are the same, update the synchronized start position for this split
		//search operation.
		if(precedingRegionCurrentStartPos == followingRegionCurrentStartPos)
		{
			synchronizedStartPos = precedingRegionCurrentStartPos;
			synchronizedStartPosPrecedingDivider = (targetDivider->vertical)? precedingContentIterator->contentRegion->topDivider: precedingContentIterator->contentRegion->leftDivider;
			synchronizedStartPosFollowingDivider = (targetDivider->vertical)? followingContentIterator->contentRegion->topDivider: followingContentIterator->contentRegion->leftDivider;
		}

		//Determine if the divider is eligible for splitting at the current position
		bool canBeSplit = false;
		if(targetDivider->vertical)
		{
			canBeSplit = (precedingContentIterator->contentRegion->bottomDivider == followingContentIterator->contentRegion->bottomDivider) || ((precedingContentIterator->contentRegion->bottomDivider != 0) && (followingContentIterator->contentRegion->bottomDivider != 0) && (precedingContentIterator->contentRegion->bottomDivider->cachedPosY == followingContentIterator->contentRegion->bottomDivider->cachedPosY));
		}
		else
		{
			canBeSplit = (precedingContentIterator->contentRegion->rightDivider == followingContentIterator->contentRegion->rightDivider) || ((precedingContentIterator->contentRegion->rightDivider != 0) && (followingContentIterator->contentRegion->rightDivider != 0) && (precedingContentIterator->contentRegion->rightDivider->cachedPosX == followingContentIterator->contentRegion->rightDivider->cachedPosX));
		}

		//If the divider can be split at this position, add it to the list of split
		//positions.
		if(canBeSplit)
		{
			//Determine if two dividers need to be merged as a result of this divider
			//split operation. This is only required if there is more than one content
			//region on either side of the divider, otherwise we're simply moving this
			//divider along its bounding dividers.
			bool dividerMergeRequired = ((targetDivider->precedingContent.size() > 1) || (targetDivider->followingContent.size() > 1));

			//Calculate the start position and length of the split grabber along the
			//target divider
			static const int dividerSplitGrabberSize = dividerSize * 3;
			int splitDividerRegionLength = (precedingRegionCurrentStartPos - synchronizedStartPos) + ((targetDivider->vertical)? precedingContentIterator->contentRegion->height: precedingContentIterator->contentRegion->width);
			int splitDividerLength = (dividerSplitGrabberSize > splitDividerRegionLength)? splitDividerRegionLength: dividerSplitGrabberSize;
			int splitDividerStartPos = synchronizedStartPos + (((splitDividerRegionLength / 2) - (splitDividerLength / 2)));

			//Add information on this split position to the list of split positions
			DividerSplitPosition splitPositionInfo;
			splitPositionInfo.mergeRequired = dividerMergeRequired;
			splitPositionInfo.dividerSplitterStartPos = splitDividerStartPos;
			splitPositionInfo.dividerSplitterLength = splitDividerLength;
			splitPositionInfo.startPrecedingMergeDivider = synchronizedStartPosPrecedingDivider;
			splitPositionInfo.startFollowingMergeDivider = synchronizedStartPosFollowingDivider;
			splitPositionInfo.endPrecedingMergeDivider = (targetDivider->vertical)? precedingContentIterator->contentRegion->bottomDivider: precedingContentIterator->contentRegion->rightDivider;
			splitPositionInfo.endFollowingMergeDivider = (targetDivider->vertical)? followingContentIterator->contentRegion->bottomDivider: followingContentIterator->contentRegion->rightDivider;
			splitPositions.push_back(splitPositionInfo);
		}

		//Determine which region list to advance based on the current region start
		//positions and sizes, and move to the next entry now.
		int precedingRegionLength = (targetDivider->vertical)? precedingContentIterator->contentRegion->height: precedingContentIterator->contentRegion->width;
		int followingRegionLength = (targetDivider->vertical)? followingContentIterator->contentRegion->height: followingContentIterator->contentRegion->width;
		int newPrecedingRegionCurrentStartPos = precedingRegionCurrentStartPos;
		int newFollowingRegionCurrentStartPos = followingRegionCurrentStartPos;
		if((precedingRegionCurrentStartPos + precedingRegionLength) <= (followingRegionCurrentStartPos + followingRegionLength))
		{
			newPrecedingRegionCurrentStartPos += precedingRegionLength + dividerSize;
			++precedingContentIterator;
		}
		if((precedingRegionCurrentStartPos + precedingRegionLength) >= (followingRegionCurrentStartPos + followingRegionLength))
		{
			newFollowingRegionCurrentStartPos += followingRegionLength + dividerSize;
			++followingContentIterator;
		}
		precedingRegionCurrentStartPos = newPrecedingRegionCurrentStartPos;
		followingRegionCurrentStartPos = newFollowingRegionCurrentStartPos;
	}

	//If we only found a single split position spanning the entire length of the target
	//divider, and there are no merge candidates for the current divider, discard the
	//split position, since it will have no effect.
	if((splitPositions.size() == 1))
	{
		const DividerSplitPosition& splitPosition = splitPositions.front();
		if((splitPosition.startPrecedingMergeDivider == targetDivider->startAnchorDivider) && (splitPosition.endPrecedingMergeDivider == targetDivider->endAnchorDivider) && GetMergeCandidates(targetDivider, false).empty())
		{
			splitPositions.clear();
		}
	}

	//Return the list of identified split positions to the caller
	return splitPositions;
}

//----------------------------------------------------------------------------------------
std::list<DashboardWindow::DividerExtendButtonPosition> DashboardWindow::GetExtendButtonPositionsForDivider(Divider* targetDivider) const
{
	//Create the list to store information on the extend button positions, and retrieve
	//info on the target divider.
	std::list<DividerExtendButtonPosition> extendButtonPositions;
	const int dividerExtendButtonLength = 8;
	int dividerLength = (targetDivider->vertical)? targetDivider->cachedHeight: targetDivider->cachedWidth;

	//Add any extend buttons for the start anchor divider to the list of extend buttons
	//for the target divider
	if(targetDivider->startAnchorDivider != 0)
	{
		//Determine whether any content lists can be extended for the start anchor of the
		//target divider, and the available space for placing extend buttons on the target
		//divider.
		ContentRegion* contentRegionToTruncateForPrecedingContent = targetDivider->startAnchorDivider->precedingContent.front().contentRegion;
		ContentRegion* contentRegionToTruncateForFollowingContent = targetDivider->startAnchorDivider->precedingContent.back().contentRegion;
		bool allowExtendPrecedingContent = (targetDivider->vertical && ((contentRegionToTruncateForPrecedingContent->cachedPosX + contentRegionToTruncateForPrecedingContent->width) >= (targetDivider->cachedPosX + targetDivider->cachedWidth))) || (!targetDivider->vertical && ((contentRegionToTruncateForPrecedingContent->cachedPosY + contentRegionToTruncateForPrecedingContent->height) >= (targetDivider->cachedPosY + targetDivider->cachedHeight)));
		bool allowExtendFollowingContent = (targetDivider->vertical && (contentRegionToTruncateForFollowingContent->cachedPosX <= targetDivider->cachedPosX)) || (!targetDivider->vertical && (contentRegionToTruncateForFollowingContent->cachedPosY <= targetDivider->cachedPosY));

		//Add any available extend buttons for the start anchor of the target divider
		if(allowExtendPrecedingContent)
		{
			DividerExtendButtonPosition extendButtonPosition;
			extendButtonPosition.regionToTruncate = contentRegionToTruncateForPrecedingContent;
			extendButtonPosition.dividerExtendButtonLength = dividerExtendButtonLength;
			extendButtonPosition.dividerExtendButtonStartPos = 0;
			extendButtonPosition.extendFromStart = true;
			extendButtonPosition.extendPrecedingContent = true;
			extendButtonPositions.push_back(extendButtonPosition);
		}
		if(allowExtendFollowingContent)
		{
			DividerExtendButtonPosition extendButtonPosition;
			extendButtonPosition.regionToTruncate = contentRegionToTruncateForFollowingContent;
			extendButtonPosition.dividerExtendButtonLength = dividerExtendButtonLength;
			extendButtonPosition.dividerExtendButtonStartPos = (allowExtendPrecedingContent)? extendButtonPosition.dividerExtendButtonLength: 0;
			extendButtonPosition.extendFromStart = true;
			extendButtonPosition.extendPrecedingContent = false;
			extendButtonPositions.push_back(extendButtonPosition);
		}
	}

	//Add any extend buttons for the end anchor divider to the list of extend buttons for
	//the target divider
	if(targetDivider->endAnchorDivider != 0)
	{
		//Determine whether any content lists can be extended for the end anchor of the
		//target divider, and the available space for placing extend buttons on the target
		//divider.
		ContentRegion* contentRegionToTruncateForPrecedingContent = targetDivider->endAnchorDivider->followingContent.front().contentRegion;
		ContentRegion* contentRegionToTruncateForFollowingContent = targetDivider->endAnchorDivider->followingContent.back().contentRegion;
		bool allowExtendPrecedingContent = (targetDivider->vertical && ((contentRegionToTruncateForPrecedingContent->cachedPosX + contentRegionToTruncateForPrecedingContent->width) >= (targetDivider->cachedPosX + targetDivider->cachedWidth))) || (!targetDivider->vertical && ((contentRegionToTruncateForPrecedingContent->cachedPosY + contentRegionToTruncateForPrecedingContent->height) >= (targetDivider->cachedPosY + targetDivider->cachedHeight)));
		bool allowExtendFollowingContent = (targetDivider->vertical && (contentRegionToTruncateForFollowingContent->cachedPosX <= targetDivider->cachedPosX)) || (!targetDivider->vertical && (contentRegionToTruncateForFollowingContent->cachedPosY <= targetDivider->cachedPosY));

		//Add any available extend buttons for the end anchor of the target divider
		if(allowExtendPrecedingContent)
		{
			DividerExtendButtonPosition extendButtonPosition;
			extendButtonPosition.regionToTruncate = contentRegionToTruncateForPrecedingContent;
			extendButtonPosition.dividerExtendButtonLength = dividerExtendButtonLength;
			extendButtonPosition.dividerExtendButtonStartPos = dividerLength - (extendButtonPosition.dividerExtendButtonLength + (allowExtendFollowingContent? extendButtonPosition.dividerExtendButtonLength: 0));
			extendButtonPosition.extendFromStart = false;
			extendButtonPosition.extendPrecedingContent = true;
			extendButtonPositions.push_back(extendButtonPosition);
		}
		if(allowExtendFollowingContent)
		{
			DividerExtendButtonPosition extendButtonPosition;
			extendButtonPosition.regionToTruncate = contentRegionToTruncateForFollowingContent;
			extendButtonPosition.dividerExtendButtonLength = dividerExtendButtonLength;
			extendButtonPosition.dividerExtendButtonStartPos = dividerLength - extendButtonPosition.dividerExtendButtonLength;
			extendButtonPosition.extendFromStart = false;
			extendButtonPosition.extendPrecedingContent = false;
			extendButtonPositions.push_back(extendButtonPosition);
		}
	}

	//Return the list of extend button positions to the caller
	return extendButtonPositions;
}

//----------------------------------------------------------------------------------------
DashboardWindow::Divider* DashboardWindow::SwapContinuingDividerAtJunction(Divider* firstDividerToMerge, Divider* secondDividerToMerge, Divider* dividerToSplit)
{
	//Replace references to the divider being merged from the preceding content list
	for(std::list<DividerContentEntry>::iterator i = secondDividerToMerge->precedingContent.begin(); i != secondDividerToMerge->precedingContent.end(); ++i)
	{
		DividerContentEntry& contentEntry = *i;
		if(secondDividerToMerge->vertical)
		{
			contentEntry.contentRegion->rightDivider = firstDividerToMerge;
		}
		else
		{
			contentEntry.contentRegion->bottomDivider = firstDividerToMerge;
		}
		if((contentEntry.followingContentDivider != 0) && (contentEntry.followingContentDivider->endAnchorDivider == secondDividerToMerge))
		{
			contentEntry.followingContentDivider->endAnchorDivider = firstDividerToMerge;
		}
	}

	//Replace references to the divider being merged from the following content list
	for(std::list<DividerContentEntry>::iterator i = secondDividerToMerge->followingContent.begin(); i != secondDividerToMerge->followingContent.end(); ++i)
	{
		DividerContentEntry& contentEntry = *i;
		if(secondDividerToMerge->vertical)
		{
			contentEntry.contentRegion->leftDivider = firstDividerToMerge;
		}
		else
		{
			contentEntry.contentRegion->topDivider = firstDividerToMerge;
		}
		if((contentEntry.followingContentDivider != 0) && (contentEntry.followingContentDivider->startAnchorDivider == secondDividerToMerge))
		{
			contentEntry.followingContentDivider->startAnchorDivider = firstDividerToMerge;
		}
	}

	//Replace references to the divider being merged from the end anchor divider
	if(secondDividerToMerge->endAnchorDivider != 0)
	{
		std::list<DividerContentEntry>::iterator precedingContentIterator = secondDividerToMerge->endAnchorDivider->precedingContent.begin();
		while(precedingContentIterator != secondDividerToMerge->endAnchorDivider->precedingContent.end())
		{
			DividerContentEntry& contentEntry = *precedingContentIterator;
			if(contentEntry.followingContentDivider == secondDividerToMerge)
			{
				contentEntry.followingContentDivider = firstDividerToMerge;
				break;
			}
			++precedingContentIterator;
		}
	}

	//Assign the end anchor divider from the second merge divider into the first merge
	//divider
	firstDividerToMerge->endAnchorDivider = secondDividerToMerge->endAnchorDivider;

	//Create a new divider for the second half of the divider we're splitting, and add it
	//to the list of dividers.
	Divider* newDivider = new Divider();
	newDivider->vertical = dividerToSplit->vertical;
	newDivider->startAnchorDivider = firstDividerToMerge;
	newDivider->endAnchorDivider = dividerToSplit->endAnchorDivider;
	dividers.push_back(newDivider);

	//Set the following content divider for the preceding and following content lists in
	//the first merge divider, in preparation for merging the content lists from the
	//second merge divider into it.
	if(!firstDividerToMerge->precedingContent.empty())
	{
		firstDividerToMerge->precedingContent.back().followingContentDivider = dividerToSplit;
	}
	if(!firstDividerToMerge->followingContent.empty())
	{
		firstDividerToMerge->followingContent.back().followingContentDivider = newDivider;
	}

	//Move the lists of content regions from the divider being merged into the divider it
	//is merging with
	firstDividerToMerge->precedingContent.splice(firstDividerToMerge->precedingContent.end(), secondDividerToMerge->precedingContent);
	firstDividerToMerge->followingContent.splice(firstDividerToMerge->followingContent.end(), secondDividerToMerge->followingContent);

	//Move all preceding content entries that occur after the split position into the new
	//divider
	std::list<DividerContentEntry>::iterator splitDividerPrecedingContentIterator = dividerToSplit->precedingContent.begin();
	while(splitDividerPrecedingContentIterator != dividerToSplit->precedingContent.end())
	{
		if(splitDividerPrecedingContentIterator->followingContentDivider == firstDividerToMerge)
		{
			splitDividerPrecedingContentIterator->followingContentDivider = 0;
			break;
		}
		++splitDividerPrecedingContentIterator;
	}
	if(splitDividerPrecedingContentIterator != dividerToSplit->precedingContent.end())
	{
		newDivider->precedingContent.splice(newDivider->precedingContent.begin(), dividerToSplit->precedingContent, ++splitDividerPrecedingContentIterator, dividerToSplit->precedingContent.end());
	}

	//Move all following content entries that occur after the split position into the new
	//divider, and update the following content divider entry at the split position to
	//reference the merged divider.
	std::list<DividerContentEntry>::iterator splitDividerFollowingContentIterator = dividerToSplit->followingContent.begin();
	while(splitDividerFollowingContentIterator != dividerToSplit->followingContent.end())
	{
		if(splitDividerFollowingContentIterator->followingContentDivider == secondDividerToMerge)
		{
			splitDividerFollowingContentIterator->followingContentDivider = 0;
			break;
		}
		++splitDividerFollowingContentIterator;
	}
	if(splitDividerFollowingContentIterator != dividerToSplit->followingContent.end())
	{
		newDivider->followingContent.splice(newDivider->followingContent.begin(), dividerToSplit->followingContent, ++splitDividerFollowingContentIterator, dividerToSplit->followingContent.end());
	}

	//Replace references to the divider being merged from the preceding content list
	for(std::list<DividerContentEntry>::iterator i = newDivider->precedingContent.begin(); i != newDivider->precedingContent.end(); ++i)
	{
		DividerContentEntry& contentEntry = *i;
		if(newDivider->vertical)
		{
			contentEntry.contentRegion->rightDivider = newDivider;
		}
		else
		{
			contentEntry.contentRegion->bottomDivider = newDivider;
		}
		if((contentEntry.followingContentDivider != 0) && (contentEntry.followingContentDivider->endAnchorDivider == dividerToSplit))
		{
			contentEntry.followingContentDivider->endAnchorDivider = newDivider;
		}
	}

	//Replace references to the divider being merged from the following content list
	for(std::list<DividerContentEntry>::iterator i = newDivider->followingContent.begin(); i != newDivider->followingContent.end(); ++i)
	{
		DividerContentEntry& contentEntry = *i;
		if(newDivider->vertical)
		{
			contentEntry.contentRegion->leftDivider = newDivider;
		}
		else
		{
			contentEntry.contentRegion->topDivider = newDivider;
		}
		if((contentEntry.followingContentDivider != 0) && (contentEntry.followingContentDivider->startAnchorDivider == dividerToSplit))
		{
			contentEntry.followingContentDivider->startAnchorDivider = newDivider;
		}
	}

	//Replace references to the divider being split from the end anchor divider
	if(dividerToSplit->endAnchorDivider != 0)
	{
		std::list<DividerContentEntry>::iterator precedingContentIterator = dividerToSplit->endAnchorDivider->precedingContent.begin();
		while(precedingContentIterator != dividerToSplit->endAnchorDivider->precedingContent.end())
		{
			DividerContentEntry& contentEntry = *precedingContentIterator;
			if(contentEntry.followingContentDivider == dividerToSplit)
			{
				contentEntry.followingContentDivider = newDivider;
				break;
			}
			++precedingContentIterator;
		}
	}

	//Set the end anchor for the first part of our split divider to the merged divider
	dividerToSplit->endAnchorDivider = firstDividerToMerge;

	//Remove the second half of the merged divider from the list of dividers
	std::list<Divider*>::iterator dividerIterator = dividers.begin();
	while(dividerIterator != dividers.end())
	{
		if(*dividerIterator == secondDividerToMerge)
		{
			dividers.erase(dividerIterator);
			break;
		}
		++dividerIterator;
	}

	//Delete the second half of the merged divider now that the merge is complete
	delete secondDividerToMerge;

	//Update our cached divider locations using the new divider information
	UpdateCachedLocations();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	//Return the new divider that was created by splitting the existing divider
	return newDivider;
}

//----------------------------------------------------------------------------------------
void DashboardWindow::ExtendDivider(Divider* targetDivider, bool extendFromStart, bool extendPrecedingContent)
{
	//Determine which divider and region are being truncated in this extend operation
	Divider* dividerToTruncate;
	ContentRegion* regionToTruncate;
	if(extendFromStart)
	{
		dividerToTruncate = targetDivider->startAnchorDivider;
		regionToTruncate = (extendPrecedingContent)? dividerToTruncate->precedingContent.front().contentRegion: dividerToTruncate->precedingContent.back().contentRegion;
	}
	else
	{
		dividerToTruncate = targetDivider->endAnchorDivider;
		regionToTruncate = (extendPrecedingContent)? dividerToTruncate->followingContent.front().contentRegion: dividerToTruncate->followingContent.back().contentRegion;
	}

	//Adjust the size and bounding dividers for all content regions being extended, adjust
	//the start and end anchors for all dividers being extended, and calculate the total
	//size that needs to be truncated from the region being extended into.
	int combinedSizeToTruncate = 0;
	if(extendPrecedingContent)
	{
		//Update the bounding dividers for each region being extended, and the anchor
		//dividers for each divider being extended.
		std::list<DividerContentEntry>& contentListToExtend = (extendFromStart)? dividerToTruncate->followingContent: dividerToTruncate->precedingContent;
		std::list<DividerContentEntry>::const_iterator contentListToExtendIterator = contentListToExtend.begin();
		while(contentListToExtendIterator != contentListToExtend.end())
		{
			//Update the bounding dividers and anchor dividers for this region and
			//bounding divider
			ContentRegion* regionToExtend = contentListToExtendIterator->contentRegion;
			if(targetDivider->vertical)
			{
				regionToExtend->height += dividerToTruncate->cachedHeight + regionToTruncate->height;
				if(extendFromStart)
				{
					regionToExtend->topDivider = regionToTruncate->topDivider;
					contentListToExtendIterator->followingContentDivider->startAnchorDivider = regionToTruncate->topDivider;
				}
				else
				{
					regionToExtend->bottomDivider = regionToTruncate->bottomDivider;
					contentListToExtendIterator->followingContentDivider->endAnchorDivider = regionToTruncate->bottomDivider;
				}
				combinedSizeToTruncate += regionToExtend->width + contentListToExtendIterator->followingContentDivider->cachedWidth;
			}
			else
			{
				regionToExtend->width += dividerToTruncate->cachedWidth + regionToTruncate->width;
				if(extendFromStart)
				{
					regionToExtend->leftDivider = regionToTruncate->leftDivider;
					contentListToExtendIterator->followingContentDivider->startAnchorDivider = regionToTruncate->leftDivider;
				}
				else
				{
					regionToExtend->rightDivider = regionToTruncate->rightDivider;
					contentListToExtendIterator->followingContentDivider->endAnchorDivider = regionToTruncate->rightDivider;
				}
				combinedSizeToTruncate += regionToExtend->height + contentListToExtendIterator->followingContentDivider->cachedHeight;
			}

			//Advance to the next content entry until we reach the target divider
			if(contentListToExtendIterator->followingContentDivider == targetDivider)
			{
				++contentListToExtendIterator;
				break;
			}
			++contentListToExtendIterator;
		}

		//Move the list of extended content regions from the divider that was truncated to
		//the new bounding divider content list, if any.
		if(!extendFromStart && ((targetDivider->vertical && (regionToTruncate->bottomDivider == 0)) || (!targetDivider->vertical && (regionToTruncate->rightDivider == 0))))
		{
			//If there is no bounding divider for the extended content regions in the
			//direction they were extended, erase the extended content region entries from
			//the list of content entries in the truncated divider.
			contentListToExtend.erase(contentListToExtend.begin(), contentListToExtendIterator);
		}
		else
		{
			//Determine which content list we need to shift the left of extended regions
			//into
			std::list<DividerContentEntry>* contentListToShiftExtendedContentInto;
			if(targetDivider->vertical)
			{
				contentListToShiftExtendedContentInto = (extendFromStart)? ((regionToTruncate->topDivider != 0)? &regionToTruncate->topDivider->followingContent: &topLevelDividersFromTop): &regionToTruncate->bottomDivider->precedingContent;
			}
			else
			{
				contentListToShiftExtendedContentInto = (extendFromStart)? ((regionToTruncate->leftDivider != 0)? &regionToTruncate->leftDivider->followingContent: &topLevelDividersFromLeft): &regionToTruncate->rightDivider->precedingContent;
			}

			//Find the target position at which to insert the list of extended regions
			std::list<DividerContentEntry>::iterator contentListToShiftExtendedContentIntoIterator = contentListToShiftExtendedContentInto->begin();
			while((contentListToShiftExtendedContentIntoIterator != contentListToShiftExtendedContentInto->end()) && (contentListToShiftExtendedContentIntoIterator->contentRegion != regionToTruncate))
			{
				++contentListToShiftExtendedContentIntoIterator;
			}

			//Move the list of extended regions from the content list of the truncated
			//divider to the target content region list
			contentListToShiftExtendedContentInto->splice(contentListToShiftExtendedContentIntoIterator, contentListToExtend, contentListToExtend.begin(), contentListToExtendIterator);
		}

		//Remove the truncated region from the list of content regions in the divider that
		//it is no longer bordered with
		std::list<DividerContentEntry>& contentListToRemoveEntry = (targetDivider->vertical)? ((regionToTruncate->leftDivider != 0)? regionToTruncate->leftDivider->followingContent: topLevelDividersFromLeft): ((regionToTruncate->topDivider != 0)? regionToTruncate->topDivider->followingContent: topLevelDividersFromTop);
		std::list<DividerContentEntry>::iterator contentListToRemoveEntryIterator = contentListToRemoveEntry.begin();
		while(contentListToRemoveEntryIterator != contentListToRemoveEntry.end())
		{
			if(contentListToRemoveEntryIterator->contentRegion == regionToTruncate)
			{
				contentListToRemoveEntry.erase(contentListToRemoveEntryIterator);
				break;
			}
			else if(contentListToRemoveEntryIterator->followingContentDivider == dividerToTruncate)
			{
				std::list<DividerContentEntry>::iterator previousContentListToRemoveEntryIterator = contentListToRemoveEntryIterator++;
				previousContentListToRemoveEntryIterator->followingContentDivider = contentListToRemoveEntryIterator->followingContentDivider;
				contentListToRemoveEntry.erase(contentListToRemoveEntryIterator);
				break;
			}
			++contentListToRemoveEntryIterator;
		}
	}
	else
	{
		//Update the bounding dividers for each region being extended, and the anchor
		//dividers for each divider being extended.
		std::list<DividerContentEntry>& contentListToExtend = (extendFromStart)? dividerToTruncate->followingContent: dividerToTruncate->precedingContent;
		std::list<DividerContentEntry>::reverse_iterator contentListToExtendIterator = contentListToExtend.rbegin();
		while(contentListToExtendIterator != contentListToExtend.rend())
		{
			//If we've reached the target divider, clear the reference to the following
			//divider, and terminate the search for content to extend.
			if(contentListToExtendIterator->followingContentDivider == targetDivider)
			{
				combinedSizeToTruncate += (targetDivider->vertical)? targetDivider->cachedWidth: targetDivider->cachedHeight;
				contentListToExtendIterator->followingContentDivider = 0;
				break;
			}

			//Update the bounding dividers and anchor dividers for this region and
			//bounding divider
			ContentRegion* regionToExtend = contentListToExtendIterator->contentRegion;
			if(targetDivider->vertical)
			{
				regionToExtend->height += dividerToTruncate->cachedHeight + regionToTruncate->height;
				if(extendFromStart)
				{
					regionToExtend->topDivider = regionToTruncate->topDivider;
					if(contentListToExtendIterator->followingContentDivider != 0)
					{
						contentListToExtendIterator->followingContentDivider->startAnchorDivider = regionToTruncate->topDivider;
					}
				}
				else
				{
					regionToExtend->bottomDivider = regionToTruncate->bottomDivider;
					if(contentListToExtendIterator->followingContentDivider != 0)
					{
						contentListToExtendIterator->followingContentDivider->endAnchorDivider = regionToTruncate->bottomDivider;
					}
				}
				combinedSizeToTruncate += regionToExtend->width + ((contentListToExtendIterator->followingContentDivider != 0)? contentListToExtendIterator->followingContentDivider->cachedWidth: 0);
			}
			else
			{
				regionToExtend->width += dividerToTruncate->cachedWidth + regionToTruncate->width;
				if(extendFromStart)
				{
					regionToExtend->leftDivider = regionToTruncate->leftDivider;
					if(contentListToExtendIterator->followingContentDivider != 0)
					{
						contentListToExtendIterator->followingContentDivider->startAnchorDivider = regionToTruncate->leftDivider;
					}
				}
				else
				{
					regionToExtend->rightDivider = regionToTruncate->rightDivider;
					if(contentListToExtendIterator->followingContentDivider != 0)
					{
						contentListToExtendIterator->followingContentDivider->endAnchorDivider = regionToTruncate->rightDivider;
					}
				}
				combinedSizeToTruncate += regionToExtend->height + ((contentListToExtendIterator->followingContentDivider != 0)? contentListToExtendIterator->followingContentDivider->cachedHeight: 0);
			}

			//Advance to the next content entry
			++contentListToExtendIterator;
		}

		//Move the list of extended content regions from the divider that was truncated to
		//the new bounding divider content list, if any.
		if(!extendFromStart && ((targetDivider->vertical && (regionToTruncate->bottomDivider == 0)) || (!targetDivider->vertical && (regionToTruncate->rightDivider == 0))))
		{
			//If there is no bounding divider for the extended content regions in the
			//direction they were extended, erase the extended content region entries from
			//the list of content entries in the truncated divider. Note that we
			//deliberately use the forward iterator returned by the base method without
			//offsetting it. This is not an error, as in this case, we've iterated to one
			//element past the element we want to erase from, so we want the offset that
			//occurs when the base method is called.
			contentListToExtend.erase(contentListToExtendIterator.base(), contentListToExtend.end());
		}
		else
		{
			//Determine which content list we need to shift the left of extended regions
			//into
			std::list<DividerContentEntry>* contentListToShiftExtendedContentInto;
			if(targetDivider->vertical)
			{
				contentListToShiftExtendedContentInto = (extendFromStart)? ((regionToTruncate->topDivider != 0)? &regionToTruncate->topDivider->followingContent: &topLevelDividersFromTop): &regionToTruncate->bottomDivider->precedingContent;
			}
			else
			{
				contentListToShiftExtendedContentInto = (extendFromStart)? ((regionToTruncate->leftDivider != 0)? &regionToTruncate->leftDivider->followingContent: &topLevelDividersFromLeft): &regionToTruncate->rightDivider->precedingContent;
			}

			//Find the target position at which to insert the list of extended regions,
			//and fix the following dividers for the region being truncated and the last
			//region being extended.
			std::list<DividerContentEntry>::iterator contentListToShiftExtendedContentIntoIterator = contentListToShiftExtendedContentInto->begin();
			while(contentListToShiftExtendedContentIntoIterator != contentListToShiftExtendedContentInto->end())
			{
				if(contentListToShiftExtendedContentIntoIterator->contentRegion == regionToTruncate)
				{
					contentListToExtend.back().followingContentDivider = contentListToShiftExtendedContentIntoIterator->followingContentDivider;
					contentListToShiftExtendedContentIntoIterator->followingContentDivider = targetDivider;
					++contentListToShiftExtendedContentIntoIterator;
					break;
				}
				++contentListToShiftExtendedContentIntoIterator;
			}

			//Move the list of extended regions from the content list of the truncated
			//divider to the target content region list. Note that we deliberately use the
			//forward iterator returned by the base method without offsetting it. This is
			//not an error, as in this case, we've iterated to one element past the
			//element we want to erase from, so we want the offset that occurs when the
			//base method is called.
			contentListToShiftExtendedContentInto->splice(contentListToShiftExtendedContentIntoIterator, contentListToExtend, contentListToExtendIterator.base(), contentListToExtend.end());
		}

		//Remove the truncated region from the list of content regions in the divider that
		//it is no longer bordered with
		std::list<DividerContentEntry>* contentListToRemoveEntry = (targetDivider->vertical)? ((regionToTruncate->rightDivider != 0)? &regionToTruncate->rightDivider->precedingContent: 0): ((regionToTruncate->bottomDivider != 0)? &regionToTruncate->bottomDivider->precedingContent: 0);
		if(contentListToRemoveEntry != 0)
		{
			std::list<DividerContentEntry>::iterator contentListToRemoveEntryIterator = contentListToRemoveEntry->begin();
			while(contentListToRemoveEntryIterator != contentListToRemoveEntry->end())
			{
				if(contentListToRemoveEntryIterator->contentRegion == regionToTruncate)
				{
					contentListToRemoveEntry->erase(contentListToRemoveEntryIterator);
					break;
				}
				else if(contentListToRemoveEntryIterator->followingContentDivider == dividerToTruncate)
				{
					std::list<DividerContentEntry>::iterator previousContentListToRemoveEntryIterator = contentListToRemoveEntryIterator++;
					previousContentListToRemoveEntryIterator->followingContentDivider = contentListToRemoveEntryIterator->followingContentDivider;
					contentListToRemoveEntry->erase(contentListToRemoveEntryIterator);
					break;
				}
				++contentListToRemoveEntryIterator;
			}
		}
	}

	//Adjust the size and bounding dividers for the content region being truncated, and
	//adjust the anchor dividers for the divider being truncated.
	if(targetDivider->vertical)
	{
		regionToTruncate->width -= combinedSizeToTruncate;
		if(extendPrecedingContent)
		{
			regionToTruncate->leftDivider = targetDivider;
			dividerToTruncate->startAnchorDivider = targetDivider;
		}
		else
		{
			regionToTruncate->rightDivider = targetDivider;
			dividerToTruncate->endAnchorDivider = targetDivider;
		}
	}
	else
	{
		regionToTruncate->height -= combinedSizeToTruncate;
		if(extendPrecedingContent)
		{
			regionToTruncate->topDivider = targetDivider;
			dividerToTruncate->startAnchorDivider = targetDivider;
		}
		else
		{
			regionToTruncate->bottomDivider = targetDivider;
			dividerToTruncate->endAnchorDivider = targetDivider;
		}
	}

	//Update the anchor dividers for the extended divider, and add the region we truncated
	//into the content list for the extended divider now that it borders the truncated
	//region.
	if(extendFromStart)
	{
		targetDivider->startAnchorDivider = (targetDivider->vertical)? regionToTruncate->topDivider: regionToTruncate->leftDivider;
		DividerContentEntry newDividerContentEntry(regionToTruncate, dividerToTruncate);
		if(extendPrecedingContent)
		{
			targetDivider->followingContent.push_front(newDividerContentEntry);
		}
		else
		{
			targetDivider->precedingContent.push_front(newDividerContentEntry);
		}
	}
	else
	{
		targetDivider->endAnchorDivider = (targetDivider->vertical)? regionToTruncate->bottomDivider: regionToTruncate->rightDivider;
		DividerContentEntry newDividerContentEntry(regionToTruncate, 0);
		if(extendPrecedingContent)
		{
			targetDivider->followingContent.back().followingContentDivider = dividerToTruncate;
			targetDivider->followingContent.push_back(newDividerContentEntry);
		}
		else
		{
			targetDivider->precedingContent.back().followingContentDivider = dividerToTruncate;
			targetDivider->precedingContent.push_back(newDividerContentEntry);
		}
	}

	//Update our cached divider locations using the new divider information
	UpdateCachedLocations();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);
}

//----------------------------------------------------------------------------------------
//Sizing methods
//----------------------------------------------------------------------------------------
void DashboardWindow::HandleSizeChanged(int newWidth, int newHeight)
{
	//Update the stored width and height of the control
	controlWidth = newWidth;
	controlHeight = newHeight;

	//Begin a session for processing this batch of window size changes. Processing all the
	//window size and position changes in a single operation in this manner gives the best
	//performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos((int)regions.size());

	//Resize the regions which are anchored to the bottom or right edges of the window
	for(std::list<ContentRegion*>::iterator i = regions.begin(); i != regions.end(); ++i)
	{
		//Ensure this content region is anchored to the right and/or bottom edge of the
		//window
		ContentRegion& region = *(*i);
		if((region.rightDivider != 0) && (region.bottomDivider != 0))
		{
			continue;
		}

		//Calculate the new width and height of the content region
		if(region.rightDivider == 0)
		{
			region.width = (controlWidth <= region.cachedPosX)? 0: (controlWidth - region.cachedPosX);
		}
		if(region.bottomDivider == 0)
		{
			region.height = (controlHeight <= region.cachedPosY)? 0: (controlHeight - region.cachedPosY);
		}

		//Add this region to the window size update session
		DeferWindowPos(deferWindowPosSession, region.windowHandle, NULL, 0, 0, region.width, region.height, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	//Process all the window size changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);

	//Update our cached divider locations using the new region sizes
	UpdateCachedLocations();

	//Force the control to redraw
	InvalidateRect(hwnd, NULL, FALSE);
}

//----------------------------------------------------------------------------------------
std::list<DashboardWindow::Divider*> DashboardWindow::GetDividersAtPosition(int posX, int posY) const
{
	std::list<Divider*> dividerList;
	bool foundDivider = false;
	std::list<Divider*>::const_iterator dividerIterator = dividers.begin();
	while(!foundDivider && (dividerIterator != dividers.end()))
	{
		//If the target position is outside the boundaries of this divider, skip it.
		Divider* divider = *dividerIterator;
		if((posX < divider->cachedPosX) || (posX >= (divider->cachedPosX + divider->cachedWidth))
		|| (posY < divider->cachedPosY) || (posY >= (divider->cachedPosY + divider->cachedHeight)))
		{
			++dividerIterator;
			continue;
		}

		//Flag that we've found a divider under the target location
		foundDivider = true;

		//Add this divider to the list of dividers under the target location
		dividerList.push_back(divider);

		//Calculate the distance along this divider where the target position lies
		int targetPositionAlongDivider = (divider->vertical)? (posY - divider->cachedPosY): (posX - divider->cachedPosX);

		//Add any dividers that intersect this divider at the target position to the list
		//of dividers
		std::list<const std::list<DividerContentEntry>*> contentEntryListsToSearch;
		contentEntryListsToSearch.push_back(&divider->followingContent);
		contentEntryListsToSearch.push_back(&divider->precedingContent);
		for(std::list<const std::list<DividerContentEntry>*>::const_iterator i = contentEntryListsToSearch.begin(); i != contentEntryListsToSearch.end(); ++i)
		{
			const std::list<DividerContentEntry>& contentEntryList = *(*i);
			int dividerStartPos = 0;
			Divider* dividerAfterLastRegion = 0;
			bool firstContentEntry = true;
			std::list<DividerContentEntry>::const_iterator contentIterator = contentEntryList.begin();
			while((targetPositionAlongDivider >= (dividerStartPos + dividerSize)) && (contentIterator != contentEntryList.end()))
			{
				dividerStartPos += (!firstContentEntry)? dividerSize: 0;
				firstContentEntry = false;
				if(divider->vertical)
				{
					dividerStartPos += contentIterator->contentRegion->height;
				}
				else
				{
					dividerStartPos += contentIterator->contentRegion->width;
				}
				dividerAfterLastRegion = contentIterator->followingContentDivider;
				++contentIterator;
			}
			if((dividerAfterLastRegion != 0) && (targetPositionAlongDivider >= dividerStartPos) && (targetPositionAlongDivider < (dividerStartPos + dividerSize)))
			{
				dividerList.push_back(dividerAfterLastRegion);
			}
		}
	}

	//Return the list of dividers to the caller
	return dividerList;
}

//----------------------------------------------------------------------------------------
//Layout functions
//----------------------------------------------------------------------------------------
void DashboardWindow::SaveLayoutInfo(std::list<DividerListEntry>& dividerList) const
{
	//Iterate repeatedly over the divider list, saving information on each divider in the
	//correct order. Note that the order is important, as we want to be able to re-create
	//each divider as a series of splits of existing content regions, starting with a
	//single content region and no dividers. This requires us to save information on the
	//dividers in a manner that a divider does not need to be attached to another divider
	//that hasn't already been defined.
	std::map<Divider*, int> processedDividers;
	const int topWindowEdgeDividerID = 1;
	const int leftWindowEdgeDividerID = 2;
	int nextDividerID = leftWindowEdgeDividerID + 1;
	bool foundNewDividerLastPass = true;
	while(foundNewDividerLastPass)
	{
		//Iterate over each divider in the divider list, and look for any dividers which
		//haven't been processed already which can now be added to the saved divider list.
		foundNewDividerLastPass = false;
		for(std::list<Divider*>::const_iterator i = dividers.begin(); i != dividers.end(); ++i)
		{
			//Ensure we haven't already processed this divider
			Divider* targetDivider = *i;
			if(processedDividers.find(targetDivider) != processedDividers.end())
			{
				continue;
			}

			//Ensure the dividers both at the start and end of this divider have already
			//been processed
			if(((targetDivider->startAnchorDivider != 0) && (processedDividers.find(targetDivider->startAnchorDivider) == processedDividers.end()))
			|| ((targetDivider->endAnchorDivider != 0) && (processedDividers.find(targetDivider->endAnchorDivider) == processedDividers.end())))
			{
				continue;
			}

			//Retrieve the ID of the parent divider for this divider, and calculate the
			//distance along the parent divider where this divider appears.
			int parentDividerID;
			int distanceAlongParent;
			if(targetDivider->startAnchorDivider != 0)
			{
				std::map<Divider*, int>::const_iterator parentDividerIterator = processedDividers.find(targetDivider->startAnchorDivider);
				Divider* parentDivider = parentDividerIterator->first;
				parentDividerID = parentDividerIterator->second;
				distanceAlongParent = (targetDivider->vertical)? (targetDivider->cachedPosX - parentDivider->cachedPosX): (targetDivider->cachedPosY - parentDivider->cachedPosY);
			}
			else if(targetDivider->vertical)
			{
				parentDividerID = topWindowEdgeDividerID;
				distanceAlongParent = targetDivider->cachedPosX;
			}
			else
			{
				parentDividerID = leftWindowEdgeDividerID;
				distanceAlongParent = targetDivider->cachedPosY;
			}

			//Build the set of information to save for this divider
			int dividerID = ++nextDividerID;
			DividerListEntry dividerListEntry;
			dividerListEntry.dividerID = dividerID;
			dividerListEntry.parentDividerID = parentDividerID;
			dividerListEntry.dividerDistanceAlongParent = distanceAlongParent;
			dividerList.push_back(dividerListEntry);

			//Save information on this divider to the list of dividers, and flag that at
			//least one new divider has been processed in this pass over the divider list.
			processedDividers.insert(std::make_pair(targetDivider, dividerID));
			foundNewDividerLastPass = true;
		}
	}
}

//----------------------------------------------------------------------------------------
void DashboardWindow::LoadLayoutInfo(const std::list<DividerListEntry>& dividerList)
{
	//Remove all existing content from the control
	RemoveAllRegions();

	//Create a map to hold information on each loaded divider, and add initial entries for
	//the left and top edge of the control.
	const int topWindowEdgeDividerID = 1;
	const int leftWindowEdgeDividerID = 2;
	std::map<int, std::pair<bool, std::list<DividerContentEntry>*>> dividerIDToFollowingDividerList;
	dividerIDToFollowingDividerList.insert(std::make_pair(topWindowEdgeDividerID, std::make_pair(false, &topLevelDividersFromTop)));
	dividerIDToFollowingDividerList.insert(std::make_pair(leftWindowEdgeDividerID, std::make_pair(true, &topLevelDividersFromLeft)));

	//Create each divider in the divider list
	for(std::list<DividerListEntry>::const_iterator i = dividerList.begin(); i != dividerList.end(); ++i)
	{
		//Attempt to retrieve information on the parent divider of this entry
		const DividerListEntry& entry = *i;
		std::map<int, std::pair<bool, std::list<DividerContentEntry>*>>::const_iterator dividerIDToFollowingDividerListIterator = dividerIDToFollowingDividerList.find(entry.parentDividerID);
		if(dividerIDToFollowingDividerListIterator == dividerIDToFollowingDividerList.end())
		{
			continue;
		}
		std::list<DividerContentEntry>& followingDividerList = *(dividerIDToFollowingDividerListIterator->second.second);
		bool parentDividerVertical = dividerIDToFollowingDividerListIterator->second.first;
		InsertDirection insertDirection = (parentDividerVertical)? InsertDirection::Bottom: InsertDirection::Right;

		//Attempt to locate the following region of the parent divider that we need to
		//split in order to re-create the divider, and calculate the position within the
		//region at which to insert the divider.
		int currentDistanceAlongParent = 0;
		ContentRegion* targetContentRegion;
		int distanceAlongTargetContentRegion;
		std::list<DividerContentEntry>::const_iterator followingDividerListIterator = followingDividerList.begin();
		bool foundTargetContentRegion = false;
		while(!foundTargetContentRegion && (followingDividerListIterator != followingDividerList.end()))
		{
			const DividerContentEntry& dividerContentEntry = *followingDividerListIterator;
			int nextRegionSize = (parentDividerVertical)? dividerContentEntry.contentRegion->height: dividerContentEntry.contentRegion->width;
			if((entry.dividerDistanceAlongParent >= currentDistanceAlongParent) && ((dividerContentEntry.followingContentDivider == 0) || (entry.dividerDistanceAlongParent < (currentDistanceAlongParent + nextRegionSize + dividerSize))))
			{
				targetContentRegion = dividerContentEntry.contentRegion;
				distanceAlongTargetContentRegion = entry.dividerDistanceAlongParent - currentDistanceAlongParent;
				foundTargetContentRegion = true;
				continue;
			}

			currentDistanceAlongParent += (parentDividerVertical)? dividerContentEntry.contentRegion->height: dividerContentEntry.contentRegion->width;
			currentDistanceAlongParent += dividerSize;
			++followingDividerListIterator;
		}
		if(!foundTargetContentRegion)
		{
			continue;
		}

		//Split the target region to insert the new divider, and save information on the
		//created divider.
		Divider* newDivider = InsertRegion(*targetContentRegion, insertDirection, NULL, distanceAlongTargetContentRegion);
		dividerIDToFollowingDividerList.insert(std::make_pair(entry.dividerID, std::make_pair(newDivider->vertical, &newDivider->followingContent)));
	}
}

//----------------------------------------------------------------------------------------
//Canvas helper functions
//----------------------------------------------------------------------------------------
void DashboardWindow::DrawImageLine(IImage& image, unsigned int startPosX, unsigned int startPosY, unsigned int endPosX, unsigned int endPosY, const std::map<int, unsigned char>& penColorComponents)
{
	unsigned int lengthX = ((endPosX > startPosX)? (endPosX - startPosX): (startPosX - endPosX)) + 1;
	unsigned int lengthY = ((endPosY > startPosY)? (endPosY - startPosY): (startPosY - endPosY)) + 1;
	unsigned int totalStepsInLine = (lengthX > lengthY)? lengthX: lengthY;
	float incrementFactorX = ((float)endPosX - (float)startPosX) / (float)(totalStepsInLine - 1);
	float incrementFactorY = ((float)endPosY - (float)startPosY) / (float)(totalStepsInLine - 1);
	float roundAdjustmentX = (incrementFactorX < 0)? -0.5f: 0.5f;
	float roundAdjustmentY = (incrementFactorY < 0)? -0.5f: 0.5f;

	for(unsigned int stepNo = 0; stepNo < totalStepsInLine; ++stepNo)
	{
		unsigned int currentPosX = (unsigned int)((int)startPosX + (int)((incrementFactorX * (float)stepNo) + roundAdjustmentX));
		unsigned int currentPosY = (unsigned int)((int)startPosY + (int)((incrementFactorY * (float)stepNo) + roundAdjustmentY));
		for(std::map<int, unsigned char>::const_iterator i = penColorComponents.begin(); i != penColorComponents.end(); ++i)
		{
			image.WritePixelData(currentPosX, currentPosY, i->first, i->second);
		}
	}
}

//----------------------------------------------------------------------------------------
void DashboardWindow::DrawImageSquare(IImage& image, unsigned int startPosX, unsigned int startPosY, unsigned int width, unsigned int height, const std::map<int, unsigned char>& penColorComponents)
{
	for(unsigned int currentPosX = startPosX; currentPosX < (startPosX + width); ++currentPosX)
	{
		for(unsigned int currentPosY = startPosY; currentPosY < (startPosY + height); ++currentPosY)
		{
			for(std::map<int, unsigned char>::const_iterator i = penColorComponents.begin(); i != penColorComponents.end(); ++i)
			{
				image.WritePixelData(currentPosX, currentPosY, i->first, i->second);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
HBITMAP DashboardWindow::ImageToLoadedDIB(IImage& image, HDC deviceContext)
{
	//Attempt to convert the image to a device independent bitmap header and pixel data
	//buffer
	Image::BITMAPV3INFOHEADER bitmapHeader;
	bitmapHeader.biSize = sizeof(bitmapHeader);
	Stream::Buffer pixelData;
	if(!image.SaveDIBImage(pixelData, (BITMAPINFOHEADER*)&bitmapHeader))
	{
		return NULL;
	}

	//Attempt to construct a device independent bitmap using the supplied data context and
	//constructed bitmap header information
	void* pixelDataPointer;
	HBITMAP bitmapHandle = CreateDIBSection(deviceContext, (BITMAPINFO*)&bitmapHeader, DIB_RGB_COLORS, &pixelDataPointer, NULL, 0);
	if(bitmapHandle == NULL)
	{
		return NULL;
	}

	//Load the bitmap pixel data into the bitmap
	SetDIBits(deviceContext, bitmapHandle, 0, bitmapHeader.biHeight, pixelData.GetRawBuffer(), (BITMAPINFO*)&bitmapHeader, DIB_RGB_COLORS);

	//Return the bitmap object to the caller
	return bitmapHandle;
}
