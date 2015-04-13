//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class DashboardWindow::WindowMessages :unsigned int
{
	WMBase = WM_USER + 0x100,
	AddContentWindow,
	RemoveContentWindow,
	GetRegionCount,
	GetRegionWindow,
	SetRegionWindow,
	RemoveAllRegions,
	SaveLayoutToDividerList,
	LoadLayoutFromDividerList
};

//----------------------------------------------------------------------------------------
enum class DashboardWindow::InsertDirection
{
	Top,
	Bottom,
	Left,
	Right
};

//----------------------------------------------------------------------------------------
enum class DashboardWindow::DockTargetPos
{
	Left,
	Right,
	Top,
	Bottom,
	Center
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct DashboardWindow::AddWindowParams
{
	AddWindowParams(HWND awindowHandle)
	:windowHandle(awindowHandle), existingWindow(NULL)
	{}
	AddWindowParams(HWND awindowHandle, HWND aexistingWindow, InsertDirection ainsertDirection, double aregionProportion = 0.5)
	:windowHandle(awindowHandle), existingWindow(aexistingWindow), insertDirection(ainsertDirection), regionProportion(aregionProportion)
	{}

	HWND windowHandle;
	HWND existingWindow;
	InsertDirection insertDirection;
	double regionProportion;
};

//----------------------------------------------------------------------------------------
struct DashboardWindow::DividerListEntry
{
	int dividerID;
	int parentDividerID;
	int dividerDistanceAlongParent;
};

//----------------------------------------------------------------------------------------
struct DashboardWindow::DividerContentEntry
{
	DividerContentEntry()
	{}
	DividerContentEntry(ContentRegion* acontentRegion, Divider* afollowingContentDivider)
	:contentRegion(acontentRegion), followingContentDivider(afollowingContentDivider)
	{}

	ContentRegion* contentRegion;
	Divider* followingContentDivider;
};

//----------------------------------------------------------------------------------------
struct DashboardWindow::Divider
{
	Divider* startAnchorDivider;
	Divider* endAnchorDivider;
	std::list<DividerContentEntry> precedingContent;
	std::list<DividerContentEntry> followingContent;
	bool vertical;
	int cachedPosX;
	int cachedPosY;
	int cachedWidth;
	int cachedHeight;
};

//----------------------------------------------------------------------------------------
struct DashboardWindow::ContentRegion
{
	ContentRegion()
	:windowHandle(NULL), windowAsDockingWindow(0), leftDivider(0), rightDivider(0), topDivider(0), bottomDivider(0), width(0), height(0), cachedPosX(-1), cachedPosY(-1), windowWidth(-1), windowHeight(-1), windowPosX(-1), windowPosY(-1)
	{}

	HWND windowHandle;
	IDockingWindow* windowAsDockingWindow;
	Divider* leftDivider;
	Divider* rightDivider;
	Divider* topDivider;
	Divider* bottomDivider;
	int width;
	int height;
	int cachedPosX;
	int cachedPosY;
	int windowWidth;
	int windowHeight;
	int windowPosX;
	int windowPosY;
};

//----------------------------------------------------------------------------------------
struct DashboardWindow::DashboardWindowDropTargetInfo :public IDockingWindowDropTargetInfo
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
	HWND existingWindow;
	int targetRegionNo;
	InsertDirection insertDirection;
	double regionProportion;
	HWND owningDockingWindow;
};

//----------------------------------------------------------------------------------------
struct DashboardWindow::DividerSplitPosition
{
	DividerSplitPosition()
	:mergeRequired(false), dividerSplitterStartPos(0), dividerSplitterLength(0), startPrecedingMergeDivider(0), startFollowingMergeDivider(0), endPrecedingMergeDivider(0), endFollowingMergeDivider(0)
	{ }

	bool mergeRequired;
	int dividerSplitterStartPos;
	int dividerSplitterLength;
	Divider* startPrecedingMergeDivider;
	Divider* startFollowingMergeDivider;
	Divider* endPrecedingMergeDivider;
	Divider* endFollowingMergeDivider;
};

//----------------------------------------------------------------------------------------
struct DashboardWindow::DividerExtendButtonPosition
{
	DividerExtendButtonPosition()
	:regionToTruncate(0), dividerExtendButtonStartPos(0), dividerExtendButtonLength(0), extendFromStart(true), extendPrecedingContent(true)
	{ }

	ContentRegion* regionToTruncate;
	int dividerExtendButtonStartPos;
	int dividerExtendButtonLength;
	bool extendFromStart;
	bool extendPrecedingContent;
};
