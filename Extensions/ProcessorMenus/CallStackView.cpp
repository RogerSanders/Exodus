#include "CallStackView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
CallStackView::CallStackView(IUIManager& auiManager, CallStackViewPresenter& apresenter, IProcessor& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_STACK));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR CallStackView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR CallStackView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	int tabsize = 40;
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabsize);

	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CallStackView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR CallStackView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_RESETCONTENT, 0, NULL);
	unsigned int pcLength = model.GetPCCharWidth();
	std::list<IProcessor::CallStackEntry> callStack = model.GetCallStack();
	for(std::list<IProcessor::CallStackEntry>::const_iterator i = callStack.begin(); i != callStack.end(); ++i)
	{
		std::wstringstream buffer;
		//				buffer << L"From " << std::setw(i->pcLength) << std::setfill(L'0') << std::hex << std::uppercase << i->sourceAddress << L'\t';
		//				buffer << L"To " << std::setw(i->pcLength) << std::setfill(L'0') << std::hex << std::uppercase << i->targetAddress << L'\t';
		//				buffer << L"Returning " << std::setw(i->pcLength) << std::setfill(L'0') << std::hex << std::uppercase << i->returnAddress << L'\t';
		//				buffer << L" - " << i->disassembly;
		buffer << std::setw(pcLength) << std::setfill(L'0') << std::hex << std::uppercase << i->sourceAddress << L'\t';
		buffer << std::setw(pcLength) << std::setfill(L'0') << std::hex << std::uppercase << i->targetAddress << L'\t';
		buffer << std::setw(pcLength) << std::setfill(L'0') << std::hex << std::uppercase << i->returnAddress << L'\t';
		buffer << i->disassembly;

		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_ADDSTRING, 0, (LPARAM)buffer.str().c_str());
	}
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), NULL, FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CallStackView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_STACK_DISASSEMBLE:{
			model.SetCallStackDisassemble(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_STACK_CLEAR:
			model.ClearCallStack();
			break;
		}
	}

	return TRUE;
}
