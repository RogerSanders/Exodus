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
	UpdateWindowSizes,
	RecalculateSizes
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
	AddWindowParams(HWND awindowHandle, int arowNo = 0, int acolumnNo = 0)
	:windowHandle(awindowHandle), rowNo(arowNo), columnNo(acolumnNo), rowSpan(1), columnSpan(1), horizontalSizeMode(WindowSizeMode::Proportional), verticalSizeMode(WindowSizeMode::Proportional), horizontalAlignmentDisplacement(0.0), verticalAlignmentDisplacement(0.0), rowProportionStart(1.0), rowProportionEnd(1.0), columnProportionStart(1.0), columnProportionEnd(1.0), paddingLeft(0), paddingRight(0), paddingTop(0), paddingBottom(0), minWidth(-1), minHeight(-1), maxWidth(-1), maxHeight(-1)
	{}

	AddWindowParams& SetSpan(int arowSpan, int acolumnSpan)
	{
		rowSpan = arowSpan;
		columnSpan = acolumnSpan;
		return *this;
	}
	AddWindowParams& SetSizeMode(WindowSizeMode ahorizontalAndVerticalSizeMode)
	{
		horizontalSizeMode = ahorizontalAndVerticalSizeMode;
		verticalSizeMode = ahorizontalAndVerticalSizeMode;
		return *this;
	}
	AddWindowParams& SetSizeMode(WindowSizeMode ahorizontalSizeMode, WindowSizeMode averticalSizeMode)
	{
		horizontalSizeMode = ahorizontalSizeMode;
		verticalSizeMode = averticalSizeMode;
		return *this;
	}
	AddWindowParams& SetAlignment(HorizontalAlignment ahorizontalAlignment, VerticalAlignment averticalAlignment)
	{
		horizontalAlignmentDisplacement = (ahorizontalAlignment == HorizontalAlignment::Right)? 1.0: ((ahorizontalAlignment == HorizontalAlignment::Centre)? 0.5: 0.0);
		verticalAlignmentDisplacement = (averticalAlignment == VerticalAlignment::Bottom)? 1.0: ((averticalAlignment == VerticalAlignment::Centre)? 0.5: 0.0);
		return *this;
	}
	AddWindowParams& SetAlignment(double ahorizontalAlignmentDisplacement, double averticalAlignmentDisplacement)
	{
		horizontalAlignmentDisplacement = ahorizontalAlignmentDisplacement;
		verticalAlignmentDisplacement = averticalAlignmentDisplacement;
		return *this;
	}
	AddWindowParams& SetPadding(int apadding)
	{
		paddingLeft = apadding;
		paddingRight = apadding;
		paddingTop = apadding;
		paddingBottom = apadding;
		return *this;
	}
	AddWindowParams& SetPadding(int apaddingHorizontal, int apaddingVertical)
	{
		paddingLeft = apaddingHorizontal;
		paddingRight = apaddingHorizontal;
		paddingTop = apaddingVertical;
		paddingBottom = apaddingVertical;
		return *this;
	}
	AddWindowParams& SetPadding(int apaddingLeft, int apaddingRight, int apaddingTop, int apaddingBottom)
	{
		paddingLeft = apaddingLeft;
		paddingRight = apaddingRight;
		paddingTop = apaddingTop;
		paddingBottom = apaddingBottom;
		return *this;
	}
	AddWindowParams& SetMinSize(int aminWidth, int aminHeight)
	{
		minWidth = aminWidth;
		minHeight = aminHeight;
		return *this;
	}
	AddWindowParams& SetMaxSize(int amaxWidth, int amaxHeight)
	{
		maxWidth = amaxWidth;
		maxHeight = amaxHeight;
		return *this;
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
	UpdateWindowSizesParams(HWND awindowHandle)
	:windowHandle(awindowHandle), horizontalAlignmentDisplacement(0.0), verticalAlignmentDisplacement(0.0), paddingLeft(0), paddingRight(0), paddingTop(0), paddingBottom(0), minWidth(-1), minHeight(-1), maxWidth(-1), maxHeight(-1)
	{}

	UpdateWindowSizesParams& SetAlignment(HorizontalAlignment ahorizontalAlignment, VerticalAlignment averticalAlignment)
	{
		horizontalAlignmentDisplacement = (ahorizontalAlignment == HorizontalAlignment::Right)? 1.0: ((ahorizontalAlignment == HorizontalAlignment::Centre)? 0.5: 0.0);
		verticalAlignmentDisplacement = (averticalAlignment == VerticalAlignment::Bottom)? 1.0: ((averticalAlignment == VerticalAlignment::Centre)? 0.5: 0.0);
		return *this;
	}
	UpdateWindowSizesParams& SetAlignment(double ahorizontalAlignmentDisplacement, double averticalAlignmentDisplacement)
	{
		horizontalAlignmentDisplacement = ahorizontalAlignmentDisplacement;
		verticalAlignmentDisplacement = averticalAlignmentDisplacement;
		return *this;
	}
	UpdateWindowSizesParams& SetPadding(int apaddingLeft, int apaddingRight, int apaddingTop, int apaddingBottom)
	{
		paddingLeft = apaddingLeft;
		paddingRight = apaddingRight;
		paddingTop = apaddingTop;
		paddingBottom = apaddingBottom;
		return *this;
	}
	UpdateWindowSizesParams& SetMinSize(int aminWidth, int aminHeight)
	{
		minWidth = aminWidth;
		minHeight = aminHeight;
		return *this;
	}
	UpdateWindowSizesParams& SetMaxSize(int amaxWidth, int amaxHeight)
	{
		maxWidth = amaxWidth;
		maxHeight = amaxHeight;
		return *this;
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
