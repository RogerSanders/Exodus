//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class WC_StackPanel::WindowMessages :unsigned int
{
	WMBase = WM_USER + 0x100,
	AddWindow,
	InsertWindow,
	RemoveWindow,
	SetDirection,
	SetWrappingEnabled,
	SetAlignment,
	SetSizeMode,
	SetVerticalPadding,
	SetHorizontalPadding
};

//----------------------------------------------------------------------------------------
enum class WC_StackPanel::StackDirection
{
	Horizontal,
	Vertical
};

//----------------------------------------------------------------------------------------
enum class WC_StackPanel::VerticalAlignment
{
	Top,
	Bottom,
	Centre
};

//----------------------------------------------------------------------------------------
enum class WC_StackPanel::HorizontalAlignment
{
	Left,
	Right,
	Centre
};

//----------------------------------------------------------------------------------------
enum class WC_StackPanel::SizeMode
{
	Fixed,
	StackRowSize,
	ContentWidth,
	ContentHeight,
	ContentWidthAndHeight,
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct WC_StackPanel::SetAlignmentParams
{
	SetAlignmentParams(HorizontalAlignment ahorizontalAlignment, VerticalAlignment averticalAlignment)
	{
		horizontalAlignmentDisplacement = (ahorizontalAlignment == HorizontalAlignment::Right)? 1.0: ((ahorizontalAlignment == HorizontalAlignment::Centre)? 0.5: 0.0);
		verticalAlignmentDisplacement = (averticalAlignment == VerticalAlignment::Bottom)? 1.0: ((averticalAlignment == VerticalAlignment::Centre)? 0.5: 0.0);
	}
	SetAlignmentParams(double ahorizontalAlignmentDisplacement, double averticalAlignmentDisplacement)
	{
		horizontalAlignmentDisplacement = ahorizontalAlignmentDisplacement;
		verticalAlignmentDisplacement = averticalAlignmentDisplacement;
	}

	double horizontalAlignmentDisplacement;
	double verticalAlignmentDisplacement;
};

//----------------------------------------------------------------------------------------
struct WC_StackPanel::HostedWindowInfo
{
	HWND windowHandle;
	int currentSizeX;
	int currentSizeY;
};

//----------------------------------------------------------------------------------------
struct WC_StackPanel::RowInfo
{
	RowInfo()
	:rowStackDirectionSize(0), rowNonStackDirectionSize(0)
	{}

	std::list<unsigned int> hostedWindows;
	int rowStackDirectionSize;
	int rowNonStackDirectionSize;
};
