#include "WC_HexEdit.h"
#include "Stream/Stream.pkg"
#include "DataConversion/DataConversion.pkg"
#include <sstream>
#include <iomanip>

//----------------------------------------------------------------------------------------
//Constants
//----------------------------------------------------------------------------------------
const wchar_t* WC_HexEdit::WindowClassName = L"EX_HexEdit";

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WC_HexEdit::WC_HexEdit(HINSTANCE moduleHandle, HWND hwnd)
:_moduleHandle(moduleHandle), _hwnd(hwnd)
{
	_hfont = NULL;
	_hcontextMenu = NULL;

	_colorBackground = WinColor(255, 255, 255);
	_colorAddressText = WinColor(0, 0, 0);
	_colorAddressBack = WinColor(255, 255, 255);
	_colorLine = WinColor(0, 0, 0);
	_colorDataText1 = WinColor(0, 0, 255);
	_colorDataBack1 = WinColor(255, 255, 255);
	_colorDataText2 = WinColor(0, 0, 132);
	_colorDataBack2 = WinColor(255, 251, 255);
	_colorMarkedDataText = WinColor(255, 255, 255);
	_colorMarkedDataBack = WinColor(255, 0, 0);
	_colorModifiedDataText = WinColor(255, 0, 0);
	_colorModifiedDataBack = WinColor(255, 255, 255);
	_colorTextText = WinColor(0, 0, 0);
	_colorTextBack = WinColor(255, 255, 255);
	_colorMarkedTextText = WinColor(255, 255, 255);
	_colorMarkedTextBack = WinColor(255, 0, 0);

	_addressOffset = 0;
	_valuesPerColumn = 2;
	_addressWidth = 4;
	_textDisplayEnabled = true;
	_fontPointSize = 8;
	_fontTypefaceName = L"Courier New";

	_dataSize = 0;
	_dataViewPos = 0;

	_selectedByte = 0;
	_firstNybbleWritten = false;
	_inputOnTextWindow = false;

	_hwndTooltip = NULL;
	_tooltipVisible = false;

	_mouseButtonDown = false;
	_dragSelectActive = false;

	_markingEnabled = false;
}

