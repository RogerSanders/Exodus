#include "ExceptionsView.h"
#include "resource.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExceptionsView::ExceptionsView(IUIManager& auiManager, ExceptionsViewPresenter& apresenter, IM68000& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	addressBusCharWidth = 6;
	exceptionListIndex = -1;
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_M68000_EXCEPTIONS));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR ExceptionsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR ExceptionsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);
	UpdateExceptionEntry(hwnd, exceptionEntry);
	exceptionListIndex = -1;
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ExceptionsView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR ExceptionsView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	//Check if we need to refresh the exception debug list
	std::list<IM68000::ExceptionDebuggingEntry> currentExceptionList = model.GetExceptionDebugEntries();
	bool refreshExceptionList = (cachedExceptionList.size() != currentExceptionList.size());
	if(!refreshExceptionList)
	{
		std::list<IM68000::ExceptionDebuggingEntry>::const_iterator currentExceptionListIterator = currentExceptionList.begin();
		std::list<IM68000::ExceptionDebuggingEntry>::const_iterator cachedExceptionListIterator = cachedExceptionList.begin();
		while(!refreshExceptionList && (currentExceptionListIterator != currentExceptionList.end()) && (cachedExceptionListIterator != cachedExceptionList.end()))
		{
			refreshExceptionList |= (currentExceptionListIterator->vectorNumber != cachedExceptionListIterator->vectorNumber);
			++currentExceptionListIterator;
			++cachedExceptionListIterator;
		}
	}

	//Refresh the exception debug list if it has changed
	if(refreshExceptionList)
	{
		//Save the new exception debug list
		cachedExceptionList = currentExceptionList;

		//Refresh the list of exception debug entries
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), WM_SETREDRAW, FALSE, 0);
		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_GETTOPINDEX, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_RESETCONTENT, 0, NULL);
		for(std::list<IM68000::ExceptionDebuggingEntry>::iterator i = cachedExceptionList.begin(); i != cachedExceptionList.end(); ++i)
		{
			std::wstring exceptionName = BuildExceptionName((*i).vectorNumber);
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_ADDSTRING, 0, (LPARAM)exceptionName.c_str());
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
INT_PTR ExceptionsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			case IDC_M68000_EXCEPTIONS_VECTORNUMBER:
				exceptionEntry.vectorNumber = GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			case IDC_M68000_EXCEPTIONS_VECTORADDRESS:
				exceptionEntry.vectorNumber = GetDlgItemHex(hwnd, LOWORD(wparam)) / 4;
				break;
			}
			UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORNUMBER, addressBusCharWidth, exceptionEntry.vectorNumber);
			UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORADDRESS, addressBusCharWidth, exceptionEntry.vectorNumber * 4);
			UpdateDlgItemString(hwnd, IDC_M68000_EXCEPTIONS_VECTORNAME, model.GetExceptionName(exceptionEntry.vectorNumber));
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
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
			model.TriggerException(exceptionEntry.vectorNumber);
			break;

		case IDC_M68000_EXCEPTIONS_LOGALL:
			model.SetLogAllExceptions(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_M68000_EXCEPTIONS_BREAKALL:
			model.SetBreakOnAllExceptions(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_M68000_EXCEPTIONS_DISABLEALL:
			model.SetDisableAllExceptions(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;

		case IDC_M68000_EXCEPTIONS_NEW:
			exceptionEntry = IM68000::ExceptionDebuggingEntry();
			UpdateExceptionEntry(hwnd, exceptionEntry);
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, (WPARAM)-1, NULL);
			exceptionListIndex = -1;
			break;
		case IDC_M68000_EXCEPTIONS_SAVE:{
			std::wstring exceptionName = BuildExceptionName(exceptionEntry.vectorNumber);
			if(exceptionListIndex != -1)
			{
				int exceptionIndex = 0;
				std::list<IM68000::ExceptionDebuggingEntry>::iterator cachedExceptionListIterator = cachedExceptionList.begin();
				while(cachedExceptionListIterator != cachedExceptionList.end())
				{
					if(exceptionIndex == exceptionListIndex)
					{
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_DELETESTRING, exceptionListIndex, NULL);
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_INSERTSTRING, exceptionListIndex, (LPARAM)exceptionName.c_str());
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, exceptionListIndex, NULL);
						*cachedExceptionListIterator = exceptionEntry;
						model.SetExceptionDebugEntries(cachedExceptionList);
						break;
					}
					++cachedExceptionListIterator;
					++exceptionIndex;
				}
			}
			else
			{
				cachedExceptionList.push_back(exceptionEntry);
				model.SetExceptionDebugEntries(cachedExceptionList);
				exceptionListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_ADDSTRING, 0, (LPARAM)exceptionName.c_str());
			}
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, exceptionListIndex, NULL);
			break;}
		case IDC_M68000_EXCEPTIONS_DELETE:
			if(exceptionListIndex != -1)
			{
				int exceptionIndex = 0;
				std::list<IM68000::ExceptionDebuggingEntry>::iterator cachedExceptionListIterator = cachedExceptionList.begin();
				while(cachedExceptionListIterator != cachedExceptionList.end())
				{
					if(exceptionIndex == exceptionListIndex)
					{
						cachedExceptionList.erase(cachedExceptionListIterator);
						model.SetExceptionDebugEntries(cachedExceptionList);
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_DELETESTRING, exceptionListIndex, NULL);
						if(exceptionListIndex >= (int)cachedExceptionList.size())
						{
							exceptionListIndex -= 1;
						}
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, exceptionListIndex, NULL);
						break;
					}
					++cachedExceptionListIterator;
					++exceptionIndex;
				}
			}
			exceptionEntry = IM68000::ExceptionDebuggingEntry();
			UpdateExceptionEntry(hwnd, exceptionEntry);
			break;
		case IDC_M68000_EXCEPTIONS_DELETEALL:{
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_RESETCONTENT, 0, NULL);
			cachedExceptionList.clear();
			model.SetExceptionDebugEntries(cachedExceptionList);

			//Clear the current exception info
			exceptionEntry = IM68000::ExceptionDebuggingEntry();
			UpdateExceptionEntry(hwnd, exceptionEntry);
			exceptionListIndex = -1;
			break;}
		}
	}
	else if(HIWORD(wparam) == LBN_SELCHANGE)
	{
		switch(LOWORD(wparam))
		{
		case IDC_M68000_EXCEPTIONS_LIST:
			int selectedItem = (int)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_GETCURSEL, 0, NULL);
			if(selectedItem != LB_ERR)
			{
				int exceptionIndex = 0;
				std::list<IM68000::ExceptionDebuggingEntry>::const_iterator cachedExceptionListIterator = cachedExceptionList.begin();
				while(cachedExceptionListIterator != cachedExceptionList.end())
				{
					if(exceptionIndex == exceptionListIndex)
					{
						exceptionListIndex = selectedItem;
						UpdateExceptionEntry(hwnd, exceptionEntry);
						break;
					}
					++cachedExceptionListIterator;
					++exceptionIndex;
				}
			}
			break;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Exception functions
//----------------------------------------------------------------------------------------
void ExceptionsView::UpdateExceptionEntry(HWND hwnd, const IM68000::ExceptionDebuggingEntry& targetExceptionEntry)
{
	CheckDlgButton(hwnd, IDC_M68000_EXCEPTIONS_LOG, (targetExceptionEntry.enableLogging)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_M68000_EXCEPTIONS_BREAK, (targetExceptionEntry.enableBreak)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_M68000_EXCEPTIONS_DISABLE, (targetExceptionEntry.disable)? BST_CHECKED: BST_UNCHECKED);

	UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORNUMBER, addressBusCharWidth, targetExceptionEntry.vectorNumber);
	UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORADDRESS, addressBusCharWidth, targetExceptionEntry.vectorNumber * 4);
	UpdateDlgItemString(hwnd, IDC_M68000_EXCEPTIONS_VECTORNAME, model.GetExceptionName(targetExceptionEntry.vectorNumber));
}

//----------------------------------------------------------------------------------------
std::wstring ExceptionsView::BuildExceptionName(unsigned int vectorNumber) const
{
	std::wstring vectorNumberAsString;
	std::wstring vectorAddressAsString;
	IntToStringBase16(vectorNumber, vectorNumberAsString, 2, true);
	IntToStringBase16(vectorNumber * 4, vectorAddressAsString, 4, true);
	std::wstring exceptionName;
	exceptionName = vectorNumberAsString + L"(" + vectorAddressAsString + L") " + model.GetExceptionName(vectorNumber);
	return exceptionName;
}
