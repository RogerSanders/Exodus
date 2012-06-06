#include "MemoryViewerOldView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryViewerOldView::MemoryViewerOldView(MemoryRead* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	memoryLocation = 0;
	memorySize = 0;
	displayWidth = 1;
	addressWidth = 0;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Old Memory Viewer");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_MEMORY_DATA));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR MemoryRead::MemoryViewerOldView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR MemoryRead::MemoryViewerOldView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	unsigned int initialMemorySize = (unsigned int)device->GetInterfaceSize();
	unsigned int addressWidth = 0;
	unsigned int totalSize = initialMemorySize - 1;
	while(totalSize != 0)
	{
		++addressWidth;
		totalSize >>= 1;
	}
	if((addressWidth % 4) != 0)
	{
		addressWidth += 4;
	}
	addressWidth /= 4;
	initialMemorySize = (initialMemorySize > 0x1000)? 0x1000: initialMemorySize;
	addressWidth = addressWidth;
	memorySize = initialMemorySize;

	int tabsize = 3;
	SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabsize);

	UpdateDlgItemHex(hwnd, IDC_MEMORY_DATA_LOCATION, addressWidth, memoryLocation);
	UpdateDlgItemHex(hwnd, IDC_MEMORY_DATA_SIZE, addressWidth, memorySize);
	UpdateDlgItemBin(hwnd, IDC_MEMORY_DATA_WIDTH, displayWidth);
	PostMessage(hwnd, WM_TIMER, 0, 0);

	SetTimer(hwnd, 1, 500, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR MemoryRead::MemoryViewerOldView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR MemoryRead::MemoryViewerOldView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_RESETCONTENT, 0, NULL);
	for(unsigned int i = 0; ((memoryLocation + i) < device->GetInterfaceSize()) && (i < memorySize); i += 0x10)
	{
		std::wstringstream text;
		text << std::setw(addressWidth) << std::setfill(L'0') << std::hex << std::uppercase << memoryLocation + i << L'\t';
		for(unsigned int j = 0; (j < 0x10) && ((memoryLocation + i + j) < device->GetInterfaceSize()) && ((i + j) < memorySize); j += displayWidth)
		{
			text << L"\t";
			for(unsigned int k = 0; (k < displayWidth) && ((memoryLocation + i + j + k) < device->GetInterfaceSize()); ++k)
			{
				Data data(8);
				device->TransparentReadInterface(0, memoryLocation + i + j + k, data, device->GetDeviceContext(), 0);
				text << std::setw(2) << std::setfill(L'0') << std::hex << std::uppercase << data.GetData();
			}
		}
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_ADDSTRING, 0, (LPARAM)text.str().c_str());
		SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)memoryLocation + i);
	}
	SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_MEMORY_LIST), NULL, FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR MemoryRead::MemoryViewerOldView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == LBN_DBLCLK)
	{
		//Load the memory editor for the selected location
		switch(LOWORD(wparam))
		{
		case IDC_MEMORY_LIST:{
			int currentItemListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_GETCURSEL, 0, NULL);
			unsigned int targetItemMemoryAddress = (unsigned int)SendMessage(GetDlgItem(hwnd, IDC_MEMORY_LIST), LB_GETITEMDATA, currentItemListIndex, NULL);

			device->OpenOldMemoryEditorView(targetItemMemoryAddress);
			break;}
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if(HIWORD(wparam) == EN_KILLFOCUS)
	{
		switch(LOWORD(wparam))
		{
		case IDC_MEMORY_DATA_LOCATION:
			memoryLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
			UpdateDlgItemHex(hwnd, IDC_MEMORY_DATA_LOCATION, addressWidth, memoryLocation);
			break;
		case IDC_MEMORY_DATA_SIZE:
			memorySize = GetDlgItemHex(hwnd, LOWORD(wparam));
			UpdateDlgItemHex(hwnd, IDC_MEMORY_DATA_SIZE, addressWidth, memorySize);
			break;
		case IDC_MEMORY_DATA_WIDTH:
			unsigned int targetDisplayWidth = GetDlgItemBin(hwnd, LOWORD(wparam));
			displayWidth = 1;
			while((displayWidth < targetDisplayWidth) && (displayWidth < 16))
			{
				displayWidth <<= 1;
			}
			UpdateDlgItemBin(hwnd, IDC_MEMORY_DATA_WIDTH, displayWidth);
			break;
		}
	}

	return TRUE;
}
