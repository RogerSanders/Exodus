#include "WatchpointView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WatchpointView::WatchpointView(IUIManager& uiManager, WatchpointViewPresenter& presenter, IProcessor& model)
:ViewBase(uiManager, presenter), _presenter(presenter), _model(model), _initializedDialog(false), _currentControlFocus(0)
{
	_selectedWatchpoint = 0;
	_selectedWatchpointListIndex = -1;
	SetDialogTemplateSettings(presenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_WATCH));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR WatchpointView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR WatchpointView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);
	ClearWatchpointData();
	UpdateWatchpointFields(hwnd);
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR WatchpointView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR WatchpointView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	_initializedDialog = true;

	//Check if the watchpoint list has changed since it was last checked
	std::list<IWatchpoint*> currentWatchpointList = _model.GetWatchpointList();
	std::list<IWatchpoint*>::const_iterator curentWatchpointListIterator = currentWatchpointList.begin();
	std::list<IWatchpoint*>::const_iterator cachedWatchpointListIterator = _cachedWatchpointList.begin();
	bool watchpointListChanged = currentWatchpointList.size() != _cachedWatchpointList.size();
	while(!watchpointListChanged && (curentWatchpointListIterator != currentWatchpointList.end()) && (cachedWatchpointListIterator != _cachedWatchpointList.end()))
	{
		if(*curentWatchpointListIterator != *cachedWatchpointListIterator)
		{
			watchpointListChanged = true;
			continue;
		}
		++curentWatchpointListIterator;
		++cachedWatchpointListIterator;
	}

	//Refresh the watchpoint list if it has changed
	if(watchpointListChanged)
	{
		//Save the new watchpoint list
		_cachedWatchpointList = currentWatchpointList;

		//Refresh the list of watchpoints
		bool foundSelectedWatchpoint = false;
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), WM_SETREDRAW, FALSE, 0);
		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETTOPINDEX, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_RESETCONTENT, 0, NULL);
		for(std::list<IWatchpoint*>::const_iterator i = _cachedWatchpointList.begin(); i != _cachedWatchpointList.end(); ++i)
		{
			IWatchpoint* watchpoint = *i;
			if(_model.LockWatchpoint(watchpoint))
			{
				std::wstring watchpointName = watchpoint->GetName();
				int listIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_ADDSTRING, 0, (LPARAM)watchpointName.c_str());
				if(watchpoint == _selectedWatchpoint)
				{
					foundSelectedWatchpoint = true;
					_selectedWatchpointListIndex = listIndex;
				}
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETITEMDATA, listIndex, (LPARAM)watchpoint);
				_model.UnlockWatchpoint(watchpoint);
			}
		}
		if(!foundSelectedWatchpoint && (_selectedWatchpointListIndex >= 0))
		{
			_selectedWatchpoint = 0;
			_selectedWatchpointListIndex = -1;
		}
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, (WPARAM)_selectedWatchpointListIndex, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETTOPINDEX, top, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), NULL, FALSE);
	}

	//If we currently have a selected watchpoint from the list, update its hit counter.
	if(_selectedWatchpoint != 0)
	{
		if(_model.LockWatchpoint(_selectedWatchpoint))
		{
			_watchpointCounter = _selectedWatchpoint->GetHitCounter();
			UpdateDlgItemBin(hwnd, IDC_PROCESSOR_WATCH_HITCOUNT, _watchpointCounter);
			_model.UnlockWatchpoint(_selectedWatchpoint);
		}
		UpdateWatchpointFields(hwnd);
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR WatchpointView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			case IDC_PROCESSOR_WATCH_NAME:
				_watchpointName = GetDlgItemString(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_WATCH_LOCCONDDATA1:
				_watchpointLocation1 = GetDlgItemHex(hwnd, LOWORD(wparam)) & _watchpointLocationMask;
				break;
			case IDC_PROCESSOR_WATCH_LOCCONDDATA2:
				_watchpointLocation2 = GetDlgItemHex(hwnd, LOWORD(wparam)) & _watchpointLocationMask;
				break;
			case IDC_PROCESSOR_WATCH_LOCMASK:
				_watchpointLocationMask = GetDlgItemHex(hwnd, LOWORD(wparam)) & _model.GetAddressBusMask();
				break;
			case IDC_PROCESSOR_WATCH_BREAKCOUNTER:
				_watchpointCounterInterval = GetDlgItemBin(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_WATCH_READCONDDATA1:
				_watchpointReadConditionData1 = GetDlgItemHex(hwnd, LOWORD(wparam)) & _model.GetDataBusCharWidth();
				break;
			case IDC_PROCESSOR_WATCH_READCONDDATA2:
				_watchpointReadConditionData2 = GetDlgItemHex(hwnd, LOWORD(wparam)) & _model.GetDataBusCharWidth();
				break;
			case IDC_PROCESSOR_WATCH_WRITECONDDATA1:
				_watchpointWriteConditionData1 = GetDlgItemHex(hwnd, LOWORD(wparam)) & _model.GetDataBusCharWidth();
				break;
			case IDC_PROCESSOR_WATCH_WRITECONDDATA2:
				_watchpointWriteConditionData2 = GetDlgItemHex(hwnd, LOWORD(wparam)) & _model.GetDataBusCharWidth();
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_WATCH_ENABLE:
			_watchpointEnable = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_LOG:
			_watchpointLog = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_BREAK:
			_watchpointBreak = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_ONREAD:
			_watchpointOnRead = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_ONWRITE:
			_watchpointOnWrite = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_BREAKONCOUNTER:
			_watchpointCounterEnable = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_HITCOUNT_RESET:
			if((_selectedWatchpoint != 0) && _model.LockWatchpoint(_selectedWatchpoint))
			{
				_watchpointCounter = 0;
				_selectedWatchpoint->SetHitCounter(0);
				_model.UnlockWatchpoint(_selectedWatchpoint);
			}
			break;

		case IDC_PROCESSOR_WATCH_LOCCONDNOT:
			_watchpointConditionNot = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND1:
			_watchpointCondition = IWatchpoint::Condition::Equal;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND2:
			_watchpointCondition = IWatchpoint::Condition::Greater;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND3:
			_watchpointCondition = IWatchpoint::Condition::Less;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND4:
			_watchpointCondition = IWatchpoint::Condition::GreaterAndLess;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), TRUE);
			break;
		case IDC_PROCESSOR_WATCH_READCONDENABLED:
			_watchpointReadConditionEnabled = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_READCONDNOT:
			_watchpointReadConditionNot = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_WRITECONDENABLED:
			_watchpointWriteConditionEnabled = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_WRITECONDNOT:
			_watchpointWriteConditionNot = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_READCOND1:
			_watchpointReadCondition = IWatchpoint::Condition::Equal;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_READCOND2:
			_watchpointReadCondition = IWatchpoint::Condition::Greater;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_READCOND3:
			_watchpointReadCondition = IWatchpoint::Condition::Less;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_READCOND4:
			_watchpointReadCondition = IWatchpoint::Condition::GreaterAndLess;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), TRUE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND1:
			_watchpointWriteCondition = IWatchpoint::Condition::Equal;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND2:
			_watchpointWriteCondition = IWatchpoint::Condition::Greater;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND3:
			_watchpointWriteCondition = IWatchpoint::Condition::Less;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND4:
			_watchpointWriteCondition = IWatchpoint::Condition::GreaterAndLess;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), TRUE);
			break;

		case IDC_PROCESSOR_WATCH_NEW:
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, (WPARAM)-1, NULL);
			_selectedWatchpointListIndex = -1;
			_selectedWatchpoint = 0;
			ClearWatchpointData();
			UpdateWatchpointFields(hwnd);
			break;
		case IDC_PROCESSOR_WATCH_SAVE:
			if(_selectedWatchpoint == 0)
			{
				_selectedWatchpoint = _model.CreateWatchpoint();
			}
			if(_model.LockWatchpoint(_selectedWatchpoint))
			{
				SaveDataToWatchpoint(_selectedWatchpoint);
				_model.UnlockWatchpoint(_selectedWatchpoint);
			}
			if(_selectedWatchpointListIndex != -1)
			{
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_DELETESTRING, _selectedWatchpointListIndex, NULL);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_INSERTSTRING, _selectedWatchpointListIndex, (LPARAM)_watchpointName.c_str());
			}
			else
			{
				_selectedWatchpointListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_ADDSTRING, 0, (LPARAM)_watchpointName.c_str());
			}
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETITEMDATA, _selectedWatchpointListIndex, (LPARAM)_selectedWatchpoint);
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, _selectedWatchpointListIndex, NULL);
			break;
		case IDC_PROCESSOR_WATCH_DELETE:
			if(_selectedWatchpoint != 0)
			{
				_model.DeleteWatchpoint(_selectedWatchpoint);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_DELETESTRING, _selectedWatchpointListIndex, NULL);
				LRESULT getItemDataReturn = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, _selectedWatchpointListIndex, 0);
				if(getItemDataReturn == LB_ERR)
				{
					_selectedWatchpoint = 0;
					_selectedWatchpointListIndex = -1;
				}
				else
				{
					_selectedWatchpoint = (IWatchpoint*)getItemDataReturn;
					if(_model.LockWatchpoint(_selectedWatchpoint))
					{
						LoadDataFromWatchpoint(_selectedWatchpoint);
						_model.UnlockWatchpoint(_selectedWatchpoint);
					}
				}
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, _selectedWatchpointListIndex, NULL);
			}
			else
			{
				ClearWatchpointData();
			}
			UpdateWatchpointFields(hwnd);
			break;
		case IDC_PROCESSOR_WATCH_ENABLEALL:
			for(std::list<IWatchpoint*>::const_iterator i = _cachedWatchpointList.begin(); i != _cachedWatchpointList.end(); ++i)
			{
				IWatchpoint* watchpoint = *i;
				if(_model.LockWatchpoint(watchpoint))
				{
					watchpoint->SetEnabled(true);
					_model.UnlockWatchpoint(watchpoint);
				}
			}
			break;
		case IDC_PROCESSOR_WATCH_DISABLEALL:
			for(std::list<IWatchpoint*>::const_iterator i = _cachedWatchpointList.begin(); i != _cachedWatchpointList.end(); ++i)
			{
				IWatchpoint* watchpoint = *i;
				if(_model.LockWatchpoint(watchpoint))
				{
					watchpoint->SetEnabled(false);
					_model.UnlockWatchpoint(watchpoint);
				}
			}
			break;
		case IDC_PROCESSOR_WATCH_DELETEALL:
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_RESETCONTENT, 0, NULL);
			for(std::list<IWatchpoint*>::const_iterator i = _cachedWatchpointList.begin(); i != _cachedWatchpointList.end(); ++i)
			{
				IWatchpoint* watchpoint = *i;
				_model.DeleteWatchpoint(watchpoint);
			}

			//Clear the current watchpoint info
			_cachedWatchpointList.clear();
			_selectedWatchpointListIndex = -1;
			_selectedWatchpoint = 0;
			ClearWatchpointData();
			UpdateWatchpointFields(hwnd);
			break;
		}
	}
	else if(HIWORD(wparam) == LBN_SELCHANGE)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_WATCH_LIST:
			int selectedItem = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETCURSEL, 0, NULL);
			if(selectedItem != LB_ERR)
			{
				_selectedWatchpointListIndex = selectedItem;
				_selectedWatchpoint = (IWatchpoint*)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, _selectedWatchpointListIndex, 0);
				if(_model.LockWatchpoint(_selectedWatchpoint))
				{
					LoadDataFromWatchpoint(_selectedWatchpoint);
					_model.UnlockWatchpoint(_selectedWatchpoint);
				}
				UpdateWatchpointFields(hwnd);
			}
			break;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Watchpoint functions
