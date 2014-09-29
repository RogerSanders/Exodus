#include "WC_HexEdit.h"
#include "Stream/Stream.pkg"
#include "DataConversion/DataConversion.pkg"
#include <sstream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* WC_HexEdit::windowClassName = L"EX_HexEdit";

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WC_HexEdit::WC_HexEdit(HINSTANCE amoduleHandle, HWND ahwnd)
:moduleHandle(amoduleHandle), hwnd(ahwnd)
{
	hfont = NULL;
	hcontextMenu = NULL;

	colorBackground = WinColor(255, 255, 255);
	colorAddressText = WinColor(0, 0, 0);
	colorAddressBack = WinColor(255, 255, 255);
	colorLine = WinColor(0, 0, 0);
	colorDataText1 = WinColor(0, 0, 255);
	colorDataBack1 = WinColor(255, 255, 255);
	colorDataText2 = WinColor(0, 0, 132);
	colorDataBack2 = WinColor(255, 251, 255);
	colorMarkedDataText = WinColor(255, 255, 255);
	colorMarkedDataBack = WinColor(255, 0, 0);
	colorModifiedDataText = WinColor(255, 0, 0);
	colorModifiedDataBack = WinColor(255, 255, 255);
	colorTextText = WinColor(0, 0, 0);
	colorTextBack = WinColor(255, 255, 255);
	colorMarkedTextText = WinColor(255, 255, 255);
	colorMarkedTextBack = WinColor(255, 0, 0);

	addressOffset = 0;
	valuesPerColumn = 2;
	addressWidth = 4;
	textDisplayEnabled = true;
	fontPointSize = 8;
	fontTypefaceName = L"Courier New";

	dataSize = 0;
	dataViewPos = 0;

	selectedByte = 0;
	firstNybbleWritten = false;
	inputOnTextWindow = false;

	hwndTooltip = NULL;
	tooltipVisible = false;

	mouseButtonDown = false;
	dragSelectActive = false;

	markingEnabled = false;
}

//----------------------------------------------------------------------------------------
//Class registration
//----------------------------------------------------------------------------------------
bool WC_HexEdit::RegisterWindowClass(HINSTANCE amoduleHandle)
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
bool WC_HexEdit::UnregisterWindowClass(HINSTANCE amoduleHandle)
{
	//Attempt to unregister the window class for this control, and return the result to
	//the caller.
	BOOL unregisterClassReturn = UnregisterClass(windowClassName, amoduleHandle);
	return (unregisterClassReturn != 0);
}

//----------------------------------------------------------------------------------------
//Drag selection functions
//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateDragSelectPos(int newSelectPosSigned)
{
	unsigned int newSelectPos;
	if(newSelectPosSigned < 0)
	{
		newSelectPos = 0;
	}
	else if((unsigned int)newSelectPosSigned > dataSize)
	{
		newSelectPos = dataSize;
	}
	else
	{
		newSelectPos = (unsigned int)newSelectPosSigned;
	}

	if(newSelectPos < selectedByte)
	{
		dragSelectStartPos = newSelectPos;
		dragSelectEndPos = selectedByte;
	}
	else
	{
		dragSelectStartPos = selectedByte;
		dragSelectEndPos = newSelectPos;
	}
	//Update the tooltip text, so that the drag select region shown in the tooltip will
	//be updated when the keyboard is used to adjust the selected region, or when the
	//mouse is being held outside the window area.
	UpdateTooltipText(tooltipLastValue);
}

