#include "ViewManager.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* ViewManager::dialogFrameWindowClassName = L"EX_DialogFrame";

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewManager::ViewManager(HINSTANCE aviewManagerAssemblyHandle, HWND amainWindow, HWND amainDockingWindow, ISystemGUIInterface& asystem)
:viewManagerAssemblyHandle(aviewManagerAssemblyHandle), mainWindow(amainWindow), mainDockingWindow(amainDockingWindow), system(asystem), messageWindow(NULL), eventProcessingPaused(false), pendingUIThreadInvoke(false)
{
	//Retrieve the current thread ID, and save it as the UI thread ID.
	uithreadID = GetCurrentThreadId();

	//Initialize the default child window position and position increment values
	defaultWindowPosX = 0;
	defaultWindowPosY = 0;
	defaultWindowPosIncrement = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);

	//Generate the window class name for our message window from the window procedure
	//address
	std::wstring windowClassName;
	PointerToString(WndProcMessageWindow, windowClassName);

	//Register the window class for our message window
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProcMessageWindow;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = viewManagerAssemblyHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = windowClassName.c_str();
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);

	//Create the message window
	messageWindow = CreateWindow(windowClassName.c_str(), L"", 0, 0, 0, 0, 0, NULL, NULL, viewManagerAssemblyHandle, (LPVOID)this);
}

