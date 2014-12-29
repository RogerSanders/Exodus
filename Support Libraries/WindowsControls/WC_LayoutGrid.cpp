#include "WC_LayoutGrid.h"
#include <set>
#include <cmath>

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* WC_LayoutGrid::windowClassName = L"EX_LayoutGrid";

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WC_LayoutGrid::WC_LayoutGrid(HINSTANCE amoduleHandle, HWND ahwnd)
:moduleHandle(amoduleHandle), hwnd(ahwnd)
{
	//Set the recorded current size of the control to 0
	currentControlWidth = 0;
	currentControlHeight = 0;

	//Add the default row to the control
	RowInfo defaultRowInfo;
	defaultRowInfo.rowNo = 0;
	defaultRowInfo.defaultEntry = true;
	defaultRowInfo.minHeight = -1;
	defaultRowInfo.maxHeight = -1;
	defaultRowInfo.sizeMode = SizeMode::Proportional;
	defaultRowInfo.proportionalRatio = 1.0;
	rows.push_back(defaultRowInfo);

	//Add the default column to the control
	ColumnInfo defaultColumnInfo;
	defaultColumnInfo.columnNo = 0;
	defaultColumnInfo.defaultEntry = true;
	defaultColumnInfo.minWidth = -1;
	defaultColumnInfo.maxWidth = -1;
	defaultColumnInfo.sizeMode = SizeMode::Proportional;
	defaultColumnInfo.proportionalRatio = 1.0;
	columns.push_back(defaultColumnInfo);
}

//----------------------------------------------------------------------------------------
//Class registration
//----------------------------------------------------------------------------------------
bool WC_LayoutGrid::RegisterWindowClass(HINSTANCE amoduleHandle)
{
	//Attempt to register the window class for this control, and return the result to the
	//caller.
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = amoduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = windowClassName;
	wc.hIconSm       = NULL;
	ATOM registerClassExReturn = RegisterClassEx(&wc);
	return (registerClassExReturn != 0);
}

//----------------------------------------------------------------------------------------
bool WC_LayoutGrid::UnregisterWindowClass(HINSTANCE amoduleHandle)
{
	//Attempt to unregister the window class for this control, and return the result to
	//the caller.
	BOOL unregisterClassReturn = UnregisterClass(windowClassName, amoduleHandle);
	return (unregisterClassReturn != 0);
}

