#include "ExceptionsView.h"
#include "resource.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExceptionsView::ExceptionsView(IUIManager& uiManager, ExceptionsViewPresenter& presenter, IM68000& model)
:ViewBase(uiManager, presenter), _presenter(presenter), _model(model), _initializedDialog(false), _currentControlFocus(0)
{
	_addressBusCharWidth = 6;
	_exceptionListIndex = -1;
	SetDialogTemplateSettings(presenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_M68000_EXCEPTIONS));
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
	UpdateExceptionEntry(hwnd, _exceptionEntry);
	_exceptionListIndex = -1;
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
	_initializedDialog = true;

	//Check if we need to refresh the exception debug list
	std::list<IM68000::ExceptionDebuggingEntry> currentExceptionList = _model.GetExceptionDebugEntries();
	bool refreshExceptionList = (_cachedExceptionList.size() != currentExceptionList.size());
	if(!refreshExceptionList)
	{
		std::list<IM68000::ExceptionDebuggingEntry>::const_iterator currentExceptionListIterator = currentExceptionList.begin();
		std::list<IM68000::ExceptionDebuggingEntry>::const_iterator cachedExceptionListIterator = _cachedExceptionList.begin();
		while(!refreshExceptionList && (currentExceptionListIterator != currentExceptionList.end()) && (cachedExceptionListIterator != _cachedExceptionList.end()))
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
		_cachedExceptionList = currentExceptionList;

		//Refresh the list of exception debug entries
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), WM_SETREDRAW, FALSE, 0);
		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_GETTOPINDEX, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_RESETCONTENT, 0, NULL);
		for(std::list<IM68000::ExceptionDebuggingEntry>::iterator i = _cachedExceptionList.begin(); i != _cachedExceptionList.end(); ++i)
		{
			std::wstring exceptionName = BuildExceptionName((*i).vectorNumber);
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_ADDSTRING, 0, (LPARAM)exceptionName.c_str());
		}
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, (WPARAM)-1, 0);
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETTOPINDEX, top, 0);
		SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), NULL, FALSE);

		_exceptionListIndex = -1;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ExceptionsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && _initializedDialog)
	{
		_previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		_currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && _initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != _previousText)
		{
			switch(LOWORD(wparam))
			{
			case IDC_M68000_EXCEPTIONS_VECTORNUMBER:
				_exceptionEntry.vectorNumber = (IM68000::Exceptions)GetDlgItemHex(hwnd, LOWORD(wparam));
				break;
			case IDC_M68000_EXCEPTIONS_VECTORADDRESS:
				_exceptionEntry.vectorNumber = (IM68000::Exceptions)(GetDlgItemHex(hwnd, LOWORD(wparam)) / 4);
				break;
			}
			UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORNUMBER, _addressBusCharWidth, (unsigned int)_exceptionEntry.vectorNumber);
			UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORADDRESS, _addressBusCharWidth, (unsigned int)_exceptionEntry.vectorNumber * 4);
			UpdateDlgItemString(hwnd, IDC_M68000_EXCEPTIONS_VECTORNAME, _model.GetExceptionName(_exceptionEntry.vectorNumber));
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_M68000_EXCEPTIONS_LOG:
			_exceptionEntry.enableLogging = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_M68000_EXCEPTIONS_BREAK:
			_exceptionEntry.enableBreak = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_M68000_EXCEPTIONS_DISABLE:
			_exceptionEntry.disable = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_M68000_EXCEPTIONS_TRIGGER:
			_model.TriggerException(_exceptionEntry.vectorNumber);
			break;

		case IDC_M68000_EXCEPTIONS_LOGALL:
			_model.SetLogAllExceptions(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_M68000_EXCEPTIONS_BREAKALL:
			_model.SetBreakOnAllExceptions(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_M68000_EXCEPTIONS_DISABLEALL:
			_model.SetDisableAllExceptions(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;

		case IDC_M68000_EXCEPTIONS_NEW:
			_exceptionEntry = IM68000::ExceptionDebuggingEntry();
			UpdateExceptionEntry(hwnd, _exceptionEntry);
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, (WPARAM)-1, NULL);
			_exceptionListIndex = -1;
			break;
		case IDC_M68000_EXCEPTIONS_SAVE:{
			std::wstring exceptionName = BuildExceptionName(_exceptionEntry.vectorNumber);
			if(_exceptionListIndex != -1)
			{
				int exceptionIndex = 0;
				std::list<IM68000::ExceptionDebuggingEntry>::iterator cachedExceptionListIterator = _cachedExceptionList.begin();
				while(cachedExceptionListIterator != _cachedExceptionList.end())
				{
					if(exceptionIndex == _exceptionListIndex)
					{
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_DELETESTRING, _exceptionListIndex, NULL);
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_INSERTSTRING, _exceptionListIndex, (LPARAM)exceptionName.c_str());
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, _exceptionListIndex, NULL);
						*cachedExceptionListIterator = _exceptionEntry;
						_model.SetExceptionDebugEntries(_cachedExceptionList);
						break;
					}
					++cachedExceptionListIterator;
					++exceptionIndex;
				}
			}
			else
			{
				_cachedExceptionList.push_back(_exceptionEntry);
				_model.SetExceptionDebugEntries(_cachedExceptionList);
				_exceptionListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_ADDSTRING, 0, (LPARAM)exceptionName.c_str());
			}
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, _exceptionListIndex, NULL);
			break;}
		case IDC_M68000_EXCEPTIONS_DELETE:
			if(_exceptionListIndex != -1)
			{
				int exceptionIndex = 0;
				std::list<IM68000::ExceptionDebuggingEntry>::iterator cachedExceptionListIterator = _cachedExceptionList.begin();
				while(cachedExceptionListIterator != _cachedExceptionList.end())
				{
					if(exceptionIndex == _exceptionListIndex)
					{
						_cachedExceptionList.erase(cachedExceptionListIterator);
						_model.SetExceptionDebugEntries(_cachedExceptionList);
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_DELETESTRING, _exceptionListIndex, NULL);
						if(_exceptionListIndex >= (int)_cachedExceptionList.size())
						{
							_exceptionListIndex -= 1;
						}
						SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_SETCURSEL, _exceptionListIndex, NULL);
						break;
					}
					++cachedExceptionListIterator;
					++exceptionIndex;
				}
			}
			_exceptionEntry = IM68000::ExceptionDebuggingEntry();
			UpdateExceptionEntry(hwnd, _exceptionEntry);
			break;
		case IDC_M68000_EXCEPTIONS_DELETEALL:{
			SendMessage(GetDlgItem(hwnd, IDC_M68000_EXCEPTIONS_LIST), LB_RESETCONTENT, 0, NULL);
			_cachedExceptionList.clear();
			_model.SetExceptionDebugEntries(_cachedExceptionList);

			//Clear the current exception info
			_exceptionEntry = IM68000::ExceptionDebuggingEntry();
			UpdateExceptionEntry(hwnd, _exceptionEntry);
			_exceptionListIndex = -1;
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
				std::list<IM68000::ExceptionDebuggingEntry>::const_iterator cachedExceptionListIterator = _cachedExceptionList.begin();
				while(cachedExceptionListIterator != _cachedExceptionList.end())
				{
					if(exceptionIndex == _exceptionListIndex)
					{
						_exceptionListIndex = selectedItem;
						UpdateExceptionEntry(hwnd, _exceptionEntry);
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

	UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORNUMBER, _addressBusCharWidth, (unsigned int)targetExceptionEntry.vectorNumber);
	UpdateDlgItemHex(hwnd, IDC_M68000_EXCEPTIONS_VECTORADDRESS, _addressBusCharWidth, (unsigned int)targetExceptionEntry.vectorNumber * 4);
	UpdateDlgItemString(hwnd, IDC_M68000_EXCEPTIONS_VECTORNAME, _model.GetExceptionName(targetExceptionEntry.vectorNumber));
}

//----------------------------------------------------------------------------------------
std::wstring ExceptionsView::BuildExceptionName(IM68000::Exceptions vectorNumber) const
{
	std::wstring vectorNumberAsString;
	std::wstring vectorAddressAsString;
	IntToStringBase16((unsigned int)vectorNumber, vectorNumberAsString, 2, true);
	IntToStringBase16((unsigned int)vectorNumber * 4, vectorAddressAsString, 4, true);
	std::wstring exceptionName;
	exceptionName = vectorNumberAsString + L"(" + vectorAddressAsString + L") " + _model.GetExceptionName(vectorNumber);
	return exceptionName;
}
