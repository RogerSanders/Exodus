#include "CallStackView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::CallStackView::CallStackView(Processor* adevice)
:device(adevice)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Call Stack");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_STACK));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR Processor::CallStackView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR Processor::CallStackView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	int tabsize = 40;
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_SETTABSTOPS, (WPARAM)1, (LPARAM)&tabsize);

	SetTimer(hwnd, 1, 500, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::CallStackView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::CallStackView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	boost::mutex::scoped_lock lock(device->debugMutex);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_STACK_STACK), LB_RESETCONTENT, 0, NULL);
	unsigned int pcLength = device->GetPCCharWidth();
	for(Processor::CallStack::const_iterator i = device->callStack.begin(); i != device->callStack.end(); ++i)
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
INT_PTR Processor::CallStackView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_STACK_DISASSEMBLE:{
			boost::mutex::scoped_lock lock(device->debugMutex);
			device->stackDisassemble = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_PROCESSOR_STACK_CLEAR:
			device->ClearCallStack();
			break;
		}
	}

	return TRUE;
}