//----------------------------------------------------------------------------------------
//Message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK WC_LayoutGrid::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		WC_LayoutGrid* object = (WC_LayoutGrid*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->WndProcPrivate(message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		WC_LayoutGrid* object = new WC_LayoutGrid((HINSTANCE)createStruct->lpCreateParams, hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->WndProcPrivate(message, wParam, lParam);}
	case WM_DESTROY:{
		WC_LayoutGrid* object = (WC_LayoutGrid*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			LPARAM result = object->WndProcPrivate(message, wParam, lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
			delete object;
			return result;
		}
		break;}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		return msgWM_CREATE(wParam, lParam);
	case WM_SIZE:
		return msgWM_SIZE(wParam, lParam);
	case WM_ERASEBKGND:
		return msgWM_ERASEBKGND(wParam, lParam);
	case WM_COMMAND:
		return msgWM_COMMAND(wParam, lParam);
	case WM_NOTIFY:
		return msgWM_NOTIFY(wParam, lParam);
	case WM_BOUNCE:
		return msgWM_BOUNCE(wParam, lParam);
	case (unsigned int)WindowMessages::AddRow:
		return msgGRID_ADDROW(wParam, lParam);
	case (unsigned int)WindowMessages::AddColumn:
		return msgGRID_ADDCOLUMN(wParam, lParam);
	case (unsigned int)WindowMessages::AddWindow:
		return msgGRID_ADDWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::RemoveWindow:
		return msgGRID_REMOVEWINDOW(wParam, lParam);
	case (unsigned int)WindowMessages::UpdateWindowSizes:
		return msgGRID_UPDATEWINDOWSIZES(wParam, lParam);
	case(unsigned int)WindowMessages::RecalculateSizes:
		return msgGRID_RECALCULATESIZES(wParam, lParam);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	int newClientWidth = (int)(rect.right - rect.left);
	int newClientHeight = (int)(rect.bottom - rect.top);

	//Process the initial size of the window
	HandleSizeChanged(newClientWidth, newClientHeight);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the client region of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	int newClientWidth = (int)(rect.right - rect.left);
	int newClientHeight = (int)(rect.bottom - rect.top);

	//Handle this size changed event
	HandleSizeChanged(newClientWidth, newClientHeight);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam)
{
	//Since we want the grid control to essentially have a transparent background, we
	//don't perform any operation when a background erase is requested. Note that this
	//requires the containing window to use the WS_EX_COMPOSITED style, and for our layout
	//grid control itself to use the WS_EX_TRANSPARENT style, in order to get the result
	//we want.
	return TRUE;
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgWM_COMMAND(WPARAM wParam, LPARAM lParam)
{
	//Forward this message directly to the parent window
	return SendMessage(GetAncestor(hwnd, GA_PARENT), WM_COMMAND, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgWM_NOTIFY(WPARAM wParam, LPARAM lParam)
{
	//Forward this message directly to the parent window
	return SendMessage(GetAncestor(hwnd, GA_PARENT), WM_NOTIFY, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgWM_BOUNCE(WPARAM wParam, LPARAM lParam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lParam;
	if(bounceMessage->uMsg == WM_SIZE)
	{
		//If the size of a hosted window has changed, update the size of all child windows.
		std::map<HWND, HostedWindowInfo>::const_iterator hostedWindowsIterator = hostedWindows.find(bounceMessage->hwnd);
		if(hostedWindowsIterator != hostedWindows.end())
		{
			const HostedWindowInfo& windowInfo = hostedWindowsIterator->second;
			RECT rect;
			GetClientRect(windowInfo.windowHandle, &rect);
			if((windowInfo.currentSizeX  != rect.right) || (windowInfo.currentSizeY != rect.bottom))
			{
				UpdateChildWindowSizes();
			}
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgGRID_ADDROW(WPARAM wParam, LPARAM lParam)
{
	const AddRowParams& params = *((const AddRowParams*)lParam);
	AddRow(params.minHeight, params.maxHeight, params.sizeMode, params.proportionalRatio);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgGRID_ADDCOLUMN(WPARAM wParam, LPARAM lParam)
{
	const AddColumnParams& params = *((const AddColumnParams*)lParam);
	AddColumn(params.minWidth, params.maxWidth, params.sizeMode, params.proportionalRatio);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgGRID_ADDWINDOW(WPARAM wParam, LPARAM lParam)
{
	//Calculate the actual width and height of the target window
	const AddWindowParams& params = *((const AddWindowParams*)lParam);
	RECT rect;
	GetClientRect(params.windowHandle, &rect);
	int actualWindowWidth = (int)(rect.right - rect.left);
	int actualWindowHeight = (int)(rect.bottom - rect.top);

	//Build a HostedWindowInfo object to record information on this new hosted window
	HostedWindowInfo hostedWindowInfo;
	hostedWindowInfo.windowHandle = params.windowHandle;
	hostedWindowInfo.rowNo = params.rowNo;
	hostedWindowInfo.columnNo = params.columnNo;
	hostedWindowInfo.rowSpan = params.rowSpan;
	hostedWindowInfo.columnSpan = params.columnSpan;
	hostedWindowInfo.horizontalSizeMode = params.horizontalSizeMode;
	hostedWindowInfo.verticalSizeMode = params.verticalSizeMode;
	hostedWindowInfo.horizontalAlignmentDisplacement = params.horizontalAlignmentDisplacement;
	hostedWindowInfo.verticalAlignmentDisplacement = params.verticalAlignmentDisplacement;
	hostedWindowInfo.rowProportionStart = params.rowProportionStart;
	hostedWindowInfo.rowProportionEnd = params.rowProportionEnd;
	hostedWindowInfo.columnProportionStart = params.columnProportionStart;
	hostedWindowInfo.columnProportionEnd = params.columnProportionEnd;
	hostedWindowInfo.minWidth = params.minWidth;
	hostedWindowInfo.minHeight = params.minHeight;
	hostedWindowInfo.maxWidth = params.maxWidth;
	hostedWindowInfo.maxHeight = params.maxHeight;
	hostedWindowInfo.paddingLeft = params.paddingLeft;
	hostedWindowInfo.paddingRight = params.paddingRight;
	hostedWindowInfo.paddingTop = params.paddingTop;
	hostedWindowInfo.paddingBottom = params.paddingBottom;
	hostedWindowInfo.currentPosX = -1;
	hostedWindowInfo.currentPosY = -1;
	hostedWindowInfo.currentSizeX = actualWindowWidth;
	hostedWindowInfo.currentSizeY = actualWindowHeight;

	//Add the target window to the control
	AddWindow(hostedWindowInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgGRID_REMOVEWINDOW(WPARAM wParam, LPARAM lParam)
{
	HWND windowHandle = (HWND)lParam;
	RemoveWindow(windowHandle);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgGRID_UPDATEWINDOWSIZES(WPARAM wParam, LPARAM lParam)
{
	const UpdateWindowSizesParams& params = *((const UpdateWindowSizesParams*)lParam);
	UpdateWindowSizes(params);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_LayoutGrid::msgGRID_RECALCULATESIZES(WPARAM wParam, LPARAM lParam)
{
	UpdateChildWindowSizes();
	return 0;
}

//----------------------------------------------------------------------------------------
//Cell methods
//----------------------------------------------------------------------------------------
void WC_LayoutGrid::AddRow(int minHeight, int maxHeight, SizeMode sizeMode, double proportionalRatio)
{
	//Record information on this new row
	RowInfo rowInfo;
	rowInfo.defaultEntry = false;
	rowInfo.minHeight = minHeight;
	rowInfo.maxHeight = maxHeight;
	rowInfo.currentPosY = -1;
	rowInfo.currentHeight = -1;
	rowInfo.sizeMode = sizeMode;
	rowInfo.proportionalRatio = proportionalRatio;

	//Add the row to our list of rows, replacing the default entry if it is currently
	//present.
	if((rows.size() == 1) && (rows[0].defaultEntry))
	{
		rows[0] = rowInfo;
	}
	else
	{
		rows.push_back(rowInfo);
	}

	//Refresh the position and size of each hosted control
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
void WC_LayoutGrid::AddColumn(int minWidth, int maxWidth, SizeMode sizeMode, double proportionalRatio)
{
	//Record information on this new column
	ColumnInfo columnInfo;
	columnInfo.defaultEntry = false;
	columnInfo.minWidth = minWidth;
	columnInfo.maxWidth = maxWidth;
	columnInfo.currentPosX = -1;
	columnInfo.currentWidth = -1;
	columnInfo.sizeMode = sizeMode;
	columnInfo.proportionalRatio = proportionalRatio;

	//Add the column to our list of column, replacing the default entry if it is currently
	//present.
	if((columns.size() == 1) && (columns[0].defaultEntry))
	{
		columns[0] = columnInfo;
	}
	else
	{
		columns.push_back(columnInfo);
	}

	//Refresh the position and size of each hosted control
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
//Hosted window methods
//----------------------------------------------------------------------------------------
void WC_LayoutGrid::AddWindow(const HostedWindowInfo& windowInfo)
{
	//Ensure this window isn't currently hosted in our control
	std::map<HWND, HostedWindowInfo>::iterator hostedWindowsIterator = hostedWindows.find(windowInfo.windowHandle);
	if(hostedWindowsIterator != hostedWindows.end())
	{
		return;
	}

	//Record information on this new hosted window
	hostedWindows[windowInfo.windowHandle] = windowInfo;

	//Set the target window as a child window of our control
	SetWindowParent(windowInfo.windowHandle, hwnd);

	//Subclass the target window so we can intercept size change events
	SetWindowSubclass(windowInfo.windowHandle, BounceBackSubclassProc, 0, 0);

	//Refresh the position and size of each hosted control
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
void WC_LayoutGrid::RemoveWindow(HWND windowHandle)
{
	//Attempt to retrieve info on the target hosted window
	std::map<HWND, HostedWindowInfo>::iterator hostedWindowsIterator = hostedWindows.find(windowHandle);
	if(hostedWindowsIterator == hostedWindows.end())
	{
		return;
	}
	const HostedWindowInfo& hostedWindowInfo = hostedWindowsIterator->second;

	//Remove the target window as a child window of our control
	SetWindowParent(hostedWindowInfo.windowHandle, NULL);

	//Remove the window from the list of hosted windows
	hostedWindows.erase(hostedWindowsIterator);
}

//----------------------------------------------------------------------------------------
void WC_LayoutGrid::UpdateWindowSizes(const UpdateWindowSizesParams& params)
{
	//Attempt to retrieve info on the target hosted window
	std::map<HWND, HostedWindowInfo>::iterator hostedWindowsIterator = hostedWindows.find(params.windowHandle);
	if(hostedWindowsIterator == hostedWindows.end())
	{
		return;
	}
	HostedWindowInfo& hostedWindowInfo = hostedWindowsIterator->second;

	//Update the stored window size info
	hostedWindowInfo.verticalAlignmentDisplacement = params.verticalAlignmentDisplacement;
	hostedWindowInfo.horizontalAlignmentDisplacement = params.horizontalAlignmentDisplacement;
	hostedWindowInfo.paddingLeft = params.paddingLeft;
	hostedWindowInfo.paddingRight = params.paddingRight;
	hostedWindowInfo.paddingTop = params.paddingTop;
	hostedWindowInfo.paddingBottom = params.paddingBottom;
	hostedWindowInfo.minWidth = params.minWidth;
	hostedWindowInfo.minHeight = params.minHeight;
	hostedWindowInfo.maxWidth = params.maxWidth;
	hostedWindowInfo.maxHeight = params.maxHeight;

	//Refresh the position and size of each hosted control
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
//Sizing methods
//----------------------------------------------------------------------------------------
void WC_LayoutGrid::HandleSizeChanged(int newWidth, int newHeight)
{
	//Save the new client window dimensions
	currentControlWidth = newWidth;
	currentControlHeight = newHeight;

	//Update all child window sizes now that the layout has been altered
	UpdateChildWindowSizes();
}

//----------------------------------------------------------------------------------------
void WC_LayoutGrid::UpdateChildWindowSizes()
{
	//Begin a session for processing this batch of window size changes. Processing all the
	//window size and position changes in a single operation in this manner gives the best
	//performance and appearance.
	HDWP deferWindowPosSession = BeginDeferWindowPos((int)hostedWindows.size());

	//Build maps of all hosted windows, grouped by row and column.
	std::map<unsigned int, std::set<HWND>> windowsByRow;
	std::map<unsigned int, std::set<HWND>> windowsByColumn;
	for(std::map<HWND, HostedWindowInfo>::const_iterator i = hostedWindows.begin(); i != hostedWindows.end(); ++i)
	{
		const HostedWindowInfo& windowInfo = i->second;
		for(unsigned int rowNo = windowInfo.rowNo; rowNo < (windowInfo.rowNo + windowInfo.rowSpan); ++rowNo)
		{
			windowsByRow[rowNo].insert(windowInfo.windowHandle);
		}
		for(unsigned int columnNo = windowInfo.columnNo; columnNo < (windowInfo.columnNo + windowInfo.columnSpan); ++columnNo)
		{
			windowsByColumn[columnNo].insert(windowInfo.windowHandle);
		}
	}

	//Update the current height for each fixed and content sized row in the grid, and
	//build a list of all proportional size rows.
	int usedControlHeightForFixedRows = 0;
	std::set<unsigned int> proportionalRows;
	for(unsigned int rowNo = 0; rowNo < (unsigned int)rows.size(); ++rowNo)
	{
		RowInfo& rowInfo = rows[rowNo];
		switch(rowInfo.sizeMode)
		{
		case SizeMode::Fixed:
			usedControlHeightForFixedRows += rowInfo.currentHeight;
			break;
		case SizeMode::Proportional:
			proportionalRows.insert(rowNo);
			break;
		case SizeMode::Content:{
			//Calculate the size of this row based on all fixed size content windows
			//within it. Note that proportionally sized windows aren't considered at this
			//point, as they are relative to the size of the row, and we calculate the
			//size of the rows before the windows.
			int newRowHeight = -1;
			for(std::set<HWND>::const_iterator i = windowsByRow[rowNo].begin(); i != windowsByRow[rowNo].end(); ++i)
			{
				const HostedWindowInfo& windowInfo = hostedWindows[*i];
				if(windowInfo.verticalSizeMode == WindowSizeMode::Fixed)
				{
					double rowProportion = (windowInfo.rowNo == rowNo)? windowInfo.rowProportionStart: (((windowInfo.rowNo + (windowInfo.rowSpan - 1)) == rowNo)? windowInfo.rowProportionEnd: 1.0);
					int heightForWindowInRow = (int)((double)(windowInfo.currentSizeY + windowInfo.paddingTop + windowInfo.paddingBottom) * rowProportion);
					newRowHeight = ((newRowHeight < 0) || (heightForWindowInRow > newRowHeight))? heightForWindowInRow: newRowHeight;
				}
			}
			newRowHeight = (newRowHeight < 0)? rowInfo.minHeight: (((rowInfo.maxHeight >= 0) && (newRowHeight > rowInfo.maxHeight))? rowInfo.maxHeight: newRowHeight);
			rowInfo.currentHeight = (newRowHeight >= 0)? newRowHeight: 0;
			usedControlHeightForFixedRows += newRowHeight;
			break;}
		}
	}

	//Calculate the sum of all the proportion numbers for each proportional row in this
	//grid
	//##TODO## Update this comment
	int combinedRowMinHeight = 0;
	double summedRowProportion = 0.0;
	for(std::set<unsigned int>::const_iterator i = proportionalRows.begin(); i != proportionalRows.end(); ++i)
	{
		RowInfo& rowInfo = rows[*i];
		summedRowProportion += rowInfo.proportionalRatio;
		combinedRowMinHeight += (rowInfo.minHeight >= 0)? rowInfo.minHeight: 0;
	}

	//Update the height for each proportional row
	//##FIX## Min and max sizes affect the proportional calculations. Where we limit the
	//size of a cell, we need to allocate that size to other proportional rows.
	//##TODO## Process minimum requirements first. Those requirements must be met, and
	//will allow us to calculate a minimum total allocated size.
	int initialRemainingControlHeight = (usedControlHeightForFixedRows < currentControlHeight)? currentControlHeight - usedControlHeightForFixedRows: 0;
	for(std::set<unsigned int>::const_iterator i = proportionalRows.begin(); i != proportionalRows.end(); ++i)
	{
		RowInfo& rowInfo = rows[*i];
		double proportionForRow = rowInfo.proportionalRatio / summedRowProportion;
		int newRowHeight = (int)std::ceil(proportionForRow * (double)initialRemainingControlHeight);
		newRowHeight = ((rowInfo.minHeight >= 0) && (newRowHeight < rowInfo.minHeight))? rowInfo.minHeight: (((rowInfo.maxHeight >= 0) && (newRowHeight > rowInfo.maxHeight))? rowInfo.maxHeight: newRowHeight);
		rowInfo.currentHeight = newRowHeight;
	}

	//Update the calculated position of each row
	int nextRowPosY = 0;
	for(unsigned int rowNo = 0; rowNo < (unsigned int)rows.size(); ++rowNo)
	{
		RowInfo& rowInfo = rows[rowNo];
		rowInfo.currentPosY = nextRowPosY;
		nextRowPosY += rowInfo.currentHeight;
	}

	//Update the current width for each fixed size column in the grid, and build a list
	//of all proportional size columns.
	int usedControlWidthForFixedColumns = 0;
	std::set<unsigned int> proportionalColumns;
	for(unsigned int columnNo = 0; columnNo < (unsigned int)columns.size(); ++columnNo)
	{
		ColumnInfo& columnInfo = columns[columnNo];
		switch(columnInfo.sizeMode)
		{
		case SizeMode::Fixed:
			usedControlWidthForFixedColumns += columnInfo.currentWidth;
			break;
		case SizeMode::Proportional:
			proportionalColumns.insert(columnNo);
			break;
		case SizeMode::Content:{
			//Calculate the size of this column based on all fixed size content windows
			//within it. Note that proportionally sized windows aren't considered at this
			//point, as they are relative to the size of the column, and we calculate the
			//size of the columns before the windows.
			int newRowWidth = -1;
			for(std::set<HWND>::const_iterator i = windowsByColumn[columnNo].begin(); i != windowsByColumn[columnNo].end(); ++i)
			{
				const HostedWindowInfo& windowInfo = hostedWindows[*i];
				if(windowInfo.horizontalSizeMode == WindowSizeMode::Fixed)
				{
					double rowProportion = (windowInfo.columnNo == columnNo)? windowInfo.columnProportionStart: (((windowInfo.columnNo + (windowInfo.columnSpan - 1)) == columnNo)? windowInfo.columnProportionEnd: 1.0);
					int widthForWindowInColumn = (int)((double)(windowInfo.currentSizeX + windowInfo.paddingLeft + windowInfo.paddingRight) * rowProportion);
					newRowWidth = ((newRowWidth < 0) || (widthForWindowInColumn > newRowWidth))? widthForWindowInColumn: newRowWidth;
				}
			}
			newRowWidth = (newRowWidth < 0)? columnInfo.minWidth: (((columnInfo.maxWidth >= 0) && (newRowWidth > columnInfo.maxWidth))? columnInfo.maxWidth: newRowWidth);
			columnInfo.currentWidth = (newRowWidth >= 0)? newRowWidth: 0;
			usedControlWidthForFixedColumns += newRowWidth;
			break;}
		}
	}

	//Calculate the sum of all the proportion numbers for each proportional column in this
	//grid
	double summedColumnProportion = 0.0;
	for(std::set<unsigned int>::const_iterator i = proportionalColumns.begin(); i != proportionalColumns.end(); ++i)
	{
		summedColumnProportion += columns[*i].proportionalRatio;
	}

	//Update the width for each proportional column
	int initialRemainingControlWidth = (usedControlWidthForFixedColumns < currentControlWidth)? currentControlWidth - usedControlWidthForFixedColumns: 0;
	for(std::set<unsigned int>::const_iterator i = proportionalColumns.begin(); i != proportionalColumns.end(); ++i)
	{
		ColumnInfo& columnInfo = columns[*i];
		double proportionForRow = columnInfo.proportionalRatio / summedColumnProportion;
		int newColumnWidth = (int)std::ceil(proportionForRow * (double)initialRemainingControlWidth);
		newColumnWidth = ((columnInfo.minWidth >= 0) && (newColumnWidth < columnInfo.minWidth))? columnInfo.minWidth: (((columnInfo.maxWidth >= 0) && (newColumnWidth > columnInfo.maxWidth))? columnInfo.maxWidth: newColumnWidth);
		columnInfo.currentWidth = newColumnWidth;
	}

	//Update the calculated position of each column
	int nextColumnPosX = 0;
	for(unsigned int columnNo = 0; columnNo < (unsigned int)columns.size(); ++columnNo)
	{
		ColumnInfo& columnInfo = columns[columnNo];
		columnInfo.currentPosX = nextColumnPosX;
		nextColumnPosX += columnInfo.currentWidth;
	}

	//Position and size each window in the grid
	for(std::map<HWND, HostedWindowInfo>::iterator i = hostedWindows.begin(); i != hostedWindows.end(); ++i)
	{
		//Retrieve info for the target window, and its associated row and column.
		HostedWindowInfo& windowInfo = i->second;

		//Calculate the new height of this window
		int newWindowSizeY = 0;
		switch(windowInfo.verticalSizeMode)
		{
		case WindowSizeMode::Fixed:
			newWindowSizeY = windowInfo.currentSizeY;
			break;
		case WindowSizeMode::Proportional:
			for(unsigned int rowNo = windowInfo.rowNo; rowNo < (windowInfo.rowNo + windowInfo.rowSpan); ++rowNo)
			{
				const RowInfo& rowInfo = rows[rowNo];
				double totalProportion = (windowInfo.rowSpan > 1)? windowInfo.rowProportionStart + windowInfo.rowProportionEnd + ((double)(windowInfo.rowSpan - 2) * 1.0): windowInfo.rowProportionStart;
				double rowProportion = ((rowNo == windowInfo.rowNo)? windowInfo.rowProportionStart: ((rowNo == (windowInfo.rowNo + (windowInfo.rowSpan - 1)))? windowInfo.rowProportionEnd: 1.0) / totalProportion);
				newWindowSizeY += (int)(rowProportion * rowInfo.currentHeight);
			}
			newWindowSizeY -= ((windowInfo.paddingTop + windowInfo.paddingBottom) < newWindowSizeY)? (windowInfo.paddingTop + windowInfo.paddingBottom): newWindowSizeY;
			break;
		}

		//Calculate the combined height of all rows that this window is a member of
		int combinedRowHeight = 0;
		for(unsigned int rowNo = windowInfo.rowNo; rowNo < (windowInfo.rowNo + windowInfo.rowSpan); ++rowNo)
		{
			combinedRowHeight += rows[rowNo].currentHeight;
		}

		//Calculate the vertical position of this window
		int startRowPosY = rows[windowInfo.rowNo].currentPosY;
		int newWindowPosY = (startRowPosY + windowInfo.paddingTop) + (int)((double)((combinedRowHeight - (windowInfo.paddingTop + windowInfo.paddingBottom)) - newWindowSizeY) * windowInfo.verticalAlignmentDisplacement);

		//Calculate the new width of this window
		int newWindowSizeX = 0;
		switch(windowInfo.horizontalSizeMode)
		{
		case WindowSizeMode::Fixed:
			newWindowSizeX = windowInfo.currentSizeX;
			break;
		case WindowSizeMode::Proportional:
			for(unsigned int columnNo = windowInfo.columnNo; columnNo < (windowInfo.columnNo + windowInfo.columnSpan); ++columnNo)
			{
				const ColumnInfo& columnInfo = columns[columnNo];
				double totalProportion = (windowInfo.columnSpan > 1)? windowInfo.columnProportionStart + windowInfo.columnProportionEnd + ((double)(windowInfo.columnSpan - 2) * 1.0): windowInfo.columnProportionStart;
				double columnProportion = ((columnNo == windowInfo.columnNo)? windowInfo.columnProportionStart: ((columnNo == (windowInfo.columnNo + (windowInfo.columnSpan - 1)))? windowInfo.columnProportionEnd: 1.0) / totalProportion);
				newWindowSizeX += (int)(columnProportion * columnInfo.currentWidth);
			}
			newWindowSizeX -= ((windowInfo.paddingLeft + windowInfo.paddingRight) < newWindowSizeX)? (windowInfo.paddingLeft + windowInfo.paddingRight): newWindowSizeX;
			break;
		}

		//Calculate the combined width of all columns that this window is a member of
		int combinedColumnWidth = 0;
		for(unsigned int columnNo = windowInfo.columnNo; columnNo < (windowInfo.columnNo + windowInfo.columnSpan); ++columnNo)
		{
			combinedColumnWidth += columns[columnNo].currentWidth;
		}

		//Calculate the horizontal position of this window
		int startColumnPosX = columns[windowInfo.columnNo].currentPosX;
		int newWindowPosX = (startColumnPosX + windowInfo.paddingLeft) + (int)((double)((combinedColumnWidth - (windowInfo.paddingLeft + windowInfo.paddingRight)) - newWindowSizeX) * windowInfo.horizontalAlignmentDisplacement);

		//If the size or position of this docked window has changed, add it to the update
		//session.
		if((newWindowPosX != windowInfo.currentPosX) || (newWindowPosY != windowInfo.currentPosY) || (newWindowSizeX != windowInfo.currentSizeX) || (newWindowSizeY != windowInfo.currentSizeY))
		{
			UINT deferWindowPosFlags = SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE;
			deferWindowPosFlags |= ((newWindowPosX != windowInfo.currentPosX) || (newWindowPosY != windowInfo.currentPosY))? 0: SWP_NOREPOSITION;
			deferWindowPosFlags |= ((newWindowSizeX != windowInfo.currentSizeX) || (newWindowSizeY != windowInfo.currentSizeY))? 0: SWP_NOSIZE;
			DeferWindowPos(deferWindowPosSession, windowInfo.windowHandle, NULL, newWindowPosX, newWindowPosY, newWindowSizeX, newWindowSizeY, deferWindowPosFlags);
		}

		//Update the stored position and size of this window
		windowInfo.currentPosX = newWindowPosX;
		windowInfo.currentPosY = newWindowPosY;
		windowInfo.currentSizeX = newWindowSizeX;
		windowInfo.currentSizeY = newWindowSizeY;
	}

	//Process all the window size and position changes involved in this update
	EndDeferWindowPos(deferWindowPosSession);
}
