#include "ExceptionsView.h"
#include "resource.h"
#include "DataConversion/DataConversion.pkg"
namespace M68000{

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
M68000::ExceptionsView::ExceptionsView(M68000* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	exceptionListIndex = -1;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Exception Control");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_M68000_EXCEPTIONS));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR M68000::ExceptionsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR M68000::ExceptionsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);
	exceptionEntry.Initialize();
	UpdateExceptionEntry(hwnd, exceptionEntry, device->GetAddressBusCharWidth());
	exceptionListIndex = -1;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR M68000::ExceptionsView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR M68000::ExceptionsView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	boost::mutex::scoped_lock lock(device->debugMutex);
	initializedDialog = true;

	//Check if we need to refresh the exception list
	bool refreshExceptionList = (exceptionListCopy.size() != device->exceptionList.size());
	if(!refreshExceptionList)
	{
		ExceptionList::const_iterator exceptionListIterator = device->exceptionList.begin();
		ExceptionList::const_iterator exceptionListCopyIterator = exceptionListCopy.begin();
		while(!refreshExceptionList && (exceptionListIterator != device->exceptionList.end()) && (exceptionListCopyIterator != exceptionListCopy.end()))
		{
			refreshExceptionList |= (*exceptionListIterator != *exceptionListCopyIterator);
			++exceptionListIterator;
			++exceptionListCopyIterator;
		}
	}
	exceptionListCopy = device->exceptionList;

	//Refresh the exception list if required
	if(refreshExceptionList)
	{
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), WM_SETREDRAW, FALSE, 0);

		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_GETTOPINDEX, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_RESETCONTENT, 0, NULL);
		for(ExceptionList::iterator i = device->exceptionList.begin(); i != device->exceptionList.end(); ++i)
		{
			std::wstring exceptionName = BuildExceptionName((*i)->vectorNumber);
			int listIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_ADDSTRING, 0, (LPARAM)exceptionName.c_str());
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETITEMDATA, listIndex, (LPARAM)*i);
		}
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, (WPARAM)-1, 0);
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETTOPINDEX, top, 0);

		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), NULL, FALSE);

		exceptionListIndex = -1;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR M68000::ExceptionsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			case IDC_M68000_EXCEPTIONS_VECTORNUMBER:
				exceptionEntry.vectorNumber = GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			case IDC_M68000_EXCEPTIONS_VECTORADDRESS:
				exceptionEntry.vectorNumber = GetDlgItemHex(hwnd, LOWORD(wparam)) / 4;
				break;
			}
			UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORNUMBER, device->GetAddressBusCharWidth(), exceptionEntry.vectorNumber);
			UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORADDRESS, device->GetAddressBusCharWidth(), exceptionEntry.vectorNumber * 4);
			UpdateDlgItemString(hwnd, IDC_M68000_EXCEPTIONS_VECTORNAME, device->GetExceptionName(exceptionEntry.vectorNumber));
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		boost::mutex::scoped_lock lock(device->debugMutex);

		switch(LOWORD(wparam))
		{
		case IDC_M68000_EXCEPTIONS_LOG:
			exceptionEntry.enableLogging = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_M68000_EXCEPTIONS_BREAK:
			exceptionEntry.enableBreak = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_M68000_EXCEPTIONS_DISABLE:
			exceptionEntry.disable = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_M68000_EXCEPTIONS_TRIGGER:
			device->TriggerExceptionFromDebugger(exceptionEntry.vectorNumber);
			break;

		case IDC_M68000_EXCEPTIONS_LOGALL:
			device->logAllExceptions = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_M68000_EXCEPTIONS_BREAKALL:
			device->breakOnAllExceptions = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_M68000_EXCEPTIONS_DISABLEALL:
			device->disableAllExceptions = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;

		case IDC_M68000_EXCEPTIONS_NEW:
			exceptionEntry.Initialize();
			UpdateExceptionEntry(hwnd, exceptionEntry, device->GetAddressBusCharWidth());
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, (WPARAM)-1, NULL);
			exceptionListIndex = -1;
			break;
		case IDC_M68000_EXCEPTIONS_SAVE:{
			if(exceptionListIndex != -1)
			{
				ExceptionDebuggingEntry* targetExceptionEntry = (ExceptionDebuggingEntry*)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_GETITEMDATA, exceptionListIndex, 0);
				SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_DELETESTRING, exceptionListIndex, NULL);

				std::wstring exceptionName = BuildExceptionName(exceptionEntry.vectorNumber);
				SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_INSERTSTRING, exceptionListIndex, (LPARAM)exceptionName.c_str());
				SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETITEMDATA, exceptionListIndex, (LPARAM)targetExceptionEntry);
				SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, exceptionListIndex, NULL);
				*targetExceptionEntry = exceptionEntry;
			}
			else
			{
				ExceptionDebuggingEntry* targetExceptionEntry = new ExceptionDebuggingEntry(exceptionEntry);
				device->exceptionList.push_back(targetExceptionEntry);
				device->exceptionListEmpty = device->exceptionList.empty();
				exceptionListCopy = device->exceptionList;

				std::wstring exceptionName = BuildExceptionName(targetExceptionEntry->vectorNumber);
				exceptionListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_ADDSTRING, 0, (LPARAM)exceptionName.c_str());
				SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETITEMDATA, exceptionListIndex, (LPARAM)targetExceptionEntry);
				SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, exceptionListIndex, NULL);
			}
			break;}
		case IDC_M68000_EXCEPTIONS_DELETE:{
			if(exceptionListIndex != -1)
			{
				ExceptionDebuggingEntry* targetException = (ExceptionDebuggingEntry*)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_GETITEMDATA, exceptionListIndex, 0);
				bool done = false;
				ExceptionList::iterator i = device->exceptionList.begin();
				while(!done && (i != device->exceptionList.end()))
				{
					if(*i == targetException)
					{
						delete *i;
						device->exceptionList.erase(i);
						done = true;
					}
					else 
					{
						++i;
					}
				}
				device->exceptionListEmpty = device->exceptionList.empty();
				exceptionListCopy = device->exceptionList;
				SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_DELETESTRING, exceptionListIndex, NULL);
				if(exceptionListIndex >= (int)exceptionListCopy.size())
				{
					exceptionListIndex -= 1;
				}
			}
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, exceptionListIndex, NULL);

			//Load the data for the new exception list entry
			if(exceptionListIndex != -1)
			{
				ExceptionDebuggingEntry* targetExceptionEntry = (ExceptionDebuggingEntry*)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_GETITEMDATA, exceptionListIndex, 0);
				exceptionEntry = *targetExceptionEntry;
			}
			else
			{
				exceptionEntry.Initialize();
			}
			UpdateExceptionEntry(hwnd, exceptionEntry, device->GetAddressBusCharWidth());
			break;}
		case IDC_M68000_EXCEPTIONS_DELETEALL:{
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_RESETCONTENT, 0, NULL);
			for(ExceptionList::iterator i = device->exceptionList.begin(); i != device->exceptionList.end(); ++i)
			{
				delete *i;
			}
			device->exceptionList.clear();
			device->exceptionListEmpty = device->exceptionList.empty();
			exceptionListCopy.clear();

			//Clear the current exception info
			exceptionEntry.Initialize();
			UpdateExceptionEntry(hwnd, exceptionEntry, device->GetAddressBusCharWidth());
			exceptionListIndex = -1;
			break;}
		}
	}
	else if(HIWORD(wparam) == LBN_SELCHANGE)
	{
		boost::mutex::scoped_lock lock(device->debugMutex);

		switch(LOWORD(wparam))
		{
		case IDC_M68000_EXCEPTIONS_LIST:
			int selectedItem = (int)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_GETCURSEL, 0, NULL);
			if(selectedItem != LB_ERR)
			{
				exceptionListIndex = selectedItem;
				ExceptionDebuggingEntry* targetExceptionEntry = (ExceptionDebuggingEntry*)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_GETITEMDATA, exceptionListIndex, 0);
				exceptionEntry = *targetExceptionEntry;
				UpdateExceptionEntry(hwnd, exceptionEntry, device->GetAddressBusCharWidth());
			}
			break;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Exception functions
