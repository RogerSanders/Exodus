//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class WC_DataGrid::WindowMessages :unsigned int
{
	WMBase = WM_USER + 0x100,
	SetManualScrolling,
	SetDataAreaFont,
	InsertColumn,
	DeleteColumn,
	InsertRows,
	DeleteRows,
	GetColumnInfo,
	SetColumnInfo,
	SetCellInfo,
	UpdateColumnText,
	UpdateCellText,
	SetControlColor,
	SetRowColor,
	SetCellColor,
	GetRowCount,
	SetVScrollInfo
};

//----------------------------------------------------------------------------------------
enum class WC_DataGrid::WindowNotifications :unsigned int
{
	NewRowCount = 0x100,
	ShiftRowsUp,
	ShiftRowsDown,
	NewScrollPosition,
	SelectionEvent,
	CellEdit,
	CellButtonClick,
	TreeEntryExpandEvent
};

//----------------------------------------------------------------------------------------
enum class WC_DataGrid::CellControlType
{
	TextBox,
	CheckBox,
	ComboBox,
	Button,
	TreeEntry,
	//##TODO## Our "TEXTBOXWITHBUTTON" type restricts us to a single right-aligned button
	//per cell. Consider allowing an optional list of adorner buttons for all column
	//types, which can be aligned either left or right on a button by button basis, and
	//stack from the outermost edge to the innermost edge. With this in place, we can then
	//phase out the "TEXTBOXWITHBUTTON" type.
	TextBoxWithButton
};

//----------------------------------------------------------------------------------------
enum class WC_DataGrid::TextEllipsisMode
{
	None,
	Text,
	Path
};

//----------------------------------------------------------------------------------------
enum class WC_DataGrid::ColumnSizeMode
{
	Absolute,
	SizeToContent,
	Proportional
};

