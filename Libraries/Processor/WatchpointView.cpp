#include "WatchpointView.h"
#include "resource.h"
#include "DataConversion/DataConversion.pkg"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::WatchpointView::WatchpointView(Processor* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0), watchpoint(adevice->GetAddressBusWidth())
{
	watchpointListIndex = -1;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Watchpoints");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_PROCESSOR_WATCH));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR Processor::WatchpointView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR Processor::WatchpointView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 100, NULL);
	watchpoint.Initialize(device->GetAddressBusWidth());
	UpdateWatchpoint(hwnd, watchpoint, device->GetPCCharWidth(), device->GetDataBusCharWidth());
	watchpointListIndex = -1;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::WatchpointView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::WatchpointView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	boost::mutex::scoped_lock lock(device->debugMutex);
	initializedDialog = true;

	//Check if we need to refresh the watchpoint list
	bool refreshWatchpointList = (watchpointsCopy.size() != device->watchpoints.size());
	if(!refreshWatchpointList)
	{
		WatchpointList::const_iterator watchpointsIterator = device->watchpoints.begin();
		WatchpointList::const_iterator watchpointsCopyIterator = watchpointsCopy.begin();
		while(!refreshWatchpointList && (watchpointsIterator != device->watchpoints.end()) && (watchpointsCopyIterator != watchpointsCopy.end()))
		{
			refreshWatchpointList |= (*watchpointsIterator != *watchpointsCopyIterator);
			++watchpointsIterator;
			++watchpointsCopyIterator;
		}
	}
	watchpointsCopy = device->watchpoints;

	//Refresh the watchpoint list if required
	if(refreshWatchpointList)
	{
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), WM_SETREDRAW, FALSE, 0);

		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETTOPINDEX, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_RESETCONTENT, 0, NULL);
		for(WatchpointList::iterator i = device->watchpoints.begin(); i != device->watchpoints.end(); ++i)
		{
			int listIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_ADDSTRING, 0, (LPARAM)(*i)->GetName().c_str());
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETITEMDATA, listIndex, (LPARAM)*i);
		}
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, (WPARAM)-1, 0);
		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETTOPINDEX, top, 0);

		SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), NULL, FALSE);

		watchpointListIndex = -1;
	}

	//If we currently have a selected watchpoint from the list, update the hit count.
	if(watchpointListIndex >= 0)
	{
		Watchpoint* targetWatchpoint = (Watchpoint*)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, watchpointListIndex, 0);
		UpdateDlgItemBin(hwnd, IDC_PROCESSOR_WATCH_HITCOUNT, targetWatchpoint->GetHitCounter());
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR Processor::WatchpointView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			case IDC_PROCESSOR_WATCH_NAME:
				watchpoint.SetName(GetDlgItemString(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_WATCH_LOCCONDDATA1:{
				bool updateName = false;
				unsigned int watchLocationFromName;
				StringToInt(watchpoint.GetName(), watchLocationFromName);
				if((watchpoint.GetName().length() == 0) || (watchLocationFromName == watchpoint.GetLocationConditionData1()))
				{
					updateName = true;
				}
				watchpoint.SetLocationConditionData1(GetDlgItemHex(hwnd, LOWORD(wparam)));
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetPCCharWidth(), watchpoint.GetLocationConditionData1());
				if(updateName)
				{
					std::wstring watchNameFromLocation;
					IntToStringBase16(watchpoint.GetLocationConditionData1(), watchNameFromLocation, device->GetPCCharWidth(), false);
					watchpoint.SetName(watchNameFromLocation);
					UpdateDlgItemString(hwnd, IDC_PROCESSOR_WATCH_NAME, watchpoint.GetName());
				}
				break;}
			case IDC_PROCESSOR_WATCH_LOCCONDDATA2:
				watchpoint.SetLocationConditionData2(GetDlgItemHex(hwnd, LOWORD(wparam)));
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetPCCharWidth(), watchpoint.GetLocationConditionData2());
				break;
			case IDC_PROCESSOR_WATCH_LOCMASK:
				watchpoint.SetLocationMask(GetDlgItemHex(hwnd, LOWORD(wparam)));
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetPCCharWidth(), watchpoint.GetLocationMask());
				break;
			case IDC_PROCESSOR_WATCH_BREAKCOUNTER:
				watchpoint.SetBreakCounter(GetDlgItemBin(hwnd, LOWORD(wparam)));
				break;
			case IDC_PROCESSOR_WATCH_READCONDDATA1:
				watchpoint.SetReadConditionData1(GetDlgItemHex(hwnd, LOWORD(wparam)), device->GetDataBusWidth());
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetDataBusCharWidth(), watchpoint.GetReadConditionData1());
				break;
			case IDC_PROCESSOR_WATCH_READCONDDATA2:
				watchpoint.SetReadConditionData2(GetDlgItemHex(hwnd, LOWORD(wparam)), device->GetDataBusWidth());
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetDataBusCharWidth(), watchpoint.GetReadConditionData2());
				break;
			case IDC_PROCESSOR_WATCH_WRITECONDDATA1:
				watchpoint.SetWriteConditionData1(GetDlgItemHex(hwnd, LOWORD(wparam)), device->GetDataBusWidth());
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetDataBusCharWidth(), watchpoint.GetWriteConditionData1());
				break;
			case IDC_PROCESSOR_WATCH_WRITECONDDATA2:
				watchpoint.SetWriteConditionData2(GetDlgItemHex(hwnd, LOWORD(wparam)), device->GetDataBusWidth());
				UpdateDlgItemHex(hwnd, LOWORD(wparam), device->GetDataBusCharWidth(), watchpoint.GetWriteConditionData2());
				break;
			}
		}
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		boost::mutex::scoped_lock lock(device->debugMutex);

		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_WATCH_ENABLE:
			watchpoint.SetEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_LOG:
			watchpoint.SetLogEvent(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_BREAK:
			watchpoint.SetBreakEvent(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_ONREAD:
			watchpoint.SetOnRead(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_ONWRITE:
			watchpoint.SetOnWrite(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_BREAKONCOUNTER:
			watchpoint.SetBreakOnCounter(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_HITCOUNT_RESET:
			if(watchpointListIndex != -1)
			{
				Watchpoint* targetWatchpoint = (Watchpoint*)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, watchpointListIndex, 0);
				targetWatchpoint->SetHitCounter(0);
			}
			break;

		case IDC_PROCESSOR_WATCH_LOCCONDNOT:
			watchpoint.SetLocationConditionNot(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND1:
			watchpoint.SetLocationCondition(Watchpoint::CONDITION_EQUAL);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND2:
			watchpoint.SetLocationCondition(Watchpoint::CONDITION_GREATER);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND3:
			watchpoint.SetLocationCondition(Watchpoint::CONDITION_LESS);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_LOCCOND4:
			watchpoint.SetLocationCondition(Watchpoint::CONDITION_GREATER_AND_LESS);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), TRUE);
			break;

		case IDC_PROCESSOR_WATCH_NEW:
			watchpoint.Initialize(device->GetAddressBusWidth());
			UpdateWatchpoint(hwnd, watchpoint, device->GetPCCharWidth(), device->GetDataBusCharWidth());
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, (WPARAM)-1, NULL);
			watchpointListIndex = -1;
			break;
		case IDC_PROCESSOR_WATCH_SAVE:{
			if(watchpoint.GetName().length() == 0)
			{
				std::wstring watchNameFromLocation;
				IntToStringBase16(watchpoint.GetLocationConditionData1(), watchNameFromLocation, device->GetPCCharWidth(), false);
				watchpoint.SetName(watchNameFromLocation);
				UpdateDlgItemString(hwnd, IDC_PROCESSOR_BREAK_NAME, watchpoint.GetName());
			}
			if(watchpointListIndex != -1)
			{
				Watchpoint* targetWatchpoint = (Watchpoint*)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, watchpointListIndex, 0);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_DELETESTRING, watchpointListIndex, NULL);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_INSERTSTRING, watchpointListIndex, (LPARAM)watchpoint.GetName().c_str());
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETITEMDATA, watchpointListIndex, (LPARAM)targetWatchpoint);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, watchpointListIndex, NULL);
				watchpoint.SetHitCounter(targetWatchpoint->GetHitCounter());
				*targetWatchpoint = watchpoint;
			}
			else
			{
				Watchpoint* targetWatchpoint = new Watchpoint(watchpoint);
				device->watchpoints.push_back(targetWatchpoint);
				device->watchpointExists = !device->watchpoints.empty();
				watchpointsCopy = device->watchpoints;

				watchpointListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_ADDSTRING, 0, (LPARAM)watchpoint.GetName().c_str());
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, watchpointListIndex, NULL);
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETITEMDATA, watchpointListIndex, (LPARAM)targetWatchpoint);
			}
			break;}
		case IDC_PROCESSOR_WATCH_DELETE:
			if(watchpointListIndex != -1)
			{
				Watchpoint* targetWatchpoint = (Watchpoint*)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, watchpointListIndex, 0);
				bool done = false;
				WatchpointList::iterator i = device->watchpoints.begin();
				while(!done && (i != device->watchpoints.end()))
				{
					if(*i == targetWatchpoint)
					{
						delete *i;
						device->watchpoints.erase(i);
						done = true;
					}
					else 
					{
						++i;
					}
				}
				device->watchpointExists = !device->watchpoints.empty();
				watchpointsCopy = device->watchpoints;
				SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_DELETESTRING, watchpointListIndex, NULL);
				if(watchpointListIndex >= (int)watchpointsCopy.size())
				{
					watchpointListIndex -= 1;
				}
			}
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_SETCURSEL, watchpointListIndex, NULL);

			//Load the data for the new watchpoint
			if(watchpointListIndex != -1)
			{
				Watchpoint* targetWatchpoint = (Watchpoint*)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, watchpointListIndex, 0);
				watchpoint = *targetWatchpoint;
			}
			else
			{
				watchpoint.Initialize(device->GetAddressBusWidth());
			}
			UpdateWatchpoint(hwnd, watchpoint, device->GetPCCharWidth(), device->GetDataBusCharWidth());
			break;
		case IDC_PROCESSOR_WATCH_ENABLEALL:
			for(WatchpointList::iterator i = device->watchpoints.begin(); i != device->watchpoints.end(); ++i)
			{
				(*i)->SetEnabled(true);
			}
			if(watchpointListIndex != -1)
			{
				watchpoint.SetEnabled(true);
				UpdateWatchpoint(hwnd, watchpoint, device->GetPCCharWidth(), device->GetDataBusCharWidth());
			}
			break;
		case IDC_PROCESSOR_WATCH_DISABLEALL:
			for(WatchpointList::iterator i = device->watchpoints.begin(); i != device->watchpoints.end(); ++i)
			{
				(*i)->SetEnabled(false);
			}
			if(watchpointListIndex != -1)
			{
				watchpoint.SetEnabled(false);
				UpdateWatchpoint(hwnd, watchpoint, device->GetPCCharWidth(), device->GetDataBusCharWidth());
			}
			break;
		case IDC_PROCESSOR_WATCH_DELETEALL:
			SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_RESETCONTENT, 0, NULL);
			for(WatchpointList::iterator i = device->watchpoints.begin(); i != device->watchpoints.end(); ++i)
			{
				delete *i;
			}
			device->watchpoints.clear();
			device->watchpointExists = false;
			watchpointsCopy.clear();

			//Clear the current watchpoint info
			watchpoint.Initialize(device->GetAddressBusWidth());
			UpdateWatchpoint(hwnd, watchpoint, device->GetPCCharWidth(), device->GetDataBusCharWidth());
			watchpointListIndex = -1;
			break;
		case IDC_PROCESSOR_WATCH_READCONDENABLED:
			watchpoint.SetReadConditionEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_READCONDNOT:
			watchpoint.SetReadConditionNot(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_WRITECONDENABLED:
			watchpoint.SetWriteConditionEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_WRITECONDNOT:
			watchpoint.SetWriteConditionNot(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;
		case IDC_PROCESSOR_WATCH_READCOND1:
			watchpoint.SetReadCondition(Watchpoint::CONDITION_EQUAL);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_READCOND2:
			watchpoint.SetReadCondition(Watchpoint::CONDITION_GREATER);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_READCOND3:
			watchpoint.SetReadCondition(Watchpoint::CONDITION_LESS);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_READCOND4:
			watchpoint.SetReadCondition(Watchpoint::CONDITION_GREATER_AND_LESS);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), TRUE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND1:
			watchpoint.SetWriteCondition(Watchpoint::CONDITION_EQUAL);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND2:
			watchpoint.SetWriteCondition(Watchpoint::CONDITION_GREATER);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND3:
			watchpoint.SetWriteCondition(Watchpoint::CONDITION_LESS);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
			break;
		case IDC_PROCESSOR_WATCH_WRITECOND4:
			watchpoint.SetWriteCondition(Watchpoint::CONDITION_GREATER_AND_LESS);
			EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), TRUE);
			break;
		}
	}
	else if(HIWORD(wparam) == LBN_SELCHANGE)
	{
		boost::mutex::scoped_lock lock(device->debugMutex);

		switch(LOWORD(wparam))
		{
		case IDC_PROCESSOR_WATCH_LIST:
			int selectedItem = (int)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETCURSEL, 0, NULL);
			if(selectedItem != LB_ERR)
			{
				watchpointListIndex = selectedItem;
				Watchpoint* targetWatchpoint = (Watchpoint*)SendMessage(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LIST), LB_GETITEMDATA, watchpointListIndex, 0);
				watchpoint = *targetWatchpoint;
				UpdateWatchpoint(hwnd, watchpoint, device->GetPCCharWidth(), device->GetDataBusCharWidth());
			}
			break;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Watchpoint functions
