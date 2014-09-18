//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum WC_DockPanel::DockLocation
{
	DOCKLOCATION_LEFT,
	DOCKLOCATION_RIGHT,
	DOCKLOCATION_TOP,
	DOCKLOCATION_BOTTOM
};

//----------------------------------------------------------------------------------------
enum WC_DockPanel::WindowMessages
{
	DOCK_WMBASE = WM_USER + 0x100,
	DOCK_ADDCONTENTWINDOW,
	DOCK_REMOVECONTENTWINDOW,
	DOCK_ADDDOCKEDWINDOW,
	DOCK_ADDDOCKEDWINDOWTOFRONT,
	DOCK_REMOVEDOCKEDWINDOW,
	DOCK_GETDOCKEDWINDOWDESIREDWIDTH,
	DOCK_SETDOCKEDWINDOWDESIREDWIDTH,
	DOCK_GETDOCKEDWINDOWDESIREDHEIGHT,
	DOCK_SETDOCKEDWINDOWDESIREDHEIGHT,
	DOCK_GETCONTENTRECT,
	DOCK_CALCULATENEWDOCKEDWINDOWRECT
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