//----------------------------------------------------------------------------------------
void M68000::ExceptionsView::UpdateExceptionEntry(HWND hwnd, const ExceptionDebuggingEntry& targetExceptionEntry, unsigned int addressCharWidth)
{
	CheckDlgButton(hwnd, IDC_M68000_EXCEPTIONS_LOG, (targetExceptionEntry.enableLogging)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_M68000_EXCEPTIONS_BREAK, (targetExceptionEntry.enableBreak)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_M68000_EXCEPTIONS_DISABLE, (targetExceptionEntry.disable)? BST_CHECKED: BST_UNCHECKED);

	UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORNUMBER, addressCharWidth, targetExceptionEntry.vectorNumber);
	UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORADDRESS, addressCharWidth, targetExceptionEntry.vectorNumber * 4);
	UpdateDlgItemString(hwnd, IDC_M68000_EXCEPTIONS_VECTORNAME, device->GetExceptionName(targetExceptionEntry.vectorNumber));
}

//----------------------------------------------------------------------------------------
std::wstring M68000::ExceptionsView::BuildExceptionName(unsigned int vectorNumber) const
{
	std::wstring vectorNumberAsString;
	std::wstring vectorAddressAsString;
	IntToStringBase16(vectorNumber, vectorNumberAsString, 2, true);
	IntToStringBase16(vectorNumber * 4, vectorAddressAsString, 4, true);
	std::wstring exceptionName;
	exceptionName = vectorNumberAsString + L"(" + vectorAddressAsString + L") " + device->GetExceptionName(vectorNumber);
	return exceptionName;
}

} //Close namespace M68000
