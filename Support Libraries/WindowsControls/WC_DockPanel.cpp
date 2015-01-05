#include "WC_DockPanel.h"
#include <set>

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* WC_DockPanel::windowClassName = L"EX_DockPanel";

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WC_DockPanel::WC_DockPanel(HINSTANCE amoduleHandle, HWND ahwnd)
:moduleHandle(amoduleHandle), hwnd(ahwnd)
{
	currentControlWidth = 0;
	currentControlHeight = 0;
	hostedContentWindowPosX = -1;
	hostedContentWindowPosY = -1;
	hostedContentWindowWidth = -1;
	hostedContentWindowHeight = -1;
}

//----------------------------------------------------------------------------------------
//Class registration
//----------------------------------------------------------------------------------------
bool WC_DockPanel::RegisterWindowClass(HINSTANCE amoduleHandle)
{
	//Attempt to register the window class for this control, and return the result to the
	//caller.
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
	ATOM registerClassExReturn = RegisterClassEx(&wc);
	return (registerClassExReturn != 0);
}

//----------------------------------------------------------------------------------------
bool WC_DockPanel::UnregisterWindowClass(HINSTANCE amoduleHandle)
{
	//Attempt to unregister the window class for this control, and return the result to
	//the caller.
	BOOL unregisterClassReturn = UnregisterClass(windowClassName, amoduleHandle);
	return (unregisterClassReturn != 0);
}

