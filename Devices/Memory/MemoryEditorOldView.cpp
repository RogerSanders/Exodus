#include "MemoryEditorOldView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryEditorOldView::MemoryEditorOldView(MemoryRead* adevice, unsigned int atargetMemoryAddress)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	memoryLocation = atargetMemoryAddress;
	addressWidth = 0;
	memoryLocationModified = false;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Old Memory Editor");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_MEMORY_DATA_EDIT));
}

//----------------------------------------------------------------------------------------
//Memory address functions
//----------------------------------------------------------------------------------------
void MemoryRead::MemoryEditorOldView::SetTargetMemoryAddress(unsigned int atargetMemoryAddress)
{
	memoryLocation = atargetMemoryAddress;
	memoryLocationModified = true;
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR MemoryRead::MemoryEditorOldView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR MemoryRead::MemoryEditorOldView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
	addressWidth = addressWidth;

	//Limit the maximum size of the textboxes
	for(unsigned int i = 0; i < 0x10; ++i)
	{
		SendMessage(GetDlgItem(hwnd, IDC_MEMORY_DATA_EDIT_0 + i), EM_LIMITTEXT, 2, 0);
	}
	//Update the text labels
	for(unsigned int i = 0; i < 0x10; ++i)
	{
		std::wstringstream stream;
		stream << std::uppercase << std::hex << ((memoryLocation + i) & 0x0F);
		SetWindowText(GetDlgItem(hwnd, IDC_MEMORY_DATA_LABEL_0 + i), stream.str().c_str());
	}
	std::wstringstream stream;
	stream << std::uppercase << std::hex << std::setfill(L'0') << std::setw(addressWidth) << memoryLocation;
	SetWindowText(GetDlgItem(hwnd, IDC_MEMORY_DATA_LABEL_LINE0), stream.str().c_str());
	stream.str(L"");
	stream << std::uppercase << std::hex << std::setfill(L'0') << std::setw(addressWidth) << memoryLocation + 0x08;
	SetWindowText(GetDlgItem(hwnd, IDC_MEMORY_DATA_LABEL_LINE1), stream.str().c_str());

	PostMessage(hwnd, WM_TIMER, 0, 0);
	SetTimer(hwnd, 1, 500, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR MemoryRead::MemoryEditorOldView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR MemoryRead::MemoryEditorOldView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	if(memoryLocationModified)
	{
		UpdateMemoryLocation(hwnd, memoryLocation);
		memoryLocationModified = false;
	}

	for(unsigned int i = 0; i < 0x10; ++i)
	{
		if(currentControlFocus != IDC_MEMORY_DATA_EDIT_0 + i)
		{
			Data data(8);
			device->TransparentReadInterface(0, memoryLocation + i, data, device->GetDeviceContext());
			UpdateDlgItemHex(hwnd, IDC_MEMORY_DATA_EDIT_0 + i, 2, data.GetData());
		}
	}
	if(currentControlFocus != IDC_MEMORY_DATA_EDIT_LOCATION)
	{
		UpdateDlgItemHex(hwnd, IDC_MEMORY_DATA_EDIT_LOCATION, addressWidth, memoryLocation);
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR MemoryRead::MemoryEditorOldView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if(HIWORD(wparam) == EN_KILLFOCUS)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			if((LOWORD(wparam) >= IDC_MEMORY_DATA_EDIT_0) && (LOWORD(wparam) < (IDC_MEMORY_DATA_EDIT_0 + 0x10)))
			{
				device->TransparentWriteInterface(0, memoryLocation + (LOWORD(wparam) - IDC_MEMORY_DATA_EDIT_0), Data(8, GetDlgItemHex(hwnd, LOWORD(wparam))), device->GetDeviceContext());
			}
			else if((LOWORD(wparam) == IDC_MEMORY_DATA_EDIT_LOCATION))
			{
				//Obtain the new memory location value
				memoryLocation = GetDlgItemHex(hwnd, LOWORD(wparam));
				memoryLocationModified = true;
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Update functions
//----------------------------------------------------------------------------------------
void MemoryRead::MemoryEditorOldView::UpdateMemoryLocation(HWND hwnd, unsigned int targetMemoryLocation)
{
	//Update the location textbox
	UpdateDlgItemHex(hwnd, IDC_MEMORY_DATA_EDIT_LOCATION, addressWidth, memoryLocation);

	//Update the text labels
	for(unsigned int i = 0; i < 0x10; ++i)
	{
		std::wstringstream stream;
		stream << std::uppercase << std::hex << ((memoryLocation + i) & 0x0F);
		SetWindowText(GetDlgItem(hwnd, IDC_MEMORY_DATA_LABEL_0 + i), stream.str().c_str());
	}
	std::wstringstream stream;
	stream << std::uppercase << std::hex << std::setfill(L'0') << std::setw(addressWidth) << memoryLocation;
	SetWindowText(GetDlgItem(hwnd, IDC_MEMORY_DATA_LABEL_LINE0), stream.str().c_str());
	stream.str(L"");
	stream << std::uppercase << std::hex << std::setfill(L'0') << std::setw(addressWidth) << memoryLocation + 0x08;
	SetWindowText(GetDlgItem(hwnd, IDC_MEMORY_DATA_LABEL_LINE1), stream.str().c_str());
}
