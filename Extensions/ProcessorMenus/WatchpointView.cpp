#include "WatchpointView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WatchpointView::WatchpointView(IUIManager& auiManager, WatchpointViewPresenter& apresenter, IProcessor& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	selectedWatchpoint = 0;
	selectedWatchpointListIndex = -1;
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_WATCH));
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
	initializedDialog = true;

	//Check if the watchpoint list has changed since it was last checked
	std::list<IWatchpoint*> currentWatchpointList = model.GetWatchpointList();
	std::list<IWatchpoint*>::const_iterator curentWatchpointListIterator = currentWatchpointList.begin();
	std::list<IWatchpoint*>::const_iterator cachedWatchpointListIterator = cachedWatchpointList.begin();
	bool watchpointListChanged = currentWatchpointList.size() != cachedWatchpointList.size();
	while(!watchpointListChanged && (curentWatchpointListIterator != currentWatchpointList.end()) && (cachedWatchpointListIterator != cachedWatchpointList.end()))
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
		cachedWatchpointList = currentWatchpointList;

		//Refresh the list of watchpoints
		bool foundSelectedWatchpoint = false;
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), WM_SETREDRAW, FALSE, 0);
		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETTOPINDEX, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_RESETCONTENT, 0, NULL);
		for(std::list<IWatchpoint*>::const_iterator i = cachedWatchpointList.begin(); i != cachedWatchpointList.end(); ++i)
		{
			IWatchpoint* watchpoint = *i;
			if(model.LockWatchpoint(watchpoint))
			{
				std::wstring watchpointName = watchpoint->GetName();
				int listIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_ADDSTRING, 0, (LPARAM)watchpointName.c_str());
				if(watchpoint == selectedWatchpoint)
				{
					foundSelectedWatchpoint = true;
					selectedWatchpointListIndex = listIndex;
				}
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETITEMDATA, listIndex, (LPARAM)watchpoint);
				model.UnlockWatchpoint(watchpoint);
			}
		}
		if(!foundSelectedWatchpoint && (selectedWatchpointListIndex >= 0))
		{
			selectedWatchpoint = 0;
			selectedWatchpointListIndex = -1;
		}
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, (WPARAM)selectedWatchpointListIndex, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETTOPINDEX, top, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), NULL, FALSE);
	}

	//If we currently have a selected watchpoint from the list, update its hit counter.
	if(selectedWatchpoint != 0)
	{
		if(model.LockWatchpoint(selectedWatchpoint))
		{
			watchpointCounter = selectedWatchpoint->GetHitCounter();
			UpdateDlgItemBin(hwnd, IDC_PROCESSOR_WATCH_HITCOUNT, watchpointCounter);
			model.UnlockWatchpoint(selectedWatchpoint);
		}
		UpdateWatchpointFields(hwnd);
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR WatchpointView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			case IDC_PROCESSOR_WATCH_NAME:
				watchpointName = GetDlgItemString(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_WATCH_LOCCONDDATA1:
				watchpointLocation1 = GetDlgItemHex(hwnd, LOWORD(wparam)) & watchpointLocationMask;
				break;
			case IDC_PROCESSOR_WATCH_LOCCONDDATA2:
				watchpointLocation2 = GetDlgItemHex(hwnd, LOWORD(wparam)) & watchpointLocationMask;
				break;
			case IDC_PROCESSOR_WATCH_LOCMASK:
				watchpointLocationMask = GetDlgItemHex(hwnd, LOWORD(wparam)) & model.GetAddressBusMask();
				break;
			case IDC_PROCESSOR_WATCH_BREAKCOUNTER:
				watchpointCounterInterval = GetDlgItemBin(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_WATCH_READCONDDATA1:
				watchpointReadConditionData1 = GetDlgItemHex(hwnd, LOWORD(wparam)) & model.GetDataBusCharWidth();
				break;
			case IDC_PROCESSOR_WATCH_READCONDDATA2:
				watchpointReadConditionData2 = GetDlgItemHex(hwnd, LOWORD(wparam)) & model.GetDataBusCharWidth();
				break;
			case IDC_PROCESSOR_WATCH_WRITECONDDATA1:
				watchpointWriteConditionData1 = GetDlgItemHex(hwnd, LOWORD(wparam)) & model.GetDataBusCharWidth();
				break;
			case IDC_PROCESSOR_WATCH_WRITECONDDATA2:
				watchpointWriteConditionData2 = GetDlgItemHex(hwnd, LOWORD(wparam)) & model.GetDataBusCharWidth();
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_WATCH_ENABLE:
			watchpointEnable = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_LOG:
			watchpointLog = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_BREAK:
			watchpointBreak = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_ONREAD:
			watchpointOnRead = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_ONWRITE:
			watchpointOnWrite = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_BREAKONCOUNTER:
			watchpointCounterEnable = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_HITCOUNT_RESET:
			if((selectedWatchpoint != 0) && model.LockWatchpoint(selectedWatchpoint))
			{
				watchpointCounter = 0;
				selectedWatchpoint->SetHitCounter(0);
				model.UnlockWatchpoint(selectedWatchpoint);
			}
			break;

		case IDC_PROCESSOR_WATCH_LOCCONDNOT:
			watchpointConditionNot = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND1:
			watchpointCondition = IWatchpoint::CONDITION_EQUAL;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND2:
			watchpointCondition = IWatchpoint::CONDITION_GREATER;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND3:
			watchpointCondition = IWatchpoint::CONDITION_LESS;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND4:
			watchpointCondition = IWatchpoint::CONDITION_GREATER_AND_LESS;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), TRUE);
			break;
		case IDC_PROCESSOR_WATCH_READCONDENABLED:
			watchpointReadConditionEnabled = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_READCONDNOT:
			watchpointReadConditionNot = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_WRITECONDENABLED:
			watchpointWriteConditionEnabled = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_WRITECONDNOT:
			watchpointWriteConditionNot = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_WATCH_READCOND1:
			watchpointReadCondition = IWatchpoint::CONDITION_EQUAL;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_READCOND2:
			watchpointReadCondition = IWatchpoint::CONDITION_GREATER;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_READCOND3:
			watchpointReadCondition = IWatchpoint::CONDITION_LESS;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_READCOND4:
			watchpointReadCondition = IWatchpoint::CONDITION_GREATER_AND_LESS;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), TRUE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND1:
			watchpointWriteCondition = IWatchpoint::CONDITION_EQUAL;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND2:
			watchpointWriteCondition = IWatchpoint::CONDITION_GREATER;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND3:
			watchpointWriteCondition = IWatchpoint::CONDITION_LESS;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND4:
			watchpointWriteCondition = IWatchpoint::CONDITION_GREATER_AND_LESS;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), TRUE);
			break;

		case IDC_PROCESSOR_WATCH_NEW:
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, (WPARAM)-1, NULL);
			selectedWatchpointListIndex = -1;
			selectedWatchpoint = 0;
			ClearWatchpointData();
			UpdateWatchpointFields(hwnd);
			break;
		case IDC_PROCESSOR_WATCH_SAVE:
			if(selectedWatchpoint == 0)
			{
				selectedWatchpoint = model.CreateWatchpoint();
			}
			if(model.LockWatchpoint(selectedWatchpoint))
			{
				SaveDataToWatchpoint(selectedWatchpoint);
				model.UnlockWatchpoint(selectedWatchpoint);
			}
			if(selectedWatchpointListIndex != -1)
			{
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_DELETESTRING, selectedWatchpointListIndex, NULL);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_INSERTSTRING, selectedWatchpointListIndex, (LPARAM)watchpointName.c_str());
			}
			else
			{
				selectedWatchpointListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_ADDSTRING, 0, (LPARAM)watchpointName.c_str());
			}
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETITEMDATA, selectedWatchpointListIndex, (LPARAM)selectedWatchpoint);
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, selectedWatchpointListIndex, NULL);
			break;
		case IDC_PROCESSOR_WATCH_DELETE:
			if(selectedWatchpoint != 0)
			{
				model.DeleteWatchpoint(selectedWatchpoint);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_DELETESTRING, selectedWatchpointListIndex, NULL);
				LRESULT getItemDataReturn = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, selectedWatchpointListIndex, 0);
				if(getItemDataReturn == LB_ERR)
				{
					selectedWatchpoint = 0;
					selectedWatchpointListIndex = -1;
				}
				else
				{
					selectedWatchpoint = (IWatchpoint*)getItemDataReturn;
					if(model.LockWatchpoint(selectedWatchpoint))
					{
						LoadDataFromWatchpoint(selectedWatchpoint);
						model.UnlockWatchpoint(selectedWatchpoint);
					}
				}
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, selectedWatchpointListIndex, NULL);
			}
			else
			{
				ClearWatchpointData();
			}
			UpdateWatchpointFields(hwnd);
			break;
		case IDC_PROCESSOR_WATCH_ENABLEALL:
			for(std::list<IWatchpoint*>::const_iterator i = cachedWatchpointList.begin(); i != cachedWatchpointList.end(); ++i)
			{
				IWatchpoint* watchpoint = *i;
				if(model.LockWatchpoint(watchpoint))
				{
					watchpoint->SetEnabled(true);
					model.UnlockWatchpoint(watchpoint);
				}
			}
			break;
		case IDC_PROCESSOR_WATCH_DISABLEALL:
			for(std::list<IWatchpoint*>::const_iterator i = cachedWatchpointList.begin(); i != cachedWatchpointList.end(); ++i)
			{
				IWatchpoint* watchpoint = *i;
				if(model.LockWatchpoint(watchpoint))
				{
					watchpoint->SetEnabled(false);
					model.UnlockWatchpoint(watchpoint);
				}
			}
			break;
		case IDC_PROCESSOR_WATCH_DELETEALL:
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_RESETCONTENT, 0, NULL);
			for(std::list<IWatchpoint*>::const_iterator i = cachedWatchpointList.begin(); i != cachedWatchpointList.end(); ++i)
			{
				IWatchpoint* watchpoint = *i;
				model.DeleteWatchpoint(watchpoint);
			}

			//Clear the current watchpoint info
			cachedWatchpointList.clear();
			selectedWatchpointListIndex = -1;
			selectedWatchpoint = 0;
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
				selectedWatchpointListIndex = selectedItem;
				selectedWatchpoint = (IWatchpoint*)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, selectedWatchpointListIndex, 0);
				if(model.LockWatchpoint(selectedWatchpoint))
				{
					LoadDataFromWatchpoint(selectedWatchpoint);
					model.UnlockWatchpoint(selectedWatchpoint);
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
	watchpointName = watchpoint->GetName();
	watchpointEnable = watchpoint->GetEnabled();
	watchpointLog = watchpoint->GetLogEvent();
	watchpointBreak = watchpoint->GetBreakEvent();
	watchpointConditionNot = watchpoint->GetLocationConditionNot();
	watchpointOnRead = watchpoint->GetOnRead();
	watchpointOnWrite = watchpoint->GetOnWrite();
	watchpointReadConditionEnabled = watchpoint->GetReadConditionEnabled();
	watchpointReadConditionNot = watchpoint->GetReadConditionNot();
	watchpointWriteConditionEnabled = watchpoint->GetWriteConditionEnabled();
	watchpointWriteConditionNot = watchpoint->GetWriteConditionNot();
	watchpointCondition = watchpoint->GetLocationCondition();
	watchpointReadCondition = watchpoint->GetReadCondition();
	watchpointWriteCondition = watchpoint->GetWriteCondition();
	watchpointLocation1 = watchpoint->GetLocationConditionData1();
	watchpointLocation2 = watchpoint->GetLocationConditionData2();
	watchpointLocationMask = watchpoint->GetLocationMask();
	watchpointReadConditionData1 = watchpoint->GetReadConditionData1();
	watchpointReadConditionData2 = watchpoint->GetReadConditionData2();
	watchpointWriteConditionData1 = watchpoint->GetWriteConditionData1();
	watchpointWriteConditionData2 = watchpoint->GetWriteConditionData2();
	watchpointCounterEnable = watchpoint->GetBreakOnCounter();
	watchpointCounter = watchpoint->GetHitCounter();
	watchpointCounterInterval = watchpoint->GetBreakCounter();
}

