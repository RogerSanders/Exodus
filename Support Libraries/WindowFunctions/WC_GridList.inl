//----------------------------------------------------------------------------------------
//Message numbers
//----------------------------------------------------------------------------------------
enum WC_GridList::WindowMessages
{
	GRID_WMBASE = WM_USER + 0x100,
	GRID_INSERTCOLUMN,
	GRID_DELETECOLUMN,
	GRID_GETCOLUMNINFO,
	GRID_SETCOLUMNINFO,
	GRID_UPDATECOLUMNTEXT,
	GRID_GETROWCOUNT,
	GRID_SETVSCROLLINFO
};

//----------------------------------------------------------------------------------------
enum WC_GridList::WindowNotifications
{
	GRID_NEWROWCOUNT = 100,
	GRID_SHIFTROWSUP,
	GRID_SHIFTROWSDOWN,
	GRID_NEWSCROLLPOSITION
};

//----------------------------------------------------------------------------------------
//Message parameter structures
//----------------------------------------------------------------------------------------
struct WC_GridList::Grid_InsertColumn
{
	Grid_InsertColumn(const std::wstring& aname = L"", unsigned int acolumnID = 0, unsigned int awidth = 50)
	:name(aname), columnID(acolumnID), width(awidth)
	{}

	std::wstring name;
	unsigned int columnID;
	unsigned int width;
};

//----------------------------------------------------------------------------------------
struct WC_GridList::Grid_SetColumnInfo
{
	Grid_SetColumnInfo()
	:columnID(0), width(50)
	{}

	std::wstring name;
	unsigned int columnID;
	unsigned int width;
};

//----------------------------------------------------------------------------------------
struct WC_GridList::Grid_GetColumnInfo
{
	Grid_GetColumnInfo()
	{}

	std::wstring name;
	unsigned int columnID;
	unsigned int width;
	unsigned int order;
};

//----------------------------------------------------------------------------------------
struct WC_GridList::Grid_SetVScrollInfo
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
struct WC_GridList::Grid_NewRowCount
{
	Grid_NewRowCount()
	{}

	unsigned int visibleRows;
};

//----------------------------------------------------------------------------------------
struct WC_GridList::Grid_ShiftRowsUp
{
	Grid_ShiftRowsUp()
	{}

	unsigned int shiftCount;
};

//----------------------------------------------------------------------------------------
struct WC_GridList::Grid_ShiftRowsDown
{
	Grid_ShiftRowsDown()
	{}

	unsigned int shiftCount;
};

//----------------------------------------------------------------------------------------
struct WC_GridList::Grid_NewScrollPosition
{
	Grid_NewScrollPosition()
	{}

	unsigned int scrollPos;
};
