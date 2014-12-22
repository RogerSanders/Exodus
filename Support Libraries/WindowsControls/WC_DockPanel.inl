//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class WC_DockPanel::WindowMessages :unsigned int
{
	WMBase = WM_USER + 0x100,
	AddContentWindow,
	RemoveContentWindow,
	AddDockedWindow,
	AddDockedWindowToFront,
	RemoveDockedWindow,
	GetDockedWindowDesiredWidth,
	SetDockedWindowDesiredWidth,
	GetDockedWindowDesiredHeight,
	SetDockedWindowDesiredHeight,
	GetContentRect,
	CalculateNewDockedWindowRect
};

//----------------------------------------------------------------------------------------
enum class WC_DockPanel::DockLocation
{
	Left,
	Right,
	Top,
	Bottom
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct WC_DockPanel::CalculateNewDockedWindowRectParams
{
	DockLocation dockLocation;
	bool addToFront;
	int preferredWidth;
	int preferredHeight;
	RECT rect;
};

//----------------------------------------------------------------------------------------
struct WC_DockPanel::DockedWindowEntry
{
	HWND dockedWindow;
	DockLocation dockLocation;
	int desiredWindowWidth;
	int desiredWindowHeight;
	int windowPosX;
	int windowPosY;
	int windowWidth;
	int windowHeight;
};
