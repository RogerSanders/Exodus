//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum DockingWindow::WindowMessages
{
	DOCKWIN_WMBASE = WM_USER + 0x100,
	DOCKWIN_ADDCONTENTWINDOW,
	DOCKWIN_REMOVECONTENTWINDOW,
	DOCKWIN_MODIFYCONTENTWINDOW,
	DOCKWIN_ADDDOCKEDWINDOW,
	DOCKWIN_REMOVEDOCKEDWINDOW,
	DOCKWIN_GETCONTENTWINDOWINDEXFROMHANDLE,
	DOCKWIN_GETDOCKEDWINDOWINDEXFROMHANDLE,
	DOCKWIN_GETCONTENTWINDOWCOUNT,
	DOCKWIN_GETCONTENTWINDOWINFO,
	DOCKWIN_GETDOCKEDWINDOWCOUNT,
	DOCKWIN_GETDOCKEDWINDOWINFO,
	DOCKWIN_GETACTIVECONTENTWINDOW,
	DOCKWIN_SETACTIVECONTENTWINDOW
};

//----------------------------------------------------------------------------------------
enum DockingWindow::ContentWindowInfo
{
	CONTENTWINDOWINFO_HANDLE = 0x00000001,
	CONTENTWINDOWINFO_TITLE  = 0x00000002
};

//----------------------------------------------------------------------------------------
enum DockingWindow::DockedWindowInfo
{
	DOCKEDWINDOWINFO_HANDLE = 0x00000001
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct DockingWindow::AddContentWindowParams
{
	HWND hwnd;
	const wchar_t* windowTitle;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::ModifyContentWindowParams
{
	ContentWindowInfo contentWindowInfo;
	HWND hwnd;
	const wchar_t* windowTitle;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::AddDockedWindowParams
{
	AddDockedWindowParams()
	:hwnd(NULL), dockLocation(WC_DockPanel::DOCKLOCATION_LEFT), forceToTopOfDockingOrder(false), autoHide(false)
	{}

	HWND hwnd;
	WC_DockPanel::DockLocation dockLocation;
	bool forceToTopOfDockingOrder;
	bool autoHide;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::GetContentWindowInfo
{
	GetContentWindowInfo()
	:hwnd(NULL), windowTitle(0), tabIndex(0)
	{}

	HWND hwnd;
	const wchar_t* windowTitle;
	unsigned int contentIndex;
	int tabIndex;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::GetDockedWindowInfo
{
	GetDockedWindowInfo()
	:hwnd(NULL)
	{}

	HWND hwnd;
	WC_DockPanel::DockLocation dockLocation;
	bool autoHide;
	int desiredWidth;
	int desiredHeight;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::ChildContainerEntry
{
	IDockingWindow* childContainer;
	WC_DockPanel::DockLocation dockLocation;
	bool autoHide;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::ContentEntry
{
	HWND contentWindow;
	std::wstring contentTitle;
	int tabIndex;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::AutoHideDockTab
{
	AutoHideDockTab()
	:contentWindow(NULL), tabPosX(0), tabPosY(0), tabWidth(0), tabHeight(0)
	{}

	HWND contentWindow;
	int tabPosX;
	int tabPosY;
	int tabWidth;
	int tabHeight;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::AutoHideDockTabGroup
{
	AutoHideDockTabGroup()
	:childContainer(0)
	{}

	IDockingWindow* childContainer;
	std::list<AutoHideDockTab> dockTabs;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::AutoHideDockInfo
{
	AutoHideDockInfo()
	:dockWindow(NULL), tabRowCount(1)
	{}

	HWND dockWindow;
	int tabRowCount;
	WC_DockPanel::DockLocation dockLocation;
	std::list<AutoHideDockTabGroup> dockTabGroups;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::TabRenderInfo
{
	TabRenderInfo(AutoHideDockTab& adockTabInfo, const std::wstring& awindowTitle)
	:dockTabInfo(adockTabInfo), windowTitle(awindowTitle)
	{}

	AutoHideDockTab& dockTabInfo;
	std::wstring windowTitle;
	int tabPosX;
	int tabPosY;
	int tabWidth;
	int tabHeight;
	int borderPosX;
	int borderPosY;
	int borderWidth;
	int borderHeight;
};
