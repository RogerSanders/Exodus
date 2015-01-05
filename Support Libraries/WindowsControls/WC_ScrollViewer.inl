//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class WC_ScrollViewer::WindowMessages :unsigned int
{
	WMBase = WM_USER + 0x100,
	AddWindow,
	RemoveWindow
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct WC_ScrollViewer::HostedWindowInfo
{
	HWND windowHandle;
	int currentSizeX;
	int currentSizeY;
};
