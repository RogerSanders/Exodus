#include "TraceView.h"
#include "resource.h"
#include "Stream/Stream.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
TraceView::TraceView(IUIManager& auiManager, TraceViewPresenter& apresenter, IProcessor& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_TRACE));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR TraceView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
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
INT_PTR TraceView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	int tabsize = 40;
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabsize);

	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR TraceView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR TraceView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	CheckDlgButton(hwnd, IDC_PROCESSOR_TRACE_ENABLED, (model.GetTraceEnabled())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_TRACE_DISASSEMBLE, (model.GetTraceDisassemble())? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_PROCESSOR_TRACE_LENGTH) UpdateDlgItemBin(hwnd, IDC_PROCESSOR_TRACE_LENGTH, model.GetTraceLength());

	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_TRACE_LIST), LB_RESETCONTENT, 0, NULL);
	unsigned int width = model.GetAddressBusCharWidth();
	std::list<IProcessor::TraceLogEntry> traceList = model.GetTraceLog();
	for(std::list<IProcessor::TraceLogEntry>::const_iterator i = traceList.begin(); i != traceList.end(); ++i)
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
INT_PTR TraceView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			case IDC_PROCESSOR_TRACE_LENGTH:
				model.SetTraceLength(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_TRACE_ENABLED:{
			model.SetTraceEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_TRACE_DISASSEMBLE:{
			model.SetTraceDisassemble(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_TRACE_CLEAR:{
			model.ClearTraceLog();
			break;}
		}
	}

	return TRUE;
}