//----------------------------------------------------------------------------------------
//Class registration
//----------------------------------------------------------------------------------------
bool WC_HexEdit::RegisterWindowClass(HINSTANCE moduleHandle)
{
	//Attempt to register the window class for this control, and return the result to the
	//caller.
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = moduleHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = WindowClassName;
	wc.hIconSm       = NULL;
	ATOM registerClassExReturn = RegisterClassEx(&wc);
	return (registerClassExReturn != 0);
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::UnregisterWindowClass(HINSTANCE moduleHandle)
{
	//Attempt to unregister the window class for this control, and return the result to
	//the caller.
	BOOL unregisterClassReturn = UnregisterClass(WindowClassName, moduleHandle);
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
	else if((unsigned int)newSelectPosSigned > _dataSize)
	{
		newSelectPos = _dataSize;
	}
	else
	{
		newSelectPos = (unsigned int)newSelectPosSigned;
	}

	if(newSelectPos < _selectedByte)
	{
		_dragSelectStartPos = newSelectPos;
		_dragSelectEndPos = _selectedByte;
	}
	else
	{
		_dragSelectStartPos = _selectedByte;
		_dragSelectEndPos = newSelectPos;
	}
	//Update the tooltip text, so that the drag select region shown in the tooltip will
	//be updated when the keyboard is used to adjust the selected region, or when the
	//mouse is being held outside the window area.
	UpdateTooltipText(_tooltipLastValue);
}

//----------------------------------------------------------------------------------------
unsigned int WC_HexEdit::GetDragSelectPos() const
{
	if(_dragSelectEndPos != _selectedByte)
	{
		return _dragSelectEndPos;
	}
	else
	{
		return _dragSelectStartPos;
	}
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::StartDragSelect()
{
	if(!_dragSelectActive)
	{
		_dragSelectActive = true;
		_dragSelectStartPos = _selectedByte;
		_dragSelectEndPos = _selectedByte;
		_lastCursorDragSelectPosX = 0;
		_lastCursorDragSelectPosY = 0;
		EnableSelectionMenuItems(true);
	}
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::StopDragSelect()
{
	if(_dragSelectActive)
	{
		_dragSelectActive = false;
		_selectedByte = GetDragSelectPos();
		EnableSelectionMenuItems(false);
	}
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::DragSelectActive() const
{
	return _dragSelectActive;
}

//----------------------------------------------------------------------------------------
//Tooltip functions
//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateTooltipText(unsigned int newSelectedByte)
{
	//Build the new text for the tooltip control
	std::wstringstream stream;
	unsigned int dragSelectSize = _dragSelectEndPos - _dragSelectStartPos;
	if(DragSelectActive() && (dragSelectSize > 0))
	{
		stream << L"0x" << std::setw(_addressWidth) << std::setfill(L'0') << std::hex << std::uppercase << _dragSelectStartPos;
		stream << L"-";
		stream << L"0x" << std::setw(_addressWidth) << std::setfill(L'0') << std::hex << std::uppercase << _dragSelectEndPos;
		stream << L" (";
		stream << "0x" << std::setw(_addressWidth) << std::setfill(L'0') << std::hex << std::uppercase << dragSelectSize;
		stream << L"/";
		stream << std::dec << dragSelectSize;
		stream << L")";
	}
	else
	{
		stream << L"0x" << std::setw(_addressWidth) << std::setfill(L'0') << std::hex << std::uppercase << newSelectedByte;
	}
	std::wstring text = stream.str();

	//Update the tooltip text
	if(text != _lastTooltipText)
	{
		_lastTooltipText = text;
		TOOLINFO toolInfo;
		toolInfo.cbSize = sizeof(TOOLINFO);
		toolInfo.lpReserved = 0;
		toolInfo.hwnd = _hwnd;
		toolInfo.hinst = _moduleHandle;
		toolInfo.uId = (UINT_PTR)_hwnd;
		toolInfo.lpszText = &text[0];
		SendMessage(_hwndTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
	}
}

//----------------------------------------------------------------------------------------
//Context menu functions
//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateContextMenu()
{
	//Create our rightclick context menu
	if(_hcontextMenu != NULL)
	{
		DestroyMenu(_hcontextMenu);
	}
	_hcontextMenu = CreatePopupMenu();

	std::wstring copyText = L"Copy";
	MENUITEMINFO copyMenuItem;
	copyMenuItem.cbSize = sizeof(copyMenuItem);
	copyMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	copyMenuItem.fType = MFT_STRING;
	copyMenuItem.wID = (UINT)MenuItem::Copy;
	copyMenuItem.dwTypeData = &copyText[0];
	copyMenuItem.cch = (UINT)copyText.size();
	InsertMenuItem(_hcontextMenu, 9999, TRUE, &copyMenuItem);

	std::wstring pasteText = L"Paste";
	MENUITEMINFO pasteMenuItem;
	pasteMenuItem.cbSize = sizeof(pasteMenuItem);
	pasteMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	pasteMenuItem.fType = MFT_STRING;
	pasteMenuItem.wID = (UINT)MenuItem::Paste;
	pasteMenuItem.dwTypeData = &pasteText[0];
	pasteMenuItem.cch = (UINT)pasteText.size();
	InsertMenuItem(_hcontextMenu, 9999, TRUE, &pasteMenuItem);

	if(_markingEnabled)
	{
		std::wstring markText = _markName;
		MENUITEMINFO markMenuItem;
		markMenuItem.cbSize = sizeof(markMenuItem);
		markMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
		markMenuItem.fType = MFT_STRING;
		markMenuItem.wID = (UINT)MenuItem::Mark;
		markMenuItem.dwTypeData = &markText[0];
		markMenuItem.cch = (UINT)markText.size();
		InsertMenuItem(_hcontextMenu, 9999, TRUE, &markMenuItem);

		std::wstring unmarkText = _unmarkName;
		MENUITEMINFO unmarkMenuItem;
		unmarkMenuItem.cbSize = sizeof(unmarkMenuItem);
		unmarkMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
		unmarkMenuItem.fType = MFT_STRING;
		unmarkMenuItem.wID = (UINT)MenuItem::Unmark;
		unmarkMenuItem.dwTypeData = &unmarkText[0];
		unmarkMenuItem.cch = (UINT)unmarkText.size();
		InsertMenuItem(_hcontextMenu, 9999, TRUE, &unmarkMenuItem);
	}

	MENUITEMINFO divider1MenuItem;
	divider1MenuItem.cbSize = sizeof(divider1MenuItem);
	divider1MenuItem.fMask = MIIM_FTYPE;
	divider1MenuItem.fType = MFT_SEPARATOR;
	InsertMenuItem(_hcontextMenu, 9999, TRUE, &divider1MenuItem);

	HMENU hcolumnSizeMenu = CreatePopupMenu();
	std::wstring columnSizeMenuText1 = L"1 byte";
	MENUITEMINFO columnSizeMenuItem1;
	columnSizeMenuItem1.cbSize = sizeof(columnSizeMenuItem1);
	columnSizeMenuItem1.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_STATE;
	columnSizeMenuItem1.fType = MFT_STRING | MFT_RADIOCHECK;
	columnSizeMenuItem1.fState = (_valuesPerColumn == 1)? MFS_CHECKED: MFS_UNCHECKED;
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
	columnSizeMenuItem2.fState = (_valuesPerColumn == 2)? MFS_CHECKED: MFS_UNCHECKED;
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
	columnSizeMenuItem4.fState = (_valuesPerColumn == 4)? MFS_CHECKED: MFS_UNCHECKED;
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
	columnSizeMenuItem8.fState = (_valuesPerColumn == 8)? MFS_CHECKED: MFS_UNCHECKED;
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
	columnSizeMenuItem16.fState = (_valuesPerColumn == 16)? MFS_CHECKED: MFS_UNCHECKED;
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
	InsertMenuItem(_hcontextMenu, 9999, TRUE, &columnSizeMenuItem);

	std::wstring enableTextMenuText = L"Show Text";
	MENUITEMINFO enableTextMenuItem;
	enableTextMenuItem.cbSize = sizeof(enableTextMenuItem);
	enableTextMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_STATE;
	enableTextMenuItem.fType = MFT_STRING;
	enableTextMenuItem.fState = (_textDisplayEnabled)? MFS_CHECKED: MFS_UNCHECKED;
	enableTextMenuItem.wID = (UINT)MenuItem::EnableText;
	enableTextMenuItem.dwTypeData = &enableTextMenuText[0];
	enableTextMenuItem.cch = (UINT)enableTextMenuText.size();
	InsertMenuItem(_hcontextMenu, 9999, TRUE, &enableTextMenuItem);

	MENUITEMINFO divider2MenuItem;
	divider2MenuItem.cbSize = sizeof(divider2MenuItem);
	divider2MenuItem.fMask = MIIM_FTYPE;
	divider2MenuItem.fType = MFT_SEPARATOR;
	InsertMenuItem(_hcontextMenu, 9999, TRUE, &divider2MenuItem);

	std::wstring saveToFileText = L"Save to File";
	MENUITEMINFO saveToFileMenuItem;
	saveToFileMenuItem.cbSize = sizeof(saveToFileMenuItem);
	saveToFileMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	saveToFileMenuItem.fType = MFT_STRING;
	saveToFileMenuItem.wID = (UINT)MenuItem::SaveToFile;
	saveToFileMenuItem.dwTypeData = &saveToFileText[0];
	saveToFileMenuItem.cch = (UINT)saveToFileText.size();
	InsertMenuItem(_hcontextMenu, 9999, TRUE, &saveToFileMenuItem);

	std::wstring loadFromFileText = L"Load from File";
	MENUITEMINFO loadFromFileMenuItem;
	loadFromFileMenuItem.cbSize = sizeof(loadFromFileMenuItem);
	loadFromFileMenuItem.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
	loadFromFileMenuItem.fType = MFT_STRING;
	loadFromFileMenuItem.wID = (UINT)MenuItem::LoadFromFile;
	loadFromFileMenuItem.dwTypeData = &loadFromFileText[0];
	loadFromFileMenuItem.cch = (UINT)loadFromFileText.size();
	InsertMenuItem(_hcontextMenu, 9999, TRUE, &loadFromFileMenuItem);

	EnableSelectionMenuItems(DragSelectActive());
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::EnableSelectionMenuItems(bool state)
{
	UINT enableState = state? MF_ENABLED: MF_GRAYED;
	EnableMenuItem(_hcontextMenu, (UINT)MenuItem::Copy, enableState);
	if(_markingEnabled)
	{
		EnableMenuItem(_hcontextMenu, (UINT)MenuItem::Mark, enableState);
		EnableMenuItem(_hcontextMenu, (UINT)MenuItem::Unmark, enableState);
	}
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::MarkSelection(bool state)
{
	if(_dragSelectActive)
	{
		Hex_UpdateDataMarkingState updateDataMarkingState;
		updateDataMarkingState.offset = _dragSelectStartPos;
		updateDataMarkingState.size = _dragSelectEndPos - _dragSelectStartPos;
		updateDataMarkingState.state = state;
		SendMessage(GetParent(_hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(_hwnd) & 0xFFFF), WindowNotifications::UpdateDataMarking), (LPARAM)&updateDataMarkingState);
	}
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::CopyToClipboard()
{
	if(!_dragSelectActive)
	{
		return false;
	}
	if(OpenClipboard(_hwnd) == 0)
	{
		return false;
	}

	//Copy data to the clipboard
	unsigned int dragSelectByteSize = _dragSelectEndPos - _dragSelectStartPos;
	unsigned int dragSelectTextSize = dragSelectByteSize;
	if(!_inputOnTextWindow)
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

		bool readBlockResult = ReadBlockToBuffer(_dragSelectStartPos, dragSelectByteSize, buffer);
		if(readBlockResult)
		{
			for(unsigned int i = 0; i < dragSelectByteSize; ++i)
			{
				if(_inputOnTextWindow)
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
	if(OpenClipboard(_hwnd) == 0)
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
		unsigned int pastePos = _selectedByte;
		if(DragSelectActive())
		{
			pastePos = _dragSelectStartPos;
			StopDragSelect();
		}
		unsigned int blockSize = _dataSize - pastePos;
		unsigned char* buffer = new unsigned char[blockSize];
		unsigned int bufferOffset = 0;

		if(_inputOnTextWindow)
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
		ForceByteIntoView(_selectedByte);
	}

	CloseClipboard();
	return true;
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::SaveToFile()
{
	//Obtain a path to the target file
	std::wstring selectedFilePath;
	if(!SelectNewFile(_hwnd, L"Binary files|bin", L"bin", L"", L"", selectedFilePath))
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
	unsigned int blockSize = _dataSize;
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
	if(!SelectExistingFile(_hwnd, L"Binary files|bin", L"bin", L"", L"", selectedFilePath))
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
	unsigned int blockSize = _dataSize;
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
	_dataViewPos = windowPos;
	_dataBuffer.resize(windowSize);
	_dataMarkBuffer.resize(windowSize);

	//Notify the parent control about the change in position and size so it can update
	//the data buffer.
	Hex_NewWindowPosInfo windowPosInfo;
	windowPosInfo.windowPos = _dataViewPos;
	windowPosInfo.windowSize = windowSize;
	SendMessage(GetParent(_hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(_hwnd) & 0xFFFF), WindowNotifications::NewWindowPos), (LPARAM)&windowPosInfo);
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateColumnSettings(unsigned int columnCount, unsigned int valuesPerColumn, bool textAreaEnabled)
{
	_columns = columnCount;
	valuesPerColumn = valuesPerColumn;
	_textDisplayEnabled = textAreaEnabled;
	//Ensure there is always at least one column visible, and at least one value in each
	//column.
	if(_columns < 1)
	{
		_columns = 1;
	}
	if(valuesPerColumn < 1)
	{
		valuesPerColumn = 1;
	}

	//Calculate the number of rows which can be shown within the control. We lock the
	//control at a minimum of 1 complete row.
	unsigned int completeRows = (_controlHeight / _fontHeight);
	unsigned int partialRows = ((_controlHeight % _fontHeight) != 0)? 1: 0;
	if(completeRows < 1)
	{
		completeRows = 1;
	}

	//Calculate the maximum number of lines we can display
	_valuesPerRow = _columns * valuesPerColumn;
	_valuesPerPage = completeRows * _valuesPerRow;
	_visibleRows = completeRows + partialRows;
	_visibleValuesPerPage = _visibleRows * _valuesPerRow;

	//Calculate the positions and sizes of the various sections within the control
	_addressSectionPos = 0;
	_addressSectionWidth = _addressWidth * _fontWidth;
	_dataDividerLinePosX = _addressSectionPos + _addressSectionWidth + (_dividerSpace / 2);
	_dataSectionPos = _addressSectionPos + _addressSectionWidth + _dividerSpace;
	_dataSectionWidth = (valuesPerColumn * _columns * _fontWidth * 2) + ((_columns - 1) * _columnSpace);
	_charDividerLinePosX = _dataSectionPos + _dataSectionWidth + (_dividerSpace / 2);
	_textSectionPos = _dataSectionPos + _dataSectionWidth + _dividerSpace;
	_textSectionWidth = valuesPerColumn * _columns * _fontWidth;

	//When the column size changes, we need to re-select the current byte so that
	//the caret is drawn in the correct location.
	SelectByte((int)_selectedByte);

	//Update the data window settings with the new size
	SetWindowSettings(_dataViewPos, _visibleValuesPerPage);
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::UpdateControlSize(unsigned int clientWidth, unsigned int clientHeight)
{
	_controlWidth = clientWidth;
	_controlHeight = clientHeight;

	//Calculate the maximum possible number of columns that can fit in the control
	int availableSpace = _controlWidth;
	availableSpace -= (_addressWidth * _fontWidth);
	availableSpace -= _dividerSpace;
	if(_textDisplayEnabled)
	{
		availableSpace -= _dividerSpace;
	}
	if(availableSpace < 0)
	{
		availableSpace = 0;
	}
	unsigned int columnWidth = (_valuesPerColumn * 2 * _fontWidth) + _columnSpace;
	if(_textDisplayEnabled)
	{
		columnWidth += _fontWidth * _valuesPerColumn;
	}
	unsigned int newColumnCount = (unsigned int)availableSpace / columnWidth;
	if(newColumnCount != _columns)
	{
		//Only force a full redraw if the column sizes change. This prevents
		//excessive flickering when resizing the control.
		InvalidateRect(_hwnd, NULL, FALSE);
	}
	//Apply the new window size settings
	UpdateColumnSettings(newColumnCount, _valuesPerColumn, _textDisplayEnabled);

	//Calculate the new settings for the scrollbar
	unsigned int newScrollCurrentPos = 0;
	unsigned int newScrollMaxPos = 0;
	if(_valuesPerRow > 0)
	{
		newScrollCurrentPos = _dataViewPos - (_dataViewPos % _valuesPerRow);
		newScrollMaxPos = _dataSize - (_dataSize % _valuesPerRow) + _valuesPerRow;
	}
	SetWindowSettings(newScrollCurrentPos, _visibleValuesPerPage);
	_scrollMaxPos = newScrollMaxPos;

	//Apply the new scrollbar settings
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.nMin = 0;
	scrollInfo.nMax = _scrollMaxPos;
	scrollInfo.nPos = _dataViewPos;
	scrollInfo.nPage = _valuesPerPage;
	scrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	SetScrollInfo(_hwnd, SB_VERT, &scrollInfo, TRUE);
}

//----------------------------------------------------------------------------------------
//Byte selection functions
//----------------------------------------------------------------------------------------
bool WC_HexEdit::DataSelectedFromCursorPos(int cursorPosX, int cursorPosY) const
{
	return (cursorPosX > (int)_dataSectionPos) && (cursorPosX < ((int)_dataSectionPos + (int)_dataSectionWidth));
}

//----------------------------------------------------------------------------------------
bool WC_HexEdit::TextSelectedFromCursorPos(int cursorPosX, int cursorPosY) const
{
	return (cursorPosX > (int)_textSectionPos) && (cursorPosX < ((int)_textSectionPos + (int)_textSectionWidth));
}

//----------------------------------------------------------------------------------------
int WC_HexEdit::ByteNumberFromCursorPos(int cursorPosX, int cursorPosY) const
{
	int selectedRow = cursorPosY / (int)_fontHeight;
	int newSelectedByte = (int)_dataViewPos + (selectedRow * (int)_valuesPerRow);
	if(DataSelectedFromCursorPos(cursorPosX, cursorPosY))
	{
		unsigned int columnWidth = (_fontWidth * _valuesPerColumn * 2) + _columnSpace;
		unsigned int columnNo = (cursorPosX - _dataSectionPos) / columnWidth;
		unsigned int valueNo = ((cursorPosX - _dataSectionPos) % columnWidth) / (_fontWidth * 2);
		newSelectedByte += (int)((columnNo * _valuesPerColumn) + valueNo);
	}
	else if(TextSelectedFromCursorPos(cursorPosX, cursorPosY))
	{
		newSelectedByte += (cursorPosX - (int)_textSectionPos) / (int)_fontWidth;
	}
	return newSelectedByte;
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::SelectByte(int bytePos)
{
	//Commit the write to the current byte if one is in progress
	if(_firstNybbleWritten)
	{
		_firstNybbleWritten = false;
		unsigned char originalByte = ReadByte(_selectedByte);
		unsigned char dataToWrite = (_storedNybble << 4) | (originalByte & 0x0F);
		WriteByte(_selectedByte, dataToWrite);
	}

	//Don't allow the selected byte to move past the non-existent element after the last
	//byte.
	if(bytePos < 0)
	{
		bytePos = 0;
	}
	else if(bytePos > (int)_dataSize)
	{
		bytePos = (int)_dataSize;
	}
	_selectedByte = (unsigned int)bytePos;

	//Calculate the position of the selected byte in the data region
	unsigned int selectedCharacterValue = _selectedByte % _valuesPerRow;
	_selectedCharacterDataPosX = ((selectedCharacterValue % _valuesPerColumn) * (_fontWidth * 2));
	_selectedCharacterDataPosX += ((selectedCharacterValue / _valuesPerColumn) * (_columnSpace + (_valuesPerColumn * _fontWidth * 2)));

	if(_textDisplayEnabled)
	{
		//Calculate the position of the selected byte in the text region
		_selectedCharacterTextPosX = (selectedCharacterValue * _fontWidth);
	}
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::ForceByteIntoView(unsigned int bytePos)
{
	//If the caret position is outside the currently viewable area of the control, shift
	//the view position to bring it back into view.
	if(bytePos < _dataViewPos)
	{
		int newViewPos = bytePos - (bytePos % _valuesPerRow);
		UpdateViewPos(newViewPos);
	}
	else if(bytePos >= (_dataViewPos + _valuesPerPage))
	{
		int newViewPos = ((bytePos - (bytePos % _valuesPerRow)) - _valuesPerPage) + _valuesPerRow;
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
	else if((unsigned int)newViewPosSigned > (_scrollMaxPos - _valuesPerPage))
	{
		newViewPos = (_scrollMaxPos - _valuesPerPage);
	}
	else
	{
		newViewPos = (unsigned int)newViewPosSigned;
	}

	if(newViewPos != _dataViewPos)
	{
		SetWindowSettings(newViewPos, _visibleValuesPerPage);

		//Force the entire control to redraw if the view position changes
		InvalidateRect(_hwnd, NULL, FALSE);

		//Update the current scroll position
		SCROLLINFO scrollInfo;
		scrollInfo.cbSize = sizeof(scrollInfo);
		scrollInfo.fMask = SIF_POS;
		scrollInfo.nPos = newViewPos;
		SetScrollInfo(_hwnd, SB_VERT, &scrollInfo, TRUE);
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
	SendMessage(GetParent(_hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(_hwnd) & 0xFFFF), WindowNotifications::ReadData), (LPARAM)&readDataInfo);
	return readDataInfo.data;
}

//----------------------------------------------------------------------------------------
void WC_HexEdit::WriteByte(unsigned int bytePos, unsigned char data)
{
	Hex_WriteDataInfo writeDataInfo;
	writeDataInfo.offset = bytePos;
	writeDataInfo.data = data;
	SendMessage(GetParent(_hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(_hwnd) & 0xFFFF), WindowNotifications::WriteData), (LPARAM)&writeDataInfo);
	if((bytePos >= _dataViewPos) && (bytePos < (_dataViewPos + _visibleValuesPerPage)))
	{
		_dataBuffer[bytePos - _dataViewPos] = data;
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
	SendMessage(GetParent(_hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(_hwnd) & 0xFFFF), WindowNotifications::ReadDataBlock), (LPARAM)&readDataBlockInfo);
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
	SendMessage(GetParent(_hwnd), WM_COMMAND, MAKEWPARAM(((long long)GetMenu(_hwnd) & 0xFFFF), WindowNotifications::WriteDataBlock), (LPARAM)&writeDataBlockInfo);
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

	case (unsigned int)WindowMessages::SetWindowSize:
		return msgHEX_SETWINDOWSIZE(wParam, lParam);
	case (unsigned int)WindowMessages::SetWindowPos:
		return msgHEX_SETWINDOWPOS(wParam, lParam);
	case (unsigned int)WindowMessages::SetWindowAddressOffset:
		return msgHEX_SETWINDOWADDRESSOFFSET(wParam, lParam);
	case (unsigned int)WindowMessages::SetWindowAddressWidth:
		return msgHEX_SETWINDOWADDRESSWIDTH(wParam, lParam);
	case (unsigned int)WindowMessages::GetWindowSize:
		return msgHEX_GETWINDOWSIZE(wParam, lParam);
	case (unsigned int)WindowMessages::GetWindowPos:
		return msgHEX_GETWINDOWPOS(wParam, lParam);
	case (unsigned int)WindowMessages::UpdateWindowData:
		return msgHEX_UPDATEWINDOWDATA(wParam, lParam);
	case (unsigned int)WindowMessages::SetMarkingInfo:
		return msgHEX_SETMARKINGINFO(wParam, lParam);
	}

	return DefWindowProc(_hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_CREATE(WPARAM wParam, LPARAM lParam)
{
	//Set our font options, and read the font metrics
	HDC hdc = GetDC(_hwnd);
	int fontnHeight = -MulDiv(_fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	_hfont = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &_fontTypefaceName[0]);
	HFONT hfontOld = (HFONT)SelectObject(hdc, _hfont);

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
	_fontWidth = textMetric.tmAveCharWidth;
	_fontHeight = textMetric.tmHeight;
	_dividerSpace = (unsigned int)((float)_fontWidth * 1.0f);
	_columnSpace = (unsigned int)((float)_fontWidth * 0.7f);

	SelectObject(hdc, hfontOld);
	ReleaseDC(_hwnd, hdc);

	UpdateContextMenu();

	//Initialize common controls
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_BAR_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	//Create a tooltip window
	_hwndTooltip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_NOFADE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, _hwnd, NULL, _moduleHandle, NULL);

	//Set the tooltip window as topmost
	SetWindowPos(_hwndTooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	//Define the properties of the tooltip
	std::wstring text;
	text = L" ";
	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.lpReserved = 0;
	toolInfo.uFlags = TTF_TRACK | TTF_ABSOLUTE | TTF_IDISHWND;
	toolInfo.hwnd = _hwnd;
	toolInfo.hinst = _moduleHandle;
	toolInfo.uId = (UINT_PTR)_hwnd;
	toolInfo.lpszText = &text[0];

	//Register the tooltip with the tooltip control
	SendMessage(_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_TIMER(WPARAM wParam, LPARAM lParam)
{
	//If drag select has been activated, update the end position
	int newSelectedByte = ByteNumberFromCursorPos(_lastCursorDragSelectPosX, _lastCursorDragSelectPosY);
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
		if(_tooltipVisible)
		{
			_tooltipVisible = false;
			TOOLINFO toolInfo;
			toolInfo.cbSize = sizeof(TOOLINFO);
			toolInfo.lpReserved = 0;
			toolInfo.hwnd = _hwnd;
			toolInfo.uId = (UINT_PTR)_hwnd;
			SendMessage(_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&toolInfo);
		}
		return 0;
	}

	//If drag select has been activated, update the end position
	if(_mouseButtonDown && (newSelectedByte != (int)GetDragSelectPos()))
	{
		_lastCursorDragSelectPosX = cursorPosX;
		_lastCursorDragSelectPosY = cursorPosY;
		if(!DragSelectActive())
		{
			StartDragSelect();
			SetTimer(_hwnd, 1, 50, NULL);
		}
		UpdateDragSelectPos(newSelectedByte);
		ForceByteIntoView(GetDragSelectPos());
	}

	//If the tooltip is currently hidden, activate it. Note that we enable the tooltip
	//before we position it, as due to an undocumented quirk or bug in the Win32 API, we
	//get a crash if we send a TTM_GETBUBBLESIZE message to the tooltip when it is not
	//activated.
	bool tooltipInitiallyVisible = _tooltipVisible;
	if(!_tooltipVisible)
	{
		_tooltipVisible = true;
		TOOLINFO toolInfo;
		toolInfo.cbSize = sizeof(TOOLINFO);
		toolInfo.lpReserved = 0;
		toolInfo.hwnd = _hwnd;
		toolInfo.uId = (UINT_PTR)_hwnd;
		SendMessage(_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&toolInfo);

		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(trackMouseEvent);
		trackMouseEvent.dwFlags = TME_LEAVE;
		trackMouseEvent.hwndTrack = _hwnd;
		TrackMouseEvent(&trackMouseEvent);
	}

	//If we've only just activated the tooltip control, or the selected byte has
	//changed, update the text for the tooltip.
	if(!tooltipInitiallyVisible || ((int)_tooltipLastValue != newSelectedByte))
	{
		UpdateTooltipText(newSelectedByte);
	}
	_tooltipLastValue = newSelectedByte;

	//Read the current dimensions of the tooltip
	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.lpReserved = 0;
	toolInfo.hwnd = _hwnd;
	toolInfo.hinst = _moduleHandle;
	toolInfo.uId = (UINT_PTR)_hwnd;
	LRESULT tooltipSize = SendMessage(_hwndTooltip, TTM_GETBUBBLESIZE, 0, (LPARAM)&toolInfo);

	//Calculate the position of the mouse cursor in screen coordinates
	POINT position;
	position.x = (short)LOWORD(lParam);
	position.y = (short)HIWORD(lParam);
	ClientToScreen(_hwnd, &position);

	//Adjust the tooltip position to place it above the cursor
	position.y -= (short)HIWORD(tooltipSize);
	position.x += 8;
	position.y -= 8;
	SendMessage(_hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(position.x, position.y));

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_MOUSELEAVE(WPARAM wParam, LPARAM lParam)
{
	_tooltipVisible = false;
	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.lpReserved = 0;
	toolInfo.hwnd = _hwnd;
	toolInfo.uId = (UINT_PTR)_hwnd;
	SendMessage(_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&toolInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_CONTEXTMENU(WPARAM wParam, LPARAM lParam)
{
	TrackPopupMenuEx(_hcontextMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, (short)LOWORD(lParam), (short)HIWORD(lParam), _hwnd, NULL);
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
			_textDisplayEnabled = !_textDisplayEnabled;
			RECT rect;
			GetClientRect(_hwnd, &rect);
			UpdateControlSize(rect.right, rect.bottom);
			MENUITEMINFO menuItemInfo;
			menuItemInfo.cbSize = sizeof(menuItemInfo);
			menuItemInfo.fMask = MIIM_STATE;
			menuItemInfo.fState = (_textDisplayEnabled)? MFS_CHECKED: MFS_UNCHECKED;
			SetMenuItemInfo(_hcontextMenu, (UINT)MenuItem::EnableText, FALSE, &menuItemInfo);
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
			_valuesPerColumn = newColumnWidth;
			RECT rect;
			GetClientRect(_hwnd, &rect);
			UpdateControlSize(rect.right, rect.bottom);

			MENUITEMINFO menuItemInfo;
			menuItemInfo.cbSize = sizeof(menuItemInfo);
			menuItemInfo.fMask = MIIM_DATA;
			GetMenuItemInfo(_hcontextMenu, (UINT)MenuItem::ColumnWidth, FALSE, &menuItemInfo);
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
	DeleteObject(_hfont);
	DestroyMenu(_hcontextMenu);
	return DefWindowProc(_hwnd, WM_DESTROY, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_KILLFOCUS(WPARAM wParam, LPARAM lParam)
{
	//Destroy the window caret
	DestroyCaret();

	//Force the entire control to redraw
	InvalidateRect(_hwnd, NULL, FALSE);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_SETFOCUS(WPARAM wParam, LPARAM lParam)
{
	//Create a caret for the window
	CreateCaret(_hwnd, NULL, 2, _fontHeight);
	ShowCaret(_hwnd);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_LBUTTONUP(WPARAM wParam, LPARAM lParam)
{
	ReleaseCapture();
	KillTimer(_hwnd, 1);

	_mouseButtonDown = false;
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
		_inputOnTextWindow = false;
	}
	else if(TextSelectedFromCursorPos(cursorPosX, cursorPosY))
	{
		byteSelected = true;
		_inputOnTextWindow = true;
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
			SetCapture(_hwnd);

			//Stop drag-select if it is currently in progress, and flag that the left
			//mouse button is being held down.
			StopDragSelect();
			_mouseButtonDown = true;

			//Apply the new selection
			SelectByte((int)newSelectedByte);
			ForceByteIntoView(_selectedByte);
		}
		//Force the entire control to redraw if the selected byte changes
		InvalidateRect(_hwnd, NULL, FALSE);
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
			UpdateDragSelectPos((int)_dataSize);
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
		_inputOnTextWindow = !_inputOnTextWindow;
		SelectByte((int)_selectedByte);
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
			SelectByte((int)_selectedByte - 1);
			ForceByteIntoView(_selectedByte);
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
			SelectByte((int)_selectedByte + 1);
			ForceByteIntoView(_selectedByte);
		}
		break;
	case VK_UP:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() - (int)_valuesPerRow);
			ForceByteIntoView(GetDragSelectPos());
		}
		else
		{
			StopDragSelect();
			SelectByte((int)_selectedByte - (int)_valuesPerRow);
			ForceByteIntoView(_selectedByte);
		}
		break;
	case VK_DOWN:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() + (int)_valuesPerRow);
			ForceByteIntoView(GetDragSelectPos());
		}
		else
		{
			StopDragSelect();
			SelectByte((int)_selectedByte + (int)_valuesPerRow);
			ForceByteIntoView(_selectedByte);
		}
		break;
	case VK_PRIOR:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() - (int)_valuesPerPage);
			UpdateViewPos((int)_dataViewPos - (int)_valuesPerPage);
		}
		else
		{
			StopDragSelect();
			SelectByte((int)_selectedByte - (int)_valuesPerPage);
			UpdateViewPos((int)_dataViewPos - (int)_valuesPerPage);
		}
		break;
	case VK_NEXT:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)GetDragSelectPos() + (int)_valuesPerPage);
			UpdateViewPos((int)_dataViewPos + (int)_valuesPerPage);
		}
		else
		{
			StopDragSelect();
			SelectByte((int)_selectedByte + (int)_valuesPerPage);
			UpdateViewPos((int)_dataViewPos + (int)_valuesPerPage);
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
			ForceByteIntoView(_selectedByte);
		}
		break;
	case VK_END:
		if((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			StartDragSelect();
			UpdateDragSelectPos((int)_dataSize);
			ForceByteIntoView(GetDragSelectPos());
		}
		else
		{
			StopDragSelect();
			SelectByte((int)_dataSize);
			ForceByteIntoView(_selectedByte);
		}
		break;
	}

	//Force the entire control to redraw
	InvalidateRect(_hwnd, NULL, FALSE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_CHAR(WPARAM wParam, LPARAM lParam)
{
	//If the caret position is past the last byte of the block, ignore all characters.
	if(_selectedByte >= _dataSize)
	{
		return 0;
	}

	ForceByteIntoView(_selectedByte);

	if(_inputOnTextWindow)
	{
		unsigned char byte = (unsigned char)wParam;
		if((byte >= 0x20) && (byte < 0x7F))
		{
			WriteByte(_selectedByte, byte);
			SelectByte((int)_selectedByte + 1);
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
			if(_firstNybbleWritten)
			{
				_firstNybbleWritten = false;
				dataToWrite = (_storedNybble << 4) | dataToWrite;
				WriteByte(_selectedByte, dataToWrite);
				SelectByte((int)_selectedByte + 1);
			}
			else
			{
				_firstNybbleWritten = true;
				_storedNybble = dataToWrite;
			}
		}
	}

	ForceByteIntoView(_selectedByte);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam)
{
	//Calculate the number of lines to scroll in the window. We displace the view pos
	//by a minimum of 3 lines per division of the scroll wheel. We also invert the
	//sign, since we want a scroll down to increase the view position.
	int scrollUnits = ((short)HIWORD(wParam) / WHEEL_DELTA);
	scrollUnits *= -3 * _valuesPerRow;

	UpdateViewPos(_dataViewPos + scrollUnits);

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
		GetScrollInfo(_hwnd, SB_VERT, &scrollInfo);
		int newViewPos = scrollInfo.nTrackPos - (scrollInfo.nTrackPos % _valuesPerRow);
		UpdateViewPos(newViewPos);
		break;}
	case SB_TOP:
		UpdateViewPos(0);
		break;
	case SB_BOTTOM:
		UpdateViewPos(_scrollMaxPos - _valuesPerPage);
		break;
	case SB_PAGEUP:
		UpdateViewPos(_dataViewPos - _valuesPerPage);
		break;
	case SB_PAGEDOWN:
		UpdateViewPos(_dataViewPos + _valuesPerPage);
		break;
	case SB_LINEUP:
		UpdateViewPos(_dataViewPos - _valuesPerRow);
		break;
	case SB_LINEDOWN:
		UpdateViewPos(_dataViewPos + _valuesPerRow);
		break;
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_SIZE(WPARAM wParam, LPARAM lParam)
{
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(_hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam)
{
	//We handle the WM_ERASEBKGND message here and return 0 to prevent the background
	//being erased by the default window procedure. Returning zero here will leave the
	//existing window content intact, and instead leave an erase pending for our WM_PAINT
	//message handler to process, which is what we want. Without processing this message,
	//there's a noticeable flicker when redrawing the control where the background is
	//erased before the WM_PAINT message is issued, such as when the control is resized.
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_PAINT(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintInfo;
	BeginPaint(_hwnd, &paintInfo);
	HDC hdc = paintInfo.hdc;

	//Create a bitmap we can render the control onto
	HDC hdcControl = hdc;
	HDC hdcBitmap = CreateCompatibleDC(hdcControl);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdcControl, _controlWidth, _controlHeight);
	HBITMAP hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

	//Send a WM_PRINTCLIENT message to the native control and get it to render into
	//the bitmap.
	SendMessage(_hwnd, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);

	//Transfer the final bitmap for the control into the screen buffer
	BitBlt(hdcControl, 0, 0, _controlWidth, _controlHeight, hdcBitmap, 0, 0, SRCCOPY);

	//Validate the entire client area of the control. It's REALLY important that we
	//call this here, otherwise Windows is going to send WM_PAINT messages over and
	//over again waiting for the region to be validated.
	ValidateRect(_hwnd, NULL);

	//Clean up the allocated handles
	SelectObject(hdcBitmap, hbitmapOriginal);
	DeleteObject(hbitmap);
	DeleteDC(hdcBitmap);

	EndPaint(_hwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgWM_PRINTCLIENT(WPARAM wParam, LPARAM lParam)
{
	//Fill the window with the background colour
	HDC hdc = (HDC)wParam;
	HBRUSH backgroundBrush = CreateSolidBrush(_colorBackground.GetColorREF());
	HBRUSH backgroundBrushOld = (HBRUSH)SelectObject(hdc, backgroundBrush);
	PatBlt(hdc, 0, 0, _controlWidth, _controlHeight, PATCOPY);
	SelectObject(hdc, backgroundBrushOld);
	DeleteObject(backgroundBrush);

	//Switch to our font
	HFONT hfontOld;
	hfontOld = (HFONT)SelectObject(hdc, _hfont);

	//Draw the address numbers down the side
	for(unsigned int i = 0; i < _visibleRows; ++i)
	{
		unsigned int lineAddress = _dataViewPos + (i * _columns * _valuesPerColumn);
		if(lineAddress <= _dataSize)
		{
			unsigned int printAddress = _addressOffset + lineAddress;
			printAddress &= ((1 << (_addressWidth * 4)) - 1);

			std::wstring text;
			IntToStringBase16(printAddress, text, _addressWidth, false);

			SetTextColor(hdc, _colorAddressText.GetColorREF());
			SetBkColor(hdc, _colorAddressBack.GetColorREF());
			TextOut(hdc, 0, i * _fontHeight, &text[0], (int)text.size());
		}
	}

	//Draw the dividing line between the address numbers and the data
	HPEN hpen = CreatePen(PS_SOLID, 0, _colorLine.GetColorREF());
	HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
	MoveToEx(hdc, _dataDividerLinePosX, 0, NULL);
	LineTo(hdc, _dataDividerLinePosX, (_visibleRows * _fontHeight));
	SelectObject(hdc, hpenOld);
	DeleteObject(hpen);

	//Draw the custom background colours for each column. Note that we always draw the
	//text itself with a transparent background, otherwise we'll cut off the last row of
	//pixels for each character which directly followed by another one if ClearType
	//filtering is enabled.
	SetBkMode(hdc, OPAQUE);
	for(unsigned int columnNo = 0; columnNo < _columns; ++columnNo)
	{
		unsigned int columnRasterPos = _dataSectionPos;
		columnRasterPos += (columnNo * _columnSpace);
		columnRasterPos += (columnNo * _fontWidth * _valuesPerColumn * 2);
		for(unsigned int rowNo = 0; rowNo < _visibleRows; ++rowNo)
		{
			for(unsigned int valueNo = 0; valueNo < _valuesPerColumn; ++valueNo)
			{
				unsigned int dataPos = (rowNo * _columns * _valuesPerColumn) + (columnNo * _valuesPerColumn) + valueNo;
				unsigned int fullDataPos = _dataViewPos + dataPos;

				//Determine the background colour
				WinColor colorBack;
				if((fullDataPos == _selectedByte) && _firstNybbleWritten)
				{
					colorBack = _colorModifiedDataBack;
				}
				else if(_dataMarkBuffer[dataPos])
				{
					colorBack = _colorMarkedDataBack;
				}
				else
				{
					if((columnNo % 2) == 0)
					{
						colorBack = _colorDataBack1;
					}
					else
					{
						colorBack = _colorDataBack2;
					}
				}
				if(_dragSelectActive && (fullDataPos >= _dragSelectStartPos) && (fullDataPos < _dragSelectEndPos))
				{
					colorBack.Invert();
				}

				//Fill the text region with a custom background colour if required
				if(colorBack.GetColorREF() != _colorBackground.GetColorREF())
				{
					SetBkColor(hdc, colorBack.GetColorREF());
					TextOut(hdc, columnRasterPos + (valueNo * _fontWidth * 2), rowNo * _fontHeight, L"  ", 2);
				}
			}
		}
	}

	//Draw the data values in each column
	SetBkMode(hdc, TRANSPARENT);
	for(unsigned int columnNo = 0; columnNo < _columns; ++columnNo)
	{
		unsigned int columnRasterPos = _dataSectionPos;
		columnRasterPos += (columnNo * _columnSpace);
		columnRasterPos += (columnNo * _fontWidth * _valuesPerColumn * 2);
		for(unsigned int rowNo = 0; rowNo < _visibleRows; ++rowNo)
		{
			for(unsigned int valueNo = 0; valueNo < _valuesPerColumn; ++valueNo)
			{
				unsigned int dataPos = (rowNo * _columns * _valuesPerColumn) + (columnNo * _valuesPerColumn) + valueNo;
				unsigned int fullDataPos = _dataViewPos + dataPos;

				//Determine the text colour
				WinColor colorText;
				if((fullDataPos == _selectedByte) && _firstNybbleWritten)
				{
					colorText = _colorModifiedDataText;
				}
				else if(_dataMarkBuffer[dataPos])
				{
					colorText = _colorMarkedDataText;
				}
				else
				{
					if((columnNo % 2) == 0)
					{
						colorText = _colorDataText1;
					}
					else
					{
						colorText = _colorDataText2;
					}
				}
				if(_dragSelectActive && (fullDataPos >= _dragSelectStartPos) && (fullDataPos < _dragSelectEndPos))
				{
					colorText.Invert();
				}

				//Set the text colour
				SetTextColor(hdc, colorText.GetColorREF());

				//Display the data
				if((_dataViewPos + dataPos) < _dataSize)
				{
					unsigned char byte = _dataBuffer[dataPos];
					if((fullDataPos == _selectedByte) && _firstNybbleWritten)
					{
						byte = (_storedNybble << 4) | (byte & 0xF);
					}
					std::wstring text;
					IntToStringBase16(byte, text, 2, false);
					TextOut(hdc, columnRasterPos + (valueNo * _fontWidth * 2), rowNo * _fontHeight, &text[0], 2);
				}
			}
		}
	}

	if(_textDisplayEnabled)
	{
		//Draw the dividing line between the data and the text
		HPEN hpen = CreatePen(PS_SOLID, 0, _colorLine.GetColorREF());
		HPEN hpenOld = (HPEN)SelectObject(hdc, hpen);
		MoveToEx(hdc, _charDividerLinePosX, 0, NULL);
		LineTo(hdc, _charDividerLinePosX, (_visibleRows * _fontHeight));
		SelectObject(hdc, hpenOld);
		DeleteObject(hpen);

		//Draw the custom background colours for each column. Note that we always draw the
		//text itself with a transparent background, otherwise we'll cut off the last row
		//of pixels for each character which directly followed by another one if ClearType
		//filtering is enabled.
		SetBkMode(hdc, OPAQUE);
		for(unsigned int rowNo = 0; rowNo < _visibleRows; ++rowNo)
		{
			for(unsigned int i = 0; i < _valuesPerRow; ++i)
			{
				unsigned int dataPos = (rowNo * _valuesPerRow) + i;
				unsigned int fullDataPos = _dataViewPos + (rowNo * _valuesPerRow) + i;

				//Determine the background colour
				WinColor colorBack;
				if(_dataMarkBuffer[dataPos])
				{
					colorBack = _colorMarkedTextBack;
				}
				else
				{
					colorBack = _colorTextBack;
				}
				if(_dragSelectActive && (fullDataPos >= _dragSelectStartPos) && (fullDataPos < _dragSelectEndPos))
				{
					colorBack.Invert();
				}

				//Fill the text region with a custom background colour if required
				if(colorBack.GetColorREF() != _colorBackground.GetColorREF())
				{
					SetBkColor(hdc, colorBack.GetColorREF());
					TextOut(hdc, _textSectionPos + (i * _fontWidth), rowNo * _fontHeight, L" ", 1);
				}
			}
		}

		//Draw the text values
		SetBkMode(hdc, TRANSPARENT);
		for(unsigned int rowNo = 0; rowNo < _visibleRows; ++rowNo)
		{
			for(unsigned int i = 0; i < _valuesPerRow; ++i)
			{
				unsigned int dataPos = (rowNo * _valuesPerRow) + i;
				unsigned int fullDataPos = _dataViewPos + (rowNo * _valuesPerRow) + i;

				char character = ' ';
				if(fullDataPos < _dataSize)
				{
					unsigned char data = _dataBuffer[dataPos];
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
				if(_dataMarkBuffer[dataPos])
				{
					colorText = _colorMarkedTextText;
				}
				else
				{
					colorText = _colorTextText;
				}
				if(_dragSelectActive && (fullDataPos >= _dragSelectStartPos) && (fullDataPos < _dragSelectEndPos))
				{
					colorText.Invert();
				}

				//Set the text colour
				SetTextColor(hdc, colorText.GetColorREF());

				//Draw the text
				wchar_t unicodeCharacter = (wchar_t)character;
				TextOut(hdc, _textSectionPos + (i * _fontWidth), rowNo * _fontHeight, &unicodeCharacter, 1);
			}
		}
	}

	//Check if the selected byte is within the visible area of the control
	if(!_dragSelectActive && (_selectedByte >= _dataViewPos) && (_selectedByte < (_dataViewPos + _visibleValuesPerPage)))
	{
		//Calculate the vertical pixel coordinates of the selected value
		unsigned int selectedCharacterPosY = ((_selectedByte - _dataViewPos) / _valuesPerRow) * _fontHeight;

		//Underline or position the caret at the data value
		if(_inputOnTextWindow || (GetFocus() != _hwnd))
		{
			PatBlt(hdc, _dataSectionPos + _selectedCharacterDataPosX, selectedCharacterPosY + (_fontHeight - 2), _fontWidth * 2, 2, DSTINVERT);
		}
		else
		{
			unsigned int caretPosX = _dataSectionPos + _selectedCharacterDataPosX;
			if(_firstNybbleWritten)
			{
				caretPosX += _fontWidth;
			}
			SetCaretPos(caretPosX, selectedCharacterPosY);
		}

		if(_textDisplayEnabled)
		{
			//Underline or position the caret at the text value
			if(!_inputOnTextWindow || (GetFocus() != _hwnd))
			{
				PatBlt(hdc, _textSectionPos + _selectedCharacterTextPosX, selectedCharacterPosY + (_fontHeight - 2), _fontWidth, 2, DSTINVERT);
			}
			else
			{
				SetCaretPos(_textSectionPos + _selectedCharacterTextPosX, selectedCharacterPosY);
			}
		}
	}
	else
	{
		if(GetFocus() == _hwnd)
		{
			SetCaretPos(0, _controlHeight);
		}
	}

	//Restore the state of the device context, and free any allocated handles
	SelectObject(hdc, hfontOld);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETWINDOWSIZE(WPARAM wParam, LPARAM lParam)
{
	_dataSize = (unsigned int)wParam;
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(_hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETWINDOWPOS(WPARAM wParam, LPARAM lParam)
{
	_dataViewPos = (unsigned int)wParam;
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(_hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETWINDOWADDRESSOFFSET(WPARAM wParam, LPARAM lParam)
{
	_addressOffset = (unsigned int)wParam;
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(_hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETWINDOWADDRESSWIDTH(WPARAM wParam, LPARAM lParam)
{
	_addressWidth = (unsigned int)wParam;
	//Calculate the dimensions of the drawable surface of this control
	RECT rect;
	GetClientRect(_hwnd, &rect);
	UpdateControlSize(rect.right, rect.bottom);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_GETWINDOWSIZE(WPARAM wParam, LPARAM lParam)
{
	return _visibleValuesPerPage;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_GETWINDOWPOS(WPARAM wParam, LPARAM lParam)
{
	return _dataViewPos;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_UPDATEWINDOWDATA(WPARAM wParam, LPARAM lParam)
{
	Hex_UpdateWindowData* info = (Hex_UpdateWindowData*)lParam;
	for(unsigned int i = 0; (i < info->newBufferSize) && (i < _visibleValuesPerPage); ++i)
	{
		if(info->newBufferData != 0)
		{
			_dataBuffer[i] = *(info->newBufferData + i);
		}
		if(info->newMarkBufferData != 0)
		{
			_dataMarkBuffer[i] = *(info->newMarkBufferData + i) != 0;
		}
	}
	InvalidateRect(_hwnd, NULL, FALSE);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT WC_HexEdit::msgHEX_SETMARKINGINFO(WPARAM wParam, LPARAM lParam)
{
	Hex_DataMarkingInfo* info = (Hex_DataMarkingInfo*)lParam;
	_markingEnabled = info->markingEnabled;
	_markName = info->markName;
	_unmarkName = info->unmarkName;
	UpdateContextMenu();
	return 0;
}