//----------------------------------------------------------------------------------------
void WatchpointView::LoadDataFromWatchpoint(IWatchpoint* watchpoint)
{
	_watchpointName = watchpoint->GetName();
	_watchpointEnable = watchpoint->GetEnabled();
	_watchpointLog = watchpoint->GetLogEvent();
	_watchpointBreak = watchpoint->GetBreakEvent();
	_watchpointConditionNot = watchpoint->GetLocationConditionNot();
	_watchpointOnRead = watchpoint->GetOnRead();
	_watchpointOnWrite = watchpoint->GetOnWrite();
	_watchpointReadConditionEnabled = watchpoint->GetReadConditionEnabled();
	_watchpointReadConditionNot = watchpoint->GetReadConditionNot();
	_watchpointWriteConditionEnabled = watchpoint->GetWriteConditionEnabled();
	_watchpointWriteConditionNot = watchpoint->GetWriteConditionNot();
	_watchpointCondition = watchpoint->GetLocationCondition();
	_watchpointReadCondition = watchpoint->GetReadCondition();
	_watchpointWriteCondition = watchpoint->GetWriteCondition();
	_watchpointLocation1 = watchpoint->GetLocationConditionData1();
	_watchpointLocation2 = watchpoint->GetLocationConditionData2();
	_watchpointLocationMask = watchpoint->GetLocationMask();
	_watchpointReadConditionData1 = watchpoint->GetReadConditionData1();
	_watchpointReadConditionData2 = watchpoint->GetReadConditionData2();
	_watchpointWriteConditionData1 = watchpoint->GetWriteConditionData1();
	_watchpointWriteConditionData2 = watchpoint->GetWriteConditionData2();
	_watchpointCounterEnable = watchpoint->GetBreakOnCounter();
	_watchpointCounter = watchpoint->GetHitCounter();
	_watchpointCounterInterval = watchpoint->GetBreakCounter();
}

