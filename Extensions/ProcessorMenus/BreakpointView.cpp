#include "BreakpointView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
BreakpointView::BreakpointView(IUIManager& auiManager, BreakpointViewPresenter& apresenter, IProcessor& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	selectedBreakpoint = 0;
	selectedBreakpointListIndex = -1;
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_BREAK));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR BreakpointView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR BreakpointView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);
	ClearBreakpointData();
	UpdateBreakpointFields(hwnd);
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR BreakpointView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR BreakpointView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	//Check if the breakpoint list has changed since it was last checked
	std::list<IBreakpoint*> currentBreakpointList = model.GetBreakpointList();
	std::list<IBreakpoint*>::const_iterator curentBreakpointListIterator = currentBreakpointList.begin();
	std::list<IBreakpoint*>::const_iterator cachedBreakpointListIterator = cachedBreakpointList.begin();
	bool breakpointListChanged = currentBreakpointList.size() != cachedBreakpointList.size();
	while(!breakpointListChanged && (curentBreakpointListIterator != currentBreakpointList.end()) && (cachedBreakpointListIterator != cachedBreakpointList.end()))
	{
		if(*curentBreakpointListIterator != *cachedBreakpointListIterator)
		{
			breakpointListChanged = true;
			continue;
		}
		++curentBreakpointListIterator;
		++cachedBreakpointListIterator;
	}

	//Refresh the breakpoint list if it has changed
	if(breakpointListChanged)
	{
		//Save the new breakpoint list
		cachedBreakpointList = currentBreakpointList;

		//Refresh the list of breakpoints
		bool foundSelectedBreakpoint = false;
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), WM_SETREDRAW, FALSE, 0);
		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_GETTOPINDEX, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_RESETCONTENT, 0, NULL);
		for(std::list<IBreakpoint*>::const_iterator i = cachedBreakpointList.begin(); i != cachedBreakpointList.end(); ++i)
		{
			IBreakpoint* breakpoint = *i;
			if(model.LockBreakpoint(breakpoint))
			{
				std::wstring breakpointName = breakpoint->GetName();
				int listIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_ADDSTRING, 0, (LPARAM)breakpointName.c_str());
				if(breakpoint == selectedBreakpoint)
				{
					foundSelectedBreakpoint = true;
					selectedBreakpointListIndex = listIndex;
				}
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_SETITEMDATA, listIndex, (LPARAM)breakpoint);
				model.UnlockBreakpoint(breakpoint);
			}
		}
		if(!foundSelectedBreakpoint && (selectedBreakpointListIndex >= 0))
		{
			selectedBreakpoint = 0;
			selectedBreakpointListIndex = -1;
		}
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_SETCURSEL, (WPARAM)selectedBreakpointListIndex, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_SETTOPINDEX, top, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), NULL, FALSE);
	}

	//If we currently have a selected breakpoint from the list, update its hit counter.
	if(selectedBreakpoint != 0)
	{
		if(model.LockBreakpoint(selectedBreakpoint))
		{
			breakpointCounter = selectedBreakpoint->GetHitCounter();
			UpdateDlgItemBin(hwnd, IDC_PROCESSOR_BREAK_HITCOUNT, breakpointCounter);
			model.UnlockBreakpoint(selectedBreakpoint);
		}
		UpdateBreakpointFields(hwnd);
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR BreakpointView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			case IDC_PROCESSOR_BREAK_NAME:
				breakpointName = GetDlgItemString(hwnd, LOWORD(wparam));
				break;
			case IDC_PROCESSOR_BREAK_LOCCONDDATA1:
				breakpointLocation1 = GetDlgItemHex(hwnd, LOWORD(wparam)) & breakpointLocationMask;
				break;
			case IDC_PROCESSOR_BREAK_LOCCONDDATA2:
				breakpointLocation2 = GetDlgItemHex(hwnd, LOWORD(wparam)) & breakpointLocationMask;
				break;
			case IDC_PROCESSOR_BREAK_LOCMASK:
				breakpointLocationMask = GetDlgItemHex(hwnd, LOWORD(wparam)) & model.GetAddressBusMask();
				break;
			case IDC_PROCESSOR_BREAK_BREAKCOUNTER:
				breakpointCounterInterval = GetDlgItemBin(hwnd, LOWORD(wparam));
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_BREAK_ENABLE:
			breakpointEnable = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_BREAK_LOG:
			breakpointLog = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_BREAK_BREAK:
			breakpointBreak = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_BREAK_BREAKONCOUNTER:
			breakpointCounterEnable = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_BREAK_HITCOUNT_RESET:
			if((selectedBreakpoint != 0) && model.LockBreakpoint(selectedBreakpoint))
			{
				breakpointCounter = 0;
				selectedBreakpoint->SetHitCounter(0);
				model.UnlockBreakpoint(selectedBreakpoint);
			}
			break;

		case IDC_PROCESSOR_BREAK_LOCCONDNOT:
			breakpointConditionNot = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_PROCESSOR_BREAK_LOCCOND1:
			breakpointCondition = IBreakpoint::CONDITION_EQUAL;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_BREAK_LOCCOND2:
			breakpointCondition = IBreakpoint::CONDITION_GREATER;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_BREAK_LOCCOND3:
			breakpointCondition = IBreakpoint::CONDITION_LESS;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_BREAK_LOCCOND4:
			breakpointCondition = IBreakpoint::CONDITION_GREATER_AND_LESS;
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LOCCONDDATA2), TRUE);
			break;

		case IDC_PROCESSOR_BREAK_NEW:
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_SETCURSEL, (WPARAM)-1, NULL);
			selectedBreakpointListIndex = -1;
			selectedBreakpoint = 0;
			ClearBreakpointData();
			break;
		case IDC_PROCESSOR_BREAK_SAVE:
			if(selectedBreakpoint == 0)
			{
				selectedBreakpoint = model.CreateBreakpoint();
			}
			if(model.LockBreakpoint(selectedBreakpoint))
			{
				SaveDataToBreakpoint(selectedBreakpoint);
				model.UnlockBreakpoint(selectedBreakpoint);
			}
			if(selectedBreakpointListIndex != -1)
			{
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_DELETESTRING, selectedBreakpointListIndex, NULL);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_INSERTSTRING, selectedBreakpointListIndex, (LPARAM)breakpointName.c_str());
			}
			else
			{
				selectedBreakpointListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_ADDSTRING, 0, (LPARAM)breakpointName.c_str());
			}
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_SETITEMDATA, selectedBreakpointListIndex, (LPARAM)selectedBreakpoint);
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_SETCURSEL, selectedBreakpointListIndex, NULL);
			break;
		case IDC_PROCESSOR_BREAK_DELETE:
			if(selectedBreakpoint != 0)
			{
				model.DeleteBreakpoint(selectedBreakpoint);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_DELETESTRING, selectedBreakpointListIndex, NULL);
				LRESULT getItemDataReturn = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_GETITEMDATA, selectedBreakpointListIndex, 0);
				if(getItemDataReturn == LB_ERR)
				{
					selectedBreakpoint = 0;
					selectedBreakpointListIndex = -1;
				}
				else
				{
					selectedBreakpoint = (IBreakpoint*)getItemDataReturn;
					if(model.LockBreakpoint(selectedBreakpoint))
					{
						LoadDataFromBreakpoint(selectedBreakpoint);
						model.UnlockBreakpoint(selectedBreakpoint);
					}
				}
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_SETCURSEL, selectedBreakpointListIndex, NULL);
			}
			else
			{
				ClearBreakpointData();
			}
			UpdateBreakpointFields(hwnd);
			break;
		case IDC_PROCESSOR_BREAK_ENABLEALL:
			for(std::list<IBreakpoint*>::const_iterator i = cachedBreakpointList.begin(); i != cachedBreakpointList.end(); ++i)
			{
				IBreakpoint* breakpoint = *i;
				if(model.LockBreakpoint(breakpoint))
				{
					breakpoint->SetEnabled(true);
					model.UnlockBreakpoint(breakpoint);
				}
			}
			break;
		case IDC_PROCESSOR_BREAK_DISABLEALL:
			for(std::list<IBreakpoint*>::const_iterator i = cachedBreakpointList.begin(); i != cachedBreakpointList.end(); ++i)
			{
				IBreakpoint* breakpoint = *i;
				if(model.LockBreakpoint(breakpoint))
				{
					breakpoint->SetEnabled(false);
					model.UnlockBreakpoint(breakpoint);
				}
			}
			break;
		case IDC_PROCESSOR_BREAK_DELETEALL:
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_RESETCONTENT, 0, NULL);
			for(std::list<IBreakpoint*>::const_iterator i = cachedBreakpointList.begin(); i != cachedBreakpointList.end(); ++i)
			{
				IBreakpoint* breakpoint = *i;
				model.DeleteBreakpoint(breakpoint);
			}

			//Clear the current breakpoint info
			cachedBreakpointList.clear();
			selectedBreakpointListIndex = -1;
			selectedBreakpoint = 0;
			ClearBreakpointData();
			UpdateBreakpointFields(hwnd);
			break;
		}
	}
	else if(HIWORD(wparam) == LBN_SELCHANGE)
	{
		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_BREAK_LIST:
			int selectedItem = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_GETCURSEL, 0, NULL);
			if(selectedItem != LB_ERR)
			{
				selectedBreakpointListIndex = selectedItem;
				selectedBreakpoint = (IBreakpoint*)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LIST), LB_GETITEMDATA, selectedBreakpointListIndex, 0);
				if(model.LockBreakpoint(selectedBreakpoint))
				{
					LoadDataFromBreakpoint(selectedBreakpoint);
					model.UnlockBreakpoint(selectedBreakpoint);
				}
				UpdateBreakpointFields(hwnd);
			}
			break;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Breakpoint functions
