#include "DashboardWindow.h"

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* DashboardWindow::windowClassName = L"EX_DashboardWindow";

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
}

//----------------------------------------------------------------------------------------
DashboardWindow::~DashboardWindow()
{
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

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
bool DashboardWindow::UnregisterWindowClass(HINSTANCE amoduleHandle)
{
	//Unregister each window class registered by this control
	bool result = true;
	result &= (UnregisterClass(windowClassName, amoduleHandle) != 0);

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
	case WM_SIZE:
		return msgWM_SIZE(wParam, lParam);
	case WM_SETCURSOR:
		return msgWM_SETCURSOR(wParam, lParam);
	case WM_GETFONT:
		return msgWM_GETFONT(wParam, lParam);
	case WM_SETFONT:
		return msgWM_SETFONT(wParam, lParam);
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

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_DESTROY(WPARAM wParam, LPARAM lParam)
{
	//##TODO## Detach this window from its children/parent as required
	return 0;
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
		//Calculate the change in width and height
		int widthDelta = newClientWidth - controlWidth;
		int heightDelta = newClientHeight - controlHeight;

		//Update the stored width and height of the control
		controlWidth = newClientWidth;
		controlHeight = newClientHeight;

		//Begin a session for processing this batch of window size changes. Processing all
		//the window size and position changes in a single operation in this manner gives
		//the best performance and appearance.
		HDWP deferWindowPosSession = BeginDeferWindowPos((int)regions.size());

		//Resize the regions which are anchored to the bottom or right edges of the window
		for(std::list<ContentRegion*>::iterator i = regions.begin(); i != regions.end(); ++i)
		{
			//Ensure this content region is anchored to the right and/or bottom edge of
			//the window
			ContentRegion& region = *(*i);
			if((region.rightDivider != 0) && (region.bottomDivider != 0))
			{
				continue;
			}

			//Calculate the new width and height of the content region
			if(region.rightDivider != 0)
			{
				region.width -= widthDelta;
				region.width = (region.width < 0)? 0: region.width;
			}
			if(region.bottomDivider != 0)
			{
				region.height -= heightDelta;
				region.height = (region.height < 0)? 0: region.height;
			}

			//Add this region to the window size update session
			DeferWindowPos(deferWindowPosSession, region.boundWindow, NULL, 0, 0, region.width, region.height, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}

		//Process all the window size changes involved in this update
		EndDeferWindowPos(deferWindowPosSession);

		//Update our cached divider locations using the new region sizes
		UpdateCachedLocations();
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT DashboardWindow::msgWM_SETCURSOR(WPARAM wParam, LPARAM lParam)
{
	//Calculate the current cursor position in client coordinates relative to the tab
	//control
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(hwnd, &point);
	int cursorPosX = point.x;
	int cursorPosY = point.y;

	//Show a size cursor if the mouse is currently over a divider
	//##TODO## Show the "Size All" cursor if the mouse is located within a divider at a
	//point where a divider branches off that divider either direction.
	for(std::list<Divider*>::const_iterator i = dividers.begin(); i != dividers.end(); ++i)
	{
		const Divider& divider = *(*i);
		if((cursorPosX >= divider.cachedPosX) && (cursorPosX < (divider.cachedPosX + divider.cachedWidth))
		&& (cursorPosY >= divider.cachedPosY) && (cursorPosY < (divider.cachedPosY + divider.cachedHeight)))
		{
			//Determine the cursor to show for this divider
			HCURSOR cursor = (divider.vertical)? cursorSizeHorizontal: cursorSizeVertical;

			//Apply the selected cursor at this location
			SetCursor(cursor);
			return TRUE;
		}
	}

	//Pass this message on to the default window procedure
	return DefWindowProc(hwnd, WM_SETCURSOR, wParam, lParam);
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
//Cached data
//----------------------------------------------------------------------------------------
void DashboardWindow::UpdateCachedLocations()
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
}

//----------------------------------------------------------------------------------------
void DashboardWindow::UpdateCachedDividerLocation(int regionPosX, int regionPosY, const ContentRegion& precedingRegion, Divider& divider)
{
	//Update the cached position and size of the target divider, and advance the region
	//position to the next region after this divider along the same axis.
	if(divider.vertical)
	{
		divider.cachedPosX = regionPosX + precedingRegion.width;
		divider.cachedPosY = regionPosY;
		divider.cachedWidth = dividerSize;
		divider.cachedHeight = precedingRegion.height;
		regionPosX = divider.cachedPosX + divider.cachedWidth;
	}
	else
	{
		divider.cachedPosX = regionPosX;
		divider.cachedPosY = regionPosY + precedingRegion.height;
		divider.cachedWidth = precedingRegion.width;
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
void DashboardWindow::InsertRegion(ContentRegion& existingRegion, InsertDirection insertDirection, HWND hostedWindow)
{
	//Create a new content region, and add it to the list of content regions.
	ContentRegion* newRegionPointer = new ContentRegion();
	regions.push_back(newRegionPointer);
	ContentRegion& newRegion = *newRegionPointer;

	//Attach the specified window to the new content region
	newRegion.boundWindow = hostedWindow;

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

	//Build a list of all divider content lists that need to have the new divider added to
	//them
	std::list<std::list<DividerContentEntry>*> listOfDividerLists;
	if((insertDirection == InsertDirection::Top) || (insertDirection == InsertDirection::Bottom))
	{
		listOfDividerLists.push_back((existingRegion.leftDivider != 0)? &existingRegion.leftDivider->followingContent: &topLevelDividersFromLeft);
		if(existingRegion.rightDivider != 0)
		{
			listOfDividerLists.push_back(&existingRegion.rightDivider->precedingContent);
		}
	}
	else
	{
		listOfDividerLists.push_back((existingRegion.topDivider != 0)? &existingRegion.topDivider->followingContent: &topLevelDividersFromTop);
		if(existingRegion.bottomDivider != 0)
		{
			listOfDividerLists.push_back(&existingRegion.bottomDivider->precedingContent);
		}
	}

	//Insert a divider between the new and existing content regions on all lists in which
	//they appear
	for(std::list<std::list<DividerContentEntry>*>::const_iterator i = listOfDividerLists.begin(); i != listOfDividerLists.end(); ++i)
	{
		std::list<DividerContentEntry>& dividerList = *(*i);
		std::list<DividerContentEntry>::iterator dividerIterator = dividerList.begin();
		while(dividerIterator != dividerList.end())
		{
			//If this divider content list entry doesn't refer to the existing content
			//region, advance to the next entry in the list.
			if(dividerIterator->contentRegion != &existingRegion)
			{
				++dividerIterator;
				continue;
			}

			//If our new content region is being added after the existing content region,
			//advance past the existing content region in the divider content list.
			if((insertDirection == InsertDirection::Right) || (insertDirection == InsertDirection::Bottom))
			{
				++dividerIterator;
			}

			//Add the new content region into this divider content list, and exit the loop
			//now that we've finished processing this list.
			dividerList.insert(dividerIterator, DividerContentEntry(&newRegion, newDivider));
			break;
		}
	}

	//Calculate the width and height of the new and existing content regions
	if((insertDirection == InsertDirection::Top) || (insertDirection == InsertDirection::Bottom))
	{
		newRegion.height = (existingRegion.height - dividerSize) / 2;
		existingRegion.height -= (newRegion.height + dividerSize);
	}
	else
	{
		newRegion.width = (existingRegion.width - dividerSize) / 2;
		existingRegion.width -= (newRegion.width + dividerSize);
	}

	//Begin a session for processing this batch of window size changes. Processing all the
	//window size and position changes in a single operation in this manner gives the best
	//performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos(2);

	//Resize the hosted windows in the existing and new content regions
	if(newRegion.boundWindow != NULL)
	{
		DeferWindowPos(deferWindowPosSession, newRegion.boundWindow, NULL, 0, 0, newRegion.width, newRegion.height, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
	if(existingRegion.boundWindow != NULL)
	{
		DeferWindowPos(deferWindowPosSession, existingRegion.boundWindow, NULL, 0, 0, existingRegion.width, existingRegion.height, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	//Process all the window size changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);

	//Update our cached divider locations using the new region sizes
	UpdateCachedLocations();
}
