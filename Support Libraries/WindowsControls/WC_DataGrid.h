/*----------------------------------------------------------------------------                              ----------*\
Things to do:
-Build a system for custom text highlighting mid-line. Some kind of text markup language
 applied to the source string will probably be required. This should be supported using a
 new column type, possibly "CellControlType::CELLCONTROLTYPE_FORMATTEDTEXTBOX".
-Consider implementing a context menu for the header control, to enable and disable
 columns via tickboxes on the context menu.
-Add optional row headers
-Allow variable row heights
-Add selection of cells as a separate concept from editing cells, so we can highlight for
 the purposes of copying, cutting, and pasting, as well as keyboard navigation of the
 content.
-Make the paint process more efficient by using the invalidated region of the control to
 determine which cells to redraw. Switch to a double buffered window rather than drawing
 to a bitmap and blitting it to the window in order to support flicker free partial
 updates of the rendered region.
\*--------------------------------------------------------------------------------------------------------------------*/
#ifndef __WC_DATAGRID_H__
#define __WC_DATAGRID_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <list>
#include <map>
#include <vector>
#include <set>

class WC_DataGrid
{
public:
	// Enumerations
	enum class WindowMessages :unsigned int;
	enum class WindowNotifications :unsigned int;
	enum class CellControlType;
	enum class TextEllipsisMode;
	enum class ColumnSizeMode;

	// Message parameter structures
	//##TODO## Remove the "Grid_" prefixes from these structures, and consider adding a
	// standard postfix identifying these as message parameter structures.
	struct Grid_InsertColumn;
	struct Grid_InsertRows;
	struct Grid_DeleteRows;
	struct Grid_GetColumnInfo;
	struct Grid_SetColumnInfo;
	struct Grid_SetCellInfo;
	struct Grid_UpdateCellText;
	struct Grid_SetVScrollInfo;
	struct Grid_SetControlColor;
	struct Grid_SetRowColor;
	struct Grid_SetCellColor;
	struct Grid_NewVisibleRowCount;
	struct Grid_ShiftRowsUp;
	struct Grid_ShiftRowsDown;
	struct Grid_NewScrollPosition;
	struct Grid_SelectionEvent;
	struct Grid_CellEditEvent;
	struct Grid_CellButtonClickEvent;
	struct Grid_TreeEntryExpandEvent;

	// Constants
	static const wchar_t* WindowClassName;

public:
	// Constructors
	WC_DataGrid(HINSTANCE moduleHandle, HWND hwnd);
	~WC_DataGrid();

	// Class registration
	static bool RegisterWindowClass(HINSTANCE moduleHandle);
	static bool UnregisterWindowClass(HINSTANCE moduleHandle);

	// Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	// Internal structures
	struct ColumnData;
	struct CustomColorData
	{
		CustomColorData()
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
	struct CellControlInfo;

	// Constants
	static const long long HeaderControlID = 0x101;

private:
	// Header functions
	void RebuildHeaderOrder();

	// Window size functions
	unsigned int GetTotalRowWidth() const;
	void UpdateWindowSize();
	unsigned int CalculateLargestColumnDataArraySize();
	void RecalculateScrollPosition();
	void UpdateScrollbarSettings(int newScrollHOffset, int newScrollVOffset, bool controlSizeChanged);
	void ForceControlRedraw();
	void RecalculateColumnWidths();

	// Edit functions
	bool IsCellEditingActive() const;
	bool BeginCellEditing(int cursorPosInPixels);
	void UpdateCellEditingPos(int cursorPosInPixels);
	void CompleteCellEditing();
	void CancelCellEditing();
	void SelectCharacter(int newCharacterPos);
	static size_t PerformWordSearchForNextBoundary(size_t searchStartPos, size_t searchEndPos, const std::wstring& text);
	void ForceEditCellIntoView();

	// Drag selection functions
	void UpdateDragSelectPos(int newSelectPosSigned);
	unsigned int GetDragSelectPos() const;
	void StartDragSelect();
	void StopDragSelect();
	bool DragSelectActive() const;

	// Clipboard functions
	bool CopyToClipboard();
	bool PasteFromClipboard();

	// Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NOTIFY(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(WPARAM wParam, LPARAM lParam);
	void UpdateChildControls();
	LRESULT msgWM_PRINTCLIENT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(WPARAM wParam, LPARAM lParam);
	void ShowCellSelectionList(unsigned int columnID, unsigned int rowNo, const CellControlInfo& cellControlInfo);
	LRESULT msgWM_SETFONT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_VSCROLL(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_HSCROLL(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KEYDOWN(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CHAR(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KILLFOCUS(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFOCUS(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSEMOVE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_LBUTTONUP(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CTLCOLORSTATIC(WPARAM wParam, LPARAM lParam);

	LRESULT msgGRID_SETMANUALSCROLLING(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETDATAAREAFONT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_INSERTCOLUMN(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_DELETECOLUMN(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_INSERTROWS(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_DELETEROWS(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_GETCOLUMNINFO(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETCOLUMNINFO(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETCELLINFO(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_UPDATECELLTEXT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_UPDATECOLUMNTEXT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_UPDATEROWTEXT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_UPDATEMULTIPLEROWTEXT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETCONTROLCOLOR(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETROWCOLOR(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETCELLCOLOR(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_GETROWCOUNT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_GETVISIBLEROWCOUNT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETVSCROLLINFO(WPARAM wParam, LPARAM lParam);

	// Subclass window procedures
	static LRESULT CALLBACK ChildControlClickHandlerSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

private:
	// Typedefs
	typedef std::list<ColumnData>::iterator ColumnDataIterator;
	typedef std::list<ColumnData>::const_iterator ColumnDataConstIterator;
	typedef std::pair<int, ColumnData*> ColumnIndexEntry;
	typedef std::map<int, ColumnData*>::iterator ColumnIndexIterator;
	typedef std::pair<int, ColumnData*> ColumnSortIndexEntry;
	typedef std::map<int, ColumnData*>::iterator ColumnSortIndexIterator;
	typedef std::pair<int, ColumnData*> ColumnIDIndexEntry;
	typedef std::map<int, ColumnData*>::iterator ColumnIDIndexIterator;

private:
	// Window metrics
	int _controlWidth;
	int _controlHeight;
	int _headerWidth;
	int _headerHeight;
	int _headerPosX;
	int _headerPosY;
	unsigned int _visibleRows;
	unsigned int _fullyVisibleRows;

	// Scroll settings
	bool _autoScrollingManagement;
	int _vscrollMin;
	int _vscrollMax;
	int _vscrollMaxTrueLimit;
	int _vscrollCurrent;
	int _vscrollValuesPerPage;
	int _currentScrollHOffset;

	// Handles
	HINSTANCE _moduleHandle;
	HWND _hwnd;
	HWND _hwndHeader;
	HWND _hwndComboBoxList;
	HFONT _controlFont;
	HFONT _dataAreaFont;

	//##FIX## Comment and group these correctly
	int _openComboBoxColumnID;
	int _openComboBoxRowNo;
	struct CellMapping
	{
		CellMapping(unsigned int acolumnID, unsigned int arowNo)
		:columnID(acolumnID), rowNo(arowNo)
		{}

		unsigned int columnID;
		unsigned int rowNo;
	};
	std::map<unsigned int, std::vector<CellControlInfo>> _customControlForCell;
	std::set<HWND> _childControlSet;
	std::map<HWND, CellMapping> _childControlMapping;

	// Color settings
	CustomColorData _defaultColorData;
	CustomColorData _userColorData;
	std::vector<CustomColorData> _rowColorDataArray;
	std::map<unsigned int, std::map<unsigned int, CustomColorData>> _cellCustomColorData;

	// Font metrics
	int _fontWidth;
	int _fontHeight;
	int _marginSize;

	// Selection info
	bool _rowSelected;
	bool _columnSelected;
	unsigned int _selectedRowNo;
	unsigned int _selectedColumnID;
	bool _editModeActive;
	unsigned int _selectedCharacterNo;
	bool _editModeShowCaretPending;
	bool _editModeSelectionPosChangePending;
	bool _editModeSelectionPosChangePendingInPixels;
	bool _ignoreDragSelectStart;
	int _editModeNewSelectionPosInPixels;
	std::wstring _editBuffer;
	unsigned int _editCellHorizontalPixelOffset;

	// Drag-select info
	bool _mouseButtonDown;
	bool _dragSelectActive;
	unsigned int _dragSelectStartPos;
	unsigned int _dragSelectEndPos;

	// Column data
	unsigned int _largestColumnDataArraySize;
	std::list<ColumnData> _columnData;
	std::map<int, ColumnData*> _columnIndex;
	std::map<int, ColumnData*> _columnSortIndex;
	std::map<int, ColumnData*> _columnIDIndex;
};

#include "WC_DataGrid.inl"
#endif
