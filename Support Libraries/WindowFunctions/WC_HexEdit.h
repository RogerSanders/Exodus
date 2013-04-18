/*--------------------------------------------------------------------------------------*\
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
\*--------------------------------------------------------------------------------------*/
#ifndef __WC_HEXEDIT_H__
#define __WC_HEXEDIT_H__
#include <string>
#include <vector>
#include "WindowFunctions.h"

class WC_HexEdit
{
public:
	//Message numbers
	enum WindowMessages;
	enum WindowNotifications;

	//Message parameter structures
	struct Hex_UpdateWindowData;
	struct Hex_DataMarkingInfo;
	struct Hex_ReadDataInfo;
	struct Hex_WriteDataInfo;
	struct Hex_NewWindowPosInfo;
	struct Hex_ReadDataBlockInfo;
	struct Hex_WriteDataBlockInfo;
	struct Hex_UpdateDataMarkingState;

public:
	//Constructors
	WC_HexEdit(HINSTANCE amoduleHandle, HWND ahwnd);

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//Context menu numbers
	enum MenuItem;

private:
	//Drag selection functions
	void UpdateDragSelectPos(int newSelectPosSigned);
	unsigned int GetDragSelectPos() const;
	void StartDragSelect();
	void StopDragSelect();
	bool DragSelectActive() const;

	//Tooltip functions
	void UpdateTooltipText(unsigned int newSelectedByte);

	//Context menu functions
	void UpdateContextMenu();
	void EnableSelectionMenuItems(bool state);
	void MarkSelection(bool state);
	bool CopyToClipboard();
	bool PasteFromClipboard();
	bool SaveToFile();
	bool LoadFromFile();

	//Update functions
	void SetWindowSettings(unsigned int windowPos, unsigned int windowSize);
	void UpdateColumnSettings(unsigned int columnCount, unsigned int valuesPerColumn, bool textAreaEnabled);
	void UpdateControlSize(unsigned int clientWidth, unsigned int clientHeight);

	//Byte selection functions
	bool DataSelectedFromCursorPos(int cursorPosX, int cursorPosY) const;
	bool TextSelectedFromCursorPos(int cursorPosX, int cursorPosY) const;
	int ByteNumberFromCursorPos(int cursorPosX, int cursorPosY) const;
	void SelectByte(int bytePos);
	void ForceByteIntoView(unsigned int bytePos);
	void UpdateViewPos(int newViewPosSigned);

	//Data read/write functions
	unsigned char ReadByte(unsigned int bytePos) const;
	void WriteByte(unsigned int bytePos, unsigned char data);
	bool ReadBlockToBuffer(unsigned int startPos, unsigned int blockSize, unsigned char* buffer);
	bool WriteBlockFromBuffer(unsigned int startPos, unsigned int blockSize, unsigned char* buffer);

	//Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSEMOVE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSELEAVE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CONTEXTMENU(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CLOSE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KILLFOCUS(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFOCUS(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_LBUTTONUP(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KEYDOWN(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CHAR(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_VSCROLL(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
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
	//Colour settings
	WinColor colorBackground;
	WinColor colorAddressText;
	WinColor colorAddressBack;
	WinColor colorLine;
	WinColor colorDataText1;
	WinColor colorDataBack1;
	WinColor colorDataText2;
	WinColor colorDataBack2;
	WinColor colorMarkedDataText;
	WinColor colorMarkedDataBack;
	WinColor colorModifiedDataText;
	WinColor colorModifiedDataBack;
	WinColor colorTextText;
	WinColor colorTextBack;
	WinColor colorMarkedTextText;
	WinColor colorMarkedTextBack;

	//Handles
	HINSTANCE moduleHandle;
	HWND hwnd;
	HFONT hfont;
	HMENU hcontextMenu;

	//Font settings
	unsigned int fontPointSize;
	std::wstring fontTypefaceName;

	//Font metrics
	unsigned int fontWidth;
	unsigned int fontHeight;

	//Display settings
	unsigned int dividerSpace;
	unsigned int columnSpace;
	unsigned int addressWidth;
	unsigned int addressOffset;
	unsigned int valuesPerColumn;
	bool textDisplayEnabled;

	//Data window settings
	unsigned int dataSize;
	unsigned int dataViewPos;
	std::vector<unsigned char> dataBuffer;
	std::vector<bool> dataMarkBuffer;

	//Scroll parameters
	unsigned int valuesPerRow;
	unsigned int valuesPerPage;
	unsigned int scrollMaxPos;

	//Data entry parameters
	unsigned int selectedByte;
	unsigned char storedNybble;
	bool firstNybbleWritten;
	bool inputOnTextWindow;

	//Window caret position info
	unsigned int selectedCharacterDataPosX;
	unsigned int selectedCharacterTextPosX;

	//Window metrics
	unsigned int controlWidth;
	unsigned int controlHeight;
	unsigned int columns;
	unsigned int visibleRows;
	unsigned int visibleValuesPerPage;
	unsigned int addressSectionPos;
	unsigned int addressSectionWidth;
	unsigned int dataDividerLinePosX;
	unsigned int dataSectionPos;
	unsigned int dataSectionWidth;
	unsigned int charDividerLinePosX;
	unsigned int textSectionPos;
	unsigned int textSectionWidth;

	//Tooltip info
	HWND hwndTooltip;
	bool tooltipVisible;
	unsigned int tooltipLastValue;
	std::wstring lastTooltipText;

	//Drag-select info
	bool mouseButtonDown;
	bool dragSelectActive;
	unsigned int dragSelectStartPos;
	unsigned int dragSelectEndPos;
	int lastCursorDragSelectPosX;
	int lastCursorDragSelectPosY;

	//Data marking info
	bool markingEnabled;
	std::wstring markName;
	std::wstring unmarkName;
};

#include "WC_HexEdit.inl"
#endif