//----------------------------------------------------------------------------------------
void BreakpointView::LoadDataFromBreakpoint(IBreakpoint* breakpoint)
{
	breakpointName = breakpoint->GetName();
	breakpointCounterInterval = breakpoint->GetBreakCounter();
	breakpointCounter = breakpoint->GetHitCounter();
	breakpointLocation1 = breakpoint->GetLocationConditionData1();
	breakpointLocation2 = breakpoint->GetLocationConditionData2();
	breakpointLocationMask = breakpoint->GetLocationMask();
	breakpointEnable = breakpoint->GetEnabled();
	breakpointLog = breakpoint->GetLogEvent();
	breakpointBreak = breakpoint->GetBreakEvent();
	breakpointCounterEnable = breakpoint->GetBreakOnCounter();
	breakpointConditionNot = breakpoint->GetLocationConditionNot();
	breakpointCondition = breakpoint->GetLocationCondition();
}

//----------------------------------------------------------------------------------------
void BreakpointView::SaveDataToBreakpoint(IBreakpoint* breakpoint)
{
	bool generateName = false;
	if(breakpointName.empty() || (breakpointName == breakpoint->GenerateName()))
	{
		generateName = true;
	}
	breakpoint->SetBreakCounter(breakpointCounterInterval);
	breakpoint->SetHitCounter(breakpointCounter);
	breakpoint->SetLocationConditionData1(breakpointLocation1);
	breakpoint->SetLocationConditionData2(breakpointLocation2);
	breakpoint->SetLocationMask(breakpointLocationMask);
	breakpoint->SetEnabled(breakpointEnable);
	breakpoint->SetLogEvent(breakpointLog);
	breakpoint->SetBreakEvent(breakpointBreak);
	breakpoint->SetBreakOnCounter(breakpointCounterEnable);
	breakpoint->SetLocationConditionNot(breakpointConditionNot);
	breakpoint->SetLocationCondition(breakpointCondition);
	if(generateName)
	{
		breakpointName = breakpoint->GenerateName();
	}
	breakpoint->SetName(breakpointName);
}

