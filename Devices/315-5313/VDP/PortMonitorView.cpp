#include "PortMonitorView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::PortMonitorView::PortMonitorView(VDP* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Port Monitor");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_PORTMONITOR));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR VDP::PortMonitorView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR VDP::PortMonitorView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	UpdateDlgItemBin(hwnd, IDC_VDP_PORTMONITOR_LISTSIZE, device->portMonitorListSize);
	CheckDlgButton(hwnd, IDC_VDP_PORTMONITOR_SRREAD, (device->logStatusRegisterRead)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_PORTMONITOR_DPREAD, (device->logDataPortRead)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_PORTMONITOR_HVREAD, (device->logHVCounterRead)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_PORTMONITOR_CPWRITE, (device->logControlPortWrite)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_PORTMONITOR_DPWRITE, (device->logDataPortWrite)? BST_CHECKED: BST_UNCHECKED);

	int tabsize = 20;
	SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabsize);

	SetTimer(hwnd, 1, 500, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::PortMonitorView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::PortMonitorView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;
	boost::mutex::scoped_lock lock(device->portMonitorMutex);

	SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_RESETCONTENT, 0, NULL);
	unsigned int currentItemLogIndex = 0;
	for(std::list<PortMonitorEntry>::const_iterator i = device->portMonitorList.begin(); i != device->portMonitorList.end(); ++i)
	{
		std::wstringstream buffer;
		buffer << i->target << '\t';
		buffer << std::setw(4) << std::setfill(L'0') << std::hex << std::uppercase << i->data;
		buffer << '\t' << i->source;
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_ADDSTRING, 0, (LPARAM)buffer.str().c_str());
		SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)currentItemLogIndex);
		++currentItemLogIndex;
	}
	SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), NULL, FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::PortMonitorView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == LBN_DBLCLK)
	{
		//Load the port monitor details view for the selected entry
		switch(LOWORD(wparam))
		{
		case IDC_VDP_PORTMONITOR_LIST:{
			boost::mutex::scoped_lock lock(device->portMonitorMutex);
			int currentItemListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_GETCURSEL, 0, NULL);
			unsigned int targetItemLogIndex = (unsigned int)SendMessage(GetDlgItem(hwnd, IDC_VDP_PORTMONITOR_LIST), LB_GETITEMDATA, currentItemListIndex, NULL);

			unsigned int currentItemLogIndex = 0;
			for(std::list<PortMonitorEntry>::const_iterator i = device->portMonitorList.begin(); i != device->portMonitorList.end(); ++i)
			{
				if(currentItemLogIndex == targetItemLogIndex)
				{
					device->OpenPortMonitorDetailsView(*i);
				}
				++currentItemLogIndex;
			}

			break;}
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
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
			case IDC_VDP_PORTMONITOR_LISTSIZE:
				device->portMonitorListSize = GetDlgItemBin(hwnd, LOWORD(wparam));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_VDP_PORTMONITOR_SRREAD:
			device->logStatusRegisterRead = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_VDP_PORTMONITOR_DPREAD:
			device->logDataPortRead = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_VDP_PORTMONITOR_HVREAD:
			device->logHVCounterRead = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_VDP_PORTMONITOR_CPWRITE:
			device->logControlPortWrite = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_VDP_PORTMONITOR_DPWRITE:
			device->logDataPortWrite = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_VDP_PORTMONITOR_CLEAR:
			device->ClearPortMonitorList();
			break;
		}
	}

	return TRUE;
}
