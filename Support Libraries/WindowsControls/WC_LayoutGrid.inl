//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class WC_LayoutGrid::WindowMessages :unsigned int
{
	WMBase = WM_USER + 0x100,
	AddRow,
	AddColumn,
	AddWindow,
	RemoveWindow,
	UpdateWindowSizes
};

//----------------------------------------------------------------------------------------
enum class WC_LayoutGrid::SizeMode
{
	Fixed,
	Proportional,
	Content
};

//----------------------------------------------------------------------------------------
enum class WC_LayoutGrid::WindowSizeMode
{
	Fixed,
	Proportional
};

//----------------------------------------------------------------------------------------
enum class WC_LayoutGrid::VerticalAlignment
{
	Top,
	Bottom,
	Centre
};

//----------------------------------------------------------------------------------------
enum class WC_LayoutGrid::HorizontalAlignment
{
	Left,
	Right,
	Centre
};

//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct WC_LayoutGrid::AddRowParams
{
	AddRowParams(double aproportionalRatio, int aminHeight = -1, int amaxHeight = -1)
	:minHeight(aminHeight), maxHeight(amaxHeight), sizeMode(SizeMode::Proportional), proportionalRatio(aproportionalRatio)
	{}
	AddRowParams(SizeMode asizeMode, int aminHeight = -1, int amaxHeight = -1)
	:minHeight(aminHeight), maxHeight(amaxHeight), sizeMode(asizeMode), proportionalRatio(1.0)
	{}

	int minHeight;
	int maxHeight;
	SizeMode sizeMode;
	double proportionalRatio;
};

//----------------------------------------------------------------------------------------
struct WC_LayoutGrid::AddColumnParams
{
	AddColumnParams(SizeMode asizeMode, double aproportionalRatio = 1.0, int aminWidth = -1, int amaxWidth = -1)
	:minWidth(aminWidth), maxWidth(amaxWidth), sizeMode(asizeMode), proportionalRatio(aproportionalRatio)
	{}

	int minWidth;
	int maxWidth;
	SizeMode sizeMode;
	double proportionalRatio;
};

//----------------------------------------------------------------------------------------
struct WC_LayoutGrid::AddWindowParams
{
	AddWindowParams(HWND awindowHandle, int arowNo = 0, int acolumnNo = 0, WindowSizeMode ahorizontalSizeMode = WindowSizeMode::Proportional, WindowSizeMode averticalSizeMode = WindowSizeMode::Proportional, HorizontalAlignment ahorizontalAlignment = HorizontalAlignment::Left, VerticalAlignment averticalAlignment = VerticalAlignment::Top, int arowSpan = 1, int acolumnSpan = 1)
	:windowHandle(awindowHandle), rowNo(arowNo), columnNo(acolumnNo), rowSpan(arowSpan), columnSpan(acolumnSpan), horizontalSizeMode(ahorizontalSizeMode), verticalSizeMode(averticalSizeMode), rowProportionStart(1.0), rowProportionEnd(1.0), columnProportionStart(1.0), columnProportionEnd(1.0), paddingLeft(0), paddingRight(0), paddingTop(0), paddingBottom(0), minWidth(-1), minHeight(-1), maxWidth(-1), maxHeight(-1)
	{
		horizontalAlignmentDisplacement = (ahorizontalAlignment == HorizontalAlignment::Right)? 1.0: ((ahorizontalAlignment == HorizontalAlignment::Centre)? 0.5: 0.0);
		verticalAlignmentDisplacement = (averticalAlignment == VerticalAlignment::Bottom)? 1.0: ((averticalAlignment == VerticalAlignment::Centre)? 0.5: 0.0);
	}

	HWND windowHandle;
	unsigned int rowNo;
	unsigned int columnNo;
	unsigned int rowSpan;
	unsigned int columnSpan;
	WindowSizeMode horizontalSizeMode;
	WindowSizeMode verticalSizeMode;
	double horizontalAlignmentDisplacement;
	double verticalAlignmentDisplacement;
	double rowProportionStart;
	double rowProportionEnd;
	double columnProportionStart;
	double columnProportionEnd;
	int paddingLeft;
	int paddingRight;
	int paddingTop;
	int paddingBottom;
	int minWidth;
	int minHeight;
	int maxWidth;
	int maxHeight;
};

//----------------------------------------------------------------------------------------
struct WC_LayoutGrid::UpdateWindowSizesParams
{
	UpdateWindowSizesParams(HWND awindowHandle, HorizontalAlignment ahorizontalAlignment = HorizontalAlignment::Left, VerticalAlignment averticalAlignment = VerticalAlignment::Top, int apaddingLeft = 0, int apaddingRight = 0, int apaddingTop = 0, int apaddingBottom = 0, int aminWidth = -1, int aminHeight = -1, int amaxWidth = -1, int amaxHeight = -1)
	:windowHandle(awindowHandle), paddingLeft(apaddingLeft), paddingRight(apaddingRight), paddingTop(apaddingTop), paddingBottom(apaddingBottom), minWidth(aminWidth), minHeight(aminHeight), maxWidth(amaxWidth), maxHeight(amaxHeight)
	{
		horizontalAlignmentDisplacement = (ahorizontalAlignment == HorizontalAlignment::Right)? 1.0: ((ahorizontalAlignment == HorizontalAlignment::Centre)? 0.5: 0.0);
		verticalAlignmentDisplacement = (averticalAlignment == VerticalAlignment::Bottom)? 1.0: ((averticalAlignment == VerticalAlignment::Centre)? 0.5: 0.0);
	}

	HWND windowHandle;
	double horizontalAlignmentDisplacement;
	double verticalAlignmentDisplacement;
	int paddingLeft;
	int paddingRight;
	int paddingTop;
	int paddingBottom;
	int minWidth;
	int minHeight;
	int maxWidth;
	int maxHeight;
};

//----------------------------------------------------------------------------------------
struct WC_LayoutGrid::RowInfo
{
	unsigned int rowNo;
	bool defaultEntry;
	int minHeight;
	int maxHeight;
	SizeMode sizeMode;
	double proportionalRatio;
	int currentPosY;
	int currentHeight;
};

//----------------------------------------------------------------------------------------
struct WC_LayoutGrid::ColumnInfo
{
	unsigned int columnNo;
	bool defaultEntry;
	int minWidth;
	int maxWidth;
	SizeMode sizeMode;
	double proportionalRatio;
	int currentPosX;
	int currentWidth;
};

//----------------------------------------------------------------------------------------
struct WC_LayoutGrid::HostedWindowInfo
{
	HWND windowHandle;
	unsigned int rowNo;
	unsigned int columnNo;
	unsigned int rowSpan;
	unsigned int columnSpan;
	WindowSizeMode horizontalSizeMode;
	WindowSizeMode verticalSizeMode;
	double horizontalAlignmentDisplacement;
	double verticalAlignmentDisplacement;
	double rowProportionStart;
	double rowProportionEnd;
	double columnProportionStart;
	double columnProportionEnd;
	int paddingLeft;
	int paddingRight;
	int paddingTop;
	int paddingBottom;
	int minWidth;
	int minHeight;
	int maxWidth;
	int maxHeight;
	int currentPosX;
	int currentPosY;
	int currentSizeX;
	int currentSizeY;
};