//----------------------------------------------------------------------------------------
void BreakpointView::ClearBreakpointData()
{
	breakpointName.clear();
	breakpointCounterInterval = 1;
	breakpointCounter = 0;
	breakpointLocation1 = 0;
	breakpointLocation2 = 0;
	breakpointLocationMask = model.GetAddressBusMask();
	breakpointEnable = true;
	breakpointLog = true;
	breakpointBreak = true;
	breakpointCounterEnable = false;
	breakpointConditionNot = false;
	breakpointCondition = IBreakpoint::CONDITION_EQUAL;
}

//----------------------------------------------------------------------------------------
void BreakpointView::UpdateBreakpointFields(HWND hwnd)
{
	unsigned int addressCharWidth = model.GetAddressBusCharWidth();
	if(currentControlFocus != IDC_PROCESSOR_BREAK_NAME)
	{
		UpdateDlgItemString(hwnd, IDC_PROCESSOR_BREAK_NAME, breakpointName);
	}
	if(currentControlFocus != IDC_PROCESSOR_BREAK_BREAKCOUNTER)
	{
		UpdateDlgItemBin(hwnd, IDC_PROCESSOR_BREAK_BREAKCOUNTER, breakpointCounterInterval);
	}
	if(currentControlFocus != IDC_PROCESSOR_BREAK_HITCOUNT)
	{
		UpdateDlgItemBin(hwnd, IDC_PROCESSOR_BREAK_HITCOUNT, breakpointCounter);
	}
	if(currentControlFocus != IDC_PROCESSOR_BREAK_LOCCONDDATA1)
	{
		UpdateDlgItemHex(hwnd, IDC_PROCESSOR_BREAK_LOCCONDDATA1, addressCharWidth, breakpointLocation1);
	}
	if(currentControlFocus != IDC_PROCESSOR_BREAK_LOCCONDDATA2)
	{
		UpdateDlgItemHex(hwnd, IDC_PROCESSOR_BREAK_LOCCONDDATA2, addressCharWidth, breakpointLocation2);
	}
	if(currentControlFocus != IDC_PROCESSOR_BREAK_LOCMASK)
	{
		UpdateDlgItemHex(hwnd, IDC_PROCESSOR_BREAK_LOCMASK, addressCharWidth, breakpointLocationMask);
	}

	CheckDlgButton(hwnd, IDC_PROCESSOR_BREAK_ENABLE, (breakpointEnable)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_BREAK_LOG, (breakpointLog)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_BREAK_BREAK, (breakpointBreak)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_BREAK_BREAKONCOUNTER, (breakpointCounterEnable)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_BREAK_LOCCONDNOT, (breakpointConditionNot)? BST_CHECKED: BST_UNCHECKED);
	switch(breakpointCondition)
	{
	case IBreakpoint::CONDITION_EQUAL:
		CheckRadioButton(hwnd, IDC_PROCESSOR_BREAK_LOCCOND1, IDC_PROCESSOR_BREAK_LOCCOND4, IDC_PROCESSOR_BREAK_LOCCOND1);
		break;
	case IBreakpoint::CONDITION_GREATER:
		CheckRadioButton(hwnd, IDC_PROCESSOR_BREAK_LOCCOND1, IDC_PROCESSOR_BREAK_LOCCOND4, IDC_PROCESSOR_BREAK_LOCCOND2);
		break;
	case IBreakpoint::CONDITION_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_BREAK_LOCCOND1, IDC_PROCESSOR_BREAK_LOCCOND4, IDC_PROCESSOR_BREAK_LOCCOND3);
		break;
	case IBreakpoint::CONDITION_GREATER_AND_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_BREAK_LOCCOND1, IDC_PROCESSOR_BREAK_LOCCOND4, IDC_PROCESSOR_BREAK_LOCCOND4);
		break;
	}
	if(breakpointCondition == IBreakpoint::CONDITION_GREATER_AND_LESS)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LOCCONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_BREAK_LOCCONDDATA2), FALSE);
	}
}
