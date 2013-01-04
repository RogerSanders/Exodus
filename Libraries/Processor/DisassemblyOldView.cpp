#include "DisassemblyOldView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::DisassemblyOldView::DisassemblyOldView(Processor* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	track = true;
	location = 0;
	size = 0x1000;
	currentPC = 0;
	forcePCSync = false;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Old Disassembly");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_DISASSEMBLY));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR Processor::DisassemblyOldView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR Processor::DisassemblyOldView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	int tabsize = 40;
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabsize);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_SETHORIZONTALEXTENT, (WPARAM)500, (LPARAM)NULL);
	CheckDlgButton(hwnd, IDC_PROCESSOR_DISASSEMBLY_TRACK, (track)? BST_CHECKED: BST_UNCHECKED);

	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_LOCATION, device->GetAddressBusCharWidth(), location);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_SIZE, device->GetAddressBusCharWidth(), size);
	UpdateDisassembly(location, size, hwnd, pcToListIndex);
	SetTimer(hwnd, 1, 200, NULL);

	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::DisassemblyOldView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::DisassemblyOldView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(track || forcePCSync)
	{
		unsigned int newPC = device->GetCurrentPC();
		if((newPC != currentPC) || forcePCSync)
		{
			forcePCSync = false;
			if(pcToListIndex.find(newPC) != pcToListIndex.end())
			{
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_SETCURSEL, pcToListIndex.find(newPC)->second, 0);
			}
			else
			{
				location = newPC;
				UpdateDisassembly(location, size, hwnd, pcToListIndex);
				UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_LOCATION, device->GetAddressBusCharWidth(), location);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_SETCURSEL, pcToListIndex.find(newPC)->second, 0);
			}
			currentPC = newPC;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::DisassemblyOldView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			switch(LOWORD(wparam))
			{
			case IDC_PROCESSOR_DISASSEMBLY_LOCATION:
				location = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_LOCATION, device->GetAddressBusCharWidth(), location);
				UpdateDisassembly(location, size, hwnd, pcToListIndex);
				break;
			case IDC_PROCESSOR_DISASSEMBLY_SIZE:
				size = GetDlgItemHex(hwnd, LOWORD(wparam));
				UpdateDlgItemHex(hwnd, IDC_PROCESSOR_DISASSEMBLY_SIZE, device->GetAddressBusCharWidth(), size);
				UpdateDisassembly(location, size, hwnd, pcToListIndex);
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_DISASSEMBLY_UPDATE:
			UpdateDisassembly(location, size, hwnd, pcToListIndex);
			if(track)
			{
				forcePCSync = true;
			}
			break;
		case IDC_PROCESSOR_DISASSEMBLY_TRACK:
			track = !track;
			break;
		case IDC_PROCESSOR_DISASSEMBLY_PCSYNC:
			forcePCSync = true;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Disassembly functions
//----------------------------------------------------------------------------------------
void Processor::DisassemblyOldView::UpdateDisassembly(unsigned int location, unsigned int size, HWND hwnd, std::map<unsigned int, unsigned int>& pcToListIndex)
{
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), WM_SETREDRAW, FALSE, 0);

	pcToListIndex.clear();
	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_RESETCONTENT, 0, NULL);
	for(unsigned int offset = 0; offset < size;)
	{
		std::wstringstream listEntry;
		listEntry << std::setw(device->GetAddressBusCharWidth()) << std::setfill(L'0') << std::hex << std::uppercase << location + offset << L'\t';

		unsigned int offsetIncrement = 0;
		Processor::OpcodeInfo opcodeInfo = device->GetOpcodeInfo(location + offset);
		if(opcodeInfo.valid)
		{
			if(!opcodeInfo.disassemblyComment.empty())
			{
				opcodeInfo.disassemblyComment = L";" + opcodeInfo.disassemblyComment;
			}
			offsetIncrement = opcodeInfo.opcodeSize;
		}
		else
		{
			opcodeInfo.disassemblyOpcode = L"INVALID OPCODE";
			offsetIncrement = 1;
		}

		listEntry << opcodeInfo.disassemblyOpcode << L'\t' << opcodeInfo.disassemblyArguments << L'\t' << opcodeInfo.disassemblyComment;

		unsigned int listIndex = (unsigned int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_ADDSTRING, 0, (LPARAM)listEntry.str().c_str());
		pcToListIndex.insert(std::pair<unsigned int, unsigned int>(location + offset, listIndex));
		offset += offsetIncrement;
	}
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_PROCESSOR_DISASSEMBLY_LIST), NULL, FALSE);
}