//----------------------------------------------------------------------------------------
//Message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK WC_DockPanel::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		WC_DockPanel* object = (WC_DockPanel*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->WndProcPrivate(message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		WC_DockPanel* object = new WC_DockPanel((HINSTANCE)createStruct->lpCreateParams, hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->WndProcPrivate(message, wParam, lParam);}
	case WM_DESTROY:{
		WC_DockPanel* object = (WC_DockPanel*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
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
LRESULT WC_DockPanel::WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
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
	case (unsigned int)WindowMessages::AddContentWindow:
		return msgDOCK_ADDCONTENTWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::RemoveContentWindow:
		return msgDOCK_REMOVECONTENTWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::AddDockedWindow:
		return msgDOCK_ADDDOCKEDWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::AddDockedWindowToFront:
		return msgDOCK_ADDDOCKEDWINDOWTOFRONT(wParam, lParam);
	case (unsigned int)WindowMessages::RemoveDockedWindow:
		return msgDOCK_REMOVEDOCKEDWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::GetDockedWindowDesiredWidth:
		return msgDOCK_GETDOCKEDWINDOWDESIREDWIDTH(wParam, lParam);
	case (unsigned int)WindowMessages::SetDockedWindowDesiredWidth:
		return msgDOCK_SETDOCKEDWINDOWDESIREDWIDTH(wParam, lParam);
	case (unsigned int)WindowMessages::GetDockedWindowDesiredHeight:
		return msgDOCK_GETDOCKEDWINDOWDESIREDHEIGHT(wParam, lParam);
	case (unsigned int)WindowMessages::SetDockedWindowDesiredHeight:
		return msgDOCK_SETDOCKEDWINDOWDESIREDHEIGHT(wParam, lParam);
	case (unsigned int)WindowMessages::GetContentRect:
		return msgDOCK_GETCONTENTRECT(wParam, lParam);
	case (unsigned int)WindowMessages::CalculateNewDockedWindowRect:
		return msgDOCK_CALCULATENEWDOCKEDWINDOWRECT(wParam, lParam);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
{
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
LRESULT WC_DockPanel::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	int newClientWidth = rect.right;
	int newClientHeight = rect.bottom;

	//Handle this size changed event
	if((currentControlWidth != newClientWidth) || (currentControlHeight != newClientHeight))
	{
		HandleSizeChanged(newClientWidth, newClientHeight);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam)
{
	//If the WS_EX_TRANSPARENT window style has been applied to our control, since we want
	//this control to essentially have a transparent background, we don't perform any
	//operation when a background erase is requested. Note that this requires the
	//containing window to use the WS_EX_COMPOSITED style in order to achieve the desired
	//effect. If the WS_EX_TRANSPARENT style has not been specified, we pass this message
	//on to DefWindowProc.
	if(((unsigned int)GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT) != 0)
	{
		return TRUE;
	}
	return DefWindowProc(hwnd, WM_ERASEBKGND, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgWM_NCHITTEST(WPARAM wParam, LPARAM lParam)
{
	//Make this control transparent in the client area for hit testing
	LRESULT result = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
	return (result == HTCLIENT)? HTTRANSPARENT: result;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgWM_COMMAND(WPARAM wParam, LPARAM lParam)
{
	//Forward this message directly to the parent window
	return SendMessage(GetAncestor(hwnd, GA_PARENT), WM_COMMAND, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgWM_NOTIFY(WPARAM wParam, LPARAM lParam)
{
	//Forward this message directly to the parent window
	return SendMessage(GetAncestor(hwnd, GA_PARENT), WM_NOTIFY, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgWM_BOUNCE(WPARAM wParam, LPARAM lParam)
{
	//Forward this message directly to the parent window
	return SendMessage(GetAncestor(hwnd, GA_PARENT), WM_BOUNCE, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_ADDCONTENTWINDOW(WPARAM wParam, LPARAM lParam)
{
	HWND targetHostedContent = (HWND)lParam;
	AddHostedContent(targetHostedContent);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_REMOVECONTENTWINDOW(WPARAM wParam, LPARAM lParam)
{
	HWND targetHostedContent = (HWND)lParam;
	RemoveHostedContent(targetHostedContent);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_ADDDOCKEDWINDOW(WPARAM wParam, LPARAM lParam)
{
	HWND newDockedWindow = (HWND)lParam;
	DockLocation dockLocation = (DockLocation)wParam;
	AddDockedWindow(newDockedWindow, dockLocation, false);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_ADDDOCKEDWINDOWTOFRONT(WPARAM wParam, LPARAM lParam)
{
	HWND newDockedWindow = (HWND)lParam;
	DockLocation dockLocation = (DockLocation)wParam;
	AddDockedWindow(newDockedWindow, dockLocation, true);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_REMOVEDOCKEDWINDOW(WPARAM wParam, LPARAM lParam)
{
	HWND dockedWindow = (HWND)lParam;
	RemoveDockedWindow(dockedWindow);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_GETDOCKEDWINDOWDESIREDWIDTH(WPARAM wParam, LPARAM lParam)
{
	HWND dockedWindow = (HWND)lParam;
	int desiredWidth = GetDockedWindowDesiredWidth(dockedWindow);
	return (LRESULT)desiredWidth;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_SETDOCKEDWINDOWDESIREDWIDTH(WPARAM wParam, LPARAM lParam)
{
	HWND dockedWindow = (HWND)lParam;
	int desiredWidth = (int)wParam;
	SetDockedWindowDesiredWidth(dockedWindow, desiredWidth);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_GETDOCKEDWINDOWDESIREDHEIGHT(WPARAM wParam, LPARAM lParam)
{
	HWND dockedWindow = (HWND)lParam;
	int desiredHeight = GetDockedWindowDesiredHeight(dockedWindow);
	return (LRESULT)desiredHeight;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_SETDOCKEDWINDOWDESIREDHEIGHT(WPARAM wParam, LPARAM lParam)
{
	HWND dockedWindow = (HWND)lParam;
	int desiredHeight = (int)wParam;
	SetDockedWindowDesiredHeight(dockedWindow, desiredHeight);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_GETCONTENTRECT(WPARAM wParam, LPARAM lParam)
{
	RECT& rect = *((RECT*)lParam);
	rect.left = hostedContentWindowPosX;
	rect.right = hostedContentWindowPosX + hostedContentWindowWidth;
	rect.top = hostedContentWindowPosY;
	rect.bottom = hostedContentWindowPosY + hostedContentWindowHeight;
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_DockPanel::msgDOCK_CALCULATENEWDOCKEDWINDOWRECT(WPARAM wParam, LPARAM lParam)
{
	CalculateNewDockedWindowRectParams& params = *((CalculateNewDockedWindowRectParams*)lParam);
	int dockedWindowPosX;
	int dockedWindowPosY;
	int dockedWindowWidth;
	int dockedWindowHeight;
	CalculateNewDockedWindowPositionAndSize(params.dockLocation, params.addToFront, params.preferredWidth, params.preferredHeight, dockedWindowPosX, dockedWindowPosY, dockedWindowWidth, dockedWindowHeight);
	params.rect.left = dockedWindowPosX;
	params.rect.right = dockedWindowPosX + dockedWindowWidth;
	params.rect.top = dockedWindowPosY;
	params.rect.bottom = dockedWindowPosY + dockedWindowHeight;
	return 0;
}

//----------------------------------------------------------------------------------------
//Docked window methods
//----------------------------------------------------------------------------------------
void WC_DockPanel::AddDockedWindow(HWND dockedWindow, DockLocation dockLocation, bool pushToFront)
{
	//Set this new docked window as a child window of our control
	SetWindowParent(dockedWindow, hwnd);

	//Calculate the current width and height of the new docked window
	RECT dockedWindowRect;
	GetWindowRect(dockedWindow, &dockedWindowRect);
	int dockedWindowWidth = dockedWindowRect.right - dockedWindowRect.left;
	int dockedWindowHeight = dockedWindowRect.bottom - dockedWindowRect.top;

	//Build a DockedWindowEntry object for this docked window
	DockedWindowEntry entry;
	entry.dockedWindow = dockedWindow;
	entry.dockLocation = dockLocation;
	entry.desiredWindowWidth = dockedWindowWidth;
	entry.desiredWindowHeight = dockedWindowHeight;
	entry.windowPosX = -1;
	entry.windowPosY = -1;
	entry.windowWidth = -1;
	entry.windowHeight = -1;

	//Add this docked window to the list of docked windows
	if(pushToFront)
	{
		dockedWindows.push_front(entry);
	}
	else
	{
		dockedWindows.push_back(entry);
	}

	//Update all child window sizes now that the layout has been altered
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
void WC_DockPanel::RemoveDockedWindow(HWND dockedWindow)
{
	//Attempt to locate the target docked window in our list of docked windows
	bool foundTargetWindow = false;
	std::list<DockedWindowEntry>::iterator dockedWindowsIterator = dockedWindows.begin();
	while(!foundTargetWindow && (dockedWindowsIterator != dockedWindows.end()))
	{
		if(dockedWindowsIterator->dockedWindow == dockedWindow)
		{
			foundTargetWindow = true;
			continue;
		}
		++dockedWindowsIterator;
	}
	if(!foundTargetWindow)
	{
		return;
	}

	//Check if this docked window is currently visible
	bool dockedWindowVisible = (IsWindowVisible(dockedWindow) != 0);

	//If the docked window is currently visible, hide it to avoid seeing it "jump" on the
	//screen between when we remove its parent and position it.
	if(dockedWindowVisible)
	{
		ShowWindow(dockedWindow, SW_HIDE);
	}

	//Obtain the current position of the window in screen coordinates
	RECT rect;
	GetWindowRect(dockedWindow, &rect);
	int currentDockedWindowPosX = rect.left;
	int currentDockedWindowPosY = rect.top;

	//Remove this docked window as a child window of our control
	SetWindowParent(dockedWindow, NULL);

	//Resize the docked window to its specified desired width and height, and position it
	//at its current position in screen coordinates
	unsigned int setWindowPosFlags = SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE;
	SetWindowPos(dockedWindow, NULL, currentDockedWindowPosX, currentDockedWindowPosY, dockedWindowsIterator->desiredWindowWidth, dockedWindowsIterator->desiredWindowHeight, setWindowPosFlags);

	//Show the window again if it was originally visible
	if(dockedWindowVisible)
	{
		ShowWindow(dockedWindow, SWP_SHOWWINDOW);
	}

	//Remove the target item
	dockedWindows.erase(dockedWindowsIterator);

	//Update all child window sizes now that the layout has been altered
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
int WC_DockPanel::GetDockedWindowDesiredWidth(HWND dockedWindow)
{
	//Attempt to locate the target docked window in our list of docked windows
	bool foundTargetWindow = false;
	std::list<DockedWindowEntry>::iterator dockedWindowsIterator = dockedWindows.begin();
	while(!foundTargetWindow && (dockedWindowsIterator != dockedWindows.end()))
	{
		if(dockedWindowsIterator->dockedWindow == dockedWindow)
		{
			foundTargetWindow = true;
			continue;
		}
		++dockedWindowsIterator;
	}
	if(!foundTargetWindow)
	{
		return 0;
	}

	//Return the desired width of the target window to the caller
	return dockedWindowsIterator->desiredWindowWidth;
}

//----------------------------------------------------------------------------------------
void WC_DockPanel::SetDockedWindowDesiredWidth(HWND dockedWindow, int desiredWidth)
{
	//Attempt to locate the target docked window in our list of docked windows
	bool foundTargetWindow = false;
	std::list<DockedWindowEntry>::iterator dockedWindowsIterator = dockedWindows.begin();
	while(!foundTargetWindow && (dockedWindowsIterator != dockedWindows.end()))
	{
		if(dockedWindowsIterator->dockedWindow == dockedWindow)
		{
			foundTargetWindow = true;
			continue;
		}
		++dockedWindowsIterator;
	}
	if(!foundTargetWindow)
	{
		return;
	}

	//Update the desired width of the target window
	dockedWindowsIterator->desiredWindowWidth = desiredWidth;

	//Update all child window sizes now that the layout has been altered
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
int WC_DockPanel::GetDockedWindowDesiredHeight(HWND dockedWindow)
{
	//Attempt to locate the target docked window in our list of docked windows
	bool foundTargetWindow = false;
	std::list<DockedWindowEntry>::iterator dockedWindowsIterator = dockedWindows.begin();
	while(!foundTargetWindow && (dockedWindowsIterator != dockedWindows.end()))
	{
		if(dockedWindowsIterator->dockedWindow == dockedWindow)
		{
			foundTargetWindow = true;
			continue;
		}
		++dockedWindowsIterator;
	}
	if(!foundTargetWindow)
	{
		return 0;
	}

	//Return the desired height of the target window to the caller
	return dockedWindowsIterator->desiredWindowHeight;
}

//----------------------------------------------------------------------------------------
void WC_DockPanel::SetDockedWindowDesiredHeight(HWND dockedWindow, int desiredHeight)
{
	//Attempt to locate the target docked window in our list of docked windows
	bool foundTargetWindow = false;
	std::list<DockedWindowEntry>::iterator dockedWindowsIterator = dockedWindows.begin();
	while(!foundTargetWindow && (dockedWindowsIterator != dockedWindows.end()))
	{
		if(dockedWindowsIterator->dockedWindow == dockedWindow)
		{
			foundTargetWindow = true;
			continue;
		}
		++dockedWindowsIterator;
	}
	if(!foundTargetWindow)
	{
		return;
	}

	//Update the desired height of the target window
	dockedWindowsIterator->desiredWindowHeight = desiredHeight;

	//Update all child window sizes now that the layout has been altered
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
//Hosted content methods
//----------------------------------------------------------------------------------------
void WC_DockPanel::AddHostedContent(HWND ahostedContent)
{
	//Add the specified window to the list of currently hosted content
	hostedContent.push_back(ahostedContent);

	//Set the new content window as a child window of this control
	SetWindowParent(ahostedContent, hwnd);

	//Set the position and size of the new content window to the content region position
	//and size
	SetWindowPos(ahostedContent, NULL, hostedContentWindowPosX, hostedContentWindowPosY, hostedContentWindowWidth, hostedContentWindowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

//----------------------------------------------------------------------------------------
void WC_DockPanel::RemoveHostedContent(HWND ahostedContent)
{
	//Attempt to locate the target hosted content window
	std::list<HWND>::iterator hostedContentIterator = hostedContent.begin();
	bool foundTargetContent = false;
	while(!foundTargetContent && (hostedContentIterator != hostedContent.end()))
	{
		if(*hostedContentIterator == ahostedContent)
		{
			foundTargetContent = true;
			continue;
		}
		++hostedContentIterator;
	}
	if(!foundTargetContent)
	{
		return;
	}

	//Remove the content window as a child window of our control
	SetWindowParent(ahostedContent, NULL);

	//Remove the content window from the list of hosted content windows
	hostedContent.erase(hostedContentIterator);
}

//----------------------------------------------------------------------------------------
//Sizing methods
//----------------------------------------------------------------------------------------
void WC_DockPanel::HandleSizeChanged(int newWidth, int newHeight)
{
	//Save the new client window dimensions
	currentControlWidth = newWidth;
	currentControlHeight = newHeight;

	//Update all child window sizes now that the layout has been altered
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
void WC_DockPanel::UpdateChildWindowSizes()
{
	//Begin a session for processing this batch of window size changes. Processing all the
	//window size and position changes in a single operation in this manner gives the best
	//performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos((int)dockedWindows.size() + 1);

	//Update the position and size of each docked window
	int currentClientPosX = 0;
	int currentClientPosY = 0;
	int remainingClientWidth = currentControlWidth;
	int remainingClientHeight = currentControlHeight;
	for(std::list<DockedWindowEntry>::iterator i = dockedWindows.begin(); i != dockedWindows.end(); ++i)
	{
		//Update the position and size of this docked window, and adjust the current
		//client position and remaining client size to take into account the position and
		//size of the window.
		DockedWindowEntry& entry = *i;
		bool updatedWindowSize = false;
		bool updatedWindowPosition = false;
		UpdateDockedWindowPositionAndSize(entry, updatedWindowSize, updatedWindowPosition, currentClientPosX, currentClientPosY, remainingClientWidth, remainingClientHeight);

		//If the size or position of this docked window has changed, add it to the update
		//session.
		if(updatedWindowPosition || updatedWindowSize)
		{
			DeferWindowPos(deferWindowPosSession, entry.dockedWindow, NULL, entry.windowPosX, entry.windowPosY, entry.windowWidth, entry.windowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
		}
	}

	//If the position and size of the hosted content window region has changed, add each
	//hosted content window to the update session.
	if((hostedContentWindowPosX != currentClientPosX) || (hostedContentWindowPosY != currentClientPosY) || (hostedContentWindowWidth != remainingClientWidth) || (hostedContentWindowHeight != remainingClientHeight))
	{
		//Latch the new position and size of the content region
		hostedContentWindowPosX = currentClientPosX;
		hostedContentWindowPosY = currentClientPosY;
		hostedContentWindowWidth = remainingClientWidth;
		hostedContentWindowHeight = remainingClientHeight;

		//Add each content window to the update session
		for(std::list<HWND>::const_iterator i = hostedContent.begin(); i != hostedContent.end(); ++i)
		{
			DeferWindowPos(deferWindowPosSession, *i, NULL, hostedContentWindowPosX, hostedContentWindowPosY, hostedContentWindowWidth, hostedContentWindowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
		}
	}

	//Process all the window size and position changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);
}

//----------------------------------------------------------------------------------------
void WC_DockPanel::CalculateNewDockedWindowPositionAndSize(DockLocation dockLocation, bool addToFront, int preferredWidth, int preferredHeight, int& windowPosX, int& windowPosY, int& windowWidth, int& windowHeight)
{
	//Calculate the position and size of the available client region. If the docked window
	//is being pushed to the front of the window list, the entire region is available,
	//otherwise we use the current content position and size as the available region.
	int currentClientPosX = (addToFront)? 0: hostedContentWindowPosX;
	int currentClientPosY = (addToFront)? 0: hostedContentWindowPosY;
	int remainingClientWidth = (addToFront)? currentControlWidth: hostedContentWindowWidth;
	int remainingClientHeight = (addToFront)? currentControlHeight: hostedContentWindowHeight;

	//Build a DockedWindowEntry object for the new docked window we're calculating the
	//position and size for
	DockedWindowEntry newDockedWindowEntry;
	newDockedWindowEntry.dockedWindow = NULL;
	newDockedWindowEntry.dockLocation = dockLocation;
	newDockedWindowEntry.desiredWindowWidth = preferredWidth;
	newDockedWindowEntry.desiredWindowHeight = preferredHeight;
	newDockedWindowEntry.windowPosX = -1;
	newDockedWindowEntry.windowPosY = -1;
	newDockedWindowEntry.windowWidth = -1;
	newDockedWindowEntry.windowHeight = -1;

	//Calculate the position and size of the new docked window
	bool updatedWindowSize = false;
	bool updatedWindowPosition = false;
	UpdateDockedWindowPositionAndSize(newDockedWindowEntry, updatedWindowSize, updatedWindowPosition, currentClientPosX, currentClientPosY, remainingClientWidth, remainingClientHeight);

	//Return the calculated new window position and size to the caller
	windowPosX = newDockedWindowEntry.windowPosX;
	windowPosY = newDockedWindowEntry.windowPosY;
	windowWidth = newDockedWindowEntry.windowWidth;
	windowHeight = newDockedWindowEntry.windowHeight;
}

//----------------------------------------------------------------------------------------
void WC_DockPanel::UpdateDockedWindowPositionAndSize(DockedWindowEntry& entry, bool& updatedWindowSize, bool& updatedWindowPosition, int& currentClientPosX, int& currentClientPosY, int& remainingClientWidth, int& remainingClientHeight)
{
	//Calculate the new size of this docked window
	updatedWindowSize = false;
	if((entry.dockLocation == DockLocation::Left) || (entry.dockLocation == DockLocation::Right))
	{
		if((entry.windowHeight != remainingClientHeight) || (entry.windowWidth > remainingClientWidth) || (entry.windowWidth > entry.desiredWindowWidth) || ((entry.windowWidth < remainingClientWidth) && (entry.windowWidth < entry.desiredWindowWidth)))
		{
			entry.windowHeight = remainingClientHeight;
			entry.windowWidth = (entry.desiredWindowWidth > remainingClientWidth)? remainingClientWidth: entry.desiredWindowWidth;
			updatedWindowSize = true;
		}
	}
	else if((entry.dockLocation == DockLocation::Top) || (entry.dockLocation == DockLocation::Bottom))
	{
		if((entry.windowWidth != remainingClientWidth) || (entry.windowHeight > remainingClientHeight) || (entry.windowHeight > entry.desiredWindowHeight) || ((entry.windowHeight < remainingClientHeight) && (entry.windowHeight < entry.desiredWindowHeight)))
		{
			entry.windowWidth = remainingClientWidth;
			entry.windowHeight = (entry.desiredWindowHeight > remainingClientHeight)? remainingClientHeight: entry.desiredWindowHeight;
			updatedWindowSize = true;
		}
	}

	//Calculate the new position of this docked window
	updatedWindowPosition = false;
	if((entry.dockLocation == DockLocation::Left) || (entry.dockLocation == DockLocation::Top) || (entry.dockLocation == DockLocation::Bottom))
	{
		if(entry.windowPosX != currentClientPosX)
		{
			entry.windowPosX = currentClientPosX;
			updatedWindowPosition = true;
		}
	}
	else if(entry.dockLocation == DockLocation::Right)
	{
		if(entry.windowPosX != ((currentClientPosX + remainingClientWidth) - entry.windowWidth))
		{
			entry.windowPosX = ((currentClientPosX + remainingClientWidth) - entry.windowWidth);
			updatedWindowPosition = true;
		}
	}
	if((entry.dockLocation == DockLocation::Left) || (entry.dockLocation == DockLocation::Top) || (entry.dockLocation == DockLocation::Right))
	{
		if(entry.windowPosY != currentClientPosY)
		{
			entry.windowPosY = currentClientPosY;
			updatedWindowPosition = true;
		}
	}
	else if(entry.dockLocation == DockLocation::Bottom)
	{
		if(entry.windowPosY != ((currentClientPosY + remainingClientHeight) - entry.windowHeight))
		{
			entry.windowPosY = ((currentClientPosY + remainingClientHeight) - entry.windowHeight);
			updatedWindowPosition = true;
		}
	}

	//Update the current client position and remaining space within our control
	switch(entry.dockLocation)
	{
	case DockLocation::Left:
		remainingClientWidth -= entry.windowWidth;
		currentClientPosX += entry.windowWidth;
		break;
	case DockLocation::Right:
		remainingClientWidth -= entry.windowWidth;
		break;
	case DockLocation::Top:
		remainingClientHeight -= entry.windowHeight;
		currentClientPosY += entry.windowHeight;
		break;
	case DockLocation::Bottom:
		remainingClientHeight -= entry.windowHeight;
		break;
	}
}
