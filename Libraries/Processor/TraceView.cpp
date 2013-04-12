#include "TraceView.h"
#include "resource.h"
#include "Stream/Stream.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::TraceView::TraceView(Processor* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Trace");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_TRACE));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR Processor::TraceView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR Processor::TraceView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	int tabsize = 40;
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabsize);

	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::TraceView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::TraceView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;
	boost::mutex::scoped_lock lock(device->debugMutex);

	CheckDlgButton(hwnd, IDC_PROCESSOR_TRACE_ENABLED, (device->GetTraceEnabled())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_TRACE_DISASSEMBLE, (device->traceDisassemble)? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_PROCESSOR_TRACE_LENGTH) UpdateDlgItemBin(hwnd, IDC_PROCESSOR_TRACE_LENGTH, device->GetTraceLength());

	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_RESETCONTENT, 0, NULL);
	unsigned int width = device->GetAddressBusCharWidth();
	for(Processor::Trace::const_iterator i = device->trace.begin(); i != device->trace.end(); ++i)
	{
		std::wstringstream buffer;
		buffer << std::setw(width) << std::setfill(L'0') << std::hex << std::uppercase << i->address << L'\t' << i->disassembly;
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_ADDSTRING, 0, (LPARAM)buffer.str().c_str());
	}
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), NULL, FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::TraceView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			boost::mutex::scoped_lock lock(device->debugMutex);
			switch(LOWORD(wparam))
			{
			case IDC_PROCESSOR_TRACE_LENGTH:
				device->SetTraceLength(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_TRACE_ENABLED:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->SetTraceEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_TRACE_DISASSEMBLE:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->traceDisassemble = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_TRACE_CLEAR:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->ClearTraceLog();
			break;}
		}
	}

	return TRUE;
}
