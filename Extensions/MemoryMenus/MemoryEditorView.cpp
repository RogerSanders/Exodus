#include "MemoryEditorView.h"
#include "resource.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "WindowsControls/WindowsControls.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryEditorView::MemoryEditorView(IUIManager& auiManager, MemoryEditorViewPresenter& apresenter, IMemory& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	hwndMem = NULL;
	SetWindowSettings(apresenter.GetUnqualifiedViewTitle(), 0, 0, 440, 500);
	SetDockableViewType(true, INITIALDOCKPOS_RIGHT, false, L"Exodus.VerticalWatchers");
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT MemoryEditorView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_PAINT:
		return msgWM_PAINT(hwnd, wparam, lparam);
	case WM_SETFOCUS:
		return msgWM_SETFOCUS(hwnd, wparam, lparam);
	case WM_KILLFOCUS:
		return msgWM_KILLFOCUS(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT MemoryEditorView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Calculate the maximum number of significant digits in the address
	unsigned int totalMemorySize = (model.GetMemoryEntryCount() * model.GetMemoryEntrySizeInBytes());
	unsigned int addressWidth = 0;
	unsigned int maxAddressValue = totalMemorySize - 1;
	while(maxAddressValue != 0)
	{
		++addressWidth;
		maxAddressValue >>= 1;
	}
	if((addressWidth % 4) != 0)
	{
		addressWidth += 4;
	}
	addressWidth /= 4;

	//Register the HexEdit window class
	WC_HexEdit::RegisterWindowClass(GetAssemblyHandle());

	//Create the HexEdit control
	hwndMem = CreateWindowEx(WS_EX_CLIENTEDGE, WC_HexEdit::windowClassName, L"", WS_CHILD | WS_VSCROLL, 0, 0, 0, 0, hwnd, (HMENU)HEXEDITCONTROL, GetAssemblyHandle(), NULL);
	SendMessage(hwndMem, WC_HexEdit::HEX_SETWINDOWSIZE, totalMemorySize, 0);
	SendMessage(hwndMem, WC_HexEdit::HEX_SETWINDOWADDRESSWIDTH, addressWidth, 0);

	if(model.IsMemoryLockingSupported())
	{
		WC_HexEdit::Hex_DataMarkingInfo info;
		info.markingEnabled = true;
		info.markName = L"Lock";
		info.unmarkName = L"Unlock";
		SendMessage(hwndMem, WC_HexEdit::HEX_SETMARKINGINFO, 0, (LPARAM)&info);
	}

	ShowWindow(hwndMem, SW_SHOWNORMAL);
	UpdateWindow(hwndMem);
	SetFocus(hwndMem);

	SetTimer(hwnd, 1, 50, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT MemoryEditorView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT MemoryEditorView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	unsigned int totalMemorySize = (model.GetMemoryEntryCount() * model.GetMemoryEntrySizeInBytes());
	unsigned int windowSize = (unsigned int)SendMessage(hwndMem, WC_HexEdit::HEX_GETWINDOWSIZE, 0, 0);
	unsigned int windowPos = (unsigned int)SendMessage(hwndMem, WC_HexEdit::HEX_GETWINDOWPOS, 0, 0);
	if(windowSize > 0)
	{
		unsigned int memoryEntrySizeInBytes = model.GetMemoryEntrySizeInBytes();
		std::vector<unsigned char> bufferSegment(windowSize);
		std::vector<unsigned char> markBufferSegment(windowSize);
		Data data(memoryEntrySizeInBytes * Data::bitsPerByte);
		unsigned int byteNoInWindow = 0;
		while((byteNoInWindow < windowSize) && ((windowPos + byteNoInWindow) < totalMemorySize))
		{
			unsigned int memoryEntryPos = ((windowPos + byteNoInWindow) / memoryEntrySizeInBytes);
			unsigned int memoryEntrySkippedBytes = ((windowPos + byteNoInWindow) % memoryEntrySizeInBytes);
			data = model.ReadMemoryEntry(memoryEntryPos);
			unsigned int byteNoInEntry = memoryEntrySkippedBytes;
			while((byteNoInWindow < windowSize) && ((windowPos + byteNoInWindow) < totalMemorySize) && (byteNoInEntry < memoryEntrySizeInBytes))
			{
				bufferSegment[byteNoInWindow] = data.GetByteFromTopDown(byteNoInEntry);
				markBufferSegment[byteNoInWindow] = model.IsAddressLocked(memoryEntryPos)? 1: 0;
				++byteNoInWindow;
				++byteNoInEntry;
			}
		}
		WC_HexEdit::Hex_UpdateWindowData info;
		info.newBufferSize = windowSize;
		info.newBufferData = &bufferSegment[0];
		info.newMarkBufferData = &markBufferSegment[0];
		SendMessage(hwndMem, WC_HexEdit::HEX_UPDATEWINDOWDATA, 0, (LPARAM)&info);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT MemoryEditorView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(LOWORD(wparam) == HEXEDITCONTROL)
	{
		if(HIWORD(wparam) == WC_HexEdit::HEX_READDATA)
		{
			WC_HexEdit::Hex_ReadDataInfo* readDataInfo = (WC_HexEdit::Hex_ReadDataInfo*)lparam;
			unsigned int memoryEntrySizeInBytes = model.GetMemoryEntrySizeInBytes();
			Data data(memoryEntrySizeInBytes * Data::bitsPerByte);
			unsigned int memoryEntryPos = (readDataInfo->offset / memoryEntrySizeInBytes);
			unsigned int memoryEntrySkippedBytes = (readDataInfo->offset % memoryEntrySizeInBytes);
			data = model.ReadMemoryEntry(memoryEntryPos);
			readDataInfo->data = data.GetByteFromTopDown(memoryEntrySkippedBytes);
			readDataInfo->processed = true;
		}
		else if(HIWORD(wparam) == WC_HexEdit::HEX_WRITEDATA)
		{
			WC_HexEdit::Hex_WriteDataInfo* writeDataInfo = (WC_HexEdit::Hex_WriteDataInfo*)lparam;
			unsigned int memoryEntrySizeInBytes = model.GetMemoryEntrySizeInBytes();
			if(memoryEntrySizeInBytes == 1)
			{
				model.WriteMemoryEntry(writeDataInfo->offset, writeDataInfo->data);
			}
			else
			{
				Data data(memoryEntrySizeInBytes * Data::bitsPerByte);
				unsigned int memoryEntryPos = (writeDataInfo->offset / memoryEntrySizeInBytes);
				unsigned int memoryEntrySkippedBytes = (writeDataInfo->offset % memoryEntrySizeInBytes);
				data = model.ReadMemoryEntry(memoryEntryPos);
				data.SetByteFromTopDown(memoryEntrySkippedBytes, writeDataInfo->data);
				model.WriteMemoryEntry(memoryEntryPos, data.GetData());
			}
		}
		else if(HIWORD(wparam) == WC_HexEdit::HEX_READDATABLOCK)
		{
			WC_HexEdit::Hex_ReadDataBlockInfo* readDataBlockInfo = (WC_HexEdit::Hex_ReadDataBlockInfo*)lparam;
			unsigned int memoryEntrySizeInBytes = model.GetMemoryEntrySizeInBytes();
			Data data(memoryEntrySizeInBytes * Data::bitsPerByte);
			unsigned int byteNoInBlock = 0;
			while(byteNoInBlock < readDataBlockInfo->size)
			{
				unsigned int memoryEntryPos = ((readDataBlockInfo->offset + byteNoInBlock) / memoryEntrySizeInBytes);
				unsigned int memoryEntrySkippedBytes = ((readDataBlockInfo->offset + byteNoInBlock) % memoryEntrySizeInBytes);
				data = model.ReadMemoryEntry(memoryEntryPos);
				unsigned int byteNoInEntry = memoryEntrySkippedBytes;
				while((byteNoInBlock < readDataBlockInfo->size) && (byteNoInEntry < memoryEntrySizeInBytes))
				{
					readDataBlockInfo->buffer[byteNoInBlock] = data.GetByteFromTopDown(byteNoInEntry);
					++byteNoInBlock;
					++byteNoInEntry;
				}
			}
			readDataBlockInfo->processed = true;
		}
		else if(HIWORD(wparam) == WC_HexEdit::HEX_WRITEDATABLOCK)
		{
			WC_HexEdit::Hex_WriteDataBlockInfo* writeDataBlockInfo = (WC_HexEdit::Hex_WriteDataBlockInfo*)lparam;
			unsigned int memoryEntrySizeInBytes = model.GetMemoryEntrySizeInBytes();
			Data data(memoryEntrySizeInBytes * Data::bitsPerByte);
			unsigned int byteNoInBlock = 0;
			while(byteNoInBlock < writeDataBlockInfo->size)
			{
				unsigned int memoryEntryPos = ((writeDataBlockInfo->offset + byteNoInBlock) / memoryEntrySizeInBytes);
				unsigned int memoryEntrySkippedBytes = ((writeDataBlockInfo->offset + byteNoInBlock) % memoryEntrySizeInBytes);
				unsigned int writeDataBlockRemainingBytes = ((writeDataBlockInfo->size - 1) - byteNoInBlock);
				if((memoryEntrySkippedBytes > 0) || (writeDataBlockRemainingBytes < memoryEntrySizeInBytes))
				{
					data = model.ReadMemoryEntry(memoryEntryPos);
				}
				unsigned int byteNoInEntry = memoryEntrySkippedBytes;
				while((byteNoInBlock < writeDataBlockInfo->size) && (byteNoInEntry < memoryEntrySizeInBytes))
				{
					data.SetByteFromTopDown(byteNoInEntry, writeDataBlockInfo->buffer[byteNoInBlock]);
					++byteNoInBlock;
					++byteNoInEntry;
				}
				model.WriteMemoryEntry(memoryEntryPos, data.GetData());
			}
			writeDataBlockInfo->processed = true;
		}
		else if(HIWORD(wparam) == WC_HexEdit::HEX_NEWWINDOWPOS)
		{
			WC_HexEdit::Hex_NewWindowPosInfo* windowPosInfo = (WC_HexEdit::Hex_NewWindowPosInfo*)lparam;
			unsigned int totalMemorySize = (model.GetMemoryEntryCount() * model.GetMemoryEntrySizeInBytes());
			unsigned int windowSize = windowPosInfo->windowSize;
			unsigned int windowPos = windowPosInfo->windowPos;
			if(windowSize > 0)
			{
				std::vector<unsigned char> bufferSegment(windowSize);
				std::vector<unsigned char> markBufferSegment(windowSize);

				unsigned int memoryEntrySizeInBytes = model.GetMemoryEntrySizeInBytes();
				Data data(memoryEntrySizeInBytes * Data::bitsPerByte);
				unsigned int byteNoInBlock = 0;
				while((byteNoInBlock < windowSize) && ((windowPos + byteNoInBlock) < totalMemorySize))
				{
					unsigned int memoryEntryPos = ((windowPos + byteNoInBlock) / memoryEntrySizeInBytes);
					unsigned int memoryEntrySkippedBytes = ((windowPos + byteNoInBlock) % memoryEntrySizeInBytes);
					data = model.ReadMemoryEntry(memoryEntryPos);
					unsigned int byteNoInEntry = memoryEntrySkippedBytes;
					while((byteNoInBlock < windowSize) && ((windowPos + byteNoInBlock) < totalMemorySize) && (byteNoInEntry < memoryEntrySizeInBytes))
					{
						bufferSegment[byteNoInBlock] = data.GetByteFromTopDown(byteNoInEntry);
						markBufferSegment[byteNoInBlock] = model.IsAddressLocked(memoryEntryPos)? 1: 0;
						++byteNoInBlock;
						++byteNoInEntry;
					}
				}

				WC_HexEdit::Hex_UpdateWindowData info;
				info.newBufferSize = windowSize;
				info.newBufferData = &bufferSegment[0];
				info.newMarkBufferData = &markBufferSegment[0];
				SendMessage(hwndMem, WC_HexEdit::HEX_UPDATEWINDOWDATA, 0, (LPARAM)&info);
			}
		}
		else if(HIWORD(wparam) == WC_HexEdit::HEX_UPDATEDATAMARKING)
		{
			WC_HexEdit::Hex_UpdateDataMarkingState* dataMarkingState = (WC_HexEdit::Hex_UpdateDataMarkingState*)lparam;
			unsigned int memoryEntrySizeInBytes = model.GetMemoryEntrySizeInBytes();
			model.LockMemoryBlock(dataMarkingState->offset / memoryEntrySizeInBytes, ((dataMarkingState->size + (memoryEntrySizeInBytes - 1)) / memoryEntrySizeInBytes), dataMarkingState->state);
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT MemoryEditorView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Dimensions and relative positions of the controls
	unsigned int hexEditControlLeftOffset = 0;
	unsigned int hexEditControlRightOffset = hexEditControlLeftOffset;
	unsigned int hexEditControlTopOffset = 0;
	unsigned int hexEditControlBottomOffset = 0;

	//Read the new client size of the window
	RECT rect;
	GetClientRect(hwnd, &rect);
	unsigned int controlWidth = rect.right;
	unsigned int controlHeight = rect.bottom;

	//Resize the HexEdit control to match
	MoveWindow(hwndMem, hexEditControlLeftOffset, hexEditControlTopOffset, controlWidth - hexEditControlLeftOffset - hexEditControlRightOffset, controlHeight - hexEditControlTopOffset - hexEditControlBottomOffset, TRUE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT MemoryEditorView::msgWM_PAINT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Fill the background of the control with the dialog background colour
	HDC hdc = GetDC(hwnd);
	HBRUSH hbrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	HBRUSH hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);

	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, hbrush);

	SelectObject(hdc, hbrushOld);
	DeleteObject(hbrush);
	ReleaseDC(hwnd, hdc);

	return DefWindowProc(hwnd, WM_PAINT, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT MemoryEditorView::msgWM_SETFOCUS(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetFocus(hwndMem);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT MemoryEditorView::msgWM_KILLFOCUS(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SendMessage(hwndMem, WM_KILLFOCUS, NULL, 0);
	return 0;
}