//----------------------------------------------------------------------------------------
void WatchpointView::SaveDataToWatchpoint(IWatchpoint* watchpoint)
{
	bool generateName = false;
	if(watchpointName.empty() || (watchpointName == watchpoint->GenerateName()))
	{
		generateName = true;
	}
	watchpoint->SetEnabled(watchpointEnable);
	watchpoint->SetLogEvent(watchpointLog);
	watchpoint->SetBreakEvent(watchpointBreak);
	watchpoint->SetLocationConditionNot(watchpointConditionNot);
	watchpoint->SetOnRead(watchpointOnRead);
	watchpoint->SetOnWrite(watchpointOnWrite);
	watchpoint->SetReadConditionEnabled(watchpointReadConditionEnabled);
	watchpoint->SetReadConditionNot(watchpointReadConditionNot);
	watchpoint->SetWriteConditionEnabled(watchpointWriteConditionEnabled);
	watchpoint->SetWriteConditionNot(watchpointWriteConditionNot);
	watchpoint->SetLocationCondition(watchpointCondition);
	watchpoint->SetReadCondition(watchpointReadCondition);
	watchpoint->SetWriteCondition(watchpointWriteCondition);
	watchpoint->SetLocationConditionData1(watchpointLocation1);
	watchpoint->SetLocationConditionData2(watchpointLocation2);
	watchpoint->SetLocationMask(watchpointLocationMask);
	watchpoint->SetReadConditionData1(watchpointReadConditionData1);
	watchpoint->SetReadConditionData2(watchpointReadConditionData2);
	watchpoint->SetWriteConditionData1(watchpointWriteConditionData1);
	watchpoint->SetWriteConditionData2(watchpointWriteConditionData2);
	watchpoint->SetBreakOnCounter(watchpointCounterEnable);
	watchpoint->SetHitCounter(watchpointCounter);
	watchpoint->SetBreakCounter(watchpointCounterInterval);
	if(generateName)
	{
		watchpointName = watchpoint->GenerateName();
	}
	watchpoint->SetName(watchpointName);
}