//----------------------------------------------------------------------------------------
void WatchpointView::SaveDataToWatchpoint(IWatchpoint* watchpoint)
{
	bool generateName = false;
	if(_watchpointName.empty() || (_watchpointName == watchpoint->GenerateName()))
	{
		generateName = true;
	}
	watchpoint->SetEnabled(_watchpointEnable);
	watchpoint->SetLogEvent(_watchpointLog);
	watchpoint->SetBreakEvent(_watchpointBreak);
	watchpoint->SetLocationConditionNot(_watchpointConditionNot);
	watchpoint->SetOnRead(_watchpointOnRead);
	watchpoint->SetOnWrite(_watchpointOnWrite);
	watchpoint->SetReadConditionEnabled(_watchpointReadConditionEnabled);
	watchpoint->SetReadConditionNot(_watchpointReadConditionNot);
	watchpoint->SetWriteConditionEnabled(_watchpointWriteConditionEnabled);
	watchpoint->SetWriteConditionNot(_watchpointWriteConditionNot);
	watchpoint->SetLocationCondition(_watchpointCondition);
	watchpoint->SetReadCondition(_watchpointReadCondition);
	watchpoint->SetWriteCondition(_watchpointWriteCondition);
	watchpoint->SetLocationConditionData1(_watchpointLocation1);
	watchpoint->SetLocationConditionData2(_watchpointLocation2);
	watchpoint->SetLocationMask(_watchpointLocationMask);
	watchpoint->SetReadConditionData1(_watchpointReadConditionData1);
	watchpoint->SetReadConditionData2(_watchpointReadConditionData2);
	watchpoint->SetWriteConditionData1(_watchpointWriteConditionData1);
	watchpoint->SetWriteConditionData2(_watchpointWriteConditionData2);
	watchpoint->SetBreakOnCounter(_watchpointCounterEnable);
	watchpoint->SetHitCounter(_watchpointCounter);
	watchpoint->SetBreakCounter(_watchpointCounterInterval);
	if(generateName)
	{
		_watchpointName = watchpoint->GenerateName();
	}
	watchpoint->SetName(_watchpointName);
}

