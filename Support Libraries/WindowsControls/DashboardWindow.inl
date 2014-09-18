//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum DashboardWindow::InsertDirection
{
	INSERTDIRECTON_TOP,
	INSERTDIRECTON_BOTTOM,
	INSERTDIRECTON_LEFT,
	INSERTDIRECTON_RIGHT
};

//----------------------------------------------------------------------------------------
//Structures
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
	int width;
	int height;
	int cachedPosX;
	int cachedPosY;
	HWND boundWindow;
	Divider* leftDivider;
	Divider* rightDivider;
	Divider* topDivider;
	Divider* bottomDivider;
};
