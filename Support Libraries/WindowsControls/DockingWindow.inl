//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class DockingWindow::WindowMessages :unsigned int
{
	WMBase = WM_USER + 0x100,
	AddContentWindow,
	RemoveContentWindow,
	ModifyContentWindow,
	AddDockedWindow,
	RemoveDockedWindow,
	GetContentWindowIndexFromHandle,
	GetDockedWindowIndexFromHandle,
	GetContentWindowCount,
	GetContentWindowInfo,
	GetDockedWindowCount,
	GetDockedWindowInfo,
	GetActiveContentWindow,
	SetActiveContentWindow
};

//----------------------------------------------------------------------------------------
enum class DockingWindow::ContentWindowInfo :unsigned int
{
	Handle = 0x00000001,
	Title  = 0x00000002
};
inline DockingWindow::ContentWindowInfo operator&(DockingWindow::ContentWindowInfo a, DockingWindow::ContentWindowInfo b) { return (DockingWindow::ContentWindowInfo)((unsigned int)a & (unsigned int)b); }
inline DockingWindow::ContentWindowInfo operator|(DockingWindow::ContentWindowInfo a, DockingWindow::ContentWindowInfo b) { return (DockingWindow::ContentWindowInfo)((unsigned int)a | (unsigned int)b); }
inline bool operator!=(DockingWindow::ContentWindowInfo a, unsigned int b) { return (unsigned int)a != b; }
inline bool operator!=(unsigned int a, DockingWindow::ContentWindowInfo b) { return (unsigned int)a != b; }
inline bool operator==(DockingWindow::ContentWindowInfo a, unsigned int b) { return (unsigned int)a == b; }
inline bool operator==(unsigned int a, DockingWindow::ContentWindowInfo b) { return (unsigned int)a == b; }

//----------------------------------------------------------------------------------------
enum class DockingWindow::DockedWindowInfo :unsigned int
{
	Handle = 0x00000001
};
inline DockingWindow::DockedWindowInfo operator&(DockingWindow::DockedWindowInfo a, DockingWindow::DockedWindowInfo b) { return (DockingWindow::DockedWindowInfo)((unsigned int)a & (unsigned int)b); }
inline DockingWindow::DockedWindowInfo operator|(DockingWindow::DockedWindowInfo a, DockingWindow::DockedWindowInfo b) { return (DockingWindow::DockedWindowInfo)((unsigned int)a | (unsigned int)b); }
inline bool operator!=(DockingWindow::DockedWindowInfo a, unsigned int b) { return (unsigned int)a != b; }
inline bool operator!=(unsigned int a, DockingWindow::DockedWindowInfo b) { return (unsigned int)a != b; }
inline bool operator==(DockingWindow::DockedWindowInfo a, unsigned int b) { return (unsigned int)a == b; }
inline bool operator==(unsigned int a, DockingWindow::DockedWindowInfo b) { return (unsigned int)a == b; }

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
	:hwnd(NULL), dockLocation(WindowEdge::Left), forceToTopOfDockingOrder(false), autoHide(false)
	{}

	HWND hwnd;
	WindowEdge dockLocation;
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
	WindowEdge dockLocation;
	bool autoHide;
	int desiredWidth;
	int desiredHeight;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::ChildContainerEntry
{
	IDockingWindow* childContainer;
	WindowEdge dockLocation;
	bool autoHide;
};

//----------------------------------------------------------------------------------------
struct DockingWindow::ContentEntry
{
	ContentEntry()
	:contentWindow(NULL), tabIndex(-1), removedWindowStyles(0), contentWindowAsDockingWindow(0)
	{}

	HWND contentWindow;
	std::wstring contentTitle;
	int tabIndex;
	DWORD removedWindowStyles;
	IDockingWindow* contentWindowAsDockingWindow;
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
	WindowEdge dockLocation;
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

//----------------------------------------------------------------------------------------
struct DockingWindow::DockingWindowDropTargetInfo :public IDockingWindowDropTargetInfo
{
public:
	//Constructors
	virtual void Destroy()
	{
		delete this;
	}

	//Info functions
	virtual HWND GetOwningDockingWindow()
	{
		return owningDockingWindow;
	}

public:
	bool dockLocationIsContentRegion;
	WindowEdge dockLocation;
	bool forceTop;
	bool autoHide;
	HWND owningDockingWindow;
};