//----------------------------------------------------------------------------------------
void WatchpointView::ClearWatchpointData()
{
	_watchpointName.clear();
	_watchpointEnable = true;
	_watchpointLog = true;
	_watchpointBreak = true;
	_watchpointConditionNot = false;
	_watchpointOnRead = true;
	_watchpointOnWrite = true;
	_watchpointReadConditionEnabled = false;
	_watchpointReadConditionNot = false;
	_watchpointWriteConditionEnabled = false;
	_watchpointWriteConditionNot = false;
	_watchpointCondition = IWatchpoint::Condition::Equal;
	_watchpointReadCondition = IWatchpoint::Condition::Equal;
	_watchpointWriteCondition = IWatchpoint::Condition::Equal;
	_watchpointLocation1 = 0;
	_watchpointLocation2 = 0;
	_watchpointLocationMask = _model.GetAddressBusMask();
	_watchpointReadConditionData1 = 0;
	_watchpointReadConditionData2 = 0;
	_watchpointWriteConditionData1 = 0;
	_watchpointWriteConditionData2 = 0;
	_watchpointCounterEnable = false;
	_watchpointCounter = 0;
	_watchpointCounterInterval = 1;
}

//----------------------------------------------------------------------------------------
void WatchpointView::UpdateWatchpointFields(HWND hwnd)
{
	unsigned int addressCharWidth = _model.GetAddressBusCharWidth();
	unsigned int dataCharWidth = _model.GetDataBusCharWidth();
	UpdateDlgItemString(hwnd, IDC_PROCESSOR_WATCH_NAME, _watchpointName);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_ENABLE, (_watchpointEnable)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_LOG, (_watchpointLog)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_BREAK, (_watchpointBreak)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_ONREAD, (_watchpointOnRead)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_ONWRITE, (_watchpointOnWrite)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_BREAKONCOUNTER, (_watchpointCounterEnable)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemBin(hwnd, IDC_PROCESSOR_WATCH_BREAKCOUNTER, _watchpointCounterInterval);
	UpdateDlgItemBin(hwnd, IDC_PROCESSOR_WATCH_HITCOUNT, _watchpointCounter);

	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA1, addressCharWidth, _watchpointLocation1);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2, addressCharWidth, _watchpointLocation2);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_LOCMASK, addressCharWidth, _watchpointLocationMask);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_LOCCONDNOT, (_watchpointConditionNot)? BST_CHECKED: BST_UNCHECKED);
	switch(_watchpointCondition)
	{
	case IWatchpoint::Condition::Equal:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND1);
		break;
	case IWatchpoint::Condition::Greater:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND2);
		break;
	case IWatchpoint::Condition::Less:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND3);
		break;
	case IWatchpoint::Condition::GreaterAndLess:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND4);
		break;
	}
	if(_watchpointCondition == IWatchpoint::Condition::GreaterAndLess)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
	}

	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_READCONDENABLED, (_watchpointReadConditionEnabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_READCONDNOT, (_watchpointReadConditionNot)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_WRITECONDENABLED, (_watchpointWriteConditionEnabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_WRITECONDNOT, (_watchpointWriteConditionNot)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA1, dataCharWidth, _watchpointReadConditionData1);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2, dataCharWidth, _watchpointReadConditionData2);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA1, dataCharWidth, _watchpointWriteConditionData1);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2, dataCharWidth, _watchpointWriteConditionData2);

	switch(_watchpointReadCondition)
	{
	case IWatchpoint::Condition::Equal:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND1);
		break;
	case IWatchpoint::Condition::Greater:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND2);
		break;
	case IWatchpoint::Condition::Less:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND3);
		break;
	case IWatchpoint::Condition::GreaterAndLess:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND4);
		break;
	}
	if(_watchpointReadCondition == IWatchpoint::Condition::GreaterAndLess)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
	}

	switch(_watchpointWriteCondition)
	{
	case IWatchpoint::Condition::Equal:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND1);
		break;
	case IWatchpoint::Condition::Greater:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND2);
		break;
	case IWatchpoint::Condition::Less:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND3);
		break;
	case IWatchpoint::Condition::GreaterAndLess:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND4);
		break;
	}
	if(_watchpointWriteCondition == IWatchpoint::Condition::GreaterAndLess)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
	}
}
