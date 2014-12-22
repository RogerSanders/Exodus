/*--------------------------------------------------------------------------------------*\
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
\*--------------------------------------------------------------------------------------*/
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
	//Constants
	static const wchar_t* windowClassName;

	//Enumerations
	enum class WindowMessages :unsigned int;
	enum class WindowNotifications :unsigned int;
	enum class CellControlType;
	enum class TextEllipsisMode;
	enum class ColumnSizeMode;

	//Message parameter structures
	//##TODO## Remove the "Grid_" prefixes from these structures, and consider adding a
	//standard postfix identifying these as message parameter structures.
	//##TODO## Add an "UpdateRowText" message to allow updating data on a row basis
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
	struct Grid_NewRowCount;
	struct Grid_ShiftRowsUp;
	struct Grid_ShiftRowsDown;
	struct Grid_NewScrollPosition;
	struct Grid_SelectionEvent;
	struct Grid_CellEditEvent;
	struct Grid_CellButtonClickEvent;
	struct Grid_TreeEntryExpandEvent;

public:
	//Constructors
	WC_DataGrid(HINSTANCE amoduleHandle, HWND ahwnd);
	~WC_DataGrid();

	//Class registration
	static bool RegisterWindowClass(HINSTANCE amoduleHandle);
	static bool UnregisterWindowClass(HINSTANCE amoduleHandle);

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//Constants
	static const long long IDC_HEADER = 0x101;

	//Internal structures
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

private:
	//Header functions
	void RebuildHeaderOrder();

	//Window size functions
	unsigned int GetTotalRowWidth() const;
	void UpdateWindowSize();
	unsigned int CalculateLargestColumnDataArraySize();
	void RecalculateScrollPosition();
	void UpdateScrollbarSettings(int newScrollHOffset, int newScrollVOffset, bool controlSizeChanged);
	void ForceControlRedraw();
	void RecalculateColumnWidths();

	//Edit functions
	bool IsCellEditingActive() const;
	bool BeginCellEditing(int cursorPosInPixels);
	void UpdateCellEditingPos(int cursorPosInPixels);
	void CompleteCellEditing();
	void CancelCellEditing();
	void SelectCharacter(int newCharacterPos);
	static size_t PerformWordSearchForNextBoundary(size_t searchStartPos, size_t searchEndPos, const std::wstring& text);
	void ForceEditCellIntoView();

	//Drag selection functions
	void UpdateDragSelectPos(int newSelectPosSigned);
	unsigned int GetDragSelectPos() const;
	void StartDragSelect();
	void StopDragSelect();
	bool DragSelectActive() const;

	//Clipboard functions
	bool CopyToClipboard();
	bool PasteFromClipboard();

	//Message handlers
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
	LRESULT msgWM_BOUNCE(WPARAM wParam, LPARAM lParam);
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
	LRESULT msgGRID_SETCONTROLCOLOR(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETROWCOLOR(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETCELLCOLOR(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_GETROWCOUNT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETVSCROLLINFO(WPARAM wParam, LPARAM lParam);

private:
	//Typedefs
	typedef std::list<ColumnData>::iterator ColumnDataIterator;
	typedef std::list<ColumnData>::const_iterator ColumnDataConstIterator;
	typedef std::pair<int, ColumnData*> ColumnIndexEntry;
	typedef std::map<int, ColumnData*>::iterator ColumnIndexIterator;
	typedef std::pair<int, ColumnData*> ColumnSortIndexEntry;
	typedef std::map<int, ColumnData*>::iterator ColumnSortIndexIterator;
	typedef std::pair<int, ColumnData*> ColumnIDIndexEntry;
	typedef std::map<int, ColumnData*>::iterator ColumnIDIndexIterator;

private:
	//Window metrics
	unsigned int controlWidth;
	unsigned int controlHeight;
	unsigned int headerWidth;
	unsigned int headerHeight;
	unsigned int headerPosX;
	unsigned int headerPosY;
	unsigned int visibleRows;
	unsigned int fullyVisibleRows;

	//Scroll settings
	bool autoScrollingManagement;
	int vscrollMin;
	int vscrollMax;
	int vscrollMaxTrueLimit;
	int vscrollCurrent;
	int vscrollValuesPerPage;
	int currentScrollHOffset;
	int lastAppliedCurrentScrollHOffset;

	//Handles
	HINSTANCE moduleHandle;
	HWND hwnd;
	HWND hwndHeader;
	HWND hwndComboBoxList;
	HFONT controlFont;
	HFONT dataAreaFont;

	//##FIX## Comment and group these correctly
	//##DEBUG##
	int openComboBoxColumnID;
	int openComboBoxRowNo;

	//##DEBUG##
	struct CellMapping
	{
		CellMapping(unsigned int acolumnID, unsigned int arowNo)
		:columnID(acolumnID), rowNo(arowNo)
		{}

		unsigned int columnID;
		unsigned int rowNo;
	};
	std::map<unsigned int, std::vector<CellControlInfo>> customControlForCell;
	std::set<HWND> childControlSet;
	std::map<HWND, CellMapping> childControlMapping;

	//Color settings
	CustomColorData defaultColorData;
	CustomColorData userColorData;
	std::vector<CustomColorData> rowColorDataArray;
	std::map<unsigned int, std::map<unsigned int, CustomColorData>> cellCustomColorData;

	//Font metrics
	unsigned int fontWidth;
	unsigned int fontHeight;
	unsigned int marginSize;

	//Selection info
	bool rowSelected;
	bool columnSelected;
	unsigned int selectedRowNo;
	unsigned int selectedColumnID;
	bool editModeActive;
	unsigned int selectedCharacterNo;
	bool editModeShowCaretPending;
	bool editModeSelectionPosChangePending;
	bool editModeSelectionPosChangePendingInPixels;
	bool ignoreDragSelectStart;
	int editModeNewSelectionPosInPixels;
	std::wstring editBuffer;
	unsigned int editCellHorizontalPixelOffset;

	//Drag-select info
	bool mouseButtonDown;
	bool dragSelectActive;
	unsigned int dragSelectStartPos;
	unsigned int dragSelectEndPos;

	//Column data
	unsigned int largestColumnDataArraySize;
	std::list<ColumnData> columnData;
	std::map<int, ColumnData*> columnIndex;
	std::map<int, ColumnData*> columnSortIndex;
	std::map<int, ColumnData*> columnIDIndex;
};

#include "WC_DataGrid.inl"
#endif