//----------------------------------------------------------------------------------------
void Processor::WatchpointView::UpdateWatchpoint(HWND hwnd, const Watchpoint& watchpoint, unsigned int addressCharWidth, unsigned int dataCharWidth)
{
	UpdateDlgItemString(hwnd, IDC_PROCESSOR_WATCH_NAME, watchpoint.GetName());
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_ENABLE, (watchpoint.GetEnabled())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_LOG, (watchpoint.GetLogEvent())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_BREAK, (watchpoint.GetBreakEvent())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_ONREAD, (watchpoint.GetOnRead())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_ONWRITE, (watchpoint.GetOnWrite())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_BREAKONCOUNTER, (watchpoint.GetBreakOnCounter())? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemBin(hwnd, IDC_PROCESSOR_WATCH_BREAKCOUNTER, watchpoint.GetBreakCounter());
	UpdateDlgItemBin(hwnd, IDC_PROCESSOR_WATCH_HITCOUNT, watchpoint.GetHitCounter());

	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA1, addressCharWidth, watchpoint.GetLocationConditionData1());
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2, addressCharWidth, watchpoint.GetLocationConditionData2());
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_LOCMASK, addressCharWidth, watchpoint.GetLocationMask());
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_LOCCONDNOT, (watchpoint.GetLocationConditionNot())? BST_CHECKED: BST_UNCHECKED);
	switch(watchpoint.GetLocationCondition())
	{
	case Watchpoint::CONDITION_EQUAL:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND1);
		break;
	case Watchpoint::CONDITION_GREATER:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND2);
		break;
	case Watchpoint::CONDITION_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND3);
		break;
	case Watchpoint::CONDITION_GREATER_AND_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_LOCCOND1, IDC_PROCESSOR_WATCH_LOCCOND4, IDC_PROCESSOR_WATCH_LOCCOND4);
		break;
	}
	if(watchpoint.GetLocationCondition() == Watchpoint::CONDITION_GREATER_AND_LESS)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_LOCCONDDATA2), FALSE);
	}

	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_READCONDENABLED, (watchpoint.GetReadConditionEnabled())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_READCONDNOT, (watchpoint.GetReadConditionNot())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_WRITECONDENABLED, (watchpoint.GetWriteConditionEnabled())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_PROCESSOR_WATCH_WRITECONDNOT, (watchpoint.GetWriteConditionNot())? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA1, dataCharWidth, watchpoint.GetReadConditionData1());
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2, dataCharWidth, watchpoint.GetReadConditionData2());
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA1, dataCharWidth, watchpoint.GetWriteConditionData1());
	UpdateDlgItemHex(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2, dataCharWidth, watchpoint.GetWriteConditionData2());

	switch(watchpoint.GetReadCondition())
	{
	case Watchpoint::CONDITION_EQUAL:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND1);
		break;
	case Watchpoint::CONDITION_GREATER:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND2);
		break;
	case Watchpoint::CONDITION_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND3);
		break;
	case Watchpoint::CONDITION_GREATER_AND_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_READCOND1, IDC_PROCESSOR_WATCH_READCOND4, IDC_PROCESSOR_WATCH_READCOND4);
		break;
	}
	if(watchpoint.GetReadCondition() == Watchpoint::CONDITION_GREATER_AND_LESS)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_READCONDDATA2), FALSE);
	}

	switch(watchpoint.GetWriteCondition())
	{
	case Watchpoint::CONDITION_EQUAL:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND1);
		break;
	case Watchpoint::CONDITION_GREATER:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND2);
		break;
	case Watchpoint::CONDITION_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND3);
		break;
	case Watchpoint::CONDITION_GREATER_AND_LESS:
		CheckRadioButton(hwnd, IDC_PROCESSOR_WATCH_WRITECOND1, IDC_PROCESSOR_WATCH_WRITECOND4, IDC_PROCESSOR_WATCH_WRITECOND4);
		break;
	}
	if(watchpoint.GetWriteCondition() == Watchpoint::CONDITION_GREATER_AND_LESS)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd, IDC_PROCESSOR_WATCH_WRITECONDDATA2), FALSE);
	}
}
