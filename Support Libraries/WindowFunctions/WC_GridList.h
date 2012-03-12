/*--------------------------------------------------------------------------------------*\
Things to do:
-Build a system for custom text highlighting mid-line. Some kind of text markup language
applied to the source string will probably be required.
-Implement some form of line selection. We need to be able to select a line with the
mouse and have the line highlighted to indicate the selection.
-Once selection is implemented, add support for keyboard line navigation using arrow keys
and home/end keys.
-Consider adding support for multiple line selection, with a rightclick context menu to
perform various functions, specifically adding support for setting a breakpoint on one or
more addresses. Lines with a breakpoint set would highlight in red.
-Consider implementing a context menu for the header control, to enable and disable
columns via tickboxes on the context menu.
-Provide messages to allow the parent control to specify colour settings
-Implement support for the WM_SETFONT message
\*--------------------------------------------------------------------------------------*/

#ifndef __WC_GRIDLIST_H__
#define __WC_GRIDLIST_H__
#include <list>
#include <map>
#include <vector>
#include "WindowFunctions.h"

class WC_GridList
{
public:
	//Constructors
	WC_GridList(HINSTANCE amoduleHandle, HWND ahwnd);

	//Message numbers
	enum WindowMessages;
	enum WindowNotifications;

	//Message parameter structures
	struct Grid_InsertColumn;
	struct Grid_SetColumnInfo;
	struct Grid_GetColumnInfo;
	struct Grid_SetVScrollInfo;
	struct Grid_NewRowCount;
	struct Grid_ShiftRowsUp;
	struct Grid_ShiftRowsDown;
	struct Grid_NewScrollPosition;

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//Header functions
	void RebuildHeaderOrder();

	//Window size functions
	unsigned int GetTotalRowWidth() const;
	void UpdateScrollbarSettings();

	//Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NOTIFY(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PRINTCLIENT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFONT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_VSCROLL(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_HSCROLL(WPARAM wParam, LPARAM lParam);

	LRESULT msgGRID_INSERTCOLUMN(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_DELETECOLUMN(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_GETCOLUMNINFO(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETCOLUMNINFO(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_UPDATECOLUMNTEXT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_GETROWCOUNT(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_SETVSCROLLINFO(WPARAM wParam, LPARAM lParam);

private:
	static const long long IDC_HEADER = 0x101;

	struct ColumnData
	{
		ColumnData(const std::wstring& aname, unsigned int acolumnID, unsigned int awidth)
		:name(aname), columnID(acolumnID), width(awidth)
		{}

		std::wstring name;
		unsigned int columnID;
		unsigned int width;
		unsigned int order;
		int index;
		std::vector<std::wstring> dataBuffer;
	};

	//Window metrics
	unsigned int controlWidth;
	unsigned int controlHeight;
	unsigned int headerWidth;
	unsigned int headerHeight;
	unsigned int headerPosX;
	unsigned int headerPosY;
	unsigned int visibleRows;

	//Scroll settings
	int vscrollMin;
	int vscrollMax;
	int vscrollCurrent;
	int vscrollValuesPerPage;
	int currentScrollHOffset;

	//Handles
	HINSTANCE moduleHandle;
	HWND hwnd;
	HWND hwndHeader;
	HWND hwndList;
	HFONT hfont;

	//Colour settings
	WinColor colorBackground;
	WinColor colorTextFront;
	WinColor colorTextBack;
	WinColor colorLine;

	//Font settings
	unsigned int fontPointSize;
	std::wstring fontTypefaceName;

	//Font metrics
	unsigned int fontWidth;
	unsigned int fontHeight;

	//Column data
	typedef std::list<ColumnData>::iterator ColumnDataIterator;
	typedef std::list<ColumnData>::const_iterator ColumnDataConstIterator;
	std::list<ColumnData> columnData;

	typedef std::pair<int, ColumnData*> ColumnIndexEntry;
	typedef std::map<int, ColumnData*>::iterator ColumnIndexIterator;
	std::map<int, ColumnData*> columnIndex;

	typedef std::pair<int, ColumnData*> ColumnSortIndexEntry;
	typedef std::map<int, ColumnData*>::iterator ColumnSortIndexIterator;
	std::map<int, ColumnData*> columnSortIndex;

	typedef std::pair<int, ColumnData*> ColumnIDIndexEntry;
	typedef std::map<int, ColumnData*>::iterator ColumnIDIndexIterator;
	std::map<int, ColumnData*> columnIDIndex;
};

#include "WC_GridList.inl"
#endif