//----------------------------------------------------------------------------------------
ViewManager::~ViewManager()
{
	//Ensure our message window is destroyed
	if(messageWindow != NULL)
	{
		DestroyWindow(messageWindow);
	}
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ViewManager::GetIViewManagerVersion() const
{
	return ThisIViewManagerVersion();
}

//----------------------------------------------------------------------------------------
unsigned int ViewManager::GetIUIManagerVersion() const
{
	return ThisIUIManagerVersion();
}

//----------------------------------------------------------------------------------------
//Class registration
//----------------------------------------------------------------------------------------
bool ViewManager::RegisterDialogFrameWindowClass(HINSTANCE amoduleHandle)
{
	//Register the window class for the dialog window frame
	bool result = true;
	ATOM registerClassExReturn;
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProcDialogWindowFrame;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = amoduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = dialogFrameWindowClassName;
	wc.hIconSm       = NULL;
	registerClassExReturn = RegisterClassEx(&wc);
	result &= (registerClassExReturn != 0);

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
bool ViewManager::UnregisterDialogFrameWindowClass(HINSTANCE amoduleHandle)
{
	//Unregister the window class for the dialog frame
	bool result = true;
	result &= (UnregisterClass(dialogFrameWindowClassName, amoduleHandle) != 0);

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
//Event processing functions
//----------------------------------------------------------------------------------------
bool ViewManager::IsEventProcessingPaused() const
{
	return eventProcessingPaused;
}

//----------------------------------------------------------------------------------------
void ViewManager::PauseEventProcessing()
{
	eventProcessingPaused = true;
}

//----------------------------------------------------------------------------------------
void ViewManager::ResumeEventProcessing()
{
	eventProcessingPaused = false;
	PostMessage(messageWindow, WM_USER, 0, 0);
}

//----------------------------------------------------------------------------------------
//Main window functions
//----------------------------------------------------------------------------------------
HWND ViewManager::GetMainWindow() const
{
	return mainWindow;
}

//----------------------------------------------------------------------------------------
//View management functions
//----------------------------------------------------------------------------------------
bool ViewManager::OpenView(IViewPresenter& aviewPresenter, bool waitToClose)
{
	return OpenViewInternal(aviewPresenter, 0, waitToClose);
}

//----------------------------------------------------------------------------------------
bool ViewManager::OpenView(IViewPresenter& aviewPresenter, IHierarchicalStorageNode& viewState, bool waitToClose)
{
	return OpenViewInternal(aviewPresenter, &viewState, waitToClose);
}

//----------------------------------------------------------------------------------------
bool ViewManager::OpenViewInternal(IViewPresenter& aviewPresenter, IHierarchicalStorageNode* viewState, bool waitToClose)
{
	std::unique_lock<std::mutex> lock(viewMutex);

	//Ensure this view isn't already open or in the queue to be opened
	if(viewInfoSet.find(&aviewPresenter) != viewInfoSet.end())
	{
		return false;
	}

	//Create the ViewInfo object and add it to the list of views
	ViewInfo* viewInfo = new ViewInfo(aviewPresenter);
	viewInfo->viewCurrentlyOpen = false;
	viewInfoSet.insert(std::pair<IViewPresenter*, ViewInfo*>(&viewInfo->viewPresenter, viewInfo));

	//Add an operation to the queue to open this view
	viewOperationQueue.push_back(ViewOperation(ViewOperationType::Open, aviewPresenter, viewState));
	PostMessage(messageWindow, WM_USER, 0, 0);

	//Wait for the view to close
	bool openSucceeded = true;
	if(waitToClose)
	{
		//Wait for the view to close
		++viewInfo->waitCount;
		viewInfo->viewClosed.wait(lock);
		openSucceeded = !viewInfo->viewOpenFailed;
		--viewInfo->waitCount;

		//Inform the worker thread if the view info is safe to delete
		if(viewInfo->waitCount <= 0)
		{
			viewInfo->viewInfoSafeToDelete.notify_all();
		}
	}

	return openSucceeded;
}

//----------------------------------------------------------------------------------------
void ViewManager::CloseView(IViewPresenter& aviewPresenter, bool waitToClose)
{
	std::unique_lock<std::mutex> lock(viewMutex);

	//Retrieve the viewInfo structure for this view
	ViewInfo* viewInfo = 0;
	std::map<IViewPresenter*, ViewInfo*>::iterator viewInfoSetIterator = viewInfoSet.find(&aviewPresenter);
	if(viewInfoSetIterator == viewInfoSet.end())
	{
		return;
	}
	viewInfo = viewInfoSetIterator->second;

	//Add an operation to the queue to close this view
	viewOperationQueue.push_back(ViewOperation(ViewOperationType::Close, aviewPresenter));
	PostMessage(messageWindow, WM_USER, 0, 0);

	//Wait for the view to close
	if(waitToClose)
	{
		//Wait for the view to close
		++viewInfo->waitCount;
		viewInfo->viewClosed.wait(lock);
		--viewInfo->waitCount;

		//Inform the worker thread if the view info is safe to delete
		if(viewInfo->waitCount <= 0)
		{
			viewInfo->viewInfoSafeToDelete.notify_all();
		}
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::ShowView(IViewPresenter& aviewPresenter)
{
	//Add an operation to the queue to show this view
	std::unique_lock<std::mutex> lock(viewMutex);
	viewOperationQueue.push_back(ViewOperation(ViewOperationType::Show, aviewPresenter));
	PostMessage(messageWindow, WM_USER, 0, 0);
}

//----------------------------------------------------------------------------------------
void ViewManager::HideView(IViewPresenter& aviewPresenter)
{
	//Add an operation to the queue to hide this view
	std::unique_lock<std::mutex> lock(viewMutex);
	viewOperationQueue.push_back(ViewOperation(ViewOperationType::Hide, aviewPresenter));
	PostMessage(messageWindow, WM_USER, 0, 0);
}

//----------------------------------------------------------------------------------------
void ViewManager::ActivateView(IViewPresenter& aviewPresenter)
{
	//Add an operation to the queue to activate this view
	std::unique_lock<std::mutex> lock(viewMutex);
	viewOperationQueue.push_back(ViewOperation(ViewOperationType::Activate, aviewPresenter));
	PostMessage(messageWindow, WM_USER, 0, 0);
}

//----------------------------------------------------------------------------------------
bool ViewManager::WaitUntilViewOpened(IViewPresenter& aviewPresenter)
{
	std::unique_lock<std::mutex> lock(viewMutex);

	//Retrieve the viewInfo structure for this view
	ViewInfo* viewInfo = 0;
	std::map<IViewPresenter*, ViewInfo*>::iterator viewInfoSetIterator = viewInfoSet.find(&aviewPresenter);
	if(viewInfoSetIterator == viewInfoSet.end())
	{
		return false;
	}
	viewInfo = viewInfoSetIterator->second;

	//Wait for the view to open
	bool openSucceeded = !viewInfo->viewOpenFailed;
	if(!viewInfo->viewCurrentlyOpen)
	{
		//Wait for the view to open
		++viewInfo->waitCount;
		viewInfo->viewOpened.wait(lock);
		openSucceeded = !viewInfo->viewOpenFailed;
		--viewInfo->waitCount;

		//Inform the worker thread if the view info is safe to delete
		if(viewInfo->waitCount <= 0)
		{
			viewInfo->viewInfoSafeToDelete.notify_all();
		}
	}

	return openSucceeded;
}

//----------------------------------------------------------------------------------------
void ViewManager::WaitUntilViewClosed(IViewPresenter& aviewPresenter)
{
	std::unique_lock<std::mutex> lock(viewMutex);

	//Retrieve the viewInfo structure for this view
	ViewInfo* viewInfo = 0;
	std::map<IViewPresenter*, ViewInfo*>::iterator viewInfoSetIterator = viewInfoSet.find(&aviewPresenter);
	if(viewInfoSetIterator == viewInfoSet.end())
	{
		return;
	}
	viewInfo = viewInfoSetIterator->second;

	//Wait for the view to close
	++viewInfo->waitCount;
	viewInfo->viewClosed.wait(lock);
	--viewInfo->waitCount;

	//Inform the worker thread if the view info is safe to delete
	if(viewInfo->waitCount <= 0)
	{
		viewInfo->viewInfoSafeToDelete.notify_all();
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::NotifyViewClosed(IViewPresenter& aviewPresenter)
{
	std::unique_lock<std::mutex> lock(viewMutex);
	viewOperationQueue.push_back(ViewOperation(ViewOperationType::Delete, aviewPresenter));
	PostMessage(messageWindow, WM_USER, 0, 0);
}

//----------------------------------------------------------------------------------------
void ViewManager::ProcessPendingEvents()
{
	std::unique_lock<std::mutex> lock(viewMutex);
	while(!viewOperationQueue.empty())
	{
		//Grab the details of the next view operation, and pop it from the queue.
		std::list<ViewOperation>::iterator nextViewOperationIterator = viewOperationQueue.begin();
		ViewOperation viewOperation = *nextViewOperationIterator;
		viewOperationQueue.erase(nextViewOperationIterator);

		//Attempt to locate the view info data for this view
		std::map<IViewPresenter*, ViewInfo*>::iterator viewInfoSetIterator = viewInfoSet.find(&viewOperation.viewPresenter);
		if(viewInfoSetIterator != viewInfoSet.end())
		{
			//Get the view info data associated with this view
			ViewInfo* viewInfo = viewInfoSetIterator->second;

			//Process the operation. Note that we explicitly release our lock here. This
			//allows the view to call back into routines which affect views.
			lock.unlock();
			switch(viewOperation.type)
			{
			case ViewOperationType::Open:
				ProcessOpenView(viewOperation.viewPresenter, viewInfo, viewOperation.viewState);
				break;
			case ViewOperationType::Close:
				ProcessCloseView(viewOperation.viewPresenter, viewInfo);
				break;
			case ViewOperationType::Delete:
				ProcessDeleteView(viewOperation.viewPresenter, viewInfo);
				break;
			case ViewOperationType::Activate:
				ProcessActivateView(viewOperation.viewPresenter, viewInfo);
				break;
			case ViewOperationType::Show:
				ProcessShowView(viewOperation.viewPresenter, viewInfo);
				break;
			case ViewOperationType::Hide:
				ProcessHideView(viewOperation.viewPresenter, viewInfo);
				break;
			}
			lock.lock();
		}
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::ProcessOpenView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo, IHierarchicalStorageNode* viewState)
{
	//Create a notifier for the view
	viewInfo->notifier = new ViewStateChangeNotifier(*static_cast<IViewManagerNotifierInterface*>(this), viewInfo->viewPresenter);

	//Request the view to open, and trigger notifications to any waiting threads.
	if(!aviewPresenter.OpenView(*static_cast<IUIManager*>(this), viewInfo->notifier, viewState))
	{
		viewInfo->viewOpenFailed = true;
		viewInfo->viewOpened.notify_all();
		viewInfo->notifier->NotifyViewClosed();
	}
	else
	{
		viewInfo->viewCurrentlyOpen = true;
		viewInfo->viewOpened.notify_all();
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::ProcessCloseView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo)
{
	//Request the view to close
	aviewPresenter.CloseView();
}

//----------------------------------------------------------------------------------------
void ViewManager::ProcessDeleteView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo)
{
	//Erase all references to this view from our lists of views
	std::unique_lock<std::mutex> lock(viewMutex);
	std::list<ViewOperation>::iterator viewOperationQueueIterator = viewOperationQueue.begin();
	while(viewOperationQueueIterator != viewOperationQueue.end())
	{
		if(&viewOperationQueueIterator->viewPresenter == &aviewPresenter)
		{
			std::list<ViewOperation>::iterator viewOperationQueueIteratorToErase = viewOperationQueueIterator;
			++viewOperationQueueIterator;
			viewOperationQueue.erase(viewOperationQueueIteratorToErase);
		}
		else
		{
			++viewOperationQueueIterator;
		}
	}
	viewInfoSet.erase(&aviewPresenter);

	//Wait for all threads to inform us that we can now safely delete the view info data
	while(viewInfo->waitCount > 0)
	{
		viewInfo->viewClosed.notify_all();
		viewInfo->viewInfoSafeToDelete.wait(lock);
	}

	//Clean up the view info
	delete viewInfo->notifier;
	viewInfo->notifier = 0;
	delete viewInfo;
}

//----------------------------------------------------------------------------------------
void ViewManager::ProcessActivateView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo)
{
	//Request the view to activate
	aviewPresenter.ActivateView();
}

//----------------------------------------------------------------------------------------
void ViewManager::ProcessShowView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo)
{
	//Request the view to show
	aviewPresenter.ShowView();
}

//----------------------------------------------------------------------------------------
void ViewManager::ProcessHideView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo)
{
	//Request the view to hide
	aviewPresenter.HideView();
}

//----------------------------------------------------------------------------------------
//Native window creation functions
//----------------------------------------------------------------------------------------
HWND ViewManager::CreateDialogWindow(IView& view, IViewPresenter& viewPresenter, HINSTANCE assemblyHandle, DLGPROC windowProcedure, LPCWSTR templateName)
{
	//Create the dialog
	HWND hwnd = CreateDialogParam(assemblyHandle, templateName, mainWindow, windowProcedure, (LPARAM)&view);
	if(hwnd == NULL)
	{
		return NULL;
	}

	//Record information on this window
	OpenWindowInfo windowInfo(hwnd, view, viewPresenter);
	windowInfoSet.insert(std::pair<HWND, OpenWindowInfo>(hwnd, windowInfo));

	//Return the window handle to the caller
	return hwnd;
}

//----------------------------------------------------------------------------------------
HWND ViewManager::CreateNativeWindow(IView& view, IViewPresenter& viewPresenter, HINSTANCE assemblyHandle, WNDPROC windowProcedure, DWORD windowStyle, DWORD extendedWindowStyle)
{
	//Create a class name from the window procedure address
	std::wstring className;
	PointerToString(windowProcedure, className);

	//Register the window class
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = windowProcedure;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = assemblyHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = className.c_str();
	wc.hIconSm       = NULL;
	if((RegisterClassEx(&wc) == 0) && (GetLastError() != ERROR_CLASS_ALREADY_EXISTS))
	{
		return NULL;
	}

	//Create the window. Note that we create the window as a child window of the main
	//window here, even though it won't really ever be drawn as a child window of the main
	//window directly. Logically, the window should be created with the WS_POPUP style
	//here, then re-parented as a child window when it's added to a docking window, but
	//unfortunately in Windows, when you create a top-level window, it is immediately
	//activated, and even if the created window doesn't have the WS_VISIBLE style set, the
	//currently activated window is still deactivated. We create the window as a child
	//window here to avoid that behaviour.
	windowStyle |= WS_CHILD;
	windowStyle |= ((extendedWindowStyle & WS_EX_COMPOSITED) == 0)? WS_CLIPCHILDREN: 0;
	windowStyle &= ~((DWORD)WS_VISIBLE);
	HWND hwnd = CreateWindowEx(extendedWindowStyle, className.c_str(), L"", windowStyle, 0, 0, 0, 0, mainWindow, NULL, assemblyHandle, (LPVOID)&view);
	if(hwnd == NULL)
	{
		return NULL;
	}

	//Record information on this window
	OpenWindowInfo windowInfo(hwnd, view, viewPresenter);
	windowInfoSet.insert(std::pair<HWND, OpenWindowInfo>(hwnd, windowInfo));

	//Return the window handle to the caller
	return hwnd;
}

//----------------------------------------------------------------------------------------
//Window management functions
//----------------------------------------------------------------------------------------
bool ViewManager::ShowWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const MarshalSupport::Marshal::In<std::wstring>& windowTitle, IHierarchicalStorageNode* windowState)
{
	//If window state has been specified for this window, it is being loaded as part of a
	//layout restore operation. In this case, we pass the window and state information
	//over to the LoadWindowState method, and return the result to the caller.
	if(windowState != 0)
	{
		return LoadWindowState(view, viewPresenter, windowHandle, *windowState, true, windowTitle);
	}

	//Show this window as the requested view type
	bool result = false;
	IView::ViewType viewType = view.GetViewType();
	switch(viewType)
	{
	case IView::ViewType::Dialog:
		result = ShowDialogWindowFirstTime(view, viewPresenter, windowHandle, windowTitle);
		break;
	case IView::ViewType::Dockable:
		result = ShowDockingWindowFirstTime(view, viewPresenter, windowHandle, windowTitle);
		break;
	case IView::ViewType::Document:
		result = ShowDocumentWindowFirstTime(view, viewPresenter, windowHandle, windowTitle);
		break;
	}

	//Return the result to the caller
	return result;
}

//----------------------------------------------------------------------------------------
bool ViewManager::ShowDialogWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle)
{
	//Build a qualified title for this window
	std::wstring qualifiedWindowTitle = BuildQualifiedWindowTitle(view, viewPresenter, windowHandle, windowTitle);

	//Register the dialog frame window class
	RegisterDialogFrameWindowClass(viewManagerAssemblyHandle);

	//Determine the window style settings to use for the dialog window frame
	IView::DialogMode dialogMode = view.GetViewDialogMode();
	DWORD dialogWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_POPUP;
	if(view.CanResizeDialog())
	{
		dialogWindowStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;
	}
	//Note that we set the toolwindow style for modeless dialogs only. We do this because
	//if we set this style for a modal dialog, the application disappears from the alt+tab
	//window list while the modal dialog is active, since tool windows are excluded from
	//the alt+tab list.
	DWORD dialogWindowExtendedStyle = (dialogMode == IView::DialogMode::Modeless)? WS_EX_TOOLWINDOW: 0;

	//Calculate the size to use for the dialog frame in order for it to allow the hosted
	//window to be fully visible
	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);
	AdjustWindowRectEx(&clientRect, dialogWindowStyle, FALSE, dialogWindowExtendedStyle);
	int dialogFrameWidth = clientRect.right - clientRect.left;
	int dialogFrameHeight = clientRect.bottom - clientRect.top;

	//If this is a modal dialog window, disable the main window and all other windows
	//owned by the main window, and build a list of disabled windows to pass on to the
	//dialog frame window procedure.
	LPVOID createWindowParam = 0;
	std::list<HWND> disabledWindowList;
	IView::DialogPos initialDialogPos = view.GetViewInitialDialogPosition();
	if(dialogMode == IView::DialogMode::Modal)
	{
		//Disable all other currently enabled top-level owned windows of the main window
		disabledWindowList = DisableAllEnabledDialogWindows(mainWindow);

		//Disable the main window itself
		if(EnableWindow(mainWindow, FALSE) == 0)
		{
			disabledWindowList.push_back(mainWindow);
		}

		//Build the argument to pass to the dialog frame window procedure
		createWindowParam = (LPVOID)&disabledWindowList;
	}

	//Create the dialog frame to use for this dialog
	HWND dialogWindowFrame = CreateWindowEx(dialogWindowExtendedStyle, dialogFrameWindowClassName, qualifiedWindowTitle.c_str(), dialogWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, dialogFrameWidth, dialogFrameHeight, mainWindow, NULL, viewManagerAssemblyHandle, createWindowParam);

	//Calculate the screen position for the dialog window
	int dialogFramePosX;
	int dialogFramePosY;
	if((initialDialogPos == IView::DialogPos::Center) || (dialogMode == IView::DialogMode::Modal))
	{
		//Calculate the position and size of the main window
		RECT rect;
		GetWindowRect(mainWindow, &rect);
		int mainWindowPosX = rect.left;
		int mainWindowPosY = rect.top;
		int mainWindowWidth = rect.right - rect.left;
		int mainWindowHeight = rect.bottom - rect.top;

		//Calculate the position of the modal dialog to be centered on the main window
		dialogFramePosX = (mainWindowPosX + (mainWindowWidth / 2)) - (dialogFrameWidth / 2);
		dialogFramePosY = (mainWindowPosY + (mainWindowHeight / 2)) - (dialogFrameHeight / 2);
	}
	else
	{
		//Calculate the size of any hidden borders attached to the target window
		int borderLeft;
		int borderRight;
		int borderTop;
		int borderBottom;
		GetHiddenBorderDimensions(dialogWindowFrame, borderLeft, borderRight, borderTop, borderBottom);

		//Calculate the screen position to display the window at
		int totalDialogFrameWidth = dialogFrameWidth + (borderLeft + borderRight);
		int totalDialogFrameHeight = dialogFrameHeight + (borderTop + borderBottom);
		if(FindBestWindowPosition(totalDialogFrameWidth, totalDialogFrameHeight, dialogFramePosX, dialogFramePosY))
		{
			//If we managed to calculate a position for the window, adjust the located
			//position to factor the hidden border region back in, since this will be
			//added in by the system when positioning the window.
			dialogFramePosX += borderLeft;
			dialogFramePosY += borderTop;
		}
		else
		{
			//Calculate the position of the client area of the main window
			POINT clientPos;
			clientPos.x = 0;
			clientPos.y = 0;
			ClientToScreen(mainWindow, &clientPos);
			int mainWindowPosX = clientPos.x;
			int mainWindowPosY = clientPos.y;

			//Fall back to placing the new window in an arbitrary default position
			dialogFramePosX = mainWindowPosX + defaultWindowPosX;
			dialogFramePosY = mainWindowPosY + defaultWindowPosY;
			defaultWindowPosX = (defaultWindowPosX + defaultWindowPosIncrement) % (defaultWindowPosIncrement * defaultWindowPosStackDepth);
			defaultWindowPosY = (defaultWindowPosY + defaultWindowPosIncrement) % (defaultWindowPosIncrement * defaultWindowPosStackDepth);
		}
	}

	//Position the dialog window
	SetWindowPos(dialogWindowFrame, NULL, dialogFramePosX, dialogFramePosY, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);

	//Make the specified window a child of the dialog frame
	SetWindowParent(windowHandle, dialogWindowFrame);

	//Show the target window
	::ShowWindow(windowHandle, SW_SHOWNA);

	//Activate and display the dialog frame
	::ShowWindow(dialogWindowFrame, SW_RESTORE);
	SetActiveWindow(dialogWindowFrame);
	SetForegroundWindow(dialogWindowFrame);

	//Update the recorded the title of this window
	std::map<HWND, OpenWindowInfo>::iterator windowInfoSetIterator = windowInfoSet.find(windowHandle);
	if(windowInfoSetIterator != windowInfoSet.end())
	{
		windowInfoSetIterator->second.windowTitle = qualifiedWindowTitle;
	}

	//Return the result to the caller
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewManager::ShowDockingWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle)
{
	//Build a qualified title for this window
	std::wstring qualifiedWindowTitle = BuildQualifiedWindowTitle(view, viewPresenter, windowHandle, windowTitle);

	//If a docking group has been specified for the window, attempt to locate a docking
	//window associated with the target docking group.
	HWND dockingWindow = NULL;
	std::wstring targetDockingGroup = view.GetViewDockingGroup();
	if(!targetDockingGroup.empty())
	{
		//Build a count of how many docking windows with the same associated docking group
		//appear within each parent docking window
		std::map<HWND, unsigned int> dockedWindowCountByParentDockingWindow;
		for(std::map<HWND, OpenWindowInfo>::const_iterator i = windowInfoSet.begin(); i != windowInfoSet.end(); ++i)
		{
			const OpenWindowInfo& windowInfo = i->second;
			std::wstring windowDockingGroup = windowInfo.view.GetViewDockingGroup();
			if(windowDockingGroup == targetDockingGroup)
			{
				HWND parentDockingWindow = GetParentDockingWindow(windowInfo.hwnd);
				if(parentDockingWindow != 0)
				{
					++dockedWindowCountByParentDockingWindow[parentDockingWindow];
				}
			}
		}

		//Use the docking window with the highest number of docked windows of the same
		//docking group as our parent docking window
		unsigned int highestMemberCount = 0;
		for(std::map<HWND, unsigned int>::const_iterator i = dockedWindowCountByParentDockingWindow.begin(); i != dockedWindowCountByParentDockingWindow.end(); ++i)
		{
			if(i->second > highestMemberCount)
			{
				dockingWindow = i->first;
				highestMemberCount = i->second;
			}
		}
	}

	//If no docking window was found for a docking group, attempt to locate an existing
	//docking window for the initial docking location.
	bool showWindow = false;
	bool activateWindow = false;
	bool viewInitiallyDocked = view.IsViewInitiallyDocked();
	IView::DockPos initialDockPos = view.GetViewInitialDockPosition();
	if((dockingWindow == NULL) && viewInitiallyDocked)
	{
		if(initialDockPos == IView::DockPos::Center)
		{
			//If we're docking to the center, use the main docking window directly.
			dockingWindow = mainDockingWindow;
			showWindow = true;
			activateWindow = true;
		}
		else
		{
			//If we're docking to any target other than the center, search the list of
			//currently docked child windows in the main docking window for the first
			//window that's docked at the target location.
			WC_DockPanel::DockLocation initialDockPosAsDockPanelDockLocation = ViewDockLocationToDockPanelDockLocation(initialDockPos);
			unsigned int currentDockedWindowNo = 0;
			unsigned int dockedWindowCount = (unsigned int)SendMessage(mainDockingWindow, (UINT)DockingWindow::WindowMessages::GetDockedWindowCount, 0, 0);
			while((dockingWindow == NULL) && (currentDockedWindowNo < dockedWindowCount))
			{
				DockingWindow::GetDockedWindowInfo dockedWindowInfo;
				if(SendMessage(mainDockingWindow, (UINT)DockingWindow::WindowMessages::GetDockedWindowInfo, (WPARAM)currentDockedWindowNo, (LPARAM)&dockedWindowInfo) == 0)
				{
					if(dockedWindowInfo.dockLocation == initialDockPosAsDockPanelDockLocation)
					{
						dockingWindow = dockedWindowInfo.hwnd;
						continue;
					}
				}
				++currentDockedWindowNo;
			}
		}
	}

	//If our view is being created in a floating window, or if the target docking location
	//doesn't currently have a docking panel present, create a new docking window for the
	//target view.
	bool viewInitiallyCollapsed = view.IsViewInitiallyCollapsed();
	if(dockingWindow == NULL)
	{
		//Obtain the current width and height of the view
		RECT windowRect;
		GetWindowRect(windowHandle, &windowRect);
		int viewWidth = windowRect.right - windowRect.left;
		int viewHeight = windowRect.bottom - windowRect.top;

		//Ensure the docking window class is registered
		DockingWindow::RegisterWindowClass(viewManagerAssemblyHandle);

		//Create a docking window to host this content window
		DWORD dockingWindowStyle = WS_SIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION;
		DWORD dockingWindowExtendedStyle = WS_EX_TOOLWINDOW;
		dockingWindow = CreateWindowEx(dockingWindowExtendedStyle, DockingWindow::windowClassName, L"", dockingWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, mainWindow, NULL, viewManagerAssemblyHandle, NULL);

		//Set the font for our new docking window based on the current font for the
		//main docking window
		HFONT dockingWindowFont = (HFONT)SendMessage(mainDockingWindow, WM_GETFONT, 0, 0);
		SendMessage(dockingWindow, WM_SETFONT, (WPARAM)dockingWindowFont, (LPARAM)TRUE);

		//Calculate the total window size to use in order to achieve a client region
		//within the docking window of the specified size
		unsigned int scaledWindowWidth = DPIScaleWidth(viewWidth);
		unsigned int scaledWindowHeight = DPIScaleHeight(viewHeight);
		RECT clientRect;
		clientRect.left = 0;
		clientRect.top = 0;
		clientRect.right = scaledWindowWidth;
		clientRect.bottom = scaledWindowHeight;
		AdjustWindowRectEx(&clientRect, dockingWindowStyle, FALSE, dockingWindowExtendedStyle);
		int adjustedWindowWidth = clientRect.right - clientRect.left;
		int adjustedWindowHeight = clientRect.bottom - clientRect.top;

		//Resize the docking window
		SetWindowPos(dockingWindow, NULL, 0, 0, adjustedWindowWidth, adjustedWindowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

		//Either dock or position this new docking window as required
		if(viewInitiallyDocked)
		{
			//Determine the initial docking location for this new docking window
			WC_DockPanel::DockLocation dockPanelDockPos = ViewDockLocationToDockPanelDockLocation(initialDockPos);

			//Add the new docking window to the main docking window
			DockingWindow::AddDockedWindowParams addDockedWindowParams;
			addDockedWindowParams.hwnd = dockingWindow;
			addDockedWindowParams.dockLocation = dockPanelDockPos;
			addDockedWindowParams.forceToTopOfDockingOrder = false;
			addDockedWindowParams.autoHide = viewInitiallyCollapsed;
			SendMessage(mainDockingWindow, (UINT)DockingWindow::WindowMessages::AddDockedWindow, 0, (LPARAM)&addDockedWindowParams);
		}
		else
		{
			//Calculate the size of any hidden borders attached to the target window
			int borderLeft;
			int borderRight;
			int borderTop;
			int borderBottom;
			GetHiddenBorderDimensions(dockingWindow, borderLeft, borderRight, borderTop, borderBottom);

			//Calculate the screen position to display the window at
			int totalWindowWidth = adjustedWindowWidth + (borderLeft + borderRight);
			int totalWindowHeight = adjustedWindowHeight + (borderTop + borderBottom);
			int windowPosX;
			int windowPosY;
			if(!FindBestWindowPosition(totalWindowWidth, totalWindowHeight, windowPosX, windowPosY))
			{
				//Calculate the position of the client area of the main window
				POINT clientPos;
				clientPos.x = 0;
				clientPos.y = 0;
				ClientToScreen(mainWindow, &clientPos);
				int mainWindowPosX = clientPos.x;
				int mainWindowPosY = clientPos.y;

				//Fall back to placing the new window in an arbitrary default position
				windowPosX = mainWindowPosX + defaultWindowPosX;
				windowPosY = mainWindowPosY + defaultWindowPosY;
				defaultWindowPosX = (defaultWindowPosX + defaultWindowPosIncrement) % (defaultWindowPosIncrement * defaultWindowPosStackDepth);
				defaultWindowPosY = (defaultWindowPosY + defaultWindowPosIncrement) % (defaultWindowPosIncrement * defaultWindowPosStackDepth);
			}

			//Position the floating docking window
			SetWindowPos(dockingWindow, NULL, windowPosX, windowPosY, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOREPOSITION | SWP_NOSIZE);
		}

		//Show the docking window if required
		if(!viewInitiallyCollapsed)
		{
			//Show the docking window
			SetWindowPos(dockingWindow, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

			//Flag that we should activate our window
			activateWindow = true;
		}

		//Flag that we need to show our window too, regardless of whether the newly
		//created docking window is collapsed or not. We need to do this, because a
		//docking window with only one content item will not attempt to show the view
		//again if it is currently hidden. If more than one content item is present in the
		//docking window, we don't need to do this, because the docking window will only
		//be shown by activating a tab item, which will show the associated content
		//window.
		showWindow = true;
	}

	//Add the view content window to the target docking window
	DockingWindow::AddContentWindowParams addContentWindowParams;
	addContentWindowParams.hwnd = windowHandle;
	addContentWindowParams.windowTitle = qualifiedWindowTitle.c_str();
	SendMessage(dockingWindow, (UINT)DockingWindow::WindowMessages::AddContentWindow, 0, (LPARAM)&addContentWindowParams);

	//Show and activate the window if requested
	if(showWindow)
	{
		//Show the window
		::ShowWindow(windowHandle, SW_SHOWNA);

		//Activate the window if requested
		if(activateWindow)
		{
			SetActiveWindow(windowHandle);
			SetForegroundWindow(windowHandle);
		}
	}

	//Update the recorded the title of this window
	std::map<HWND, OpenWindowInfo>::iterator windowInfoSetIterator = windowInfoSet.find(windowHandle);
	if(windowInfoSetIterator != windowInfoSet.end())
	{
		windowInfoSetIterator->second.windowTitle = qualifiedWindowTitle;
	}

	//Return the result to the caller
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewManager::ShowDocumentWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle)
{
	//Note that currently, dockable and document windows are treated exactly the same way,
	//but a distinction between the two types may be important later on.
	return ShowDockingWindowFirstTime(view, viewPresenter, windowHandle, windowTitle);
}

//----------------------------------------------------------------------------------------
void ViewManager::CloseWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle)
{
	IView::ViewType viewType = view.GetViewType();
	switch(viewType)
	{
	case IView::ViewType::Dialog:{
		//Attempt to close the window and its parent dialog frame. Note that in order to
		//activate the correct window in response to closing a modal dialog, we need to
		//re-enable the parent window before destroying the dialog, so we need the close
		//handler to run for the dialog frame at this point rather than directly calling
		//the DestroyWindow function.
		HWND dialogFrame = GetParentDialogWindowFrame(windowHandle);
		if(dialogFrame != NULL)
		{
			SendMessage(dialogFrame, WM_CLOSE, 0, 0);
		}
		break;}
	case IView::ViewType::Dockable:
	case IView::ViewType::Document:{
		//Remove this window from any current docking parent
		//##FIX## We should go through the normal window close message system here, giving
		//the view a chance to cancel the close request, and we should return a boolean
		//value indicating the result of the close attempt.
		HWND parentDockingWindow = GetParentDockingWindow(windowHandle);
		if(parentDockingWindow != NULL)
		{
			//Remove our window from the parent docking window
			SendMessage(parentDockingWindow, (UINT)DockingWindow::WindowMessages::RemoveContentWindow, 0, (LPARAM)windowHandle);

			//Now that we've removed our window from its parent docking window,
			//recursively close any parent docking windows that contained it until we find
			//one with remaining content, or we reach the main docking window.
			HWND searchParentDockingWindow = parentDockingWindow;
			while((searchParentDockingWindow != NULL) && (searchParentDockingWindow != mainDockingWindow))
			{
				unsigned int dockedWindowCount = (unsigned int)SendMessage(searchParentDockingWindow, (UINT)DockingWindow::WindowMessages::GetDockedWindowCount, 0, 0);
				unsigned int contentWindowCount = (unsigned int)SendMessage(searchParentDockingWindow, (UINT)DockingWindow::WindowMessages::GetContentWindowCount, 0, 0);
				if((dockedWindowCount == 0) && (contentWindowCount == 0))
				{
					HWND nextParentDockingWindow = GetParentDockingWindow(searchParentDockingWindow);
					DestroyWindow(searchParentDockingWindow);
					searchParentDockingWindow = nextParentDockingWindow;
				}
				else
				{
					searchParentDockingWindow = NULL;
				}
			}
		}

		//Destroy the window
		DestroyWindow(windowHandle);
		break;}
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::ShowWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle)
{
	//Show the window
	//##TODO## Restore the previous docking location for this window, if present.
	::ShowWindow(windowHandle, SW_SHOWNA);
	UpdateWindow(windowHandle);
}

//----------------------------------------------------------------------------------------
void ViewManager::HideWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle)
{
	//Hide the window
	//##TODO## Remove the target window from any current docking parents, and remember the
	//docking location for when it is re-shown.
	::ShowWindow(windowHandle, SW_HIDE);
}

//----------------------------------------------------------------------------------------
void ViewManager::ActivateWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle)
{
	//Activate the window
	//##FIX## If this window is docked within a parent docking container, set it as the
	//active content item in its parent, and recursively so for the parent of its parent
	//back up to the top level docking container, and implement a way to temporarily
	//expand any auto-hide docking containers for a few seconds on a timer, so the user is
	//shown the window and has a chance to move the mouse over it to keep it visible after
	//the timer expires.
	::ShowWindow(windowHandle, SW_SHOW);
	SetActiveWindow(windowHandle);
	SetForegroundWindow(windowHandle);
}

//----------------------------------------------------------------------------------------
void ViewManager::NotifyWindowDestroyed(IView& view, IViewPresenter& viewPresenter, HWND windowHandle)
{
	//This method should be invoked for any window which we created and returned by the
	//CreateDialogWindow or CreateNativeWindow methods, and should be called when the
	//WM_DESTROY message is received for these windows. We free any state information we
	//were holding about the target window here.
	windowInfoSet.erase(windowHandle);
}

//----------------------------------------------------------------------------------------
HWND ViewManager::GetParentDockingWindow(HWND windowHandle) const
{
	//Return the first parent docking window of the target window, or NULL if no parent
	//docking window can be found.
	HWND searchWindow = GetAncestor(windowHandle, GA_PARENT);
	while(searchWindow != NULL)
	{
		if(IsDockingWindow(searchWindow))
		{
			return searchWindow;
		}
		searchWindow = GetAncestor(searchWindow, GA_PARENT);
	}
	return NULL;
}

//----------------------------------------------------------------------------------------
HWND ViewManager::GetParentDialogWindowFrame(HWND windowHandle) const
{
	//Return the parent dialog window frame of the target window as the dialog window
	//frame, or NULL if no dialog window frame can be found.
	HWND dialogWindowFrame = GetAncestor(windowHandle, GA_PARENT);
	if(IsDialogFrame(dialogWindowFrame))
	{
		return dialogWindowFrame;
	}
	return NULL;
}

//----------------------------------------------------------------------------------------
bool ViewManager::IsDockingWindow(HWND windowHandle)
{
	//Retrieve the class name of the target window
	size_t windowClassNameLength = wcslen(DockingWindow::windowClassName);
	size_t classNameBufferSize = windowClassNameLength + 2;
	std::vector<wchar_t> classNameBuffer(classNameBufferSize);
	int getClassNameReturn = GetClassName(windowHandle, &classNameBuffer[0], (int)classNameBufferSize);
	if(getClassNameReturn == 0)
	{
		return false;
	}

	//Ensure the class name of the target window matches our window class name
	int stringCompareReturn = wcscmp(DockingWindow::windowClassName, &classNameBuffer[0]);
	if(stringCompareReturn != 0)
	{
		return false;
	}

	//Return true, since we've confirmed the target window is in fact a docking window.
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewManager::IsDialogFrame(HWND windowHandle)
{
	//Retrieve the class name of the target window
	size_t windowClassNameLength = wcslen(dialogFrameWindowClassName);
	size_t classNameBufferSize = windowClassNameLength + 2;
	std::vector<wchar_t> classNameBuffer(classNameBufferSize);
	int getClassNameReturn = GetClassName(windowHandle, &classNameBuffer[0], (int)classNameBufferSize);
	if(getClassNameReturn == 0)
	{
		return false;
	}

	//Ensure the class name of the target window matches our window class name
	int stringCompareReturn = wcscmp(dialogFrameWindowClassName, &classNameBuffer[0]);
	if(stringCompareReturn != 0)
	{
		return false;
	}

	//Return true, since we've confirmed the target window is in fact a docking window.
	return true;
}

//----------------------------------------------------------------------------------------
//Window state functions
//----------------------------------------------------------------------------------------
bool ViewManager::LoadWindowState(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, IHierarchicalStorageNode& windowState)
{
	return LoadWindowState(view, viewPresenter, windowHandle, windowState, false);
}

//----------------------------------------------------------------------------------------
bool ViewManager::LoadWindowState(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, IHierarchicalStorageNode& windowState, bool showingForFirstTime, const std::wstring& initialWindowTitle)
{
	//Retrieve information on the target window
	std::map<HWND, OpenWindowInfo>::iterator windowInfoIterator = windowInfoSet.find(windowHandle);
	if(windowInfoIterator == windowInfoSet.end())
	{
		return false;
	}
	OpenWindowInfo& windowInfo = windowInfoIterator->second;

	//Attempt to load the state for this window
	IHierarchicalStorageAttribute* windowIDAttribute = windowState.GetAttribute(L"WindowID");
	if(windowIDAttribute != 0)
	{
		//Attempt to retrieve information on the associated placeholder window
		unsigned int windowID = windowIDAttribute->ExtractHexValue<unsigned int>();
		std::map<unsigned int, PlaceholderWindowInfo>::iterator placeholderWindowsForViewLayoutIterator = placeholderWindowsForViewLayout.find(windowID);
		if(placeholderWindowsForViewLayoutIterator == placeholderWindowsForViewLayout.end())
		{
			return false;
		}

		//Ensure the view type of the target window matches the recorded view type for the
		//associated placeholder window.
		const PlaceholderWindowInfo& placeholderWindowInfo = placeholderWindowsForViewLayoutIterator->second;
		IView::ViewType viewType = view.GetViewType();
		if(placeholderWindowInfo.viewType != viewType)
		{
			return false;
		}

		//Determine what title to use for the target window
		std::wstring windowTitle;
		if(!showingForFirstTime)
		{
			//Retrieve the current title for the window
			windowTitle = windowInfo.windowTitle;
		}
		else
		{
			//Build a qualified title for this window
			windowTitle = BuildQualifiedWindowTitle(view, viewPresenter, windowHandle, initialWindowTitle);
		}

		//If the target window is already visible, remove it from any current parent
		//docking window.
		if(!showingForFirstTime && ((viewType == IView::ViewType::Dockable) || (viewType == IView::ViewType::Document)))
		{
			//Temporarily hide the target window
			::ShowWindow(windowHandle, SW_HIDE);

			//Remove the window from whatever docking parent it's currently bound to
			HWND parentDockingWindow = parentDockingWindow = GetParentDockingWindow(windowHandle);
			if(parentDockingWindow != NULL)
			{
				SendMessage(parentDockingWindow, (UINT)DockingWindow::WindowMessages::RemoveContentWindow, 0, (LPARAM)windowHandle);
			}
		}

		//Add the target window to its appropriate parent window based on the view type
		if(viewType == IView::ViewType::Dialog)
		{
			//Make the specified window a child of the dialog frame
			SetWindowParent(windowHandle, placeholderWindowInfo.parentWindowFrame);

			//Update the title of the dialog window frame
			SetWindowText(placeholderWindowInfo.parentWindowFrame, windowTitle.c_str());

			//Make the target window visible
			::ShowWindow(windowHandle, SW_SHOWNA);
			UpdateWindow(windowHandle);

			//Make the dialog frame visible
			::ShowWindow(placeholderWindowInfo.parentWindowFrame, SW_SHOWNA);
			UpdateWindow(placeholderWindowInfo.parentWindowFrame);
		}
		else if((viewType == IView::ViewType::Dockable) || (viewType == IView::ViewType::Document))
		{
			//Retrieve the content window index of the placeholder window
			unsigned int placeholderContentWindowIndex = (unsigned int)SendMessage(placeholderWindowInfo.parentWindowFrame, (UINT)DockingWindow::WindowMessages::GetContentWindowIndexFromHandle, 0, (LPARAM)placeholderWindowInfo.placeholderContentWindow);

			//Replace the placeholder window with our actual window
			DockingWindow::ModifyContentWindowParams modifyContentWindowParams;
			modifyContentWindowParams.contentWindowInfo = DockingWindow::ContentWindowInfo(DockingWindow::ContentWindowInfo::Handle | DockingWindow::ContentWindowInfo::Title);
			modifyContentWindowParams.hwnd = windowHandle;
			modifyContentWindowParams.windowTitle = windowTitle.c_str();
			SendMessage(placeholderWindowInfo.parentWindowFrame, (UINT)DockingWindow::WindowMessages::ModifyContentWindow, (WPARAM)placeholderContentWindowIndex, (LPARAM)&modifyContentWindowParams);

			//If the window needs to be set as the currently selected window in its parent
			//docking window, set it as the active selection now.
			if(placeholderWindowInfo.selectedContentWindow)
			{
				SendMessage(placeholderWindowInfo.parentWindowFrame, (UINT)DockingWindow::WindowMessages::SetActiveContentWindow, (WPARAM)placeholderContentWindowIndex, 0);
			}

			//If the window is initially visible, show it.
			if(placeholderWindowInfo.makeContentVisible)
			{
				unsigned int activeContentNo = (unsigned int)SendMessage(placeholderWindowInfo.parentWindowFrame, (UINT)DockingWindow::WindowMessages::GetActiveContentWindow, 0, 0);
				if(placeholderContentWindowIndex == activeContentNo)
				{
					::ShowWindow(windowHandle, SW_SHOWNA);
					UpdateWindow(windowHandle);
				}
			}

			//Destroy the placeholder window
			DestroyWindow(placeholderWindowInfo.placeholderContentWindow);
		}

		//Remove this placeholder window from the list of placeholder windows
		placeholderWindowsForViewLayout.erase(placeholderWindowsForViewLayoutIterator);

		//Update the recorded the title of this window if required
		if(showingForFirstTime)
		{
			windowInfo.windowTitle = windowTitle;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool ViewManager::SaveWindowState(const IView& view, const IViewPresenter& viewPresenter, HWND windowHandle, IHierarchicalStorageNode& windowState) const
{
	//Save the window ID associated with this window
	std::map<HWND, unsigned int>::const_iterator windowHandleToIDIterator = windowHandleToIDForViewLayout.find(windowHandle);
	if(windowHandleToIDIterator != windowHandleToIDForViewLayout.end())
	{
		windowState.CreateAttribute(L"WindowID", windowHandleToIDIterator->second);
	}
	return true;
}

//----------------------------------------------------------------------------------------
//Window size functions
//----------------------------------------------------------------------------------------
void ViewManager::ResizeWindowToTargetClientSize(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, unsigned int windowClientWidth, unsigned int windowClientHeight)
{
	//Retrieve the window style and extended window style for the created window
	DWORD windowStyleFinal = (DWORD)GetWindowLongPtr(windowHandle, GWL_STYLE);
	DWORD extendedWindowStyleFinal = (DWORD)GetWindowLongPtr(windowHandle, GWL_EXSTYLE);

	//Calculate the total window size to use in order to achieve a client region of the
	//specified size
	unsigned int scaledWindowWidth = DPIScaleWidth(windowClientWidth);
	unsigned int scaledWindowHeight = DPIScaleHeight(windowClientHeight);
	RECT clientRect;
	clientRect.left = 0;
	clientRect.top = 0;
	clientRect.right = scaledWindowWidth;
	clientRect.bottom = scaledWindowHeight;
	AdjustWindowRectEx(&clientRect, windowStyleFinal, FALSE, extendedWindowStyleFinal);
	int adjustedWindowWidth = clientRect.right - clientRect.left;
	int adjustedWindowHeight = clientRect.bottom - clientRect.top;

	//Resize the window
	UINT flags = SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOMOVE;
	SetWindowPos(windowHandle, NULL, 0, 0, adjustedWindowWidth, adjustedWindowHeight, flags);
}

//----------------------------------------------------------------------------------------
//Window title functions
//----------------------------------------------------------------------------------------
void ViewManager::UpdateWindowTitle(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const MarshalSupport::Marshal::In<std::wstring>& windowTitle)
{
	//Build a qualified title for this window
	std::wstring qualifiedWindowTitle = BuildQualifiedWindowTitle(view, viewPresenter, windowHandle, windowTitle);

	//Update the visible title of the target window according to its view type
	IView::ViewType viewType = view.GetViewType();
	if(viewType == IView::ViewType::Dialog)
	{
		//Update the text of the dialog window frame
		HWND dialogFrame = GetParentDialogWindowFrame(windowHandle);
		if(dialogFrame != NULL)
		{
			SetWindowText(dialogFrame, windowTitle.Get().c_str());
		}
	}
	else if((viewType == IView::ViewType::Dockable) || (viewType == IView::ViewType::Document))
	{
		//Update the current title for the window in its parent docking window
		HWND parentDockingWindow = GetParentDockingWindow(windowHandle);
		if(parentDockingWindow != NULL)
		{
			LRESULT getContentWindowIndexFromHandleReturn = SendMessage(parentDockingWindow, (UINT)DockingWindow::WindowMessages::GetContentWindowIndexFromHandle, 0, (LPARAM)windowHandle);
			if(getContentWindowIndexFromHandleReturn != -1)
			{
				unsigned int contentNo = (unsigned int)getContentWindowIndexFromHandleReturn;
				DockingWindow::ModifyContentWindowParams modifyContentWindowParams;
				modifyContentWindowParams.contentWindowInfo = DockingWindow::ContentWindowInfo::Title;
				modifyContentWindowParams.windowTitle = qualifiedWindowTitle.c_str();
				SendMessage(parentDockingWindow, (UINT)DockingWindow::WindowMessages::ModifyContentWindow, (WPARAM)contentNo, (LPARAM)&modifyContentWindowParams);
			}
		}
	}

	//Update the recorded the title of this window
	std::map<HWND, OpenWindowInfo>::iterator windowInfoSetIterator = windowInfoSet.find(windowHandle);
	if(windowInfoSetIterator != windowInfoSet.end())
	{
		windowInfoSetIterator->second.windowTitle = qualifiedWindowTitle;
	}
}

//----------------------------------------------------------------------------------------
std::wstring ViewManager::BuildQualifiedWindowTitle(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle) const
{
	std::wstring qualifiedWindowTitle;
	switch(viewPresenter.GetViewTarget())
	{
	default:
	case IViewPresenter::ViewTarget::System:
		qualifiedWindowTitle = windowTitle;
		break;

	case IViewPresenter::ViewTarget::Module:{
		std::wstring moduleDisplayName;
		system.GetModuleDisplayName(viewPresenter.GetViewTargetModuleID(), moduleDisplayName);
		qualifiedWindowTitle = moduleDisplayName + L" - " + windowTitle;
		break;}

	case IViewPresenter::ViewTarget::Device:{
		//Retrieve the target device instance name and module ID
		unsigned int deviceModuleID = viewPresenter.GetViewTargetModuleID();
		std::wstring deviceInstanceName = viewPresenter.GetViewTargetDeviceInstanceName();

		//Determine if the device instance name for the associated device is unique across
		//all modules in the system
		bool deviceInstanceNameUnique = true;
		std::list<IDevice*> loadedDeviceList = system.GetLoadedDevices();
		std::list<IDevice*>::const_iterator loadedDeviceListIterator = loadedDeviceList.begin();
		while(loadedDeviceListIterator != loadedDeviceList.end())
		{
			IDevice* device = *loadedDeviceListIterator;
			if((device->GetDeviceInstanceName() == deviceInstanceName) && (device->GetDeviceModuleID() != deviceModuleID))
			{
				deviceInstanceNameUnique = false;
				break;
			}
			++loadedDeviceListIterator;
		}

		//If the device instance name is unique in the system across all modules in the
		//system, qualify the window title with the device instance name only, otherwise
		//include the module display name in the title as well.
		if(deviceInstanceNameUnique)
		{
			qualifiedWindowTitle = deviceInstanceName + L" - " + windowTitle;
		}
		else
		{
			std::wstring moduleDisplayName;
			system.GetModuleDisplayName(deviceModuleID, moduleDisplayName);
			qualifiedWindowTitle = moduleDisplayName + L" - " + deviceInstanceName + L" - " + windowTitle;
		}
		break;}

	case IViewPresenter::ViewTarget::Extension:
		//Retrieve the target extension instance name and module ID
		unsigned int extensionModuleID = viewPresenter.GetViewTargetModuleID();
		std::wstring extensionInstanceName = viewPresenter.GetViewTargetExtensionInstanceName();

		//Determine if the extension instance name for the associated extension is unique
		//across all modules in the system
		bool extensionInstanceNameUnique = true;
		if(!viewPresenter.GetViewTargetGlobalExtension())
		{
			std::list<IExtension*> loadedExtensionList = system.GetLoadedExtensions();
			std::list<IExtension*>::const_iterator loadedExtensionListIterator = loadedExtensionList.begin();
			while(loadedExtensionListIterator != loadedExtensionList.end())
			{
				IExtension* extension = *loadedExtensionListIterator;
				if((extension->GetExtensionInstanceName() == extensionInstanceName) && (extension->GetExtensionModuleID() != extensionModuleID))
				{
					extensionInstanceNameUnique = false;
					break;
				}
				++loadedExtensionListIterator;
			}
		}

		//If the extension instance name is unique in the system across all modules in the
		//system, qualify the window title with the extension instance name only,
		//otherwise include the module display name in the title as well.
		if(extensionInstanceNameUnique)
		{
			qualifiedWindowTitle = extensionInstanceName + L" - " + windowTitle;
		}
		else
		{
			std::wstring moduleDisplayName;
			system.GetModuleDisplayName(extensionModuleID, moduleDisplayName);
			qualifiedWindowTitle = moduleDisplayName + L" - " + extensionInstanceName + L" - " + windowTitle;
		}
		break;
	}
	return qualifiedWindowTitle;
}

//----------------------------------------------------------------------------------------
//View closing helper functions
//----------------------------------------------------------------------------------------
void ViewManager::CloseViewsForSystem()
{
	//Build a set of all views to close
	std::unique_lock<std::mutex> lock(viewMutex);
	std::set<IViewPresenter*> viewsToClose;
	for(std::map<IViewPresenter*, ViewInfo*>::const_iterator i = viewInfoSet.begin(); i != viewInfoSet.end(); ++i)
	{
		ViewInfo& viewInfo = *i->second;
		if(viewInfo.viewCurrentlyOpen && (viewInfo.viewPresenter.GetViewTarget() == IViewPresenter::ViewTarget::System))
		{
			viewsToClose.insert(&viewInfo.viewPresenter);
		}
	}
	lock.unlock();

	//Close all selected views
	for(std::set<IViewPresenter*>::const_iterator i = viewsToClose.begin(); i != viewsToClose.end(); ++i)
	{
		CloseView(*(*i), true);
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::CloseViewsForModule(unsigned int moduleID)
{
	//Build a set of all views to close
	std::unique_lock<std::mutex> lock(viewMutex);
	std::set<IViewPresenter*> viewsToClose;
	for(std::map<IViewPresenter*, ViewInfo*>::const_iterator i = viewInfoSet.begin(); i != viewInfoSet.end(); ++i)
	{
		ViewInfo& viewInfo = *i->second;
		if(viewInfo.viewCurrentlyOpen && (viewInfo.viewPresenter.GetViewTarget() == IViewPresenter::ViewTarget::Module) && (viewInfo.viewPresenter.GetViewTargetModuleID() == moduleID))
		{
			viewsToClose.insert(&viewInfo.viewPresenter);
		}
	}
	lock.unlock();

	//Close all selected views
	for(std::set<IViewPresenter*>::const_iterator i = viewsToClose.begin(); i != viewsToClose.end(); ++i)
	{
		CloseView(*(*i), true);
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::CloseViewsForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName)
{
	//Build a set of all views to close
	std::unique_lock<std::mutex> lock(viewMutex);
	std::set<IViewPresenter*> viewsToClose;
	for(std::map<IViewPresenter*, ViewInfo*>::const_iterator i = viewInfoSet.begin(); i != viewInfoSet.end(); ++i)
	{
		ViewInfo& viewInfo = *i->second;
		if(viewInfo.viewCurrentlyOpen && (viewInfo.viewPresenter.GetViewTarget() == IViewPresenter::ViewTarget::Device) && (viewInfo.viewPresenter.GetViewTargetModuleID() == moduleID) && (viewInfo.viewPresenter.GetViewTargetDeviceInstanceName() == deviceInstanceName))
		{
			viewsToClose.insert(&viewInfo.viewPresenter);
		}
	}
	lock.unlock();

	//Close all selected views
	for(std::set<IViewPresenter*>::const_iterator i = viewsToClose.begin(); i != viewsToClose.end(); ++i)
	{
		CloseView(*(*i), true);
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::CloseAllViews()
{
	//Build a set of all views to close
	std::unique_lock<std::mutex> lock(viewMutex);
	std::set<IViewPresenter*> viewsToClose;
	for(std::map<IViewPresenter*, ViewInfo*>::const_iterator i = viewInfoSet.begin(); i != viewInfoSet.end(); ++i)
	{
		ViewInfo& viewInfo = *i->second;
		if(viewInfo.viewCurrentlyOpen)
		{
			viewsToClose.insert(&viewInfo.viewPresenter);
		}
	}
	lock.unlock();

	//Close all selected views
	for(std::set<IViewPresenter*>::const_iterator i = viewsToClose.begin(); i != viewsToClose.end(); ++i)
	{
		CloseView(*(*i), true);
	}
}

//----------------------------------------------------------------------------------------
//Floating window methods
//----------------------------------------------------------------------------------------
void ViewManager::AdjustFloatingWindowPositions(int displacementX, int displacementY)
{
	//Adjust the position of each floating window by the specified displacement
	std::list<HWND> floatingWindows = GetOpenFloatingWindows();
	for(std::list<HWND>::const_iterator i = floatingWindows.begin(); i != floatingWindows.end(); ++i)
	{
		//Retrieve the current position of the target floating window
		HWND floatingWindow = *i;
		RECT windowRect;
		GetWindowRect(floatingWindow, &windowRect);
		int windowPosX = windowRect.left;
		int windowPosY = windowRect.top;

		//Apply the specified displacement to the floating window position
		windowPosX += displacementX;
		windowPosY += displacementY;
		SetWindowPos(floatingWindow, NULL, windowPosX, windowPosY, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOREPOSITION | SWP_NOSIZE);
	}
}

//----------------------------------------------------------------------------------------
//Layout functions
//----------------------------------------------------------------------------------------
bool ViewManager::LoadViewLayout(IHierarchicalStorageNode& viewLayout, const ISystemGUIInterface::ModuleRelationshipMap& relationshipMap)
{
	//Ensure the current list of placeholder windows is clear
	placeholderWindowsForViewLayout.clear();

	//Build a set of all currently open views
	std::unique_lock<std::mutex> lock(viewMutex);
	std::set<IViewPresenter*> existingViewSet;
	for(std::map<IViewPresenter*, ViewInfo*>::const_iterator i = viewInfoSet.begin(); i != viewInfoSet.end(); ++i)
	{
		//Skip this view if it isn't currently open
		ViewInfo* viewInfo = i->second;
		if(!viewInfo->viewCurrentlyOpen)
		{
			continue;
		}

		//Add this open view to the set of open views before our layout restore operation
		existingViewSet.insert(i->first);
	}
	lock.unlock();

	//Build a list of all currently open docking which need to be closed after the layout
	//load operation is complete. Since we don't attempt to reuse docking or dialog
	//windows currently, we need to close all floating windows, and all docking windows
	//contained within the main docking window.
	std::list<HWND> existingDockingWindowsToClose;
	InvokeOnUIThread(std::bind(std::mem_fn(&ViewManager::BuildExistingWindowsToCloseList), this, std::ref(existingDockingWindowsToClose)));

	//Restore any windows attached to the main docking window. Note that we have to
	//perform this operation on the UI thread, since we need to re-create actual windows
	//as part of this.
	IHierarchicalStorageNode* mainWindowStateNode = viewLayout.GetChild(L"MainWindowState");
	if(mainWindowStateNode != 0)
	{
		InvokeOnUIThread(std::bind(std::mem_fn(&ViewManager::LoadMainWindowStateFromViewLayout), this, std::ref(*mainWindowStateNode), std::ref(placeholderWindowsForViewLayout)));
	}

	//Iterate through each valid window entry in the file, and open matching windows with
	//the specified properties. Z-Order is based on the order the windows are listed in
	//the file.
	std::list<WorkspaceViewEntryDetails> loadedWorkspaceViewInfo;
	IHierarchicalStorageNode* viewPresenterStateNode = viewLayout.GetChild(L"ViewPresenterState");
	while(viewPresenterStateNode != 0)
	{
		IHierarchicalStorageAttribute* targetAttribute = viewPresenterStateNode->GetAttribute(L"Target");
		IHierarchicalStorageAttribute* viewGroupNameAttribute = viewPresenterStateNode->GetAttribute(L"ViewPresenterGroupName");
		IHierarchicalStorageAttribute* viewNameAttribute = viewPresenterStateNode->GetAttribute(L"ViewPresenterName");
		IHierarchicalStorageAttribute* moduleIDAttribute = viewPresenterStateNode->GetAttribute(L"ModuleID");
		if((targetAttribute != 0) && (viewGroupNameAttribute != 0) && (viewNameAttribute != 0))
		{
			//Extract the basic attributes which are defined for all entries
			std::wstring target = targetAttribute->GetValue();
			std::wstring viewGroupName = viewGroupNameAttribute->GetValue();
			std::wstring viewName = viewNameAttribute->GetValue();

			//Restore the view
			if(target == L"System")
			{
				if(system.RestoreViewStateForSystem(viewGroupName, viewName, *viewPresenterStateNode))
				{
					//If the view state was restored, save info about this view.
					loadedWorkspaceViewInfo.push_back(WorkspaceViewEntryDetails(viewGroupName, viewName, IViewPresenter::ViewTarget::System));
				}
			}
			else if(target == L"Module")
			{
				if(moduleIDAttribute != 0)
				{
					unsigned int savedModuleID = moduleIDAttribute->ExtractValue<unsigned int>();

					//Attempt to restore the view state
					ISystemGUIInterface::ModuleRelationshipMap::const_iterator relationshipMapIterator = relationshipMap.find(savedModuleID);
					if(relationshipMapIterator != relationshipMap.end())
					{
						const ISystemGUIInterface::ModuleRelationship& moduleRelationship = relationshipMapIterator->second;
						if(moduleRelationship.foundMatch)
						{
							//If we found a matching loaded module for the saved data,
							//attempt to restore the view state.
							if(system.RestoreViewStateForModule(viewGroupName, viewName, *viewPresenterStateNode, moduleRelationship.loadedModuleID))
							{
								//If the view state was restored, save info about this
								//view.
								loadedWorkspaceViewInfo.push_back(WorkspaceViewEntryDetails(viewGroupName, viewName, IViewPresenter::ViewTarget::Module, moduleRelationship.loadedModuleID));
							}
						}
					}
				}
			}
			else if(target == L"Device")
			{
				IHierarchicalStorageAttribute* deviceInstanceNameAttribute = viewPresenterStateNode->GetAttribute(L"DeviceInstanceName");
				if((deviceInstanceNameAttribute != 0) && (moduleIDAttribute != 0))
				{
					std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();
					unsigned int savedModuleID = moduleIDAttribute->ExtractValue<unsigned int>();

					//Attempt to restore the view state
					ISystemGUIInterface::ModuleRelationshipMap::const_iterator relationshipMapIterator = relationshipMap.find(savedModuleID);
					if(relationshipMapIterator != relationshipMap.end())
					{
						const ISystemGUIInterface::ModuleRelationship& moduleRelationship = relationshipMapIterator->second;
						if(moduleRelationship.foundMatch)
						{
							//If we found a matching loaded module for the saved data,
							//attempt to restore the view state.
							if(system.RestoreViewStateForDevice(viewGroupName, viewName, *viewPresenterStateNode, moduleRelationship.loadedModuleID, deviceInstanceName))
							{
								//If the view state was restored, save info about this
								//view.
								loadedWorkspaceViewInfo.push_back(WorkspaceViewEntryDetails(viewGroupName, viewName, IViewPresenter::ViewTarget::Device, moduleRelationship.loadedModuleID, deviceInstanceName));
							}
						}
					}
				}
			}
			else if(target == L"Extension")
			{
				IHierarchicalStorageAttribute* extensionInstanceNameAttribute = viewPresenterStateNode->GetAttribute(L"ExtensionInstanceName");
				IHierarchicalStorageAttribute* globalAttribute = viewPresenterStateNode->GetAttribute(L"Global");
				if((extensionInstanceNameAttribute != 0) && (globalAttribute != 0) && globalAttribute->ExtractValue<bool>())
				{
					//Attempt to restore the view state
					std::wstring extensionInstanceName = extensionInstanceNameAttribute->GetValue();
					if(system.RestoreViewStateForExtension(viewGroupName, viewName, *viewPresenterStateNode, extensionInstanceName))
					{
						//If the view state was restored, save info about this view.
						loadedWorkspaceViewInfo.push_back(WorkspaceViewEntryDetails(viewGroupName, viewName, IViewPresenter::ViewTarget::Extension, 0, extensionInstanceName, true));
					}
				}
				else if((extensionInstanceNameAttribute != 0) && (moduleIDAttribute != 0))
				{
					//Attempt to restore the view state
					std::wstring extensionInstanceName = extensionInstanceNameAttribute->GetValue();
					unsigned int savedModuleID = moduleIDAttribute->ExtractValue<unsigned int>();
					ISystemGUIInterface::ModuleRelationshipMap::const_iterator relationshipMapIterator = relationshipMap.find(savedModuleID);
					if(relationshipMapIterator != relationshipMap.end())
					{
						const ISystemGUIInterface::ModuleRelationship& moduleRelationship = relationshipMapIterator->second;
						if(moduleRelationship.foundMatch)
						{
							//If we found a matching loaded module for the saved data,
							//attempt to restore the view state.
							if(system.RestoreViewStateForExtension(viewGroupName, viewName, *viewPresenterStateNode, moduleRelationship.loadedModuleID, extensionInstanceName))
							{
								//If the view state was restored, save info about this
								//view.
								loadedWorkspaceViewInfo.push_back(WorkspaceViewEntryDetails(viewGroupName, viewName, IViewPresenter::ViewTarget::Extension, moduleRelationship.loadedModuleID, extensionInstanceName));
							}
						}
					}
				}
			}
		}

		//Advance to the next ViewPresenterState child node
		viewPresenterStateNode = viewLayout.GetChild(L"ViewPresenterState", viewPresenterStateNode);
	}

	//Close any views which were not referenced in the workspace file
	for(std::set<IViewPresenter*>::const_iterator existingViewSetIterator = existingViewSet.begin(); existingViewSetIterator != existingViewSet.end(); ++existingViewSetIterator)
	{
		IViewPresenter& existingView = *(*existingViewSetIterator);
		bool viewReferenced = false;
		std::list<WorkspaceViewEntryDetails>::const_iterator loadedViewDetails = loadedWorkspaceViewInfo.begin();
		while(!viewReferenced && (loadedViewDetails != loadedWorkspaceViewInfo.end()))
		{
			if((existingView.GetViewGroupName() == loadedViewDetails->viewGroupName) && (existingView.GetViewName() == loadedViewDetails->viewName))
			{
				switch(loadedViewDetails->viewTarget)
				{
				case IViewPresenter::ViewTarget::System:
					viewReferenced = true;
					break;
				case IViewPresenter::ViewTarget::Module:
					viewReferenced = (existingView.GetViewTargetModuleID() == loadedViewDetails->moduleID);
					break;
				case IViewPresenter::ViewTarget::Device:
					viewReferenced = (existingView.GetViewTargetModuleID() == loadedViewDetails->moduleID) && (existingView.GetViewTargetDeviceInstanceName() == loadedViewDetails->instanceName);
					break;
				case IViewPresenter::ViewTarget::Extension:
					viewReferenced = (existingView.GetViewTargetExtensionInstanceName() == loadedViewDetails->instanceName) && (existingView.GetViewTargetGlobalExtension() == loadedViewDetails->globalExtension) && (loadedViewDetails->globalExtension || (existingView.GetViewTargetModuleID() == loadedViewDetails->moduleID));
					break;
				}
			}
			++loadedViewDetails;
		}
		if(!viewReferenced)
		{
			CloseView(existingView, false);
		}
	}

	//Delete any created placeholder windows which were not used in this load operation.
	//Note that this can occur with incomplete or malformed workspace files.
	for(std::map<unsigned int, PlaceholderWindowInfo>::const_iterator i = placeholderWindowsForViewLayout.begin(); i != placeholderWindowsForViewLayout.end(); ++i)
	{
		const PlaceholderWindowInfo& placeholderWindowInfo = i->second;
		SendMessage(placeholderWindowInfo.parentWindowFrame, (UINT)DockingWindow::WindowMessages::RemoveContentWindow, 0, (LPARAM)placeholderWindowInfo.placeholderContentWindow);
		DestroyWindow(placeholderWindowInfo.placeholderContentWindow);
	}
	placeholderWindowsForViewLayout.clear();

	//Destroy any existing windows which are no longer being used
	InvokeOnUIThread(std::bind(std::mem_fn(&ViewManager::CloseExistingWindows), this, std::ref(existingDockingWindowsToClose)));

	return true;
}

//----------------------------------------------------------------------------------------
bool ViewManager::SaveViewLayout(IHierarchicalStorageNode& viewLayout) const
{
	//Build a set of all currently open views
	std::unique_lock<std::mutex> lock(viewMutex);
	std::set<IViewPresenter*> existingViewSet;
	for(std::map<IViewPresenter*, ViewInfo*>::const_iterator i = viewInfoSet.begin(); i != viewInfoSet.end(); ++i)
	{
		//Skip this view if it isn't currently open
		ViewInfo* viewInfo = i->second;
		if(!viewInfo->viewCurrentlyOpen)
		{
			continue;
		}

		//Add this open view to the set of open views before our layout restore operation
		existingViewSet.insert(i->first);
	}
	lock.unlock();

	//Determine if the main window is currently maximized
	bool isWindowMaximized = (IsZoomed(mainWindow) != 0);

	//Calculate the client size of the main window. Note that if the window is maximized,
	//we calculate the restored client size of the window instead of the maximized client
	//size, so that when the view layout is loaded, we preserve not only the maximize
	//state, but the restore window size as well.
	int mainWindowClientWidth;
	int mainWindowClientHeight;
	if(isWindowMaximized)
	{
		//Retrieve the window style and extended window style for the main window
		DWORD windowStyle = (DWORD)GetWindowLongPtr(mainWindow, GWL_STYLE);
		DWORD extendedWindowStyle = (DWORD)GetWindowLongPtr(mainWindow, GWL_EXSTYLE);

		//Calculate the total size of the window frame around the client area of the main
		//window
		RECT clientRect;
		clientRect.left = 0;
		clientRect.top = 0;
		clientRect.right = 0;
		clientRect.bottom = 0;
		AdjustWindowRectEx(&clientRect, windowStyle, TRUE, extendedWindowStyle);
		int mainWindowFrameWidth = clientRect.right - clientRect.left;
		int mainWindowFrameHeight = clientRect.bottom - clientRect.top;

		//Calculate the restored main window size
		WINDOWPLACEMENT windowPlacementInfo;
		windowPlacementInfo.length = sizeof(windowPlacementInfo);
		GetWindowPlacement(mainWindow, &windowPlacementInfo);
		int mainWindowRestoredWidth = (int)(windowPlacementInfo.rcNormalPosition.right - windowPlacementInfo.rcNormalPosition.left);
		int mainWindowRestoredHeight = (int)(windowPlacementInfo.rcNormalPosition.bottom - windowPlacementInfo.rcNormalPosition.top);

		//Calculate the restored main window client size
		mainWindowClientWidth = mainWindowRestoredWidth - mainWindowFrameWidth;
		mainWindowClientHeight = mainWindowRestoredHeight - mainWindowFrameHeight;
	}
	else
	{
		//Calculate the current main window client size
		RECT mainWindowClientRect;
		GetClientRect(mainWindow, &mainWindowClientRect);
		mainWindowClientWidth = (int)(mainWindowClientRect.right - mainWindowClientRect.left);
		mainWindowClientHeight = (int)(mainWindowClientRect.bottom - mainWindowClientRect.top);
	}

	//Convert the main window client size into a DPI independent value
	mainWindowClientWidth = DPIReverseScaleWidth(mainWindowClientWidth);
	mainWindowClientHeight = DPIReverseScaleHeight(mainWindowClientHeight);

	//Save information on the main window
	IHierarchicalStorageNode& mainWindowStateNode = viewLayout.CreateChild(L"MainWindowState");
	mainWindowStateNode.CreateAttribute(L"SizeX", mainWindowClientWidth);
	mainWindowStateNode.CreateAttribute(L"SizeY", mainWindowClientHeight);
	mainWindowStateNode.CreateAttribute(L"Maximized", isWindowMaximized);

	//Save information on the main docking window
	unsigned int nextWindowID = 1;
	IHierarchicalStorageNode& mainDockedWindowNode = mainWindowStateNode.CreateChild(L"MainDockedWindow");
	SaveDockingWindowFrameStateToViewLayout(mainDockingWindow, mainDockedWindowNode, windowHandleToIDForViewLayout, nextWindowID);

	//Save information on each top-level floating window. Docked windows contained within
	//these floating windows will be saved by the parent windows.
	IHierarchicalStorageNode& floatingWindowsNode = mainWindowStateNode.CreateChild(L"FloatingWindows");
	std::list<HWND> openFloatingWindows = GetOpenFloatingWindows();
	for(std::list<HWND>::const_iterator i = openFloatingWindows.begin(); i != openFloatingWindows.end(); ++i)
	{
		//Determine the type of the target window
		HWND hwnd = *i;
		bool isDockingWindow = IsDockingWindow(hwnd);
		bool isDialogFrame = IsDialogFrame(hwnd);
		if(!isDockingWindow && !isDialogFrame)
		{
			continue;
		}

		//Retrieve any hidden border sizes around the floating window
		int borderLeft;
		int borderRight;
		int borderTop;
		int borderBottom;
		GetHiddenBorderDimensions(hwnd, borderLeft, borderRight, borderTop, borderBottom);

		//Obtain the current position of the floating window
		RECT windowRect;
		GetWindowRect(hwnd, &windowRect);
		int windowPosX = (int)windowRect.left - borderLeft;
		int windowPosY = (int)windowRect.top - borderTop;

		//Convert the view position into coordinates relative to the main window
		POINT windowPos;
		windowPos.x = windowPosX;
		windowPos.y = windowPosY;
		ScreenToClient(mainWindow, &windowPos);
		windowPosX = (int)windowPos.x;
		windowPosY = (int)windowPos.y;

		//Adjust the view position to convert it back into a DPI-independent location
		windowPosX = DPIReverseScaleWidth(windowPosX);
		windowPosY = DPIReverseScaleHeight(windowPosY);

		//Retrieve the current window size
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		int windowWidth = (int)(clientRect.right - clientRect.left);
		int windowHeight = (int)(clientRect.bottom - clientRect.top);

		//Adjust the view size to convert it back into a DPI-independent size
		windowWidth = DPIReverseScaleWidth(windowWidth);
		windowHeight = DPIReverseScaleHeight(windowHeight);

		//Save information on this window according to its type
		if(isDockingWindow)
		{
			IHierarchicalStorageNode& windowFrameNode = floatingWindowsNode.CreateChild(L"DockingWindowFrame");
			windowFrameNode.CreateAttribute(L"PosX", windowPosX);
			windowFrameNode.CreateAttribute(L"PosY", windowPosY);
			windowFrameNode.CreateAttribute(L"SizeX", windowWidth);
			windowFrameNode.CreateAttribute(L"SizeY", windowHeight);
			SaveDockingWindowFrameStateToViewLayout(hwnd, windowFrameNode, windowHandleToIDForViewLayout, nextWindowID);
		}
		else if(isDialogFrame)
		{
			IHierarchicalStorageNode& windowFrameNode = floatingWindowsNode.CreateChild(L"DialogWindowFrame");
			windowFrameNode.CreateAttribute(L"PosX", windowPosX);
			windowFrameNode.CreateAttribute(L"PosY", windowPosY);
			windowFrameNode.CreateAttribute(L"SizeX", windowWidth);
			windowFrameNode.CreateAttribute(L"SizeY", windowHeight);
			SaveDialogWindowFrameStateToViewLayout(hwnd, windowFrameNode, windowHandleToIDForViewLayout, nextWindowID);
		}
	}

	//Save the current state of each open view
	bool result = true;
	for(std::set<IViewPresenter*>::const_iterator i = existingViewSet.begin(); i != existingViewSet.end(); ++i)
	{
		IViewPresenter* viewPresenter = *i;
		IHierarchicalStorageNode& viewPresenterStateNode = viewLayout.CreateChild(L"ViewPresenterState");
		viewPresenterStateNode.CreateAttribute(L"ViewPresenterGroupName", viewPresenter->GetViewGroupName());
		viewPresenterStateNode.CreateAttribute(L"ViewPresenterName", viewPresenter->GetViewName());
		switch(viewPresenter->GetViewTarget())
		{
		default:
			result = false;
			continue;
		case IViewPresenter::ViewTarget::System:
			viewPresenterStateNode.CreateAttribute(L"Target", "System");
			break;
		case IViewPresenter::ViewTarget::Module:
			viewPresenterStateNode.CreateAttribute(L"Target", "Module");
			viewPresenterStateNode.CreateAttribute(L"ModuleID", viewPresenter->GetViewTargetModuleID());
			break;
		case IViewPresenter::ViewTarget::Device:
			viewPresenterStateNode.CreateAttribute(L"Target", "Device");
			viewPresenterStateNode.CreateAttribute(L"DeviceInstanceName", viewPresenter->GetViewTargetDeviceInstanceName());
			viewPresenterStateNode.CreateAttribute(L"ModuleID", viewPresenter->GetViewTargetModuleID());
			break;
		case IViewPresenter::ViewTarget::Extension:
			viewPresenterStateNode.CreateAttribute(L"Target", "Extension");
			viewPresenterStateNode.CreateAttribute(L"ExtensionInstanceName", viewPresenter->GetViewTargetExtensionInstanceName());
			viewPresenterStateNode.CreateAttribute(L"ModuleID", viewPresenter->GetViewTargetModuleID());
			viewPresenterStateNode.CreateAttribute(L"Global", viewPresenter->GetViewTargetGlobalExtension());
			break;
		}
		result &= viewPresenter->SaveViewState(viewPresenterStateNode);
	}

	//Return the result of the operation to the caller
	return result;
}

//----------------------------------------------------------------------------------------
void ViewManager::LoadMainWindowStateFromViewLayout(IHierarchicalStorageNode& mainWindowState, std::map<unsigned int, PlaceholderWindowInfo>& placeholderWindows)
{
	//Maximize or restore the main window if required
	bool isWindowMaximized = (IsZoomed(mainWindow) != 0);
	IHierarchicalStorageAttribute* mainWindowMaximizedAttribute = mainWindowState.GetAttribute(L"Maximized");
	if(mainWindowMaximizedAttribute != 0)
	{
		bool maximizeWindow = mainWindowMaximizedAttribute->ExtractValue<bool>();
		if(maximizeWindow != isWindowMaximized)
		{
			::ShowWindow(mainWindow, maximizeWindow? SW_MAXIMIZE: SW_RESTORE);
			isWindowMaximized = maximizeWindow;
		}
	}

	//Resize the main window if required
	IHierarchicalStorageAttribute* mainWindowClientSizeXAttribute = mainWindowState.GetAttribute(L"SizeX");
	IHierarchicalStorageAttribute* mainWindowClientSizeYAttribute = mainWindowState.GetAttribute(L"SizeY");
	if((mainWindowClientSizeXAttribute != 0) && (mainWindowClientSizeYAttribute != 0))
	{
		//Retrieve the specified main window client size
		int mainWindowWidth = mainWindowClientSizeXAttribute->ExtractValue<int>();
		int mainWindowHeight = mainWindowClientSizeYAttribute->ExtractValue<int>();

		//Convert the main window client size from a DPI independent value
		mainWindowWidth = DPIScaleWidth(mainWindowWidth);
		mainWindowHeight = DPIScaleHeight(mainWindowHeight);

		//Retrieve the window style and extended window style for the main window
		DWORD windowStyle = (DWORD)GetWindowLongPtr(mainWindow, GWL_STYLE);
		DWORD extendedWindowStyle = (DWORD)GetWindowLongPtr(mainWindow, GWL_EXSTYLE);

		//Calculate the total window size to use in order to achieve a client region of
		//the specified size
		RECT clientRect;
		clientRect.left = 0;
		clientRect.top = 0;
		clientRect.right = mainWindowWidth;
		clientRect.bottom = mainWindowHeight;
		AdjustWindowRectEx(&clientRect, windowStyle, TRUE, extendedWindowStyle);
		int adjustedWindowWidth = clientRect.right - clientRect.left;
		int adjustedWindowHeight = clientRect.bottom - clientRect.top;

		//If the window is maximized, adjust the restore size of the window, otherwise
		//adjust the current window size.
		if(isWindowMaximized)
		{
			//Set the restore window position to the requested size
			WINDOWPLACEMENT windowPlacementInfo;
			windowPlacementInfo.length = sizeof(windowPlacementInfo);
			GetWindowPlacement(mainWindow, &windowPlacementInfo);
			windowPlacementInfo.rcNormalPosition.right = windowPlacementInfo.rcNormalPosition.left + adjustedWindowWidth;
			windowPlacementInfo.rcNormalPosition.bottom = windowPlacementInfo.rcNormalPosition.top + adjustedWindowHeight;
			SetWindowPlacement(mainWindow, &windowPlacementInfo);
		}
		else
		{
			//Resize the main window
			SetWindowPos(mainWindow, NULL, 0, 0, adjustedWindowWidth, adjustedWindowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
		}
	}

	//Restore any windows attached to the main docking window
	IHierarchicalStorageNode* mainDockedWindowState = mainWindowState.GetChild(L"MainDockedWindow");
	if(mainDockedWindowState != 0)
	{
		CreateDockingWindowChildrenFromViewLayout(mainDockingWindow, *mainDockedWindowState, placeholderWindows);
	}

	//Iterate through each floating window entry in the file, and open placeholder windows
	//with the specified properties, in preparation for binding to the real windows when
	//our views are restored. Z-Order is based on the order the windows are listed in the
	//file.
	IHierarchicalStorageNode* floatingWindowsNode = mainWindowState.GetChild(L"FloatingWindows");
	if(floatingWindowsNode != 0)
	{
		std::list<IHierarchicalStorageNode*> floatingWindowsList = floatingWindowsNode->GetChildList();
		for(std::list<IHierarchicalStorageNode*>::const_iterator i = floatingWindowsList.begin(); i != floatingWindowsList.end(); ++i)
		{
			IHierarchicalStorageNode* floatingWindowNode = *i;
			std::wstring floatingWindowNodeName = floatingWindowNode->GetName();
			if(floatingWindowNodeName == L"DialogWindowFrame")
			{
				//Create the dialog window frame
				LoadDialogWindowFrameFromViewLayout(*floatingWindowNode, placeholderWindows);
			}
			else if(floatingWindowNodeName == L"DockingWindowFrame")
			{
				//Create this floating docking window
				HWND floatingDockingWindow = LoadDockingWindowFrameFromViewLayout(NULL, *floatingWindowNode);

				//Load all children of the created floating docking window
				CreateDockingWindowChildrenFromViewLayout(floatingDockingWindow, *floatingWindowNode, placeholderWindows);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
HWND ViewManager::LoadDialogWindowFrameFromViewLayout(IHierarchicalStorageNode& dialogWindowState, std::map<unsigned int, PlaceholderWindowInfo>& placeholderWindows) const
{
	//Extract any provided attributes for this dialog window
	IHierarchicalStorageAttribute* resizableAttribute = dialogWindowState.GetAttribute(L"Resizable");
	IHierarchicalStorageAttribute* posXAttribute = dialogWindowState.GetAttribute(L"PosX");
	IHierarchicalStorageAttribute* posYAttribute = dialogWindowState.GetAttribute(L"PosY");
	IHierarchicalStorageAttribute* sizeXAttribute = dialogWindowState.GetAttribute(L"SizeX");
	IHierarchicalStorageAttribute* sizeYAttribute = dialogWindowState.GetAttribute(L"SizeY");
	IHierarchicalStorageAttribute* windowIDAttribute = dialogWindowState.GetAttribute(L"WindowID");
	IHierarchicalStorageAttribute* viewTypeAttribute = dialogWindowState.GetAttribute(L"ViewType");
	if((windowIDAttribute == 0) || (viewTypeAttribute == 0))
	{
		return NULL;
	}
	bool resizable = (resizableAttribute != 0)? resizableAttribute->ExtractValue<bool>(): false;
	int posX = (posXAttribute != 0)? posXAttribute->ExtractValue<int>(): 0;
	int posY = (posYAttribute != 0)? posYAttribute->ExtractValue<int>(): 0;
	int sizeX = (sizeXAttribute != 0)? sizeXAttribute->ExtractValue<int>(): 0;
	int sizeY = (sizeYAttribute != 0)? sizeYAttribute->ExtractValue<int>(): 0;
	unsigned int windowID = windowIDAttribute->ExtractHexValue<unsigned int>();
	IView::ViewType viewType = StringToViewType(viewTypeAttribute->GetValue());

	//Adjust the view position and size to convert from DPI-independent values
	posX = DPIScaleWidth(posX);
	posY = DPIScaleHeight(posY);
	sizeX = DPIScaleWidth(sizeX);
	sizeY = DPIScaleHeight(sizeY);

	//Register the dialog frame window class
	RegisterDialogFrameWindowClass(viewManagerAssemblyHandle);

	//Determine the window style settings to use for the dialog window frame
	DWORD dialogWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	if(resizable)
	{
		dialogWindowStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;
	}
	DWORD dialogWindowExtendedStyle = WS_EX_TOOLWINDOW;

	//Calculate the size to use for the dialog frame in order for it to allow the hosted
	//window to be fully visible
	RECT clientRect;
	clientRect.left = 0;
	clientRect.top = 0;
	clientRect.right = sizeX;
	clientRect.bottom = sizeY;
	AdjustWindowRectEx(&clientRect, dialogWindowStyle, FALSE, dialogWindowExtendedStyle);
	int dialogFrameWidth = clientRect.right - clientRect.left;
	int dialogFrameHeight = clientRect.bottom - clientRect.top;

	//Create the dialog frame to use for this dialog
	HWND dialogWindowFrame = CreateWindowEx(dialogWindowExtendedStyle, dialogFrameWindowClassName, L"", dialogWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, dialogFrameWidth, dialogFrameHeight, mainWindow, NULL, viewManagerAssemblyHandle, 0);

	//Calculate the position of the client area of the main window
	POINT clientPos;
	clientPos.x = 0;
	clientPos.y = 0;
	ClientToScreen(mainWindow, &clientPos);
	int mainWindowPosX = clientPos.x;
	int mainWindowPosY = clientPos.y;

	//Calculate the screen position for the dialog window
	int dialogFramePosX = mainWindowPosX + posX;
	int dialogFramePosY = mainWindowPosY + posY;

	//Position the dialog window
	SetWindowPos(dialogWindowFrame, NULL, dialogFramePosX, dialogFramePosY, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);

	//Record information on this created placeholder window
	PlaceholderWindowInfo placeholderWindowInfo;
	placeholderWindowInfo.parentWindowFrame = dialogWindowFrame;
	placeholderWindowInfo.viewType = viewType;
	placeholderWindows[windowID] = placeholderWindowInfo;

	//Return the created dialog window frame to the caller
	return dialogWindowFrame;
}

//----------------------------------------------------------------------------------------
HWND ViewManager::LoadDockingWindowFrameFromViewLayout(HWND parentDockingWindow, IHierarchicalStorageNode& dockingWindowState) const
{
	//Ensure the docking window class is registered
	DockingWindow::RegisterWindowClass(viewManagerAssemblyHandle);

	//Create the docking window
	DWORD dockingWindowStyle = WS_SIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION;
	DWORD dockingWindowExtendedStyle = WS_EX_TOOLWINDOW;
	HWND dockingWindow = CreateWindowEx(dockingWindowExtendedStyle, DockingWindow::windowClassName, L"", dockingWindowStyle, 0, 0, 0, 0, mainWindow, NULL, viewManagerAssemblyHandle, NULL);

	//Set the font for our new docking window based on the current font for the main
	//docking window
	HFONT dockingWindowFont = (HFONT)SendMessage(mainDockingWindow, WM_GETFONT, 0, 0);
	SendMessage(dockingWindow, WM_SETFONT, (WPARAM)dockingWindowFont, (LPARAM)TRUE);

	//If this new docking window has a target parent docking window, dock it into the
	//parent, otherwise position and size this docking window as a top-level docking
	//window.
	if(parentDockingWindow == NULL)
	{
		//Extract any provided attributes for this floating docking window
		IHierarchicalStorageAttribute* posXAttribute = dockingWindowState.GetAttribute(L"PosX");
		IHierarchicalStorageAttribute* posYAttribute = dockingWindowState.GetAttribute(L"PosY");
		IHierarchicalStorageAttribute* sizeXAttribute = dockingWindowState.GetAttribute(L"SizeX");
		IHierarchicalStorageAttribute* sizeYAttribute = dockingWindowState.GetAttribute(L"SizeY");
		int posX = (posXAttribute != 0)? posXAttribute->ExtractValue<int>(): 0;
		int posY = (posYAttribute != 0)? posYAttribute->ExtractValue<int>(): 0;
		int sizeX = (sizeXAttribute != 0)? sizeXAttribute->ExtractValue<int>(): 0;
		int sizeY = (sizeYAttribute != 0)? sizeYAttribute->ExtractValue<int>(): 0;

		//Adjust the view position and size to convert from DPI-independent values
		posX = DPIScaleWidth(posX);
		posY = DPIScaleHeight(posY);
		sizeX = DPIScaleWidth(sizeX);
		sizeY = DPIScaleHeight(sizeY);

		//Retrieve any hidden border sizes around the docking window
		int borderLeft;
		int borderRight;
		int borderTop;
		int borderBottom;
		GetHiddenBorderDimensions(dockingWindow, borderLeft, borderRight, borderTop, borderBottom);

		//Convert the view position from coordinates relative to the main window, into
		//screen coordinates.
		POINT windowPos;
		windowPos.x = posX - borderLeft;
		windowPos.y = posY - borderTop;
		ClientToScreen(mainWindow, &windowPos);
		int finalWindowPosX = (int)windowPos.x;
		int finalWindowPosY = (int)windowPos.y;

		//Calculate the total window size to use in order to achieve a client region
		//within the docking window of the specified size
		RECT clientRect;
		clientRect.left = 0;
		clientRect.top = 0;
		clientRect.right = sizeX;
		clientRect.bottom = sizeY;
		AdjustWindowRectEx(&clientRect, dockingWindowStyle, FALSE, dockingWindowExtendedStyle);
		int adjustedWindowWidth = clientRect.right - clientRect.left;
		int adjustedWindowHeight = clientRect.bottom - clientRect.top;

		//Position and size the floating docking window
		SetWindowPos(dockingWindow, NULL, finalWindowPosX, finalWindowPosY, adjustedWindowWidth, adjustedWindowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOREPOSITION);

		//Show the docking window
		SetWindowPos(dockingWindow, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	}
	else
	{
		//Extract any provided attributes for this docked window
		IHierarchicalStorageAttribute* dockPosAttribute = dockingWindowState.GetAttribute(L"DockPos");
		IHierarchicalStorageAttribute* autoHideAttribute = dockingWindowState.GetAttribute(L"AutoHide");
		IHierarchicalStorageAttribute* desiredWidthAttribute = dockingWindowState.GetAttribute(L"DesiredWidth");
		IHierarchicalStorageAttribute* desiredHeightAttribute = dockingWindowState.GetAttribute(L"DesiredHeight");
		WC_DockPanel::DockLocation dockLocation = (dockPosAttribute != 0)? StringToDockLocation(dockPosAttribute->GetValue()): WC_DockPanel::DockLocation::Left;
		bool autoHide = (autoHideAttribute != 0)? autoHideAttribute->ExtractValue<bool>(): false;
		int desiredWidth = (desiredWidthAttribute != 0)? desiredWidthAttribute->ExtractValue<int>(): 200;
		int desiredHeight = (desiredHeightAttribute != 0)? desiredHeightAttribute->ExtractValue<int>(): 200;

		//Convert the child docking window size from a DPI-independent value
		desiredWidth = DPIScaleWidth(desiredWidth);
		desiredHeight = DPIScaleHeight(desiredHeight);

		//Resize the child docking window to the desired size
		SetWindowPos(dockingWindow, NULL, 0, 0, desiredWidth, desiredHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOREPOSITION | SWP_NOMOVE);

		//Add the docking window to the parent docking window
		DockingWindow::AddDockedWindowParams addDockedWindowParams;
		addDockedWindowParams.hwnd = dockingWindow;
		addDockedWindowParams.dockLocation = dockLocation;
		addDockedWindowParams.forceToTopOfDockingOrder = false;
		addDockedWindowParams.autoHide = autoHide;
		SendMessage(parentDockingWindow, (UINT)DockingWindow::WindowMessages::AddDockedWindow, 0, (LPARAM)&addDockedWindowParams);

		//Show the docking window if required
		if(!autoHide)
		{
			SetWindowPos(dockingWindow, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
		}
	}

	//Return the created docking window to the caller
	return dockingWindow;
}

//----------------------------------------------------------------------------------------
void ViewManager::CreateDockingWindowChildrenFromViewLayout(HWND dockingWindow, IHierarchicalStorageNode& dockingWindowState, std::map<unsigned int, PlaceholderWindowInfo>& placeholderWindows) const
{
	//Retrieve the selected content window ID for the target docking window
	IHierarchicalStorageAttribute* activeHostedWindowIDAttribute = dockingWindowState.GetAttribute(L"ActiveHostedWindowID");
	unsigned int selectedContentWindowID = (activeHostedWindowIDAttribute != 0)? activeHostedWindowIDAttribute->ExtractValue<unsigned int>(): (unsigned int)-1;

	//Determine if the target docking window currently has any hosted content windows
	bool dockingWindowHasChildContent = (SendMessage(dockingWindow, (UINT)DockingWindow::WindowMessages::GetContentWindowCount, 0, 0) != 0);

	//Create placeholders for each content window in this docking window
	IHierarchicalStorageNode* hostedWindowNode = dockingWindowState.GetChild(L"HostedWindow");
	while(hostedWindowNode != 0)
	{
		//Create a placeholder window for this docked window
		IHierarchicalStorageAttribute* windowIDAttribute = hostedWindowNode->GetAttribute(L"WindowID");
		IHierarchicalStorageAttribute* viewTypeAttribute = hostedWindowNode->GetAttribute(L"ViewType");
		if((windowIDAttribute != 0) && (viewTypeAttribute != 0))
		{
			//Extract the view type for the placeholder window
			IView::ViewType viewType = StringToViewType(viewTypeAttribute->GetValue());

			//Create the placeholder window
			unsigned int windowID = windowIDAttribute->ExtractHexValue<unsigned int>();
			HWND placeholderWindow = CreateWindowEx(0, WC_STATIC, L"", 0, 0, 0, 0, 0, NULL, NULL, viewManagerAssemblyHandle, NULL);

			//Add this placeholder window as a content window of the docking window
			DockingWindow::AddContentWindowParams addContentWindowParams;
			addContentWindowParams.hwnd = placeholderWindow;
			addContentWindowParams.windowTitle = L"";
			SendMessage(dockingWindow, (UINT)DockingWindow::WindowMessages::AddContentWindow, 0, (LPARAM)&addContentWindowParams);

			//Record information on this created placeholder window
			PlaceholderWindowInfo placeholderWindowInfo;
			placeholderWindowInfo.placeholderContentWindow = placeholderWindow;
			placeholderWindowInfo.parentWindowFrame = dockingWindow;
			placeholderWindowInfo.makeContentVisible = !dockingWindowHasChildContent;
			placeholderWindowInfo.selectedContentWindow = (windowID == selectedContentWindowID);
			placeholderWindowInfo.viewType = viewType;
			placeholderWindows[windowID] = placeholderWindowInfo;

			//Since the docking window now has at least one hosted content window, flag
			//that it has child content.
			dockingWindowHasChildContent = true;
		}

		//Advance to the next HostedWindow child node
		hostedWindowNode = dockingWindowState.GetChild(L"HostedWindow", hostedWindowNode);
	}

	//Restore each child docking window of this docking window
	IHierarchicalStorageNode* childDockedWindowNode = dockingWindowState.GetChild(L"DockingWindowFrame");
	while(childDockedWindowNode != 0)
	{
		//Create this child docking window
		HWND childDockingWindow = LoadDockingWindowFrameFromViewLayout(dockingWindow, *childDockedWindowNode);

		//Load all children of the created child docking window
		CreateDockingWindowChildrenFromViewLayout(childDockingWindow, *childDockedWindowNode, placeholderWindows);

		//Advance to the next DockedWindowState child node
		childDockedWindowNode = dockingWindowState.GetChild(L"DockingWindowFrame", childDockedWindowNode);
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::SaveDialogWindowFrameStateToViewLayout(HWND dialogWindow, IHierarchicalStorageNode& dialogWindowState, std::map<HWND, unsigned int>& windowHandleToID, unsigned int& nextWindowID) const
{
	//Retrieve the hosted window attached to this dialog window frame
	HWND hostedWindow = GetWindow(dialogWindow, GW_CHILD);
	if(hostedWindow == NULL)
	{
		return;
	}

	//Retrieve information on the attached hosted window
	std::map<HWND, OpenWindowInfo>::const_iterator windowInfoIterator = windowInfoSet.find(hostedWindow);
	if(windowInfoIterator == windowInfoSet.end())
	{
		return;
	}
	const OpenWindowInfo& windowInfo = windowInfoIterator->second;

	//Generate an ID for this hosted, and add it to the list of associated window handles
	//and window ID numbers.
	unsigned int windowID = nextWindowID++;
	windowHandleToID[hostedWindow] = windowID;

	//Save information on this hosted window. Note that the window title is saved here for
	//information purposes only, to make manually viewing and editing workspace files
	//easier. The title cannot be relied upon as meaningful information when restoring the
	//layout, as not only may it change depending on what modules are loaded, and how
	//things are named module files, it also may change with revisions of the extension
	//defining the view, or even with what language is currently selected in future
	//versions of the platform.
	IView::ViewType viewType = windowInfo.view.GetViewType();
	dialogWindowState.CreateAttribute(L"WindowID", windowID);
	dialogWindowState.CreateAttribute(L"ViewType", ViewTypeToString(viewType));
	dialogWindowState.CreateAttribute(L"Resizable", windowInfo.view.CanResizeDialog());
	dialogWindowState.CreateAttribute(L"WindowTitle", windowInfo.windowTitle);
}

//----------------------------------------------------------------------------------------
void ViewManager::SaveDockingWindowFrameStateToViewLayout(HWND dockingWindow, IHierarchicalStorageNode& dockedWindowState, std::map<HWND, unsigned int>& windowHandleToID, unsigned int& nextWindowID) const
{
	//Retrieve information on each hosted content window of the parent docking window, and
	//sort them according to the tab index order.
	unsigned int contentWindowCount = (unsigned int)SendMessage(dockingWindow, (UINT)DockingWindow::WindowMessages::GetContentWindowCount, 0, 0);
	std::map<int, DockingWindow::GetContentWindowInfo> contentWindowInfoList;
	for(unsigned int contentWindowNo = 0; contentWindowNo < contentWindowCount; ++contentWindowNo)
	{
		DockingWindow::GetContentWindowInfo contentWindowInfo;
		if(SendMessage(dockingWindow, (UINT)DockingWindow::WindowMessages::GetContentWindowInfo, (WPARAM)contentWindowNo, (LPARAM)&contentWindowInfo) == 0)
		{
			contentWindowInfoList.insert(std::pair<int, DockingWindow::GetContentWindowInfo>(contentWindowInfo.tabIndex, contentWindowInfo));
		}
	}

	//Retrieve the index number of the currently selected content window
	unsigned int activeContentWindowIndex = (unsigned int)SendMessage(dockingWindow, (UINT)DockingWindow::WindowMessages::GetActiveContentWindow, 0, 0);

	//Save information on each hosted content window in the target docking window, and
	//attempt to retrieve the window ID of the currently selected content window.
	bool activeHostedWindowIDDefined = false;
	unsigned int activeHostedWindowID;
	for(std::map<int, DockingWindow::GetContentWindowInfo>::const_iterator i = contentWindowInfoList.begin(); i != contentWindowInfoList.end(); ++i)
	{
		//Attempt to retrieve information on this open window
		const DockingWindow::GetContentWindowInfo& contentWindowInfo = i->second;
		std::map<HWND, OpenWindowInfo>::const_iterator windowInfoIterator = windowInfoSet.find(contentWindowInfo.hwnd);
		if(windowInfoIterator == windowInfoSet.end())
		{
			continue;
		}
		const OpenWindowInfo& windowInfo = windowInfoIterator->second;

		//Generate an ID for this window, and add it to the list of associated window
		//handles and window ID numbers.
		unsigned int windowID = nextWindowID++;
		windowHandleToID[contentWindowInfo.hwnd] = windowID;

		//If this content window is the currently selected content window in the parent
		//docking window, save the generated ID for this window as the selected content
		//window ID.
		if(contentWindowInfo.contentIndex == activeContentWindowIndex)
		{
			activeHostedWindowID = windowID;
			activeHostedWindowIDDefined = true;
		}

		//Save information on this hosted content window. Note that the window title is
		//saved here for information purposes only, to make manually viewing and editing
		//workspace files easier. The title cannot be relied upon as meaningful
		//information when restoring the layout, as not only may it change depending on
		//what modules are loaded, and how things are named module files, it also may
		//change with revisions of the extension defining the view, or even with what
		//language is currently selected in future versions of the platform.
		IView::ViewType viewType = windowInfo.view.GetViewType();
		IHierarchicalStorageNode& hostedWindowState = dockedWindowState.CreateChild(L"HostedWindow");
		hostedWindowState.CreateAttribute(L"WindowID", windowID);
		hostedWindowState.CreateAttribute(L"ViewType", ViewTypeToString(viewType));
		hostedWindowState.CreateAttribute(L"WindowTitle", std::wstring(contentWindowInfo.windowTitle));
	}

	//If we managed to locate the currently active content window, save the ID of that
	//content window as the active hosted content window for the parent docking window.
	if(activeHostedWindowIDDefined)
	{
		dockedWindowState.CreateAttribute(L"ActiveHostedWindowID", activeHostedWindowID);
	}

	//Save information on each child docking window in the target docking window
	unsigned int dockedWindowCount = (unsigned int)SendMessage(dockingWindow, (UINT)DockingWindow::WindowMessages::GetDockedWindowCount, 0, 0);
	for(unsigned int dockedWindowNo = 0; dockedWindowNo < dockedWindowCount; ++dockedWindowNo)
	{
		DockingWindow::GetDockedWindowInfo getDockedWindowInfo;
		if(SendMessage(dockingWindow, (UINT)DockingWindow::WindowMessages::GetDockedWindowInfo, (WPARAM)dockedWindowNo, (LPARAM)&getDockedWindowInfo) == 0)
		{
			IHierarchicalStorageNode& childDockedWindowNode = dockedWindowState.CreateChild(L"DockingWindowFrame");
			childDockedWindowNode.CreateAttribute(L"DockPos", DockLocationToString(getDockedWindowInfo.dockLocation));
			childDockedWindowNode.CreateAttribute(L"AutoHide", getDockedWindowInfo.autoHide);
			childDockedWindowNode.CreateAttribute(L"DesiredWidth", DPIReverseScaleWidth(getDockedWindowInfo.desiredWidth));
			childDockedWindowNode.CreateAttribute(L"DesiredHeight", DPIReverseScaleHeight(getDockedWindowInfo.desiredHeight));
			SaveDockingWindowFrameStateToViewLayout(getDockedWindowInfo.hwnd, childDockedWindowNode, windowHandleToID, nextWindowID);
		}
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::BuildExistingWindowsToCloseList(std::list<HWND>& existingWindowsToClose) const
{
	existingWindowsToClose = GetOpenFloatingWindows();
	unsigned int mainDockingWindowChildDockingWindowCount = (unsigned int)SendMessage(mainDockingWindow, (UINT)DockingWindow::WindowMessages::GetDockedWindowCount, 0, 0);
	for(unsigned int i = 0; i < mainDockingWindowChildDockingWindowCount; ++i)
	{
		DockingWindow::GetDockedWindowInfo dockedWindowInfo;
		if(SendMessage(mainDockingWindow, (UINT)DockingWindow::WindowMessages::GetDockedWindowInfo, i, (LPARAM)&dockedWindowInfo) == 0)
		{
			existingWindowsToClose.push_back(dockedWindowInfo.hwnd);
		}
	}
}

//----------------------------------------------------------------------------------------
void ViewManager::CloseExistingWindows(const std::list<HWND>& existingWindowsToClose)
{
	for(std::list<HWND>::const_iterator i = existingWindowsToClose.begin(); i != existingWindowsToClose.end(); ++i)
	{
		DestroyWindow(*i);
	}
}

//----------------------------------------------------------------------------------------
WC_DockPanel::DockLocation ViewManager::StringToDockLocation(const std::wstring& dockLocationAsString)
{
	if(dockLocationAsString == L"Left")
	{
		return WC_DockPanel::DockLocation::Left;
	}
	else if(dockLocationAsString == L"Right")
	{
		return WC_DockPanel::DockLocation::Right;
	}
	else if(dockLocationAsString == L"Top")
	{
		return WC_DockPanel::DockLocation::Top;
	}
	else if(dockLocationAsString == L"Bottom")
	{
		return WC_DockPanel::DockLocation::Bottom;
	}
	return WC_DockPanel::DockLocation::Left;
}

//----------------------------------------------------------------------------------------
std::wstring ViewManager::DockLocationToString(WC_DockPanel::DockLocation dockLocation)
{
	switch(dockLocation)
	{
	case WC_DockPanel::DockLocation::Left:
		return L"Left";
	case WC_DockPanel::DockLocation::Right:
		return L"Right";
	case WC_DockPanel::DockLocation::Top:
		return L"Top";
	case WC_DockPanel::DockLocation::Bottom:
		return L"Bottom";
	}
	return L"Unknown";
}

//----------------------------------------------------------------------------------------
WC_DockPanel::DockLocation ViewManager::ViewDockLocationToDockPanelDockLocation(IView::DockPos viewDockLocation)
{
	switch(viewDockLocation)
	{
	default:
	case IView::DockPos::Left:
		return WC_DockPanel::DockLocation::Left;
	case IView::DockPos::Right:
		return WC_DockPanel::DockLocation::Right;
	case IView::DockPos::Top:
		return WC_DockPanel::DockLocation::Top;
	case IView::DockPos::Bottom:
		return WC_DockPanel::DockLocation::Bottom;
	}
}

//----------------------------------------------------------------------------------------
IView::ViewType ViewManager::StringToViewType(const std::wstring& viewTypeAsString)
{
	if(viewTypeAsString == L"Dialog")
	{
		return IView::ViewType::Dialog;
	}
	else if(viewTypeAsString == L"Dockable")
	{
		return IView::ViewType::Dockable;
	}
	else if(viewTypeAsString == L"Document")
	{
		return IView::ViewType::Document;
	}
	return IView::ViewType::Dialog;
}

//----------------------------------------------------------------------------------------
std::wstring ViewManager::ViewTypeToString(IView::ViewType viewType)
{
	switch(viewType)
	{
	case IView::ViewType::Dialog:
		return L"Dialog";
	case IView::ViewType::Dockable:
		return L"Dockable";
	case IView::ViewType::Document:
		return L"Document";
	}
	return L"Unknown";
}

//----------------------------------------------------------------------------------------
//Window auto-position functions
//----------------------------------------------------------------------------------------
bool ViewManager::FindBestWindowPosition(int newWindowWidth, int newWindowHeight, int& newWindowPosX, int& newWindowPosY) const
{
	//Get the position of the client area of our main window in screen coordinates
	Region2D screenRegion;
	screenRegion.posx = 0;
	screenRegion.posy = 0;
	POINT clientPos;
	clientPos.x = 0;
	clientPos.y = 0;
	if(ClientToScreen(mainWindow, &clientPos) != 0)
	{
		screenRegion.posx = clientPos.x;
		screenRegion.posy = clientPos.y;
	}

	//Calculate the region within which we will allow child windows to be auto-positioned
	screenRegion.sizex = 800;
	screenRegion.sizey = 600;
	RECT rect;
	if(GetClientRect(mainWindow, &rect) != 0)
	{
		//Get the width and height of the client area of our main window
		screenRegion.sizex = rect.right;
		screenRegion.sizey = rect.bottom;

		//In the case where the height of the client area is very small, we consider the
		//client region of the window to be collapsed. In this case, make a region defined
		//by the screen height instead, taking into account the window position and width.
		if(screenRegion.sizey < 50)
		{
			//Obtain the position of the client region of our window in screen coordinates
			POINT mainWindowClientPosInScreenCoordinates;
			mainWindowClientPosInScreenCoordinates.x = 0;
			mainWindowClientPosInScreenCoordinates.y = 0;
			if(ClientToScreen(mainWindow, &mainWindowClientPosInScreenCoordinates) != 0)
			{
				//Obtain information on the dimensions of the screen
				HMONITOR monitor = MonitorFromWindow(mainWindow, MONITOR_DEFAULTTONEAREST);
				MONITORINFO monitorInfo;
				monitorInfo.cbSize = sizeof(monitorInfo);
				if(GetMonitorInfo(monitor, &monitorInfo) != 0)
				{
					//Extend the screen region down to the bottom of the display
					int monitorEndPosY = monitorInfo.rcWork.bottom;
					if(monitorEndPosY > mainWindowClientPosInScreenCoordinates.y)
					{
						screenRegion.sizey = monitorEndPosY - mainWindowClientPosInScreenCoordinates.y;
					}
				}
			}
		}
	}

	//Combine the existing child window positions
	std::list<Region2D> regionList;
	regionList.push_back(screenRegion);
	std::list<HWND> floatingWindows = GetOpenFloatingWindows();
	for(std::list<HWND>::const_iterator i = floatingWindows.begin(); i != floatingWindows.end(); ++i)
	{
		//Retrieve the position and size of the target window
		HWND targetWindow = *i;
		RECT targetWindowRect;
		GetWindowRect(targetWindow, &targetWindowRect);

		//Calculate the size of any hidden borders attached to the target window
		int borderLeft;
		int borderRight;
		int borderTop;
		int borderBottom;
		GetHiddenBorderDimensions(targetWindow, borderLeft, borderRight, borderTop, borderBottom);

		//Calculate the region of this window
		Region2D region;
		region.posx = targetWindowRect.left - borderLeft;
		region.posy = targetWindowRect.top - borderTop;
		region.sizex = (targetWindowRect.right + borderRight) - (targetWindowRect.left - borderLeft);
		region.sizey = (targetWindowRect.bottom + borderBottom) - (targetWindowRect.top - borderTop);

		//Intersect the window region with the free space region list
		std::list<Region2D>::iterator targetRegion = regionList.begin();
		while(targetRegion != regionList.end())
		{
			std::list<Region2D> newRegions;
			if(IntersectRegion(*targetRegion, region, newRegions))
			{
				regionList.splice(regionList.end(), newRegions);
				std::list<Region2D>::iterator targetRegionCopy = targetRegion;
				++targetRegion;
				regionList.erase(targetRegionCopy);
			}
			else
			{
				++targetRegion;
			}
		}
	}

	//Evaluate the best auto-generated region, if one exists.
	bool foundRegion = false;
	Region2D bestRegion;
	long bestRegionDistance;
	for(std::list<Region2D>::const_iterator i = regionList.begin(); i != regionList.end(); ++i)
	{
		if((i->sizex >= newWindowWidth) && (i->sizey >= newWindowHeight))
		{
			long regionDistance = (i->posx - screenRegion.posx) + (i->posy - screenRegion.posy);
			if(!foundRegion || (regionDistance < bestRegionDistance))
			{
				foundRegion = true;
				bestRegion = *i;
				bestRegionDistance = regionDistance;
			}
		}
	}

	//In the case that no free region can be found which is large enough, do a second pass
	//to find the next best option based on useful area.
	if(!foundRegion)
	{
		long newWindowRegionArea = newWindowWidth * newWindowHeight;
		long bestUsefulRegionArea;
		for(std::list<Region2D>::const_iterator i = regionList.begin(); i != regionList.end(); ++i)
		{
			//If our new window would pass the edge of the available screen region if
			//placed within this region, skip it.
			Region2D usefulRegion = *i;
			if(((usefulRegion.posx + newWindowWidth) > (screenRegion.posx + screenRegion.sizex)) || ((usefulRegion.posy + newWindowHeight) > (screenRegion.posy + screenRegion.sizey)))
			{
				continue;
			}

			//If this region would result in the window being fully contained within the
			//screen region, with half or more of the window area not covering up another
			//window, and it would give us the largest area of the window being visible of
			//all the regions we've examined so far, accept it as the best useful region
			//we've located so far.
			usefulRegion.sizex = (usefulRegion.sizex > newWindowWidth)? newWindowWidth: usefulRegion.sizex;
			usefulRegion.sizey = (usefulRegion.sizey > newWindowHeight)? newWindowHeight: usefulRegion.sizey;
			long usefulRegionArea = usefulRegion.sizex * usefulRegion.sizey;
			if((usefulRegionArea >= (newWindowRegionArea / 2)) && (!foundRegion || (usefulRegionArea > bestUsefulRegionArea)))
			{
				bestUsefulRegionArea = usefulRegionArea;
				bestRegion = usefulRegion;
				foundRegion = true;
			}
		}
	}

	//If we found a best location to place the window, return it to the caller.
	if(foundRegion)
	{
		newWindowPosX = (int)bestRegion.posx;
		newWindowPosY = (int)bestRegion.posy;
	}

	//Inform the caller if we managed to find a good location to place the window
	return foundRegion;
}

//----------------------------------------------------------------------------------------
bool ViewManager::IntersectRegion(const Region2D& existingRegion, const Region2D& regionToIntersect, std::list<Region2D>& newRegionsToCreate)
{
	bool result = false;

	//Check if the two regions overlap
	bool overlapsHorizontally =
	    (regionToIntersect.sizex > 0) && (existingRegion.sizex > 0)
	 && (((regionToIntersect.posx >= existingRegion.posx) && (regionToIntersect.posx < (existingRegion.posx + existingRegion.sizex))) //New region starts within old region
	  || (((regionToIntersect.posx + regionToIntersect.sizex) > existingRegion.posx) && ((regionToIntersect.posx + regionToIntersect.sizex) < (existingRegion.posx + existingRegion.sizex))) //New region ends within old region
	  || ((regionToIntersect.posx <= existingRegion.posx) && ((regionToIntersect.posx + regionToIntersect.sizex) >= (existingRegion.posx + existingRegion.sizex)))); //New region completely swallows old region
	bool overlapsVertically =
	    (regionToIntersect.sizey > 0) && (existingRegion.sizey > 0)
	 && (((regionToIntersect.posy >= existingRegion.posy) && (regionToIntersect.posy < (existingRegion.posy + existingRegion.sizey))) //New region starts within old region
	  || (((regionToIntersect.posy + regionToIntersect.sizey) > existingRegion.posy) && ((regionToIntersect.posy + regionToIntersect.sizey) < (existingRegion.posy + existingRegion.sizey))) //New region ends within old region
	  || ((regionToIntersect.posy <= existingRegion.posy) && ((regionToIntersect.posy + regionToIntersect.sizey) >= (existingRegion.posy + existingRegion.sizey)))); //New region completely swallows old region

	//If the two regions overlap, calculate the resultant regions
	if(overlapsVertically && overlapsHorizontally)
	{
		result = true;

		//We start by checking which corners of our existing region are contained within
		//the intersecting region. This will give us a basis for determining how to divide
		//the existing region.
		bool topLeftCornerOverlapped = PointWithinRegion(existingRegion.posx, existingRegion.posy, regionToIntersect);
		bool topRightCornerOverlapped = PointWithinRegion(existingRegion.posx + existingRegion.sizex, existingRegion.posy, regionToIntersect);
		bool bottomLeftCornerOverlapped = PointWithinRegion(existingRegion.posx, existingRegion.posy + existingRegion.sizey, regionToIntersect);
		bool bottomRightCornerOverlapped = PointWithinRegion(existingRegion.posx + existingRegion.sizex, existingRegion.posy + existingRegion.sizey, regionToIntersect);
		int cornerIntersectionCount = (topLeftCornerOverlapped? 1: 0) + (topRightCornerOverlapped? 1: 0) + (bottomLeftCornerOverlapped? 1: 0) + (bottomRightCornerOverlapped? 1: 0);

		//Check if none of the corners of our existing region intersect
		if(cornerIntersectionCount == 0)
		{
			//If none of the corners intersect, we have three possibilities. Since we know
			//the two regions do overlap, since we did an initial test for that above, the
			//intersecting region must either be entirely contained within our region, it
			//must be cutting into one side of our region, or it must be slicing our
			//region straight across into two pieces. To determine which is happening,
			//we're going to look for intersections against the four edges. The number of
			//edge intersections will tell us which case we've encountered.
			bool leftEdgeIntersected = RegionIntersectsVerticalLine(existingRegion.posx, existingRegion.posy, existingRegion.sizey, regionToIntersect);
			bool rightEdgeIntersected = RegionIntersectsVerticalLine(existingRegion.posx + existingRegion.sizex, existingRegion.posy, existingRegion.sizey, regionToIntersect);
			bool topEdgeIntersected = RegionIntersectsHorizontalLine(existingRegion.posx, existingRegion.posy, existingRegion.sizex, regionToIntersect);
			bool bottomEdgeIntersected = RegionIntersectsHorizontalLine(existingRegion.posx, existingRegion.posy + existingRegion.sizey, existingRegion.sizex, regionToIntersect);
			int edgeIntersectionCount = (leftEdgeIntersected? 1: 0) + (rightEdgeIntersected? 1: 0) + (topEdgeIntersected? 1: 0) + (bottomEdgeIntersected? 1: 0);
			if(edgeIntersectionCount == 0)
			{
				//The intersected region punches a hole in our region. 4 regions created.
				//==========  1. ===        2. ========== 3.            4.        ===
				//=        =     = =           ==========                         = =
				//=  ++++  =     = =                                              = =
				//=  +  +  =     = =                                              = =
				//=  ++++  =     = =                                              = =
				//=        =     = =                         ==========           = =
				//==========     ===                         ==========           ===
				Region2D region1(existingRegion.posx,
				                 existingRegion.posy,
				                 regionToIntersect.posx - existingRegion.posx,
				                 existingRegion.sizey);
				Region2D region2(existingRegion.posx,
				                 existingRegion.posy,
				                 existingRegion.sizex,
				                 regionToIntersect.posy - existingRegion.posy);
				Region2D region3(existingRegion.posx,
				                 regionToIntersect.posy + regionToIntersect.sizey,
				                 existingRegion.sizex,
				                 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
				Region2D region4(regionToIntersect.posx + regionToIntersect.sizex,
				                 existingRegion.posy,
				                 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
				                 existingRegion.sizey);
				newRegionsToCreate.push_back(region1);
				newRegionsToCreate.push_back(region2);
				newRegionsToCreate.push_back(region3);
				newRegionsToCreate.push_back(region4);
			}
			else if(edgeIntersectionCount == 1)
			{
				//The intersected region hits a side. 3 regions created.
				//==========  1. =======    2. ========== 3.           
				//=        =     =     =       ==========              
				//=      +++++   =     =                               
				//=      + = +   =     =                               
				//=      +++++   =     =                               
				//=        =     =     =                     ==========
				//==========     =======                     ==========
				if(leftEdgeIntersected)
				{
					Region2D region1(regionToIntersect.posx + regionToIntersect.sizex,
					                 existingRegion.posy,
					                 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
					                 existingRegion.sizey);
					Region2D region2(existingRegion.posx,
					                 existingRegion.posy,
					                 existingRegion.sizex,
					                 regionToIntersect.posy - existingRegion.posy);
					Region2D region3(existingRegion.posx,
					                 regionToIntersect.posy + regionToIntersect.sizey,
					                 existingRegion.sizex,
					                 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
					newRegionsToCreate.push_back(region3);
				}
				else if(rightEdgeIntersected)
				{
					Region2D region1(existingRegion.posx,
					                 existingRegion.posy,
					                 regionToIntersect.posx - existingRegion.posx,
					                 existingRegion.sizey);
					Region2D region2(existingRegion.posx,
					                 existingRegion.posy,
					                 existingRegion.sizex,
					                 regionToIntersect.posy - existingRegion.posy);
					Region2D region3(existingRegion.posx,
					                 regionToIntersect.posy + regionToIntersect.sizey,
					                 existingRegion.sizex,
					                 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
					newRegionsToCreate.push_back(region3);
				}
				else if(topEdgeIntersected)
				{
					Region2D region1(existingRegion.posx,
					                 existingRegion.posy,
					                 regionToIntersect.posx - existingRegion.posx,
					                 existingRegion.sizey);
					Region2D region2(existingRegion.posx,
					                 regionToIntersect.posy + regionToIntersect.sizey,
					                 existingRegion.sizex,
					                 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
					Region2D region3(regionToIntersect.posx + regionToIntersect.sizex,
					                 existingRegion.posy,
					                 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
					                 existingRegion.sizey);
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
					newRegionsToCreate.push_back(region3);
				}
				else //"bottomEdgeIntersected" should be the only other case
				{
					Region2D region1(existingRegion.posx,
					                 existingRegion.posy,
					                 regionToIntersect.posx - existingRegion.posx,
					                 existingRegion.sizey);
					Region2D region2(existingRegion.posx,
					                 existingRegion.posy,
					                 existingRegion.sizex,
					                 regionToIntersect.posy - existingRegion.posy);
					Region2D region3(regionToIntersect.posx + regionToIntersect.sizex,
					                 existingRegion.posy,
					                 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
					                 existingRegion.sizey);
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
					newRegionsToCreate.push_back(region3);
				}
			}
			else //"edgeIntersectionCount == 2" should be the only other case we encounter
			{
				//The intersected region slices our region. 2 regions created.
				//  ==========  1. ========== 2.           
				//  =        =     ==========              
				//++++++++++++++                           
				//+ =        = +                           
				//++++++++++++++                           
				//  =        =                   ==========
				//  ==========                   ==========
				if(leftEdgeIntersected && rightEdgeIntersected)
				{
					Region2D region1(existingRegion.posx,
					                 existingRegion.posy,
					                 existingRegion.sizex,
					                 regionToIntersect.posy - existingRegion.posy);
					Region2D region2(existingRegion.posx,
					                 regionToIntersect.posy + regionToIntersect.sizey,
					                 existingRegion.sizex,
					                 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
				}
				else //"topEdgeIntersected && bottomEdgeIntersected" should be the only other case
				{
					Region2D region1(existingRegion.posx,
					                 existingRegion.posy,
					                 regionToIntersect.posx - existingRegion.posx,
					                 existingRegion.sizey);
					Region2D region2(regionToIntersect.posx + regionToIntersect.sizex,
					                 existingRegion.posy,
					                 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
					                 existingRegion.sizey);
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
				}
			}
		}
		else if(cornerIntersectionCount == 1)
		{
			//The intersected region hits a corner. 2 regions created.
			//==========   1. ======     2. ==========
			//=        =      =    =        =        =
			//=        =      =    =        =        =
			//=        =      =    =        ==========
			//=     ++++++    =    =                  
			//=     +  = +    =    =                  
			//======+=== +    ======                  
			//      ++++++                            
			if(topLeftCornerOverlapped)
			{
				Region2D region1(regionToIntersect.posx + regionToIntersect.sizex,
				                 existingRegion.posy,
				                 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
				                 existingRegion.sizey);
				Region2D region2(existingRegion.posx,
				                 regionToIntersect.posy + regionToIntersect.sizey,
				                 existingRegion.sizex,
				                 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
				newRegionsToCreate.push_back(region1);
				newRegionsToCreate.push_back(region2);
			}
			else if(topRightCornerOverlapped)
			{
				Region2D region1(existingRegion.posx,
				                 existingRegion.posy,
				                 regionToIntersect.posx - existingRegion.posx,
				                 existingRegion.sizey);
				Region2D region2(existingRegion.posx,
				                 regionToIntersect.posy + regionToIntersect.sizey,
				                 existingRegion.sizex,
				                 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
				newRegionsToCreate.push_back(region1);
				newRegionsToCreate.push_back(region2);
			}
			else if(bottomLeftCornerOverlapped)
			{
				Region2D region1(regionToIntersect.posx + regionToIntersect.sizex,
				                 existingRegion.posy,
				                 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
				                 existingRegion.sizey);
				Region2D region2(existingRegion.posx,
				                 existingRegion.posy,
				                 existingRegion.sizex,
				                 (regionToIntersect.posy - existingRegion.posy));
				newRegionsToCreate.push_back(region1);
				newRegionsToCreate.push_back(region2);
			}
			else //"bottomRightCornerOverlapped" should be the only other case
			{
				Region2D region1(existingRegion.posx,
				                 existingRegion.posy,
				                 regionToIntersect.posx - existingRegion.posx,
				                 existingRegion.sizey);
				Region2D region2(existingRegion.posx,
				                 existingRegion.posy,
				                 existingRegion.sizex,
				                 regionToIntersect.posy - existingRegion.posy);
				newRegionsToCreate.push_back(region1);
				newRegionsToCreate.push_back(region2);
			}
		}
		else if(cornerIntersectionCount == 2)
		{
			//The intersected region restricts our region. 1 region created.
			//++++++++++++++ 1.           
			//+            +              
			//+ ========== +              
			//+ =        = +              
			//++++++++++++++              
			//  =        =      ==========
			//  =        =      =        =
			//  =        =      =        =
			//  ==========      ==========
			if(topLeftCornerOverlapped && topRightCornerOverlapped)
			{
				Region2D region1(existingRegion.posx,
				                 regionToIntersect.posy + regionToIntersect.sizey,
				                 existingRegion.sizex,
				                 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
				newRegionsToCreate.push_back(region1);
			}
			else if(bottomLeftCornerOverlapped && bottomRightCornerOverlapped)
			{
				Region2D region1(existingRegion.posx,
				                 existingRegion.posy,
				                 existingRegion.sizex,
				                 regionToIntersect.posy - existingRegion.posy);
				newRegionsToCreate.push_back(region1);
			}
			else if(topLeftCornerOverlapped && bottomLeftCornerOverlapped)
			{
				Region2D region1(regionToIntersect.posx + regionToIntersect.sizex,
				                 existingRegion.posy,
				                 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
				                 existingRegion.sizey);
				newRegionsToCreate.push_back(region1);
			}
			else //"topRightCornerOverlapped && bottomRightCornerOverlapped" should be the only other case
			{
				Region2D region1(existingRegion.posx,
				                 existingRegion.posy,
				                 regionToIntersect.posx - existingRegion.posx,
				                 existingRegion.sizey);
				newRegionsToCreate.push_back(region1);
			}
		}
		else //"cornerIntersectionCount == 4" should be the only other case we encounter
		{
			//The intersected region completely swallows our region. No regions created.
			//++++++++++
			//+        +
			//+  ====  +
			//+  =  =  +
			//+  ====  +
			//+        +
			//++++++++++
			//Since no regions are created, we have nothing to do in this case.
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool ViewManager::PointWithinRegion(int posx, int posy, const Region2D& region)
{
	return (posx >= region.posx) && (posx < (region.posx + region.sizex))  //Point is within region horizontally
	    && (posy >= region.posy) && (posy < (region.posy + region.sizey)); //Point is within region vertically
}

//----------------------------------------------------------------------------------------
bool ViewManager::RegionIntersectsHorizontalLine(int posx, int posy, int width, const Region2D& region)
{
	return (posy >= region.posy) && (posy < (region.posy + region.sizey))   //The line is overlapping with or adjacent to the region vertically
	   && ((posx <= region.posx) && ((posx + width) > region.posx)          //The line begins at or before the region, and intersects it as it moves right
	    || (posx >= region.posx) && (posx < (region.posx + region.sizex))); //The line begins within the region
}

//----------------------------------------------------------------------------------------
bool ViewManager::RegionIntersectsVerticalLine(int posx, int posy, int height, const Region2D& region)
{
	return (posx >= region.posx) && (posx < (region.posx + region.sizex))   //The line is overlapping with or adjacent to the region horizontally
	   && ((posy <= region.posy) && ((posy + height) > region.posy)         //The line begins at or before the region, and intersects it as it moves down
	    || (posy >= region.posy) && (posy < (region.posy + region.sizey))); //The line begins within the region
}

//----------------------------------------------------------------------------------------
//Child window selection functions
//----------------------------------------------------------------------------------------
std::list<HWND> ViewManager::GetOpenFloatingWindows() const
{
	//Scan all open windows, and build a list of all child windows belonging to our
	//program which are currently open. Note that the windows are listed based on Z-Order,
	//from top to bottom.
	std::list<HWND> windowList;
	EnumThreadWindows(GetCurrentThreadId(), EnumWindowsProc, (LPARAM)&windowList);

	//Build a list of all top-level windows that are currently open and visible, sorted in
	//Z-order.
	std::list<HWND> activeWindowList;
	for(std::list<HWND>::const_iterator windowIterator = windowList.begin(); windowIterator != windowList.end(); ++windowIterator)
	{
		//If the target window is the main window, skip it.
		HWND window = *windowIterator;
		if(window == mainWindow)
		{
			continue;
		}

		//If the target window isn't visible, skip it.
		if(IsWindowVisible(window) == 0)
		{
			continue;
		}

		//Add this window to the list of windows
		activeWindowList.push_back(window);
	}

	//Return the window list to the caller
	return activeWindowList;
}

//----------------------------------------------------------------------------------------
std::list<HWND> ViewManager::GetOpenFloatingDockingWindows() const
{
	//Build a list of all floating docking windows
	std::list<HWND> dockingWindowList;
	std::list<HWND> floatingWindowList = GetOpenFloatingWindows();
	for(std::list<HWND>::const_iterator i = floatingWindowList.begin(); i != floatingWindowList.end(); ++i)
	{
		//Retrieve the class name of the target window
		HWND windowHandle = *i;
		size_t windowClassNameLength = wcslen(DockingWindow::windowClassName);
		size_t classNameBufferSize = windowClassNameLength + 2;
		std::vector<wchar_t> classNameBuffer(classNameBufferSize);
		int getClassNameReturn = GetClassName(windowHandle, &classNameBuffer[0], (int)classNameBufferSize);
		if(getClassNameReturn == 0)
		{
			continue;
		}

		//Ensure the class name of the target window matches the docking window class name
		int stringCompareReturn = wcscmp(DockingWindow::windowClassName, &classNameBuffer[0]);
		if(stringCompareReturn != 0)
		{
			continue;
		}

		//Add this window to the list of docking windows
		dockingWindowList.push_back(windowHandle);
	}

	//Return the docking window list to the caller
	return dockingWindowList;
}

//----------------------------------------------------------------------------------------
BOOL CALLBACK ViewManager::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	std::list<HWND>& windowList = *((std::list<HWND>*)lParam);
	windowList.push_back(hwnd);
	return TRUE;
}

//----------------------------------------------------------------------------------------
//Window procedures
//----------------------------------------------------------------------------------------
LRESULT CALLBACK ViewManager::WndProcMessageWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ViewManager* state = (ViewManager*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_CREATE:
		//Set the object pointer
		state = (ViewManager*)((CREATESTRUCT*)lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
		}
		break;
	case WM_USER:
		if(state != 0)
		{
			//Perform any pending UI thread invocation requests
			if(state->pendingUIThreadInvoke)
			{
				std::unique_lock<std::mutex> lock(state->invokeMutex);
				while(!state->pendingInvokeUIRequests.empty())
				{
					//Retrieve the next invoke request, and remove it from the queue.
					InvokeUIParams invokeParams = state->pendingInvokeUIRequests.front();
					state->pendingInvokeUIRequests.pop_front();
					state->pendingUIThreadInvoke = !state->pendingInvokeUIRequests.empty();

					//Execute this invocation. Note that we MUST take our lock on
					//invokeMutex here again before triggering the condition to signal
					//that the callback is complete, as once the condition is signalled,
					//the condition object may be destroyed by the caller before the
					//notify_all method returns.
					lock.unlock();
					invokeParams.callback();
					lock.lock();
					invokeParams.callbackComplete.notify_all();
				}
			}

			//If we've received a message to check the event queue, process any pending
			//events now if event processing isn't paused.
			if(!state->eventProcessingPaused)
			{
				state->ProcessPendingEvents();
			}
		}
	}

	//Pass this message on for default processing
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT CALLBACK ViewManager::WndProcDialogWindowFrame(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Process the message
	switch(msg)
	{
	case WM_CREATE:{
		//If any windows have been disabled for the lifetime of this dialog, copy the list
		//of disabled windows, and associate it with this window.
		std::list<HWND>* disabledWindowList = (std::list<HWND>*)((CREATESTRUCT*)lparam)->lpCreateParams;
		if(disabledWindowList != NULL)
		{
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(new std::list<HWND>(*disabledWindowList)));
		}
		break;}
	case WM_SIZE:{
		//Resize the hosted window
		HWND hostedWindow = GetWindow(hwnd, GW_CHILD);
		if(hostedWindow != NULL)
		{
			int hostedWindowWidth = (int)LOWORD(lparam);
			int hostedWindowHeight = (int)HIWORD(lparam);
			SetWindowPos(hostedWindow, NULL, 0, 0, hostedWindowWidth, hostedWindowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
		}
		break;}
	case WM_CLOSE:
	case WM_DESTROY:{
		//If any windows have been disabled while this dialog is open, re-enable them now.
		//Note that we attempt to perform this operation in response to a WM_CLOSE message
		//This is done because in order for focus to be correctly restored, our owner
		//window needs to be enabled before our window is destroyed. See the following
		//link for more information:
		//http://blogs.msdn.com/b/oldnewthing/archive/2004/02/27/81155.aspx
		//We should handle the WM_DESTROY message too though, because a window can be
		//closed without a WM_CLOSE message being generated, IE, if DestroyWindow is
		//directly called, so in order to ensure we never have our dialog close without
		//the main window being re-enabled, we ensure that all windows have been
		//re-enabled when a WM_DESTROY message is received too.
		std::list<HWND>* disabledWindowList = (std::list<HWND>*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(disabledWindowList != 0)
		{
			EnableDialogWindows(*disabledWindowList);
			delete disabledWindowList;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
		}
		break;}
	}

	//Pass this message on to the default window procedure function
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//UI thread invocation
//----------------------------------------------------------------------------------------
void ViewManager::InvokeOnUIThread(const std::function<void()>& callback)
{
	//Either execute the supplied callback, or pass it to the UI thread for execution,
	//depending on whether we're already on the UI thread or not.
	unsigned long callingThreadID = GetCurrentThreadId();
	if(callingThreadID == uithreadID)
	{
		//Since we're already on the UI thread, execute the supplied callback directly.
		callback();
	}
	else
	{
		//Construct a set of parameters for this UI thread invocation
		std::condition_variable callbackComplete;
		InvokeUIParams invokeParams(callback, callbackComplete);

		//Add this invoke request to the list of pending invocation requests
		std::unique_lock<std::mutex> lock(invokeMutex);
		pendingInvokeUIRequests.push_back(invokeParams);
		pendingUIThreadInvoke = true;
		PostMessage(messageWindow, WM_USER, 0, 0);

		//Wait for the callback method to complete execution
		callbackComplete.wait(lock);
	}
}
