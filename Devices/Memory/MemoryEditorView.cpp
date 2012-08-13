#include "MemoryEditorView.h"
#include "resource.h"
#include "WindowFunctions/WC_HexEdit.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryEditorView::MemoryEditorView(MemoryRead* adevice)
:device(adevice)
{
	hwndMem = NULL;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Memory Editor");
	SetWindowSettings(windowTitle, WS_SIZEBOX, 0, 440, 500, true);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT MemoryRead::MemoryEditorView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_PAINT:
		msgWM_PAINT(hwnd, wparam, lparam);
		break;
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
LRESULT MemoryRead::MemoryEditorView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Calculate the maximum number of significant digits in the address
	unsigned int totalMemorySize = (unsigned int)device->GetInterfaceSize();
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

	//Create the HexEdit child control
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = &WC_HexEdit::WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = (HINSTANCE)device->GetAssemblyHandle();
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"EX_MemControl";
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);
	hwndMem = CreateWindowEx(WS_EX_CLIENTEDGE, L"EX_MemControl", L"", WS_CHILD | WS_VSCROLL, 0, 0, 0, 0, hwnd, (HMENU)HEXEDITCONTROL, (HINSTANCE)device->GetAssemblyHandle(), NULL);
	SendMessage(hwndMem, WC_HexEdit::HEX_SETWINDOWSIZE, totalMemorySize, 0);
	SendMessage(hwndMem, WC_HexEdit::HEX_SETWINDOWADDRESSWIDTH, addressWidth, 0);

	if(device->IsMemoryLockingSupported())
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
LRESULT MemoryRead::MemoryEditorView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT MemoryRead::MemoryEditorView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	unsigned int totalMemorySize = (unsigned int)device->GetInterfaceSize();
	unsigned int windowSize = (unsigned int)SendMessage(hwndMem, WC_HexEdit::HEX_GETWINDOWSIZE, 0, 0);
	unsigned int windowPos = (unsigned int)SendMessage(hwndMem, WC_HexEdit::HEX_GETWINDOWPOS, 0, 0);
	if(windowSize > 0)
	{
		std::vector<unsigned char> bufferSegment(windowSize);
		std::vector<unsigned char> markBufferSegment(windowSize);
		for(unsigned int i = 0; (i < windowSize) && (windowPos + i < totalMemorySize); ++i)
		{
			Data data(8);
			device->TransparentReadInterface(0, windowPos + i, data, device->GetDeviceContext(), 0);
			bufferSegment[i] = (unsigned char)data.GetData();
			markBufferSegment[i] = device->IsByteLocked(windowPos + i)? 1: 0;
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
LRESULT MemoryRead::MemoryEditorView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(LOWORD(wparam) == HEXEDITCONTROL)
	{
		if(HIWORD(wparam) == WC_HexEdit::HEX_READDATA)
		{
			WC_HexEdit::Hex_ReadDataInfo* readDataInfo = (WC_HexEdit::Hex_ReadDataInfo*)lparam;
			Data data(8);
			device->TransparentReadInterface(0, readDataInfo->offset, data, device->GetDeviceContext(), 0);
			readDataInfo->data = (unsigned char)data.GetData();
			readDataInfo->processed = true;
		}
		else if(HIWORD(wparam) == WC_HexEdit::HEX_WRITEDATA)
		{
			WC_HexEdit::Hex_WriteDataInfo* writeDataInfo = (WC_HexEdit::Hex_WriteDataInfo*)lparam;
			device->TransparentWriteInterface(0, writeDataInfo->offset, Data(8, writeDataInfo->data), device->GetDeviceContext(), 0);
		}
		else if(HIWORD(wparam) == WC_HexEdit::HEX_READDATABLOCK)
		{
			WC_HexEdit::Hex_ReadDataBlockInfo* readDataBlockInfo = (WC_HexEdit::Hex_ReadDataBlockInfo*)lparam;
			Data data(8);
			for(unsigned int i = 0; i < readDataBlockInfo->size; ++i)
			{
				device->TransparentReadInterface(0, readDataBlockInfo->offset + i, data, device->GetDeviceContext(), 0);
				readDataBlockInfo->buffer[i] = (unsigned char)data.GetData();
			}
			readDataBlockInfo->processed = true;
		}
		else if(HIWORD(wparam) == WC_HexEdit::HEX_WRITEDATABLOCK)
		{
			WC_HexEdit::Hex_WriteDataBlockInfo* writeDataBlockInfo = (WC_HexEdit::Hex_WriteDataBlockInfo*)lparam;
			for(unsigned int i = 0; i < writeDataBlockInfo->size; ++i)
			{
				device->TransparentWriteInterface(0, writeDataBlockInfo->offset + i, Data(8, writeDataBlockInfo->buffer[i]), device->GetDeviceContext(), 0);
			}
			writeDataBlockInfo->processed = true;
		}
		else if(HIWORD(wparam) == WC_HexEdit::HEX_NEWWINDOWPOS)
		{
			WC_HexEdit::Hex_NewWindowPosInfo* windowPosInfo = (WC_HexEdit::Hex_NewWindowPosInfo*)lparam;
			unsigned int totalMemorySize = (unsigned int)device->GetInterfaceSize();
			unsigned int windowSize = windowPosInfo->windowSize;
			unsigned int windowPos = windowPosInfo->windowPos;

			if(windowSize > 0)
			{
				std::vector<unsigned char> bufferSegment(windowSize);
				std::vector<unsigned char> markBufferSegment(windowSize);
				for(unsigned int i = 0; (i < windowSize) && (windowPos + i < totalMemorySize); ++i)
				{
					Data data(8);
					device->TransparentReadInterface(0, windowPos + i, data, device->GetDeviceContext(), 0);
					bufferSegment[i] = (unsigned char)data.GetData();
					markBufferSegment[i] = device->IsByteLocked(windowPos + i)? 1: 0;
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
			device->LockMemoryBlock(dataMarkingState->offset, dataMarkingState->size, dataMarkingState->state);
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT MemoryRead::MemoryEditorView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
LRESULT MemoryRead::MemoryEditorView::msgWM_PAINT(HWND hwnd, WPARAM wparam, LPARAM lparam)
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

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT MemoryRead::MemoryEditorView::msgWM_SETFOCUS(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetFocus(hwndMem);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT MemoryRead::MemoryEditorView::msgWM_KILLFOCUS(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SendMessage(hwndMem, WM_KILLFOCUS, NULL, 0);
	return 0;
}
