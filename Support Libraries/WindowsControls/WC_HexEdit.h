/*----------------------------------------------------------------------------                              ----------*\
Things to do:
-If you start a drag select but only select a single byte, the tooltip still shows the
drag select info.
-Implement messages which allow external code to calculate the necessary width and height
of the control in order to make X number of rows and columns visible, at the specified
window settings.
-Provide messages to specify the colour values to use for all elements of the control
externally. Default to the current values.
-Provide external messages which allow the column width to be obtained and set
-Allow selected blocks to be copied to and pasted from the clipboard. Since we don't
allow insertion or deletion, prompt for what to do when the size of the data on the
clipboard doesn't match the selected size. Inform the user of the size of the data in the
clipboard, and provide options to either abort, or "fit" the data into the selected
space, either by truncating the data or not filling the entire selected space.
-Make sure data can be copied and pasted between this control and hex workshop
-Provide a message to set the initial working directory for loading or saving the data to
a file. If no working directory has been set, the control will default to its current
behaviour of using the current working directory.
-Implement the ctrl+left/right arrow key behaviour from hex workshop
-Adjust the way we select bytes using the mouse
-Add operators to our paste operation. We should be able to AND, OR, and XOR a copied
block of data with the data in memory. This will provide quick ways to construct
interpolated data sets.
-Column selection mode, where a region can be bounded both horizontally and vertically, to
allow a column from a repeating set of data to be selected, copied, pasted, etc.
\*--------------------------------------------------------------------------------------------------------------------*/
#ifndef __WC_HEXEDIT_H__
#define __WC_HEXEDIT_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <string>
#include <vector>

class WC_HexEdit
{
public:
	// Message numbers
	enum class WindowMessages :unsigned int;
	enum class WindowNotifications :unsigned int;

	// Message parameter structures
	struct Hex_UpdateWindowData;
	struct Hex_DataMarkingInfo;
	struct Hex_ReadDataInfo;
	struct Hex_WriteDataInfo;
	struct Hex_NewWindowPosInfo;
	struct Hex_ReadDataBlockInfo;
	struct Hex_WriteDataBlockInfo;
	struct Hex_UpdateDataMarkingState;

	// Constants
	static const wchar_t* WindowClassName;

public:
	// Constructors
	WC_HexEdit(HINSTANCE moduleHandle, HWND hwnd);

	// Class registration
	static bool RegisterWindowClass(HINSTANCE moduleHandle);
	static bool UnregisterWindowClass(HINSTANCE moduleHandle);

	// Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	// Context menu numbers
	enum class MenuItem :unsigned int;

private:
	// Drag selection functions
	void UpdateDragSelectPos(int newSelectPosSigned);
	unsigned int GetDragSelectPos() const;
	void StartDragSelect();
	void StopDragSelect();
	bool DragSelectActive() const;

	// Tooltip functions
	void UpdateTooltipText(unsigned int newSelectedByte);

	// Context menu functions
	void UpdateContextMenu();
	void EnableSelectionMenuItems(bool state);
	void MarkSelection(bool state);
	bool CopyToClipboard();
	bool PasteFromClipboard();
	bool SaveToFile();
	bool LoadFromFile();

	// Update functions
	void SetWindowSettings(unsigned int windowPos, unsigned int windowSize);
	void UpdateColumnSettings(unsigned int columnCount, unsigned int valuesPerColumn, bool textAreaEnabled);
	void UpdateControlSize(unsigned int clientWidth, unsigned int clientHeight);

	// Byte selection functions
	bool DataSelectedFromCursorPos(int cursorPosX, int cursorPosY) const;
	bool TextSelectedFromCursorPos(int cursorPosX, int cursorPosY) const;
	int ByteNumberFromCursorPos(int cursorPosX, int cursorPosY) const;
	void SelectByte(int bytePos);
	void ForceByteIntoView(unsigned int bytePos);
	void UpdateViewPos(int newViewPosSigned);

	// Data read/write functions
	unsigned char ReadByte(unsigned int bytePos) const;
	void WriteByte(unsigned int bytePos, unsigned char data);
	bool ReadBlockToBuffer(unsigned int startPos, unsigned int blockSize, unsigned char* buffer);
	bool WriteBlockFromBuffer(unsigned int startPos, unsigned int blockSize, unsigned char* buffer);

	// Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSEMOVE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSELEAVE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CONTEXTMENU(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KILLFOCUS(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFOCUS(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_LBUTTONUP(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KEYDOWN(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CHAR(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_VSCROLL(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PRINTCLIENT(WPARAM wParam, LPARAM lParam);
	LRESULT msgHEX_SETWINDOWSIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgHEX_SETWINDOWPOS(WPARAM wParam, LPARAM lParam);
	LRESULT msgHEX_SETWINDOWADDRESSOFFSET(WPARAM wParam, LPARAM lParam);
	LRESULT msgHEX_SETWINDOWADDRESSWIDTH(WPARAM wParam, LPARAM lParam);
	LRESULT msgHEX_GETWINDOWSIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgHEX_GETWINDOWPOS(WPARAM wParam, LPARAM lParam);
	LRESULT msgHEX_UPDATEWINDOWDATA(WPARAM wParam, LPARAM lParam);
	LRESULT msgHEX_SETMARKINGINFO(WPARAM wParam, LPARAM lParam);

private:
	// Colour settings
	WinColor _colorBackground;
	WinColor _colorAddressText;
	WinColor _colorAddressBack;
	WinColor _colorLine;
	WinColor _colorDataText1;
	WinColor _colorDataBack1;
	WinColor _colorDataText2;
	WinColor _colorDataBack2;
	WinColor _colorMarkedDataText;
	WinColor _colorMarkedDataBack;
	WinColor _colorModifiedDataText;
	WinColor _colorModifiedDataBack;
	WinColor _colorTextText;
	WinColor _colorTextBack;
	WinColor _colorMarkedTextText;
	WinColor _colorMarkedTextBack;

	// Handles
	HINSTANCE _moduleHandle;
	HWND _hwnd;
	HFONT _hfont;
	HMENU _hcontextMenu;

	// Font settings
	unsigned int _fontPointSize;
	std::wstring _fontTypefaceName;

	// Font metrics
	unsigned int _fontWidth;
	unsigned int _fontHeight;

	// Display settings
	unsigned int _dividerSpace;
	unsigned int _columnSpace;
	unsigned int _addressWidth;
	unsigned int _addressOffset;
	unsigned int _valuesPerColumn;
	bool _textDisplayEnabled;

	// Data window settings
	unsigned int _dataSize;
	unsigned int _dataViewPos;
	std::vector<unsigned char> _dataBuffer;
	std::vector<bool> _dataMarkBuffer;

	// Scroll parameters
	unsigned int _valuesPerRow;
	unsigned int _valuesPerPage;
	unsigned int _scrollMaxPos;

	// Data entry parameters
	unsigned int _selectedByte;
	unsigned char _storedNybble;
	bool _firstNybbleWritten;
	bool _inputOnTextWindow;

	// Window caret position info
	unsigned int _selectedCharacterDataPosX;
	unsigned int _selectedCharacterTextPosX;

	// Window metrics
	unsigned int _controlWidth;
	unsigned int _controlHeight;
	unsigned int _columns;
	unsigned int _visibleRows;
	unsigned int _visibleValuesPerPage;
	unsigned int _addressSectionPos;
	unsigned int _addressSectionWidth;
	unsigned int _dataDividerLinePosX;
	unsigned int _dataSectionPos;
	unsigned int _dataSectionWidth;
	unsigned int _charDividerLinePosX;
	unsigned int _textSectionPos;
	unsigned int _textSectionWidth;

	// Tooltip info
	HWND _hwndTooltip;
	bool _tooltipVisible;
	unsigned int _tooltipLastValue;
	std::wstring _lastTooltipText;

	// Drag-select info
	bool _mouseButtonDown;
	bool _dragSelectActive;
	unsigned int _dragSelectStartPos;
	unsigned int _dragSelectEndPos;
	int _lastCursorDragSelectPosX;
	int _lastCursorDragSelectPosY;

	// Data marking info
	bool _markingEnabled;
	std::wstring _markName;
	std::wstring _unmarkName;
};

#include "WC_HexEdit.inl"
#endif