//----------------------------------------------------------------------------------------
void WatchpointView::ClearWatchpointData()
{
	watchpointName.clear();
	watchpointEnable = true;
	watchpointLog = true;
	watchpointBreak = true;
	watchpointConditionNot = false;
	watchpointOnRead = true;
	watchpointOnWrite = true;
	watchpointReadConditionEnabled = false;
	watchpointReadConditionNot = false;
	watchpointWriteConditionEnabled = false;
	watchpointWriteConditionNot = false;
	watchpointCondition = IWatchpoint::CONDITION_EQUAL;
	watchpointReadCondition = IWatchpoint::CONDITION_EQUAL;
	watchpointWriteCondition = IWatchpoint::CONDITION_EQUAL;
	watchpointLocation1 = 0;
	watchpointLocation2 = 0;
	watchpointLocationMask = model.GetAddressBusMask();
	watchpointReadConditionData1 = 0;
	watchpointReadConditionData2 = 0;
	watchpointWriteConditionData1 = 0;
	watchpointWriteConditionData2 = 0;
	watchpointCounterEnable = false;
	watchpointCounter = 0;
	watchpointCounterInterval = 1;
}

//----------------------------------------------------------------------------------------
void WatchpointView::UpdateWatchpointFields(HWND hwnd)
{
	unsigned int addressCharWidth = model.GetAddressBusCharWidth();
	unsigned int dataCharWidth = model.GetDataBusCharWidth();
	UpdateDlgItemString(hwnd, IDC_PROCESSOR_WATCH_NAME, watchpointName);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_ENABLE, (watchpointEnable)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_LOG, (watchpointLog)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_BREAK, (watchpointBreak)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_ONREAD, (watchpointOnRead)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_ONWRITE, (watchpointOnWrite)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_BREAKONCOUNTER, (watchpointCounterEnable)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemBin(hwnd, IDC_PROCESSOR_WATCH_BREAKCOUNTER, watchpointCounterInterval);
	UpdateDlgItemBin(hwnd, IDC_PROCESSOR_WATCH_HITCOUNT, watchpointCounter);

	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA1, addressCharWidth, watchpointLocation1);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2, addressCharWidth, watchpointLocation2);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_LOCMASK, addressCharWidth, watchpointLocationMask);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_LOCCONDNOT, (watchpointConditionNot)? BST_CHECKED: BST_UNCHECKED);
	switch(watchpointCondition)
	{
	case IWatchpoint::CONDITION_EQUAL:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND1);
		break;
	case IWatchpoint::CONDITION_GREATER:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND2);
		break;
	case IWatchpoint::CONDITION_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND3);
		break;
	case IWatchpoint::CONDITION_GREATER_AND_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND4);
		break;
	}
	if(watchpointCondition == IWatchpoint::CONDITION_GREATER_AND_LESS)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
	}

	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_READCONDENABLED, (watchpointReadConditionEnabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_READCONDNOT, (watchpointReadConditionNot)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_WRITECONDENABLED, (watchpointWriteConditionEnabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_WRITECONDNOT, (watchpointWriteConditionNot)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA1, dataCharWidth, watchpointReadConditionData1);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2, dataCharWidth, watchpointReadConditionData2);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA1, dataCharWidth, watchpointWriteConditionData1);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2, dataCharWidth, watchpointWriteConditionData2);

	switch(watchpointReadCondition)
	{
	case IWatchpoint::CONDITION_EQUAL:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND1);
		break;
	case IWatchpoint::CONDITION_GREATER:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND2);
		break;
	case IWatchpoint::CONDITION_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND3);
		break;
	case IWatchpoint::CONDITION_GREATER_AND_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND4);
		break;
	}
	if(watchpointReadCondition == IWatchpoint::CONDITION_GREATER_AND_LESS)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
	}

	switch(watchpointWriteCondition)
	{
	case IWatchpoint::CONDITION_EQUAL:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND1);
		break;
	case IWatchpoint::CONDITION_GREATER:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND2);
		break;
	case IWatchpoint::CONDITION_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND3);
		break;
	case IWatchpoint::CONDITION_GREATER_AND_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND4);
		break;
	}
	if(watchpointWriteCondition == IWatchpoint::CONDITION_GREATER_AND_LESS)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
	}
}