//----------------------------------------------------------------------------------------
unsigned int WC_HexEdit::GetDragSelectPos() const
{
	if(dragSelectEndPos != selectedByte)
	{
		return dragSelectEndPos;
	}
	else
	{
		return dragSelectStartPos;
	}
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::StartDragSelect()
{
	if(!dragSelectActive)
	{
		dragSelectActive = true;
		dragSelectStartPos = selectedByte;
		dragSelectEndPos = selectedByte;
		lastCursorDragSelectPosX = 0;
		lastCursorDragSelectPosY = 0;
		EnableSelectionMenuItems(true);
	}
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::StopDragSelect()
{
	if(dragSelectActive)
	{
		dragSelectActive = false;
		selectedByte = GetDragSelectPos();
		EnableSelectionMenuItems(false);
	}
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::DragSelectActive() const
{
	return dragSelectActive;
}

//----------------------------------------------------------------------------------------
//Tooltip functions
//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateTooltipText(unsigned int newSelectedByte)
{
	//Build the new text for the tooltip control
	std::wstringstream stream;
	unsigned int dragSelectSize = dragSelectEndPos - dragSelectStartPos;
	if(DragSelectActive() && (dragSelectSize > 0))
	{
		stream << L"0x" << std::setw(addressWidth) << std::setfill(L'0') << std::hex << std::uppercase << dragSelectStartPos;
		stream << L"-";
		stream << L"0x" << std::setw(addressWidth) << std::setfill(L'0') << std::hex << std::uppercase << dragSelectEndPos;
		stream << L" (";
		stream << "0x" << std::setw(addressWidth) << std::setfill(L'0') << std::hex << std::uppercase << dragSelectSize;
		stream << L"/";
		stream << std::dec << dragSelectSize;
		stream << L")";
	}
	else
	{
		stream << L"0x" << std::setw(addressWidth) << std::setfill(L'0') << std::hex << std::uppercase << newSelectedByte;
	}
	std::wstring text = stream.str();

	//Update the tooltip text
	if(text != lastTooltipText)
	{
		lastTooltipText = text;
		TOOLINFO toolInfo;
		toolInfo.cbSize = sizeof(TOOLINFO);
		toolInfo.lpReserved = 0;
		toolInfo.hwnd = hwnd;
		toolInfo.hinst = moduleHandle;
		toolInfo.uId = (UINT_PTR)hwnd;
		toolInfo.lpszText = &text[0];
		SendMessage(hwndTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
	}
}

//----------------------------------------------------------------------------------------
//Context menu functions
//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateContextMenu()
{
	//Create our rightclick context menu
	if(hcontextMenu != NULL)
	{
		DestroyMenu(hcontextMenu);
	}
	hcontextMenu = CreatePopupMenu();

	std::wstring copyText = L"Copy";
	MENUITEMINFO copyMenuItem;
	copyMenuItem.cbSize = sizeof(copyMenuItem);
	copyMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	copyMenuItem.fType = MFT_STRING;
	copyMenuItem.wID = (UINT)MenuItem::Copy;
	copyMenuItem.dwTypeData = &copyText[0];
	copyMenuItem.cch = (UINT)copyText.size();
	InsertMenuItem(hcontextMenu, 9999, TRUE, &copyMenuItem);

	std::wstring pasteText = L"Paste";
	MENUITEMINFO pasteMenuItem;
	pasteMenuItem.cbSize = sizeof(pasteMenuItem);
	pasteMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	pasteMenuItem.fType = MFT_STRING;
	pasteMenuItem.wID = (UINT)MenuItem::Paste;
	pasteMenuItem.dwTypeData = &pasteText[0];
	pasteMenuItem.cch = (UINT)pasteText.size();
	InsertMenuItem(hcontextMenu, 9999, TRUE, &pasteMenuItem);

	if(markingEnabled)
	{
		std::wstring markText = markName;
		MENUITEMINFO markMenuItem;
		markMenuItem.cbSize = sizeof(markMenuItem);
		markMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
		markMenuItem.fType = MFT_STRING;
		markMenuItem.wID = (UINT)MenuItem::Mark;
		markMenuItem.dwTypeData = &markText[0];
		markMenuItem.cch = (UINT)markText.size();
		InsertMenuItem(hcontextMenu, 9999, TRUE, &markMenuItem);

		std::wstring unmarkText = unmarkName;
		MENUITEMINFO unmarkMenuItem;
		unmarkMenuItem.cbSize = sizeof(unmarkMenuItem);
		unmarkMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
		unmarkMenuItem.fType = MFT_STRING;
		unmarkMenuItem.wID = (UINT)MenuItem::Unmark;
		unmarkMenuItem.dwTypeData = &unmarkText[0];
		unmarkMenuItem.cch = (UINT)unmarkText.size();
		InsertMenuItem(hcontextMenu, 9999, TRUE, &unmarkMenuItem);
	}

	MENUITEMINFO divider1MenuItem;
	divider1MenuItem.cbSize = sizeof(divider1MenuItem);
	divider1MenuItem.fMask = MIIM_FTYPE;
	divider1MenuItem.fType = MFT_SEPARATOR;
	InsertMenuItem(hcontextMenu, 9999, TRUE, &divider1MenuItem);

	HMENU hcolumnSizeMenu = CreatePopupMenu();
	std::wstring columnSizeMenuText1 = L"1 byte";
	MENUITEMINFO columnSizeMenuItem1;
	columnSizeMenuItem1.cbSize = sizeof(columnSizeMenuItem1);
	columnSizeMenuItem1.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_STATE;
	columnSizeMenuItem1.fType = MFT_STRING | MFT_RADIOCHECK;
	columnSizeMenuItem1.fState = (valuesPerColumn == 1)? MFS_CHECKED: MFS_UNCHECKED;
	columnSizeMenuItem1.wID = (UINT)MenuItem::ColumnWidth1;
	columnSizeMenuItem1.dwTypeData = &columnSizeMenuText1[0];
	columnSizeMenuItem1.cch = (UINT)columnSizeMenuText1.size();
	InsertMenuItem(hcolumnSizeMenu, 9999, TRUE, &columnSizeMenuItem1);

	std::wstring columnSizeMenuText2 = L"2 bytes";
	MENUITEMINFO columnSizeMenuItem2;
	columnSizeMenuItem2.cbSize = sizeof(columnSizeMenuItem2);
	columnSizeMenuItem2.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_STATE;
	columnSizeMenuItem2.fType = MFT_STRING | MFT_RADIOCHECK;
	columnSizeMenuItem2.hbmpChecked = NULL;
	columnSizeMenuItem2.hbmpUnchecked = NULL;
	columnSizeMenuItem2.fState = (valuesPerColumn == 2)? MFS_CHECKED: MFS_UNCHECKED;
	columnSizeMenuItem2.wID = (UINT)MenuItem::ColumnWidth2;
	columnSizeMenuItem2.dwTypeData = &columnSizeMenuText2[0];
	columnSizeMenuItem2.cch = (UINT)columnSizeMenuText2.size();
	InsertMenuItem(hcolumnSizeMenu, 9999, TRUE, &columnSizeMenuItem2);

	std::wstring columnSizeMenuText4 = L"4 bytes";
	MENUITEMINFO columnSizeMenuItem4;
	columnSizeMenuItem4.cbSize = sizeof(columnSizeMenuItem4);
	columnSizeMenuItem4.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_STATE;
	columnSizeMenuItem4.fType = MFT_STRING | MFT_RADIOCHECK;
	columnSizeMenuItem4.hbmpChecked = NULL;
	columnSizeMenuItem4.hbmpUnchecked = NULL;
	columnSizeMenuItem4.fState = (valuesPerColumn == 4)? MFS_CHECKED: MFS_UNCHECKED;
	columnSizeMenuItem4.wID = (UINT)MenuItem::ColumnWidth4;
	columnSizeMenuItem4.dwTypeData = &columnSizeMenuText4[0];
	columnSizeMenuItem4.cch = (UINT)columnSizeMenuText4.size();
	InsertMenuItem(hcolumnSizeMenu, 9999, TRUE, &columnSizeMenuItem4);

	std::wstring columnSizeMenuText8 = L"8 bytes";
	MENUITEMINFO columnSizeMenuItem8;
	columnSizeMenuItem8.cbSize = sizeof(columnSizeMenuItem8);
	columnSizeMenuItem8.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_STATE;
	columnSizeMenuItem8.fType = MFT_STRING | MFT_RADIOCHECK;
	columnSizeMenuItem8.hbmpChecked = NULL;
	columnSizeMenuItem8.hbmpUnchecked = NULL;
	columnSizeMenuItem8.fState = (valuesPerColumn == 8)? MFS_CHECKED: MFS_UNCHECKED;
	columnSizeMenuItem8.wID = (UINT)MenuItem::ColumnWidth8;
	columnSizeMenuItem8.dwTypeData = &columnSizeMenuText8[0];
	columnSizeMenuItem8.cch = (UINT)columnSizeMenuText8.size();
	InsertMenuItem(hcolumnSizeMenu, 9999, TRUE, &columnSizeMenuItem8);

	std::wstring columnSizeMenuText16 = L"16 bytes";
	MENUITEMINFO columnSizeMenuItem16;
	columnSizeMenuItem16.cbSize = sizeof(columnSizeMenuItem16);
	columnSizeMenuItem16.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_STATE;
	columnSizeMenuItem16.fType = MFT_STRING | MFT_RADIOCHECK;
	columnSizeMenuItem16.hbmpChecked = NULL;
	columnSizeMenuItem16.hbmpUnchecked = NULL;
	columnSizeMenuItem16.fState = (valuesPerColumn == 16)? MFS_CHECKED: MFS_UNCHECKED;
	columnSizeMenuItem16.wID = (UINT)MenuItem::ColumnWidth16;
	columnSizeMenuItem16.dwTypeData = &columnSizeMenuText16[0];
	columnSizeMenuItem16.cch = (UINT)columnSizeMenuText16.size();
	InsertMenuItem(hcolumnSizeMenu, 9999, TRUE, &columnSizeMenuItem16);

	std::wstring columnSizeMenuText = L"Column Width";
	MENUITEMINFO columnSizeMenuItem;
	columnSizeMenuItem.cbSize = sizeof(columnSizeMenuItem);
	columnSizeMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_SUBMENU | MIIM_DATA;
	columnSizeMenuItem.fType = MFT_STRING;
	columnSizeMenuItem.wID = (UINT)MenuItem::ColumnWidth;
	columnSizeMenuItem.hSubMenu = hcolumnSizeMenu;
	columnSizeMenuItem.dwItemData = (ULONG_PTR)hcolumnSizeMenu;
	columnSizeMenuItem.dwTypeData = &columnSizeMenuText[0];
	columnSizeMenuItem.cch = (UINT)columnSizeMenuText.size();
	InsertMenuItem(hcontextMenu, 9999, TRUE, &columnSizeMenuItem);

	std::wstring enableTextMenuText = L"Show Text";
	MENUITEMINFO enableTextMenuItem;
	enableTextMenuItem.cbSize = sizeof(enableTextMenuItem);
	enableTextMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_STATE;
	enableTextMenuItem.fType = MFT_STRING;
	enableTextMenuItem.fState = (textDisplayEnabled)? MFS_CHECKED: MFS_UNCHECKED;
	enableTextMenuItem.wID = (UINT)MenuItem::EnableText;
	enableTextMenuItem.dwTypeData = &enableTextMenuText[0];
	enableTextMenuItem.cch = (UINT)enableTextMenuText.size();
	InsertMenuItem(hcontextMenu, 9999, TRUE, &enableTextMenuItem);

	MENUITEMINFO divider2MenuItem;
	divider2MenuItem.cbSize = sizeof(divider2MenuItem);
	divider2MenuItem.fMask = MIIM_FTYPE;
	divider2MenuItem.fType = MFT_SEPARATOR;
	InsertMenuItem(hcontextMenu, 9999, TRUE, &divider2MenuItem);

	std::wstring saveToFileText = L"Save to File";
	MENUITEMINFO saveToFileMenuItem;
	saveToFileMenuItem.cbSize = sizeof(saveToFileMenuItem);
	saveToFileMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	saveToFileMenuItem.fType = MFT_STRING;
	saveToFileMenuItem.wID = (UINT)MenuItem::SaveToFile;
	saveToFileMenuItem.dwTypeData = &saveToFileText[0];
	saveToFileMenuItem.cch = (UINT)saveToFileText.size();
	InsertMenuItem(hcontextMenu, 9999, TRUE, &saveToFileMenuItem);

	std::wstring loadFromFileText = L"Load from File";
	MENUITEMINFO loadFromFileMenuItem;
	loadFromFileMenuItem.cbSize = sizeof(loadFromFileMenuItem);
	loadFromFileMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	loadFromFileMenuItem.fType = MFT_STRING;
	loadFromFileMenuItem.wID = (UINT)MenuItem::LoadFromFile;
	loadFromFileMenuItem.dwTypeData = &loadFromFileText[0];
	loadFromFileMenuItem.cch = (UINT)loadFromFileText.size();
	InsertMenuItem(hcontextMenu, 9999, TRUE, &loadFromFileMenuItem);

	EnableSelectionMenuItems(DragSelectActive());
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::EnableSelectionMenuItems(bool state)
{
	UINT enableState = state? MF_ENABLED: MF_GRAYED;
	EnableMenuItem(hcontextMenu, (UINT)MenuItem::Copy, enableState);
	if(markingEnabled)
	{
		EnableMenuItem(hcontextMenu, (UINT)MenuItem::Mark, enableState);
		EnableMenuItem(hcontextMenu, (UINT)MenuItem::Unmark, enableState);
	}
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::MarkSelection(bool state)
{
	if(dragSelectActive)
	{
		Hex_UpdateDataMarkingState updateDataMarkingState;
		updateDataMarkingState.offset = dragSelectStartPos;
		updateDataMarkingState.size = dragSelectEndPos - dragSelectStartPos;
		updateDataMarkingState.state = state;
		SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::UpdateDataMarking), (LPARAM)&updateDataMarkingState);
	}
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::CopyToClipboard()
{
	if(!dragSelectActive)
	{
		return false;
	}
	if(OpenClipboard(hwnd) == 0)
	{
		return false;
	}

	//Copy data to the clipboard
	unsigned int dragSelectByteSize = dragSelectEndPos - dragSelectStartPos;
	unsigned int dragSelectTextSize = dragSelectByteSize;
	if(!inputOnTextWindow)
	{
		dragSelectTextSize *= 2;
	}
	dragSelectTextSize += 1;

	HGLOBAL clipboardDataHandle = GlobalAlloc(GMEM_MOVEABLE, dragSelectTextSize);
	if(clipboardDataHandle == NULL)
	{
		CloseClipboard();
		return false;
	}

	unsigned char* clipboardData = (unsigned char*)GlobalLock(clipboardDataHandle);
	unsigned int clipboardDataOffset = 0;
	if(clipboardData != NULL)
	{
		unsigned int blockSize = dragSelectByteSize;
		unsigned char* buffer = new unsigned char[blockSize];

		bool readBlockResult = ReadBlockToBuffer(dragSelectStartPos, dragSelectByteSize, buffer);
		if(readBlockResult)
		{
			for(unsigned int i = 0; i < dragSelectByteSize; ++i)
			{
				if(inputOnTextWindow)
				{
					unsigned char byte = buffer[i];
					*(clipboardData + clipboardDataOffset++) = byte;
				}
				else
				{
					unsigned char byte = buffer[i];
					unsigned char nybble1 = (byte >> 4) & 0x0F;
					unsigned char nybble2 = byte & 0x0F;
					*(clipboardData + clipboardDataOffset++) = (unsigned char)HexNybbleToChar(nybble1);
					*(clipboardData + clipboardDataOffset++) = (unsigned char)HexNybbleToChar(nybble2);
				}
			}
			*(clipboardData + clipboardDataOffset++) = 0;
		}

		delete[] buffer;
		GlobalUnlock(clipboardDataHandle);
		if(readBlockResult)
		{
			SetClipboardData(CF_TEXT, clipboardDataHandle);
		}
	}

	CloseClipboard();
	return true;
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::PasteFromClipboard()
{
	if(IsClipboardFormatAvailable(CF_TEXT) == 0)
	{
		return false;
	}
	if(OpenClipboard(hwnd) == 0)
	{
		return false;
	}

	HANDLE clipboardDataHandle = GetClipboardData(CF_TEXT);
	if(clipboardDataHandle == NULL)
	{
		CloseClipboard();
		return false;
	}

	unsigned char* clipboardData = (unsigned char*)GlobalLock(clipboardDataHandle);
	if(clipboardData != NULL)
	{
		unsigned int pastePos = selectedByte;
		if(DragSelectActive())
		{
			pastePos = dragSelectStartPos;
			StopDragSelect();
		}
		unsigned int blockSize = dataSize - pastePos;
		unsigned char* buffer = new unsigned char[blockSize];
		unsigned int bufferOffset = 0;

		if(inputOnTextWindow)
		{
			while((*clipboardData != 0) && (bufferOffset < blockSize))
			{
				unsigned char byte = *clipboardData;
				buffer[bufferOffset++] = byte;
				++clipboardData;
			}
		}
		else
		{
			while((*(clipboardData + 0) != 0) && (*(clipboardData + 1) != 0) && (bufferOffset < blockSize))
			{
				unsigned char nybble1 = *(clipboardData + 0);
				unsigned char nybble2 = *(clipboardData + 1);
				unsigned char byte = (unsigned char)((HexCharToNybble(nybble1) << 4) | HexCharToNybble(nybble2));
				buffer[bufferOffset++] = byte;
				clipboardData += 2;
			}
		}
		GlobalUnlock(clipboardDataHandle);

		WriteBlockFromBuffer(pastePos, bufferOffset, buffer);
		SelectByte(pastePos + bufferOffset);
		ForceByteIntoView(selectedByte);
	}

	CloseClipboard();
	return true;
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::SaveToFile()
{
	//Obtain a path to the target file
	std::wstring selectedFilePath;
	if(!SelectNewFile(hwnd, L"Binary files|bin", L"bin", L"", L"", selectedFilePath))
	{
		return false;
	}

	//Open the target file
	Stream::File file;
	if(!file.Open(selectedFilePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
	{
		return false;
	}

	//Read data from our memory buffer into a temporary buffer
	unsigned int blockSize = dataSize;
	unsigned char* buffer = new unsigned char[blockSize];
	if(!ReadBlockToBuffer(0, blockSize, buffer))
	{
		delete[] buffer;
		return false;
	}

	//Write the data to the target file
	bool result = file.WriteData(buffer, blockSize);
	delete[] buffer;

	//Return the result of the operation
	return result;
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::LoadFromFile()
{
	//Obtain a path to the target file
	std::wstring selectedFilePath;
	if(!SelectExistingFile(hwnd, L"Binary files|bin", L"bin", L"", L"", selectedFilePath))
	{
		return false;
	}

	//Open the target file
	Stream::File file;
	if(!file.Open(selectedFilePath, Stream::File::OpenMode::ReadOnly, Stream::File::CreateMode::Open))
	{
		return false;
	}

	//Load data from the target file into the memory buffer
	unsigned int blockSize = dataSize;
	unsigned char* buffer = new unsigned char[blockSize];
	unsigned int fileSize = (unsigned int)file.Size();
	bool result = true;
	unsigned int readBlockSize = (fileSize < blockSize)? fileSize: blockSize;
	result &= file.ReadData(buffer, readBlockSize);
	result &= WriteBlockFromBuffer(0, blockSize, buffer);
	delete[] buffer;

	//Return the result of the operation
	return result;
}

//----------------------------------------------------------------------------------------
//Update functions
//----------------------------------------------------------------------------------------
void WC_HexEdit::SetWindowSettings(unsigned int windowPos, unsigned int windowSize)
{
	dataViewPos = windowPos;
	dataBuffer.resize(windowSize);
	dataMarkBuffer.resize(windowSize);

	//Notify the parent control about the change in position and size so it can update
	//the data buffer.
	Hex_NewWindowPosInfo windowPosInfo;
	windowPosInfo.windowPos = dataViewPos;
	windowPosInfo.windowSize = windowSize;
	SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::NewWindowPos), (LPARAM)&windowPosInfo);
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateColumnSettings(unsigned int columnCount, unsigned int valuesPerColumn, bool textAreaEnabled)
{
	columns = columnCount;
	valuesPerColumn = valuesPerColumn;
	textDisplayEnabled = textAreaEnabled;
	//Ensure there is always at least one column visible, and at least one value in each
	//column.
	if(columns < 1)
	{
		columns = 1;
	}
	if(valuesPerColumn < 1)
	{
		valuesPerColumn = 1;
	}

	//Calculate the number of rows which can be shown within the control. We lock the
	//control at a minimum of 1 complete row.
	unsigned int completeRows = (controlHeight / fontHeight);
	unsigned int partialRows = ((controlHeight % fontHeight) != 0)? 1: 0;
	if(completeRows < 1)
	{
		completeRows = 1;
	}

	//Calculate the maximum number of lines we can display
	valuesPerRow = columns * valuesPerColumn;
	valuesPerPage = completeRows * valuesPerRow;
	visibleRows = completeRows + partialRows;
	visibleValuesPerPage = visibleRows * valuesPerRow;

	//Calculate the positions and sizes of the various sections within the control
	addressSectionPos = 0;
	addressSectionWidth = addressWidth * fontWidth;
	dataDividerLinePosX = addressSectionPos + addressSectionWidth + (dividerSpace / 2);
	dataSectionPos = addressSectionPos + addressSectionWidth + dividerSpace;
	dataSectionWidth = (valuesPerColumn * columns * fontWidth * 2) + ((columns - 1) * columnSpace);
	charDividerLinePosX = dataSectionPos + dataSectionWidth + (dividerSpace / 2);
	textSectionPos = dataSectionPos + dataSectionWidth + dividerSpace;
	textSectionWidth = valuesPerColumn * columns * fontWidth;

	//When the column size changes, we need to re-select the current byte so that
	//the caret is drawn in the correct location.
	SelectByte((int)selectedByte);

	//Update the data window settings with the new size
	SetWindowSettings(dataViewPos, visibleValuesPerPage);
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateControlSize(unsigned int clientWidth, unsigned int clientHeight)
{
	controlWidth = clientWidth;
	controlHeight = clientHeight;

	//Calculate the maximum possible number of columns that can fit in the control
	int availableSpace = controlWidth;
	availableSpace -= (addressWidth * fontWidth);
	availableSpace -= dividerSpace;
	if(textDisplayEnabled)
	{
		availableSpace -= dividerSpace;
	}
	if(availableSpace < 0)
	{
		availableSpace = 0;
	}
	unsigned int columnWidth = (valuesPerColumn * 2 * fontWidth) + columnSpace;
	if(textDisplayEnabled)
	{
		columnWidth += fontWidth * valuesPerColumn;
	}
	unsigned int newColumnCount = (unsigned int)availableSpace / columnWidth;
	if(newColumnCount != columns)
	{
		//Only force a full redraw if the column sizes change. This prevents
		//excessive flickering when resizing the control.
		InvalidateRect(hwnd, NULL, FALSE);
	}
	//Apply the new window size settings
	UpdateColumnSettings(newColumnCount, valuesPerColumn, textDisplayEnabled);

	//Calculate the new settings for the scrollbar
	unsigned int newScrollCurrentPos = 0;
	unsigned int newScrollMaxPos = 0;
	if(valuesPerRow > 0)
	{
		newScrollCurrentPos = dataViewPos - (dataViewPos % valuesPerRow);
		newScrollMaxPos = dataSize - (dataSize % valuesPerRow) + valuesPerRow;
	}
	SetWindowSettings(newScrollCurrentPos, visibleValuesPerPage);
	scrollMaxPos = newScrollMaxPos;

	//Apply the new scrollbar settings
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.nMin = 0;
	scrollInfo.nMax = scrollMaxPos;
	scrollInfo.nPos = dataViewPos;
	scrollInfo.nPage = valuesPerPage;
	scrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);
}

//----------------------------------------------------------------------------------------
//Byte selection functions
//----------------------------------------------------------------------------------------
bool WC_HexEdit::DataSelectedFromCursorPos(int cursorPosX, int cursorPosY) const
{
	return (cursorPosX > (int)dataSectionPos) && (cursorPosX < ((int)dataSectionPos + (int)dataSectionWidth));
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::TextSelectedFromCursorPos(int cursorPosX, int cursorPosY) const
{
	return (cursorPosX > (int)textSectionPos) && (cursorPosX < ((int)textSectionPos + (int)textSectionWidth));
}

//----------------------------------------------------------------------------------------
int WC_HexEdit::ByteNumberFromCursorPos(int cursorPosX, int cursorPosY) const
{
	int selectedRow = cursorPosY / (int)fontHeight;
	int newSelectedByte = (int)dataViewPos + (selectedRow * (int)valuesPerRow);
	if(DataSelectedFromCursorPos(cursorPosX, cursorPosY))
	{
		unsigned int columnWidth = (fontWidth * valuesPerColumn * 2) + columnSpace;
		unsigned int columnNo = (cursorPosX - dataSectionPos) / columnWidth;
		unsigned int valueNo = ((cursorPosX - dataSectionPos) % columnWidth) / (fontWidth * 2);
		newSelectedByte += (int)((columnNo * valuesPerColumn) + valueNo);
	}
	else if(TextSelectedFromCursorPos(cursorPosX, cursorPosY))
	{
		newSelectedByte += (cursorPosX - (int)textSectionPos) / (int)fontWidth;
	}
	return newSelectedByte;
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::SelectByte(int bytePos)
{
	//Commit the write to the current byte if one is in progress
	if(firstNybbleWritten)
	{
		firstNybbleWritten = false;
		unsigned char originalByte = ReadByte(selectedByte);
		unsigned char dataToWrite = (storedNybble << 4) | (originalByte & 0x0F);
		WriteByte(selectedByte, dataToWrite);
	}

	//Don't allow the selected byte to move past the non-existent element after the last
	//byte.
	if(bytePos < 0)
	{
		bytePos = 0;
	}
	else if(bytePos > (int)dataSize)
	{
		bytePos = (int)dataSize;
	}
	selectedByte = (unsigned int)bytePos;

	//Calculate the position of the selected byte in the data region
	unsigned int selectedCharacterValue = selectedByte % valuesPerRow;
	selectedCharacterDataPosX = ((selectedCharacterValue % valuesPerColumn) * (fontWidth * 2));
	selectedCharacterDataPosX += ((selectedCharacterValue / valuesPerColumn) * (columnSpace + (valuesPerColumn * fontWidth * 2)));

	if(textDisplayEnabled)
	{
		//Calculate the position of the selected byte in the text region
		selectedCharacterTextPosX = (selectedCharacterValue * fontWidth);
	}
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::ForceByteIntoView(unsigned int bytePos)
{
	//If the caret position is outside the currently viewable area of the control, shift
	//the view position to bring it back into view.
	if(bytePos < dataViewPos)
	{
		int newViewPos = bytePos - (bytePos % valuesPerRow);
		UpdateViewPos(newViewPos);
	}
	else if(bytePos >= (dataViewPos + valuesPerPage))
	{
		int newViewPos = ((bytePos - (bytePos % valuesPerRow)) - valuesPerPage) + valuesPerRow;
		UpdateViewPos(newViewPos);
	}
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateViewPos(int newViewPosSigned)
{
	unsigned int newViewPos;
	if(newViewPosSigned < 0)
	{
		newViewPos = 0;
	}
	else if((unsigned int)newViewPosSigned > (scrollMaxPos - valuesPerPage))
	{
		newViewPos = (scrollMaxPos - valuesPerPage);
	}
	else
	{
		newViewPos = (unsigned int)newViewPosSigned;
	}

	if(newViewPos != dataViewPos)
	{
		SetWindowSettings(newViewPos, visibleValuesPerPage);

		//Force the entire control to redraw if the view position changes
		InvalidateRect(hwnd, NULL, FALSE);

		//Update the current scroll position
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_POS;
		scrollInfo.nPos = newViewPos;
		SetScrollInfo(hwnd, SB_VERT, &scrollInfo, TRUE);
	}
}

//----------------------------------------------------------------------------------------
//Data read/write functions
//----------------------------------------------------------------------------------------
unsigned char WC_HexEdit::ReadByte(unsigned int bytePos) const
{
	Hex_ReadDataInfo readDataInfo;
	readDataInfo.offset = bytePos;
	readDataInfo.processed = false;
	readDataInfo.data = 0;
	SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ReadData), (LPARAM)&readDataInfo);
	return readDataInfo.data;
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::WriteByte(unsigned int bytePos, unsigned char data)
{
	Hex_WriteDataInfo writeDataInfo;
	writeDataInfo.offset = bytePos;
	writeDataInfo.data = data;
	SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::WriteData), (LPARAM)&writeDataInfo);
	if((bytePos >= dataViewPos) && (bytePos < (dataViewPos + visibleValuesPerPage)))
	{
		dataBuffer[bytePos - dataViewPos] = data;
	}
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::ReadBlockToBuffer(unsigned int startPos, unsigned int blockSize, unsigned char* buffer)
{
	Hex_ReadDataBlockInfo readDataBlockInfo;
	readDataBlockInfo.offset = startPos;
	readDataBlockInfo.size = blockSize;
	readDataBlockInfo.buffer = buffer;
	readDataBlockInfo.processed = false;
	SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::ReadDataBlock), (LPARAM)&readDataBlockInfo);
	return readDataBlockInfo.processed;
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::WriteBlockFromBuffer(unsigned int startPos, unsigned int blockSize, unsigned char* buffer)
{
	Hex_WriteDataBlockInfo writeDataBlockInfo;
	writeDataBlockInfo.offset = startPos;
	writeDataBlockInfo.size = blockSize;
	writeDataBlockInfo.buffer = buffer;
	writeDataBlockInfo.processed = false;
	SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(hwnd) & 0xFFFF), WindowNotifications::WriteDataBlock), (LPARAM)&writeDataBlockInfo);
	return writeDataBlockInfo.processed;
}

//----------------------------------------------------------------------------------------
//Message handlers
//----------------------------------------------------------------------------------------
LRESULT CALLBACK WC_HexEdit::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:{
		WC_HexEdit* object = (WC_HexEdit*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if(object != 0)
		{
			return object->WndProcPrivate(message, wParam, lParam);
		}
		break;}
	case WM_CREATE:{
		CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
		WC_HexEdit* object = new WC_HexEdit(createStruct->hInstance, hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)object);
		return object->WndProcPrivate(message, wParam, lParam);}
	case WM_DESTROY:{
		WC_HexEdit* object = (WC_HexEdit*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
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
LRESULT WC_HexEdit::WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		return msgWM_CREATE(wParam, lParam);
	case WM_TIMER:
		return msgWM_TIMER(wParam, lParam);
	case WM_MOUSEMOVE:
		return msgWM_MOUSEMOVE(wParam, lParam);
	case WM_MOUSELEAVE:
		return msgWM_MOUSELEAVE(wParam, lParam);
	case WM_CONTEXTMENU:
		return msgWM_CONTEXTMENU(wParam, lParam);
	case WM_COMMAND:
		return msgWM_COMMAND(wParam, lParam);
	case WM_DESTROY:
		return msgWM_DESTROY(wParam, lParam);
	case WM_KILLFOCUS:
		return msgWM_KILLFOCUS(wParam, lParam);
	case WM_SETFOCUS:
		return msgWM_SETFOCUS(wParam, lParam);
	case WM_LBUTTONUP:
		return msgWM_LBUTTONUP(wParam, lParam);
	case WM_LBUTTONDOWN:
		return msgWM_LBUTTONDOWN(wParam, lParam);
	case WM_KEYDOWN:
		return msgWM_KEYDOWN(wParam, lParam);
	case WM_CHAR:
		return msgWM_CHAR(wParam, lParam);
	case WM_MOUSEWHEEL:
		return msgWM_MOUSEWHEEL(wParam, lParam);
	case WM_VSCROLL:
		return msgWM_VSCROLL(wParam, lParam);
	case WM_SIZE:
		return msgWM_SIZE(wParam, lParam);
	case WM_ERASEBKGND:
		return msgWM_ERASEBKGND(wParam, lParam);
	case WM_PAINT:
		return msgWM_PAINT(wParam, lParam);
	case WM_PRINTCLIENT:
		return msgWM_PRINTCLIENT(wParam, lParam);

	case WindowMessages::SetWindowSize:
		return msgHEX_SETWINDOWSIZE(wParam, lParam);
	case WindowMessages::SetWindowPos:
		return msgHEX_SETWINDOWPOS(wParam, lParam);
	case WindowMessages::SetWindowAddressOffset:
		return msgHEX_SETWINDOWADDRESSOFFSET(wParam, lParam);
	case WindowMessages::SetWindowAddressWidth:
		return msgHEX_SETWINDOWADDRESSWIDTH(wParam, lParam);
	case WindowMessages::GetWindowSize:
		return msgHEX_GETWINDOWSIZE(wParam, lParam);
	case WindowMessages::GetWindowPos:
		return msgHEX_GETWINDOWPOS(wParam, lParam);
	case WindowMessages::UpdateWindowData:
		return msgHEX_UPDATEWINDOWDATA(wParam, lParam);
	case WindowMessages::SetMarkingInfo:
		return msgHEX_SETMARKINGINFO(wParam, lParam);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
{
	//Set our font options, and read the font metrics
	HDC hdc = GetDC(hwnd);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	hfont = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &fontTypefaceName[0]);
	HFONT hfontOld = (HFONT)SelectObject(hdc, hfont);

	//Calculate the required font metrics of of the selected font. Note that we use the
	//tmAveCharWidth value rather than the tmMaxCharWidth value to determine our font
	//character width here. Even though we're using a monospaced font and it would seem
	//the two should be identical, where ClearType font support is enabled, Windows
	//actually extends a font character one pixel to the right, which is intended to
	//overlap the next character. In this case, the tmAveCharWidth value reports the true
	//font width for each character, while tmMaxCharWidth reports the width in pixels
	//required to display a single character, plus one extra pixel to hold the extended
	//pixel which is intended to be overlapped with a following character. We want to
	//overlap our characters to get the correct spacing, so we use the tmAveCharWidth
	//value here.
	TEXTMETRIC textMetric;
	GetTextMetrics(hdc, &textMetric);
	fontWidth = textMetric.tmAveCharWidth;
	fontHeight = textMetric.tmHeight;
	dividerSpace = (unsigned int)((float)fontWidth * 1.0f);
	columnSpace = (unsigned int)((float)fontWidth * 0.7f);

	SelectObject(hdc, hfontOld);
	ReleaseDC(hwnd, hdc);

	UpdateContextMenu();

	//Initialize common controls
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_BAR_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	//Create a tooltip window
	hwndTooltip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_NOFADE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, moduleHandle, NULL);

	//Set the tooltip window as topmost
	SetWindowPos(hwndTooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	//Define the properties of the tooltip
	std::wstring text;
	text = L" ";
	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.lpReserved = 0;
	toolInfo.uFlags = TTF_TRACK | TTF_ABSOLUTE | TTF_IDISHWND;
	toolInfo.hwnd = hwnd;
	toolInfo.hinst = moduleHandle;
	toolInfo.uId = (UINT_PTR)hwnd;
	toolInfo.lpszText = &text[0];

	//Register the tooltip with the tooltip control
	SendMessage(hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_TIMER(WPARAM wParam, LPARAM lParam)
{
	//If drag select has been activated, update the end position
	int newSelectedByte = ByteNumberFromCursorPos(lastCursorDragSelectPosX, lastCursorDragSelectPosY);
	UpdateDragSelectPos(newSelectedByte);
	ForceByteIntoView(GetDragSelectPos());
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_MOUSEMOVE(WPARAM wParam, LPARAM lParam)
{
	//Convert the cursor position to a byte number
	short cursorPosX = LOWORD(lParam);
	short cursorPosY = HIWORD(lParam);
	bool byteSelected = DataSelectedFromCursorPos(cursorPosX, cursorPosY) || TextSelectedFromCursorPos(cursorPosX, cursorPosY);
	int newSelectedByte = ByteNumberFromCursorPos(cursorPosX, cursorPosY);

	//If the current mouse position doesn't fall over a valid byte location, hide the
	//tooltip, and abort further processing.
	if(!byteSelected)
	{
		if(tooltipVisible)
		{
			tooltipVisible = false;
			TOOLINFO toolInfo;
			toolInfo.cbSize = sizeof(TOOLINFO);
			toolInfo.lpReserved = 0;
			toolInfo.hwnd = hwnd;
			toolInfo.uId = (UINT_PTR)hwnd;
			SendMessage(hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&toolInfo);
		}
		return 0;
	}

	//If drag select has been activated, update the end position
	if(mouseButtonDown && (newSelectedByte != (int)GetDragSelectPos()))
	{
		lastCursorDragSelectPosX = cursorPosX;
		lastCursorDragSelectPosY = cursorPosY;
		if(!DragSelectActive())
		{
			StartDragSelect();
			SetTimer(hwnd, 1, 50, NULL);
		}
		UpdateDragSelectPos(newSelectedByte);
		ForceByteIntoView(GetDragSelectPos());
	}

	//If the tooltip is currently hidden, activate it. Note that we enable the tooltip
	//before we position it, as due to an undocumented quirk or bug in the Win32 API, we
	//get a crash if we send a TTM_GETBUBBLESIZE message to the tooltip when it is not
	//activated.
	bool tooltipInitiallyVisible = tooltipVisible;
	if(!tooltipVisible)
	{
		tooltipVisible = true;
		TOOLINFO toolInfo;
		toolInfo.cbSize = sizeof(TOOLINFO);
		toolInfo.lpReserved = 0;
		toolInfo.hwnd = hwnd;
		toolInfo.uId = (UINT_PTR)hwnd;
		SendMessage(hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&toolInfo);

		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(trackMouseEvent);
		trackMouseEvent.dwFlags = TME_LEAVE;
		trackMouseEvent.hwndTrack = hwnd;
		TrackMouseEvent(&trackMouseEvent);
	}

	//If we've only just activated the tooltip control, or the selected byte has
	//changed, update the text for the tooltip.
	if(!tooltipInitiallyVisible || ((int)tooltipLastValue != newSelectedByte))
	{
		UpdateTooltipText(newSelectedByte);
	}
	tooltipLastValue = newSelectedByte;

	//Read the current dimensions of the tooltip
	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.lpReserved = 0;
	toolInfo.hwnd = hwnd;
	toolInfo.hinst = moduleHandle;
	toolInfo.uId = (UINT_PTR)hwnd;
	LRESULT tooltipSize = SendMessage(hwndTooltip, TTM_GETBUBBLESIZE, 0, (LPARAM)&toolInfo);

	//Calculate the position of the mouse cursor in screen coordinates
	POINT position;
	position.x = (short)LOWORD(lParam);
	position.y = (short)HIWORD(lParam);
	ClientToScreen(hwnd, &position);

	//Adjust the tooltip position to place it above the cursor
	position.y -= (short)HIWORD(tooltipSize);
	position.x += 8;
	position.y -= 8;
	if(position.y < 0)
	{
		position.y = 0;
	}
	SendMessage(hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(position.x, position.y));

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_MOUSELEAVE(WPARAM wParam, LPARAM lParam)
{
	tooltipVisible = false;
	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.lpReserved = 0;
	toolInfo.hwnd = hwnd;
	toolInfo.uId = (UINT_PTR)hwnd;
	SendMessage(hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&toolInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_CONTEXTMENU(WPARAM wParam, LPARAM lParam)
{
	TrackPopupMenuEx(hcontextMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_COMMAND(WPARAM wParam, LPARAM lParam)
{
	if(HIWORD(wParam) == 0)
	{
		unsigned int newColumnWidth = 0;
		switch((MenuItem)LOWORD(wParam))
		{
		case MenuItem::EnableText:{
			textDisplayEnabled = !textDisplayEnabled;
			RECT rect;
			GetClientRect(hwnd, &rect);
			UpdateControlSize(rect.right, rect.bottom);
			MENUITEMINFO menuItemInfo;
			menuItemInfo.cbSize = sizeof(menuItemInfo);
			menuItemInfo.fMask = MIIM_STATE;
			menuItemInfo.fState = (textDisplayEnabled)? MFS_CHECKED: MFS_UNCHECKED;
			SetMenuItemInfo(hcontextMenu, (UINT)MenuItem::EnableText, FALSE, &menuItemInfo);
			break;}
		case MenuItem::ColumnWidth1:
			newColumnWidth = (newColumnWidth != 0)? newColumnWidth: 1;
		case MenuItem::ColumnWidth2:
			newColumnWidth = (newColumnWidth != 0)? newColumnWidth: 2;
		case MenuItem::ColumnWidth4:
			newColumnWidth = (newColumnWidth != 0)? newColumnWidth: 4;
		case MenuItem::ColumnWidth8:
			newColumnWidth = (newColumnWidth != 0)? newColumnWidth: 8;
		case MenuItem::ColumnWidth16:{
			newColumnWidth = (newColumnWidth != 0)? newColumnWidth: 16;
			valuesPerColumn = newColumnWidth;
			RECT rect;
			GetClientRect(hwnd, &rect);
			UpdateControlSize(rect.right, rect.bottom);

			MENUITEMINFO menuItemInfo;
			menuItemInfo.cbSize = sizeof(menuItemInfo);
			menuItemInfo.fMask = MIIM_DATA;
			GetMenuItemInfo(hcontextMenu, (UINT)MenuItem::ColumnWidth, FALSE, &menuItemInfo);
			HMENU columnWidthMenu = (HMENU)menuItemInfo.dwItemData;
			unsigned int selectedMenuID = LOWORD(wParam);
			for(unsigned int i = (unsigned int)MenuItem::ColumnWidth1; i <= (unsigned int)MenuItem::ColumnWidth16; ++i)
			{
				MENUITEMINFO menuItemInfo;
				menuItemInfo.cbSize = sizeof(menuItemInfo);
				menuItemInfo.fMask = MIIM_STATE;
				menuItemInfo.fState = (i == selectedMenuID)? MFS_CHECKED: MFS_UNCHECKED;
				SetMenuItemInfo(columnWidthMenu, i, FALSE, &menuItemInfo);
			}
			break;}
		case MenuItem::SaveToFile:
			SaveToFile();
			break;
		case MenuItem::LoadFromFile:
			LoadFromFile();
			break;
		case MenuItem::Copy:
			CopyToClipboard();
			break;
		case MenuItem::Paste:
			PasteFromClipboard();
			break;
		case MenuItem::Mark:
			MarkSelection(true);
			break;
		case MenuItem::Unmark:
			MarkSelection(false);
			break;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_DESTROY(WPARAM wParam, LPARAM lParam)
{
	DeleteObject(hfont);
	DestroyMenu(hcontextMenu);
	return DefWindowProc(hwnd, WM_DESTROY, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_KILLFOCUS(WPARAM wParam, LPARAM lParam)
{
	//Destroy the window caret
	DestroyCaret();

	//Force the entire control to redraw
	InvalidateRect(hwnd, NULL, FALSE);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_SETFOCUS(WPARAM wParam, LPARAM lParam)
{
	//Create a caret for the window
	CreateCaret(hwnd, NULL, 2, fontHeight);
	ShowCaret(hwnd);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_LBUTTONUP(WPARAM wParam, LPARAM lParam)
{
	ReleaseCapture();
	KillTimer(hwnd, 1);

	mouseButtonDown = false;
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam)
{
	//Convert the cursor position to a byte number
	int cursorPosX = (short)LOWORD(lParam);
	int cursorPosY = (short)HIWORD(lParam);
	bool byteSelected = false;
	unsigned int newSelectedByte = ByteNumberFromCursorPos(cursorPosX, cursorPosY);
	if(DataSelectedFromCursorPos(cursorPosX, cursorPosY))
	{
		byteSelected = true;
		inputOnTextWindow = false;
	}
	else if(TextSelectedFromCursorPos(cursorPosX, cursorPosY))
	{
		byteSelected = true;
		inputOnTextWindow = true;
	}

	if(byteSelected)
	{
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			//If the user has shift+clicked on the window, trigger a drag-select to the
			//target byte.
			StartDragSelect();
			UpdateDragSelectPos(newSelectedByte);
		}
		else
		{
			//Capture the mouse, so that we can track a drag-select if the user starts
			//a drag operation and moves the cursor outside the window.
			SetCapture(hwnd);

			//Stop drag-select if it is currently in progress, and flag that the left
			//mouse button is being held down.
			StopDragSelect();
			mouseButtonDown = true;

			//Apply the new selection
			SelectByte((int)newSelectedByte);
			ForceByteIntoView(selectedByte);
		}
		//Force the entire control to redraw if the selected byte changes
		InvalidateRect(hwnd, NULL, FALSE);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_KEYDOWN(WPARAM wParam, LPARAM lParam)
{
	unsigned int virtualKeyCode = (unsigned int)wParam;

	switch(virtualKeyCode)
	{
	default:
		return 0;
	case 'A':
		//If the control key is being held down, capture the keypress as a hotkey.
		if((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			SelectByte(0);
			StartDragSelect();
			UpdateDragSelectPos((int)dataSize);
			ForceByteIntoView(0);
		}
		break;
	case 'C':
		//If the control key is being held down, capture the keypress as a hotkey.
		if((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			CopyToClipboard();
		}
		break;
	case 'V':
		//If the control key is being held down, capture the keypress as a hotkey.
		if((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			PasteFromClipboard();
		}
		break;
	case VK_TAB:
		inputOnTextWindow = !inputOnTextWindow;
		SelectByte((int)selectedByte);
		break;
	case VK_BACK:
	case VK_LEFT:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() - 1);
			ForceByteIntoView(GetDragSelectPos());
		}
		else
		{
			StopDragSelect();
			SelectByte((int)selectedByte - 1);
			ForceByteIntoView(selectedByte);
		}
		break;
	case VK_RIGHT:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() + 1);
			ForceByteIntoView(GetDragSelectPos());
		}
		else
		{
			StopDragSelect();
			SelectByte((int)selectedByte + 1);
			ForceByteIntoView(selectedByte);
		}
		break;
	case VK_UP:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() - (int)valuesPerRow);
			ForceByteIntoView(GetDragSelectPos());
		}
		else
		{
			StopDragSelect();
			SelectByte((int)selectedByte - (int)valuesPerRow);
			ForceByteIntoView(selectedByte);
		}
		break;
	case VK_DOWN:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() + (int)valuesPerRow);
			ForceByteIntoView(GetDragSelectPos());
		}
		else
		{
			StopDragSelect();
			SelectByte((int)selectedByte + (int)valuesPerRow);
			ForceByteIntoView(selectedByte);
		}
		break;
	case VK_PRIOR:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() - (int)valuesPerPage);
			UpdateViewPos((int)dataViewPos - (int)valuesPerPage);
		}
		else
		{
			StopDragSelect();
			SelectByte((int)selectedByte - (int)valuesPerPage);
			UpdateViewPos((int)dataViewPos - (int)valuesPerPage);
		}
		break;
	case VK_NEXT:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() + (int)valuesPerPage);
			UpdateViewPos((int)dataViewPos + (int)valuesPerPage);
		}
		else
		{
			StopDragSelect();
			SelectByte((int)selectedByte + (int)valuesPerPage);
			UpdateViewPos((int)dataViewPos + (int)valuesPerPage);
		}
		break;
	case VK_HOME:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos(0);
			ForceByteIntoView(GetDragSelectPos());
		}
		else
		{
			StopDragSelect();
			SelectByte(0);
			ForceByteIntoView(selectedByte);
		}
		break;
	case VK_END:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)dataSize);
			ForceByteIntoView(GetDragSelectPos());
		}
		else
		{
			StopDragSelect();
			SelectByte((int)dataSize);
			ForceByteIntoView(selectedByte);
		}
		break;
	}

	//Force the entire control to redraw
	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_CHAR(WPARAM wParam, LPARAM lParam)
{
	//If the caret position is past the last byte of the block, ignore all characters.
	if(selectedByte >= dataSize)
	{
		return 0;
	}

	ForceByteIntoView(selectedByte);

	if(inputOnTextWindow)
	{
		unsigned char byte = (unsigned char)wParam;
		if((byte >= 0x20) && (byte < 0x7F))
		{
			WriteByte(selectedByte, byte);
			SelectByte((int)selectedByte + 1);
		}
	}
	else
	{
		bool validCharacterWritten = false;
		unsigned char dataToWrite = 0;
		unsigned char byte = (unsigned char)wParam;

		//Filter out invalid characters
		if((byte >= '0') && (byte <= '9'))
		{
			dataToWrite = byte - (unsigned char)'0';
			validCharacterWritten = true;
		}
		else if((byte >= 'A') && (byte <= 'F'))
		{
			dataToWrite = (byte - (unsigned char)'A') + 0x0A;
			validCharacterWritten = true;
		}
		else if((byte >= 'a') && (byte <= 'f'))
		{
			dataToWrite = (byte - (unsigned char)'a') + 0xA;
			validCharacterWritten = true;
		}

		if(validCharacterWritten)
		{
			if(firstNybbleWritten)
			{
				firstNybbleWritten = false;
				dataToWrite = (storedNybble << 4) | dataToWrite;
				WriteByte(selectedByte, dataToWrite);
				SelectByte((int)selectedByte + 1);
			}
			else
			{
				firstNybbleWritten = true;
				storedNybble = dataToWrite;
			}
		}
	}

	ForceByteIntoView(selectedByte);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam)
{
	//Calculate the number of lines to scroll in the window. We displace the view pos
	//by a minimum of 3 lines per division of the scroll wheel. We also invert the
	//sign, since we want a scroll down to increase the view position.
	int scrollUnits = ((short)HIWORD(wParam) / WHEEL_DELTA);
	scrollUnits *= -3 * valuesPerRow;

	UpdateViewPos(dataViewPos + scrollUnits);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_VSCROLL(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case SB_THUMBTRACK:{
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(hwnd, SB_VERT, &scrollInfo);
		int newViewPos = scrollInfo.nTrackPos - (scrollInfo.nTrackPos % valuesPerRow);
		UpdateViewPos(newViewPos);
		break;}
	case SB_TOP:
		UpdateViewPos(0);
		break;
	case SB_BOTTOM:
		UpdateViewPos(scrollMaxPos - valuesPerPage);
		break;
	case SB_PAGEUP:
		UpdateViewPos(dataViewPos - valuesPerPage);
		break;
	case SB_PAGEDOWN:
		UpdateViewPos(dataViewPos + valuesPerPage);
		break;
	case SB_LINEUP:
		UpdateViewPos(dataViewPos - valuesPerRow);
		break;
	case SB_LINEDOWN:
		UpdateViewPos(dataViewPos + valuesPerRow);
		break;
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam)
{
	//We handle the WM_ERASEBKGND message here and return 0 to prevent the background
	//being erased by the default window procedure. Returning zero here will leave the
	//existing window content intact, and instead leave an erase pending for our WM_PAIN
	//message handler to process, which is what we want. Without processing this message,
	//there's a noticeable flicker when redrawing the control where the background is
	//erased before the WM_PAINT message is issued, such as when the control is resized.
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_PAINT(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintInfo;
	BeginPaint(hwnd, &paintInfo);
	HDC hdc = paintInfo.hdc;

	//Create a bitmap we can render the control onto
	HDC hdcControl = hdc;
	HDC hdcBitmap = CreateCompatibleDC(hdcControl);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdcControl, controlWidth, controlHeight);
	HBITMAP hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

	//Send a WM_PRINTCLIENT message to the native control and get it to render into
	//the bitmap.
	SendMessage(hwnd, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);

	//Transfer the final bitmap for the control into the screen buffer
	BitBlt(hdcControl, 0, 0, controlWidth, controlHeight, hdcBitmap, 0, 0, SRCCOPY);

	//Validate the entire client area of the control. It's REALLY important that we
	//call this here, otherwise Windows is going to send WM_PAINT messages over and
	//over again waiting for the region to be validated.
	ValidateRect(hwnd, NULL);

	//Clean up the allocated handles
	SelectObject(hdcBitmap, hbitmapOriginal);
	DeleteObject(hbitmap);
	DeleteDC(hdcBitmap);

	EndPaint(hwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_PRINTCLIENT(WPARAM wParam, LPARAM lParam)
{
	//Fill the window with the background colour
	HDC hdc = (HDC)wParam;
	HBRUSH backgroundBrush = CreateSolidBrush(colorBackground.GetColorREF());
	HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
	PatBlt(hdc, 0, 0, controlWidth, controlHeight, PATCOPY);
	SelectObject(hdc, backgroundBrushOld);
	DeleteObject(backgroundBrush);

	//Switch to our font
	HFONT hfontOld;
	hfontOld = (HFONT)SelectObject(hdc, hfont);

	//Draw the address numbers down the side
	for(unsigned int i = 0; i < visibleRows; ++i)
	{
		unsigned int lineAddress = dataViewPos + (i * columns * valuesPerColumn);
		if(lineAddress <= dataSize)
		{
			unsigned int printAddress = addressOffset + lineAddress;
			printAddress &= ((1 << (addressWidth * 4)) - 1);

			std::wstring text;
			IntToStringBase16(printAddress, text, addressWidth, false);

			SetTextColor(hdc, colorAddressText.GetColorREF());
			SetBkColor(hdc, colorAddressBack.GetColorREF());
			TextOut(hdc, 0, i * fontHeight, &text[0], (int)text.size());
		}
	}

	//Draw the dividing line between the address numbers and the data
	HPEN hpen = CreatePen(PS_SOLID, 0, colorLine.GetColorREF());
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
	MoveToEx(hdc, dataDividerLinePosX, 0, NULL);
	LineTo(hdc, dataDividerLinePosX, (visibleRows * fontHeight));
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);

	//Draw the custom background colours for each column. Note that we always draw the
	//text itself with a transparent background, otherwise we'll cut off the last row of
	//pixels for each character which directly followed by another one if ClearType
	//filtering is enabled.
	SetBkMode(hdc, OPAQUE);
	for(unsigned int columnNo = 0; columnNo < columns; ++columnNo)
	{
		unsigned int columnRasterPos = dataSectionPos;
		columnRasterPos += (columnNo * columnSpace);
		columnRasterPos += (columnNo * fontWidth * valuesPerColumn * 2);
		for(unsigned int rowNo = 0; rowNo < visibleRows; ++rowNo)
		{
			for(unsigned int valueNo = 0; valueNo < valuesPerColumn; ++valueNo)
			{
				unsigned int dataPos = (rowNo * columns * valuesPerColumn) + (columnNo * valuesPerColumn) + valueNo;
				unsigned int fullDataPos = dataViewPos + dataPos;

				//Determine the background colour
				WinColor colorBack;
				if((fullDataPos == selectedByte) && firstNybbleWritten)
				{
					colorBack = colorModifiedDataBack;
				}
				else if(dataMarkBuffer[dataPos])
				{
					colorBack = colorMarkedDataBack;
				}
				else
				{
					if((columnNo % 2) == 0)
					{
						colorBack = colorDataBack1;
					}
					else
					{
						colorBack = colorDataBack2;
					}
				}
				if(dragSelectActive && (fullDataPos >= dragSelectStartPos) && (fullDataPos < dragSelectEndPos))
				{
					colorBack.Invert();
				}

				//Fill the text region with a custom background colour if required
				if(colorBack.GetColorREF() != colorBackground.GetColorREF())
				{
					SetBkColor(hdc, colorBack.GetColorREF());
					TextOut(hdc, columnRasterPos + (valueNo * fontWidth * 2), rowNo * fontHeight, L"  ", 2);
				}
			}
		}
	}

	//Draw the data values in each column
	SetBkMode(hdc, TRANSPARENT);
	for(unsigned int columnNo = 0; columnNo < columns; ++columnNo)
	{
		unsigned int columnRasterPos = dataSectionPos;
		columnRasterPos += (columnNo * columnSpace);
		columnRasterPos += (columnNo * fontWidth * valuesPerColumn * 2);
		for(unsigned int rowNo = 0; rowNo < visibleRows; ++rowNo)
		{
			for(unsigned int valueNo = 0; valueNo < valuesPerColumn; ++valueNo)
			{
				unsigned int dataPos = (rowNo * columns * valuesPerColumn) + (columnNo * valuesPerColumn) + valueNo;
				unsigned int fullDataPos = dataViewPos + dataPos;

				//Determine the text colour
				WinColor colorText;
				if((fullDataPos == selectedByte) && firstNybbleWritten)
				{
					colorText = colorModifiedDataText;
				}
				else if(dataMarkBuffer[dataPos])
				{
					colorText = colorMarkedDataText;
				}
				else
				{
					if((columnNo % 2) == 0)
					{
						colorText = colorDataText1;
					}
					else
					{
						colorText = colorDataText2;
					}
				}
				if(dragSelectActive && (fullDataPos >= dragSelectStartPos) && (fullDataPos < dragSelectEndPos))
				{
					colorText.Invert();
				}

				//Set the text colour
				SetTextColor(hdc, colorText.GetColorREF());

				//Display the data
				if((dataViewPos + dataPos) < dataSize)
				{
					unsigned char byte = dataBuffer[dataPos];
					if((fullDataPos == selectedByte) && firstNybbleWritten)
					{
						byte = (storedNybble << 4) | (byte & 0xF);
					}
					std::wstring text;
					IntToStringBase16(byte, text, 2, false);
					TextOut(hdc, columnRasterPos + (valueNo * fontWidth * 2), rowNo * fontHeight, &text[0], 2);
				}
			}
		}
	}

	if(textDisplayEnabled)
	{
		//Draw the dividing line between the data and the text
		HPEN hpen = CreatePen(PS_SOLID, 0, colorLine.GetColorREF());
		HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
		MoveToEx(hdc, charDividerLinePosX, 0, NULL);
		LineTo(hdc, charDividerLinePosX, (visibleRows * fontHeight));
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);

		//Draw the custom background colours for each column. Note that we always draw the
		//text itself with a transparent background, otherwise we'll cut off the last row
		//of pixels for each character which directly followed by another one if ClearType
		//filtering is enabled.
		SetBkMode(hdc, OPAQUE);
		for(unsigned int rowNo = 0; rowNo < visibleRows; ++rowNo)
		{
			for(unsigned int i = 0; i < valuesPerRow; ++i)
			{
				unsigned int dataPos = (rowNo * valuesPerRow) + i;
				unsigned int fullDataPos = dataViewPos + (rowNo * valuesPerRow) + i;

				//Determine the background colour
				WinColor colorBack;
				if(dataMarkBuffer[dataPos])
				{
					colorBack = colorMarkedTextBack;
				}
				else
				{
					colorBack = colorTextBack;
				}
				if(dragSelectActive && (fullDataPos >= dragSelectStartPos) && (fullDataPos < dragSelectEndPos))
				{
					colorBack.Invert();
				}

				//Fill the text region with a custom background colour if required
				if(colorBack.GetColorREF() != colorBackground.GetColorREF())
				{
					SetBkColor(hdc, colorBack.GetColorREF());
					TextOut(hdc, textSectionPos + (i * fontWidth), rowNo * fontHeight, L" ", 1);
				}
			}
		}

		//Draw the text values
		SetBkMode(hdc, TRANSPARENT);
		for(unsigned int rowNo = 0; rowNo < visibleRows; ++rowNo)
		{
			for(unsigned int i = 0; i < valuesPerRow; ++i)
			{
				unsigned int dataPos = (rowNo * valuesPerRow) + i;
				unsigned int fullDataPos = dataViewPos + (rowNo * valuesPerRow) + i;

				char character = ' ';
				if(fullDataPos < dataSize)
				{
					unsigned char data = dataBuffer[dataPos];
					if((data < 0x20) || (data >= 0x7F))
					{
						character = '.';
					}
					else
					{
						character = (char)data;
					}
				}

				//Determine the text colour
				WinColor colorText;
				if(dataMarkBuffer[dataPos])
				{
					colorText = colorMarkedTextText;
				}
				else
				{
					colorText = colorTextText;
				}
				if(dragSelectActive && (fullDataPos >= dragSelectStartPos) && (fullDataPos < dragSelectEndPos))
				{
					colorText.Invert();
				}

				//Set the text colour
				SetTextColor(hdc, colorText.GetColorREF());

				//Draw the text
				wchar_t unicodeCharacter = (wchar_t)character;
				TextOut(hdc, textSectionPos + (i * fontWidth), rowNo * fontHeight, &unicodeCharacter, 1);
			}
		}
	}

	//Check if the selected byte is within the visible area of the control
	if(!dragSelectActive && (selectedByte >= dataViewPos) && (selectedByte < (dataViewPos + visibleValuesPerPage)))
	{
		//Calculate the vertical pixel coordinates of the selected value
		unsigned int selectedCharacterPosY = ((selectedByte - dataViewPos) / valuesPerRow) * fontHeight;

		//Underline or position the caret at the data value
		if(inputOnTextWindow || (GetFocus() != hwnd))
		{
			PatBlt(hdc, dataSectionPos + selectedCharacterDataPosX, selectedCharacterPosY + (fontHeight - 2), fontWidth * 2, 2, DSTINVERT);
		}
		else
		{
			unsigned int caretPosX = dataSectionPos + selectedCharacterDataPosX;
			if(firstNybbleWritten)
			{
				caretPosX += fontWidth;
			}
			SetCaretPos(caretPosX, selectedCharacterPosY);
		}

		if(textDisplayEnabled)
		{
			//Underline or position the caret at the text value
			if(!inputOnTextWindow || (GetFocus() != hwnd))
			{
				PatBlt(hdc, textSectionPos + selectedCharacterTextPosX, selectedCharacterPosY + (fontHeight - 2), fontWidth, 2, DSTINVERT);
			}
			else
			{
				SetCaretPos(textSectionPos + selectedCharacterTextPosX, selectedCharacterPosY);
			}
		}
	}
	else
	{
		if(GetFocus() == hwnd)
		{
			SetCaretPos(0, controlHeight);
		}
	}

	//Restore the state of the device context, and free any allocated handles
	SelectObject(hdc, hfontOld);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETWINDOWSIZE(WPARAM wParam, LPARAM lParam)
{
	dataSize = (unsigned int)wParam;
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETWINDOWPOS(WPARAM wParam, LPARAM lParam)
{
	dataViewPos = (unsigned int)wParam;
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETWINDOWADDRESSOFFSET(WPARAM wParam, LPARAM lParam)
{
	addressOffset = (unsigned int)wParam;
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETWINDOWADDRESSWIDTH(WPARAM wParam, LPARAM lParam)
{
	addressWidth = (unsigned int)wParam;
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_GETWINDOWSIZE(WPARAM wParam, LPARAM lParam)
{
	return visibleValuesPerPage;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_GETWINDOWPOS(WPARAM wParam, LPARAM lParam)
{
	return dataViewPos;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_UPDATEWINDOWDATA(WPARAM wParam, LPARAM lParam)
{
	Hex_UpdateWindowData* info = (Hex_UpdateWindowData*)lParam;
	for(unsigned int i = 0; (i < info->newBufferSize) && (i < visibleValuesPerPage); ++i)
	{
		if(info->newBufferData != 0)
		{
			dataBuffer[i] = *(info->newBufferData + i);
		}
		if(info->newMarkBufferData != 0)
		{
			dataMarkBuffer[i] = *(info->newMarkBufferData + i) != 0;
		}
	}
	InvalidateRect(hwnd, NULL, FALSE);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETMARKINGINFO(WPARAM wParam, LPARAM lParam)
{
	Hex_DataMarkingInfo* info = (Hex_DataMarkingInfo*)lParam;
	markingEnabled = info->markingEnabled;
	markName = info->markName;
	unmarkName = info->unmarkName;
	UpdateContextMenu();
	return 0;
}