//----------------------------------------------------------------------------------------
//Message parameter structures
//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_InsertColumn
{
	Grid_InsertColumn(const std::wstring& aname = L"", unsigned int acolumnID = 0, bool aeditingAllowed = false, CellControlType acellType = CellControlType::TextBox)
	:name(aname), columnID(acolumnID), editingAllowed(aeditingAllowed), cellType(acellType), sizeMode(ColumnSizeMode::SizeToContent), absoluteWidth(0), proportionalWidth(1.0), minWidth(-1), maxWidth(-1)
	{}

	std::wstring name;
	unsigned int columnID;
	bool editingAllowed;
	CellControlType cellType;
	ColumnSizeMode sizeMode;
	unsigned int absoluteWidth;
	float proportionalWidth;
	int minWidth;
	int maxWidth;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_InsertRows
{
	Grid_InsertRows(unsigned int atargetRowNo = 0, unsigned int arowCount = 0)
	:targetRowNo(atargetRowNo), rowCount(arowCount)
	{}

	unsigned int targetRowNo;
	unsigned int rowCount;
	std::vector<std::map<unsigned int, std::wstring>> rowData;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_DeleteRows
{
	Grid_DeleteRows(unsigned int atargetRowNo = 0, unsigned int arowCount = 0)
	:targetRowNo(atargetRowNo), rowCount(arowCount)
	{}

	unsigned int targetRowNo;
	unsigned int rowCount;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_GetColumnInfo
{
	std::wstring name;
	unsigned int columnID;
	unsigned int width;
	unsigned int order;
	bool editingAllowed;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_SetColumnInfo
{
	Grid_SetColumnInfo()
	:columnID(0), sizeMode(ColumnSizeMode::SizeToContent), absoluteWidth(0), proportionalWidth(0), minWidth(-1), maxWidth(-1)
	{}

	std::wstring name;
	unsigned int columnID;
	bool editingAllowed;
	ColumnSizeMode sizeMode;
	unsigned int absoluteWidth;
	float proportionalWidth;
	int minWidth;
	int maxWidth;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_SetCellInfo
{
	Grid_SetCellInfo()
	:defined(false), columnID(0), rowNo(0), controlType(CellControlType::TextBox), treeEntryIndentLevel(0), showExpandIcon(false), pickFromSelectionListOnly(true), ellipsisMode(TextEllipsisMode::Text)
	{}
	Grid_SetCellInfo(unsigned int acolumnID, unsigned int arowNo, CellControlType acontrolType = CellControlType::TextBox, unsigned int atreeEntryIndentLevel = 0, bool ashowExpandIcon = false, bool aexpandIconIsExpanded = false)
	:defined(true), columnID(acolumnID), rowNo(arowNo), controlType(acontrolType), treeEntryIndentLevel(atreeEntryIndentLevel), showExpandIcon(ashowExpandIcon), expandIconIsExpanded(aexpandIconIsExpanded), pickFromSelectionListOnly(true), ellipsisMode(TextEllipsisMode::Text)
	{}

	bool defined;
	unsigned int columnID;
	unsigned int rowNo;
	CellControlType controlType;
	unsigned int treeEntryIndentLevel;
	bool showExpandIcon;
	bool expandIconIsExpanded;
	std::vector<std::wstring> selectionList;
	bool pickFromSelectionListOnly;
	std::wstring customButtonText;
	TextEllipsisMode ellipsisMode;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_UpdateCellText
{
	Grid_UpdateCellText()
	:columnID(0), rowNo(0)
	{}

	std::wstring newText;
	unsigned int columnID;
	unsigned int rowNo;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_SetVScrollInfo
{
	Grid_SetVScrollInfo()
	{}
	Grid_SetVScrollInfo(int aminPos, int amaxPos, int acurrentPos, int avaluesPerPage)
	:minPos(aminPos), maxPos(amaxPos), currentPos(acurrentPos), valuesPerPage(avaluesPerPage)
	{}

	int minPos;
	int maxPos;
	int currentPos;
	int valuesPerPage;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_SetControlColor
{
	Grid_SetControlColor()
	:backgroundColorDefined(false), textColorDefined(false), editColorDefined(false), lineColorDefined(false)
	{}

	bool backgroundColorDefined;
	WinColor colorBackground;
	bool textColorDefined;
	WinColor colorTextFront;
	WinColor colorTextBack;
	bool editColorDefined;
	WinColor colorEditTextFront;
	WinColor colorEditTextBack;
	bool lineColorDefined;
	WinColor colorLine;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_SetRowColor
{
	Grid_SetRowColor()
	:backgroundColorDefined(false), textColorDefined(false), editColorDefined(false)
	{}

	bool backgroundColorDefined;
	WinColor colorBackground;
	bool textColorDefined;
	WinColor colorTextFront;
	WinColor colorTextBack;
	bool editColorDefined;
	WinColor colorEditTextFront;
	WinColor colorEditTextBack;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_SetCellColor
{
	Grid_SetCellColor()
	:backgroundColorDefined(false), textColorDefined(false), editColorDefined(false)
	{}

	unsigned int targetRowNo;
	unsigned int targetColumnID;
	bool backgroundColorDefined;
	WinColor colorBackground;
	bool textColorDefined;
	WinColor colorTextFront;
	WinColor colorTextBack;
	bool editColorDefined;
	WinColor colorEditTextFront;
	WinColor colorEditTextBack;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_NewRowCount
{
	unsigned int visibleRows;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_ShiftRowsUp
{
	unsigned int shiftCount;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_ShiftRowsDown
{
	unsigned int shiftCount;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_NewScrollPosition
{
	unsigned int scrollPos;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_SelectionEvent
{
	bool sameCellAsPrevious;
	bool rowSelected;
	bool columnSelected;
	unsigned int selectedRowNo;
	unsigned int columnID;
	bool keyPressedCtrl;
	bool keyPressedShift;
	bool ignoreSelectionEvent;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_CellEditEvent
{
	unsigned int targetRowNo;
	unsigned int targetColumnID;
	std::wstring newData;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_CellButtonClickEvent
{
	unsigned int targetRowNo;
	unsigned int targetColumnID;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::Grid_TreeEntryExpandEvent
{
	unsigned int targetRowNo;
	unsigned int targetColumnID;
	bool expand;
};

//----------------------------------------------------------------------------------------
//Internal structures
//----------------------------------------------------------------------------------------
struct WC_DataGrid::ColumnData
{
	ColumnData(const std::wstring& aname, unsigned int acolumnID, bool aeditingAllowed, CellControlType acontrolType)
	:name(aname), columnID(acolumnID), editingAllowed(aeditingAllowed), controlType(acontrolType), actualWidth(0), manuallySettingColumnWidth(false)
	{}

	std::wstring name;
	unsigned int columnID;
	unsigned int order;
	bool editingAllowed;
	CellControlType controlType;
	int index;
	std::vector<std::wstring> dataBuffer;
	ColumnSizeMode sizeMode;
	unsigned int absoluteWidth;
	float proportionalWidth;
	int minWidth;
	int maxWidth;
	unsigned int actualWidth;
	bool manuallySettingColumnWidth;
};

//----------------------------------------------------------------------------------------
struct WC_DataGrid::CellControlInfo
{
	CellControlInfo(CellControlType acontrolType = CellControlType::TextBox)
	:controlType(acontrolType), createdControlType(CellControlType::TextBox), hwnd(NULL), lastBackgroundColorSet(false), lastBackgroundColorBrush(NULL), pickFromSelectionListOnly(true), ellipsisMode(TextEllipsisMode::Text)
	{}

	CellControlType controlType;
	CellControlType createdControlType;
	unsigned int treeEntryIndentLevel;
	bool showExpandIcon;
	bool expandIconIsExpanded;
	bool lastBackgroundColorSet;
	COLORREF lastBackgroundColor;
	HBRUSH lastBackgroundColorBrush;
	HWND hwnd;
	int cellPosX;
	int cellPosY;
	int cellWidth;
	int cellHeight;
	int controlPosX;
	int controlPosY;
	int controlWidth;
	int controlHeight;
	std::vector<std::wstring> selectionList;
	bool pickFromSelectionListOnly;
	std::wstring customButtonText;
	TextEllipsisMode ellipsisMode;
};
