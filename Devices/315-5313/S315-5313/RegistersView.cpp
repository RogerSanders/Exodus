#include "RegistersView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::RegistersView::RegistersView(S315_5313* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0), activeTabDialog(NULL)
{
	lockedColor = RGB(255,127,127);
	lockedBrush = CreateSolidBrush(lockedColor);
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Registers");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_REGISTERS));
}

//----------------------------------------------------------------------------------------
S315_5313::RegistersView::~RegistersView()
{
	DeleteObject(lockedBrush);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_NOTIFY:
		return msgWM_NOTIFY(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Add our set of tab items to the list of tabs
	tabItems.push_back(TabInfo(L"Raw Registers", IDD_VDP_REGISTERS_RAWREGISTERS, WndProcRawRegistersStatic));
	tabItems.push_back(TabInfo(L"Mode Registers", IDD_VDP_REGISTERS_MODEREGISTERS, WndProcModeRegistersStatic));
	tabItems.push_back(TabInfo(L"Other Registers", IDD_VDP_REGISTERS_OTHERREGISTERS, WndProcOtherRegistersStatic));

	//Insert our tabs into the tab control
	for(unsigned int i = 0; i < (unsigned int)tabItems.size(); ++i)
	{
		TCITEM tabItem;
		tabItem.mask = TCIF_TEXT;
		tabItem.pszText = (LPWSTR)tabItems[i].tabName.c_str();
		SendMessage(GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), TCM_INSERTITEM, i, (LPARAM)&tabItem);
	}

	//Calculate the required size of the client area of the tab control to fit the largest
	//tab dialog
	int requiredTabClientWidth = 0;
	int requiredTabClientHeight = 0;
	for(unsigned int i = 0; i < (unsigned int)tabItems.size(); ++i)
	{
		//Obtain a handle to the target dialog resource
		HRSRC dialogResourceHandle = FindResource((HMODULE)device->GetAssemblyHandle(), MAKEINTRESOURCE(tabItems[i].dialogID), RT_DIALOG);
		if(dialogResourceHandle == NULL)
		{
			continue;
		}

		//Obtain a handle to the data block associated with the target dialog resource
		HGLOBAL dialogResourceDataHandle = LoadResource((HMODULE)device->GetAssemblyHandle(), dialogResourceHandle);
		if(dialogResourceDataHandle == NULL)
		{
			continue;
		}

		//Load the dialog template into memory
		DLGTEMPLATEEX* dialogTemplate = (DLGTEMPLATEEX*)LockResource(dialogResourceDataHandle);
		if(dialogTemplate == NULL)
		{
			continue;
		}

		//Calculate the required size of the dialog mapped to this tab in pixel units
		RECT tabRect;
		tabRect.left = 0;
		tabRect.top = 0;
		tabRect.right = dialogTemplate->cx;
		tabRect.bottom = dialogTemplate->cy;
		if(MapDialogRect(hwnd, &tabRect) == 0)
		{
			continue;
		}
		int tabWidth = tabRect.right;
		int tabHeight = tabRect.bottom;

		//Increase the required size of the client area for the tab control to accommodate
		//the contents of this tab, if required.
		requiredTabClientWidth = (tabWidth > requiredTabClientWidth)? tabWidth: requiredTabClientWidth;
		requiredTabClientHeight = (tabHeight > requiredTabClientHeight)? tabHeight: requiredTabClientHeight;
	}

	//Save the original size of the tab control
	RECT tabControlOriginalRect;
	GetClientRect(GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), &tabControlOriginalRect);
	int tabControlOriginalSizeX = tabControlOriginalRect.right - tabControlOriginalRect.left;
	int tabControlOriginalSizeY = tabControlOriginalRect.bottom - tabControlOriginalRect.top;

	//Calculate the exact required pixel size of the tab control to fully display the
	//content in each tab
	RECT tabControlRect;
	tabControlRect.left = 0;
	tabControlRect.top = 0;
	tabControlRect.right = requiredTabClientWidth;
	tabControlRect.bottom = requiredTabClientHeight;
	SendMessage(GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), TCM_ADJUSTRECT, (WPARAM)TRUE, (LPARAM)&tabControlRect);
	int tabControlRequiredSizeX = tabControlRect.right - tabControlRect.left;
	int tabControlRequiredSizeY = tabControlRect.bottom - tabControlRect.top;

	//Resize the tab control
	SetWindowPos(GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), NULL, 0, 0, tabControlRequiredSizeX, tabControlRequiredSizeY, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);

	//Calculate the current size of the owning window
	RECT mainDialogRect;
	GetWindowRect(hwnd, &mainDialogRect);
	int currentMainDialogWidth = mainDialogRect.right - mainDialogRect.left;
	int currentMainDialogHeight = mainDialogRect.bottom - mainDialogRect.top;

	//Resize the owning window to the required size
	int newMainDialogWidth = currentMainDialogWidth + (tabControlRequiredSizeX - tabControlOriginalSizeX);
	int newMainDialogHeight = currentMainDialogHeight + (tabControlRequiredSizeY - tabControlOriginalSizeY);
	SetWindowPos(hwnd, NULL, 0, 0, newMainDialogWidth, newMainDialogHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);

	//Explicitly select the first tab. We need to do this, otherwise no dialog is shown
	//within the tab control initially. Note that we also explicitly select the second tab
	//first, then switch to the first tab. We do this because the first tab will be
	//selected initially, but no selection change notification will have been sent, and
	//the notification is only sent if the currently selected tab is different from the
	//target tab.
	SendMessage(GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), TCM_SETCURFOCUS, (WPARAM)1, 0);
	SendMessage(GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), TCM_SETCURFOCUS, (WPARAM)0, 0);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Note that we need to explicitly destroy the child window here, since we share state
	//with the child window, passing in the "this" pointer as its state. Since the
	//destructor for our state may be called anytime after this window is destroyed, and
	//this window is fully destroyed before child windows are destroyed, we need to
	//explicitly destroy the child window here. The child window is fully destroyed before
	//the DestroyWindow() function returns, and our state is still valid until we return
	//from handling this WM_DESTROY message.
	if(activeTabDialog != NULL)
	{
		DestroyWindow(activeTabDialog);
		activeTabDialog = NULL;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgWM_NOTIFY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	NMHDR* nmhdr = (NMHDR*)lparam;
	if(nmhdr->idFrom == IDC_VDP_REGISTERS_TABCONTROL)
	{
		if((nmhdr->code == TCN_SELCHANGE))
		{
			//If we currently have a dialog already associated with the tab control,
			//destroy it.
			if(activeTabDialog != NULL)
			{
				DestroyWindow(activeTabDialog);
				activeTabDialog = NULL;
				initializedDialog = false;
			}

			//Create a new dialog for the currently selected tab on the tab control
			int currentlySelectedTab = (int)SendMessage(nmhdr->hwndFrom, TCM_GETCURSEL, 0, 0);
			if((currentlySelectedTab < 0) || (currentlySelectedTab >= (int)tabItems.size()))
			{
				currentlySelectedTab = 0;
			}
			DLGPROC dialogWindowProc = tabItems[currentlySelectedTab].dialogProc;
			LPCWSTR dialogTemplateName = MAKEINTRESOURCE(tabItems[currentlySelectedTab].dialogID);
			activeTabDialog = CreateDialogParam((HINSTANCE)device->GetAssemblyHandle(), dialogTemplateName, GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), dialogWindowProc, (LPARAM)this);

			//Position and size the dialog contained within this tab
			RECT currentTabControlRect;
			if(GetWindowRect(GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), &currentTabControlRect) != 0)
			{
				//Calculate the required pixel size and position of the dialog linked with
				//this tab
				SendMessage(GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), TCM_ADJUSTRECT, (WPARAM)FALSE, (LPARAM)&currentTabControlRect);
				POINT tabContentPoint;
				tabContentPoint.x = currentTabControlRect.left;
				tabContentPoint.y = currentTabControlRect.top;
				ScreenToClient(GetDlgItem(hwnd, IDC_VDP_REGISTERS_TABCONTROL), &tabContentPoint);
				int tabRequiredPosX = tabContentPoint.x;
				int tabRequiredPosY = tabContentPoint.y;
				int tabRequiredSizeX = currentTabControlRect.right - currentTabControlRect.left;
				int tabRequiredSizeY = currentTabControlRect.bottom - currentTabControlRect.top;

				//Position and size the dialog contained within this tab
				SetWindowPos(activeTabDialog, NULL, tabRequiredPosX, tabRequiredPosY, tabRequiredSizeX, tabRequiredSizeY, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			}

			//Show the tab window
			ShowWindow(activeTabDialog, SW_SHOW);
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
//Raw registers dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK S315_5313::RegistersView::WndProcRawRegistersStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	RegistersView* state = (RegistersView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (RegistersView*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcRawRegisters(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			INT_PTR result = state->WndProcRawRegisters(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = FALSE;
	if(state != 0)
	{
		result = state->WndProcRawRegisters(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::WndProcRawRegisters(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgRawRegistersWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgRawRegistersWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgRawRegistersWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgRawRegistersWM_COMMAND(hwnd, wparam, lparam);
	case WM_BOUNCE:
		return msgRawRegistersWM_BOUNCE(hwnd, wparam, lparam);
	case WM_CTLCOLOREDIT:
		return msgRawRegistersWM_CTLCOLOREDIT(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Raw registers dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgRawRegistersWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Tooltip messages
	const std::wstring lockingTooltip = 
		L"Selected controls on this window support register locking. "
		L"Register locking allows the current value of a register to "
		L"be frozen, preventing changes from within the system. "
		L"Attempts by any device to modify locked registers will be "
		L"ignored. The current value of locked registers can still be "
		L"modified through the debugger. To toggle locking for a "
		L"register, hold the CTRL key while clicking on a control. "
		L"Controls that support locking will change colour to indicate "
		L"when they are locked.";

	//Create tooltips for the window
	HWND hwndTooltip = CreateTooltipControl((HINSTANCE)device->GetAssemblyHandle(), hwnd);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_LOCKING_TT, lockingTooltip, true);

	//Enable system message bounce-back for controls which can be locked
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_0), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_5), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_6), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_7), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_8), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_9), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_10), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_11), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_12), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_13), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_14), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_15), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_16), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_17), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_18), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_19), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_20), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_21), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_22), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_REG_23), BounceBackSubclassProc, 0, 0);

	//Create a timer to refresh the register contents
	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgRawRegistersWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgRawRegistersWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

	//Update raw registers
	for(unsigned int i = 0; i < registerCount; ++i)
	{
		if(currentControlFocus != (IDC_REG_0 + i))	UpdateDlgItemHex(hwnd, IDC_REG_0 + i, 2, device->GetRegisterData(i, accessTarget).GetData());
	}

	//Port registers
	if(currentControlFocus != IDC_CODE)	UpdateDlgItemHex(hwnd, IDC_CODE, 2, device->commandCode.GetData());
	if(currentControlFocus != IDC_ADDRESS)	UpdateDlgItemHex(hwnd, IDC_ADDRESS, 5, device->commandAddress.GetData());
	CheckDlgButton(hwnd, IDC_WRITEPENDING, (device->commandWritePending)? BST_CHECKED: BST_UNCHECKED);

	//Interrupt registers
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_VINTPENDING, (device->vintPending)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_HINTPENDING, (device->hintPending)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_EXINTPENDING, (device->exintPending)? BST_CHECKED: BST_UNCHECKED);

	//FIFO registers
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_CODE1) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_CODE1, 2, device->fifoBuffer[0].codeRegData.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS1) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS1, 5, device->fifoBuffer[0].addressRegData.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_DATA1) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_DATA1, 4, device->fifoBuffer[0].dataPortWriteData.GetData());
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_WRITEPENDING1, (device->fifoBuffer[0].pendingDataWrite)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_HALFWRITTEN1, (device->fifoBuffer[0].dataWriteHalfWritten)? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_CODE2) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_CODE2, 2, device->fifoBuffer[1].codeRegData.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS2) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS2, 5, device->fifoBuffer[1].addressRegData.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_DATA2) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_DATA2, 4, device->fifoBuffer[1].dataPortWriteData.GetData());
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_WRITEPENDING1, (device->fifoBuffer[1].pendingDataWrite)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_HALFWRITTEN1, (device->fifoBuffer[1].dataWriteHalfWritten)? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_CODE3) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_CODE3, 2, device->fifoBuffer[2].codeRegData.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS3) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS3, 5, device->fifoBuffer[2].addressRegData.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_DATA3) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_DATA3, 4, device->fifoBuffer[2].dataPortWriteData.GetData());
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_WRITEPENDING1, (device->fifoBuffer[2].pendingDataWrite)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_HALFWRITTEN1, (device->fifoBuffer[2].dataWriteHalfWritten)? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_CODE4) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_CODE4, 2, device->fifoBuffer[3].codeRegData.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS4) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS4, 5, device->fifoBuffer[3].addressRegData.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_FIFOBUFFER_DATA4) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_DATA4, 4, device->fifoBuffer[3].dataPortWriteData.GetData());
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_WRITEPENDING1, (device->fifoBuffer[3].pendingDataWrite)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_FIFOBUFFER_HALFWRITTEN1, (device->fifoBuffer[3].dataWriteHalfWritten)? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_VDP_REGISTERS_NEXTFIFOREADENTRY) UpdateDlgItemBin(hwnd, IDC_VDP_REGISTERS_NEXTFIFOREADENTRY, device->fifoNextReadEntry);
	if(currentControlFocus != IDC_VDP_REGISTERS_NEXTFIFOWRITEENTRY) UpdateDlgItemBin(hwnd, IDC_VDP_REGISTERS_NEXTFIFOWRITEENTRY, device->fifoNextWriteEntry);
	if(currentControlFocus != IDC_VDP_REGISTERS_READBUFFER) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_READBUFFER, 4, device->readBuffer.GetData());
	CheckDlgButton(hwnd, IDC_VDP_SETTINGS_READDATAHALFCACHED, (device->readDataHalfCached)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_SETTINGS_READDATAFULLYCACHED, (device->readDataAvailable)? BST_CHECKED: BST_UNCHECKED);

	//Status and HV counter registers
	if(currentControlFocus != IDC_STATUSREGISTER) UpdateDlgItemHex(hwnd, IDC_STATUSREGISTER, 4, device->status.GetData());
	if(currentControlFocus != IDC_HVCOUNTER) UpdateDlgItemHex(hwnd, IDC_HVCOUNTER, 4, device->GetHVCounter().GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_INTERNALCOUNTERH) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_INTERNALCOUNTERH, 4, device->hcounter.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_INTERNALCOUNTERV) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_INTERNALCOUNTERV, 4, device->vcounter.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_LATCHEDCOUNTERH) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_LATCHEDCOUNTERH, 4, device->hcounterLatchedData.GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_LATCHEDCOUNTERV) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_LATCHEDCOUNTERV, 4, device->vcounterLatchedData.GetData());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgRawRegistersWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		//##TODO## Locking renderThreadMutex here is a bit heavy handed. See if we can use
		//a different locking scheme which will allow a bit more decoupling.
		boost::mutex::scoped_lock renderLock(device->renderThreadMutex);
		boost::mutex::scoped_lock accessLock(device->accessMutex);

		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		//Port registers
		case IDC_WRITEPENDING:
			device->commandWritePending = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;

		//Interrupt registers
		case IDC_VDP_REGISTERS_VINTPENDING:
			device->vintPending = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_HINTPENDING:
			device->hintPending = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_EXINTPENDING:
			device->exintPending = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;

		//FIFO registers
		case IDC_VDP_REGISTERS_FIFOBUFFER_WRITEPENDING1:
		case IDC_VDP_REGISTERS_FIFOBUFFER_WRITEPENDING2:
		case IDC_VDP_REGISTERS_FIFOBUFFER_WRITEPENDING3:
		case IDC_VDP_REGISTERS_FIFOBUFFER_WRITEPENDING4:{
			unsigned int fifoIndex = (unsigned int)(controlID - IDC_VDP_REGISTERS_FIFOBUFFER_WRITEPENDING1);
			device->fifoBuffer[fifoIndex].pendingDataWrite = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;}
		case IDC_VDP_REGISTERS_FIFOBUFFER_HALFWRITTEN1:
		case IDC_VDP_REGISTERS_FIFOBUFFER_HALFWRITTEN2:
		case IDC_VDP_REGISTERS_FIFOBUFFER_HALFWRITTEN3:
		case IDC_VDP_REGISTERS_FIFOBUFFER_HALFWRITTEN4:{
			unsigned int fifoIndex = (unsigned int)(controlID - IDC_VDP_REGISTERS_FIFOBUFFER_HALFWRITTEN1);
			device->fifoBuffer[fifoIndex].dataWriteHalfWritten = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;}
		case IDC_VDP_SETTINGS_READDATAHALFCACHED:
			device->readDataHalfCached = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_SETTINGS_READDATAFULLYCACHED:
			device->readDataAvailable = (IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		//##TODO## Locking renderThreadMutex here is a bit heavy handed. See if we can use
		//a different locking scheme which will allow a bit more decoupling.
		boost::mutex::scoped_lock renderLock(device->renderThreadMutex);
		boost::mutex::scoped_lock accessLock(device->accessMutex);

		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			AccessTarget accessTarget;
			accessTarget.AccessLatest();

			//Raw registers
			if((LOWORD(wparam) >= IDC_REG_0) && (LOWORD(wparam) < (IDC_REG_0 + registerCount)))
			{
				unsigned int registerNo = LOWORD(wparam) - IDC_REG_0;
				Data newData(8, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->SetRegisterData(registerNo, accessTarget, newData);
				device->TransparentRegisterSpecialUpdateFunction(registerNo, newData);
			}
			else switch(LOWORD(wparam))
			{
			//Port registers
			case IDC_CODE:
				device->commandCode.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_ADDRESS:
				device->commandAddress.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

			//FIFO registers
			case IDC_VDP_REGISTERS_FIFOBUFFER_CODE1:
				device->fifoBuffer[0].codeRegData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS1:
				device->fifoBuffer[0].addressRegData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_DATA1:
				device->fifoBuffer[0].dataPortWriteData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_CODE2:
				device->fifoBuffer[1].codeRegData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS2:
				device->fifoBuffer[1].addressRegData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_DATA2:
				device->fifoBuffer[1].dataPortWriteData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_CODE3:
				device->fifoBuffer[2].codeRegData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS3:
				device->fifoBuffer[2].addressRegData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_DATA3:
				device->fifoBuffer[2].dataPortWriteData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_CODE4:
				device->fifoBuffer[3].codeRegData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_ADDRESS4:
				device->fifoBuffer[3].addressRegData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_FIFOBUFFER_DATA4:
				device->fifoBuffer[3].dataPortWriteData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_READBUFFER:
				device->readBuffer.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

			//Status and HV counter registers
			case IDC_STATUSREGISTER:
				device->status.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_INTERNALCOUNTERH:
				device->hcounter.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_INTERNALCOUNTERV:
				device->vcounter.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_LATCHEDCOUNTERH:
				device->hcounterLatchedData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_LATCHEDCOUNTERV:
				device->vcounterLatchedData.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgRawRegistersWM_BOUNCE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lparam;
	int controlID = GetDlgCtrlID(bounceMessage->hwnd);
	switch(bounceMessage->uMsg)
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		if((bounceMessage->wParam & MK_CONTROL) != 0)
		{
			//If the user has control+clicked a control which supports locking, toggle
			//the lock state of the target register.
			boost::mutex::scoped_lock lock(device->accessMutex);
			ToggleRegisterLock(controlID);

			//Force the control to redraw when the lock state is toggled
			InvalidateRect(bounceMessage->hwnd, NULL, FALSE);

			bounceMessage->SetResult(TRUE);
		}
		break;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgRawRegistersWM_CTLCOLOREDIT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Handle background colour changes for edit controls which are locked
	unsigned int registerNo = ControlIDToLockedRegKey(GetDlgCtrlID((HWND)lparam));
	if((registerNo < registerCount) && device->registerLocked[registerNo])
	{
		SetBkColor((HDC)wparam, lockedColor);
		return (BOOL)HandleToLong(lockedBrush);
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------
//Mode registers dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK S315_5313::RegistersView::WndProcModeRegistersStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	RegistersView* state = (RegistersView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (RegistersView*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcModeRegisters(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			INT_PTR result = state->WndProcModeRegisters(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = FALSE;
	if(state != 0)
	{
		result = state->WndProcModeRegisters(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::WndProcModeRegisters(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgModeRegistersWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgModeRegistersWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgModeRegistersWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgModeRegistersWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Mode registers dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgModeRegistersWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Tooltip messages
	const std::wstring tooltipMessageVSI = 
		L"Vertical Scroll Inhibit. Disables vertical scrolling for columns 24-31. This "
		L"setting from the SMS VDP is still present in the Mega Drive VDP, even when mode 5 "
		L"is active. Enabling this bit stops the rightmost 8 cells (4 2-cell blocks) from "
		L"being scrolled in H32 mode. When H40 mode is activated, the exact same blocks "
		L"are still fixed and non-scrolling, however, the rest of the blocks after that "
		L"point are still allowed to scroll, IE, this setting does not inhibit vertical "
		L"scrolling for columns 32 and onwards. It should also be noted that vertical "
		L"scrolling isn't totally disabled for this region, but rather, only the highest "
		L"possible active bit of vertical scroll data affects the blocks. If the scrolling "
		L"region size is set to H32 V128, the effective scroll value jumps from 0x000 to "
		L"0x200 when the scroll value places a block from the upper 64 cells at the top of "
		L"the screen. Also note that hardware tests have shown this register setting only "
		L"affects layer A, not layer B. Hardware tests have also shown that horizontal "
		L"scrolling affects the locked region. Where both horizontal and vertical scrolling "
		L"are applied to this region, the region itself does scroll horizontally, up to the "
		L"next 2-cell boundary, at which point, the fixed vscroll region jumps back. Also "
		L"note that having the window layer active has no impact on the fixed vscroll region "
		L"at all, and the fixed vscroll region has no effect on the window. Both interact "
		L"exactly as layer A and window regions normally would.";
	const std::wstring tooltipMessageHSI = 
		L"Horizontal Scroll Inhibit. Disable horizontal scrolling for rows 0-1. This "
		L"register only has an effect when mode 4 is active. When mode 5 is enabled, this "
		L"register has no apparent effect.";
	const std::wstring tooltipMessageLCB = 
		L"Left Column Blank. Mask column 0. This setting from the SMS VDP is still present "
		L"in the Mega Drive VDP, even when mode 5 is active. This setting simply does what "
		L"it says, which is to mask the first 8-cell column of the active display. Instead "
		L"of displaying pattern data in this cell, the background layer is displayed "
		L"instead. This setting affects layers A and B and presumably sprites as well, "
		L"since it just discards any pixel data within the masked region at the time layer "
		L"priority selection is performed.";
	const std::wstring tooltipMessageIE1 = 
		L"Horizontal interrupt enable. When this register is set, a horizontal interrupt is "
		L"generated when a horizontal interrupt is flagged as pending. Horizontal interrupts "
		L"are flagged as pending using the line counter register.";
	const std::wstring tooltipMessageSS = 
		L"Sprite Shift in mode 4, and HSync Mode in mode 5. When this register is set in "
		L"mode 4, all sprites are shifted left by 8 pixels. When this register is set in "
		L"mode 5, the output mode of the HSYNC pin is altered. Instead of the HSYNC line "
		L"being asserted when the horizontal sync region begins and negated when it ends, "
		L"the HSYNC line is instead toggled at the start of each HSYNC region. When HSYNC "
		L"would normally be negated, nothing happens, and the HSYNC line retains its current "
		L"value until the next HSYNC region is reached, at which point, the line output state "
		L"is reversed. Note that the CSYNC line is unaffected by this setting, which means "
		L"that video output on the Mega Drive is unaffected in H32 mode, but if this bit is "
		L"set when the EDCLK input is being used, as it should be in H40 mode on the Mega "
		L"Drive, the video signal will be corrupted, as the EDCLK generator relies on the "
		L"HSYNC signal in order to generate the input clock signal.";
	const std::wstring tooltipMessagePS = 
		L"Palette Select. When this register is cleared, it enables an undocumented mode "
		L"where only bits 1, 5, and 9 of the CRAM data are used to generate colours. There "
		L"may be an alternate purpose for this register which is currently unknown.";
	const std::wstring tooltipMessageM2 = 
		L"Enable HV counter latching on an external interrupt. Note that hardware tests have "
		L"shown that as soon as this bit is set to 1, the HV counter is actually frozen at "
		L"that point. The HV counter will latch a new value if the HL input line is asserted "
		L"after this point, otherwise, it will retain its current value until this bit is "
		L"cleared.";
	const std::wstring tooltipMessageES = 
		L"Setting this bit to 1 appears to disable the C-SYNC output signal. Instead of "
		L"outputting the CSYNC signal encoding the HSYNC and VSYNC signals together, the "
		L"CSYNC output pin is always held high. The HSYNC and VSYNC lines, along with the "
		L"general operation of the VDP, appear otherwise unaffected. Note however that a "
		L"CSYNC signal is required on the Mega Drive in order to generate a valid video "
		L"signal. This means setting this bit effectively removes all sync information from "
		L"the video signal, preventing most monitors from locking onto it and displaying it "
		L"correctly. The \"Software Reference Manual for the Sega Mark III Console\" lists "
		L"this register as \"ES\", with the only description of it being that it enables an "
		L"\"External Sync\" setting. This may indicate there's some way for an external device "
		L"to provide a sync input line to the VDP, where this input signal is output instead "
		L"of the internally generated CSYNC signal when this register is set to 1. Note that "
		L"this input pin may no longer exist on the 315-5313 VDP in the Mega Drive, but if "
		L"this feature exists, it should be present in the 315-5124 VDP used in the Mark III "
		L"and the original SMS system.";
	const std::wstring tooltipMessageEVRAM = 
		L"Extended VRAM. Enables the extended 128Kb VRAM mode. This is unavailable in the "
		L"Mega Drive, as only 64Kb of VRAM is present in the system. The only known systems "
		L"to contain 128Kb of VRAM are the Sega TeraDrive, and according to documentation, "
		L"the \"Super Mega Drive\" development system produced by Sega. When extended VRAM "
		L"mode is activated on a system with only 64Kb of VRAM, the VDP won't function "
		L"correctly, as the additional 64Kb of VRAM is interleaved with the lower 64Kb "
		L"bank when this mode is enabled.";
	const std::wstring tooltipMessageDISP = 
		L"Display enable. If this bit is cleared, the entire display is filled with the "
		L"backdrop colour. During this time, the VDP can be accessed at any time without "
		L"having to wait. Changing this bit takes effect immediately, and it can be "
		L"toggled mid-line. Note that although not officially documented, hardware tests "
		L"have confirmed that the vblank flag in the status register is always set while "
		L"the display is disabled, even during active scan.";
	const std::wstring tooltipMessageIE0 = 
		L"Vertical interrupt enable. When this register is set, a vertical interrupt is "
		L"generated when a vertical interrupt is flagged as pending. Note that if vertical "
		L"interrupts are disabled, a pending vertical interrupt will remain flagged, and "
		L"a vertical interrupt will immediately be generated when this register is set. "
		L"Also note that this register only affects vertical interrupt generation over the "
		L"IPL lines. The separate INT line, which is connected to the Z80 in the Mega "
		L"Drive, always has a vertical interrupt generated, regardless of the state of "
		L"this register.";
	const std::wstring tooltipMessageM1 = 
		L"DMA Enable. When this bit is set, DMA operations can be performed by writing "
		L"correctly formed commands to the control port. Without this bit set, DMA "
		L"operations are not initiated.";
	const std::wstring tooltipMessageM3 = 
		L"V30 mode enable. When this bit is set, and if the VDP is running in PAL mode, "
		L"30 cell rows are displayed on the screen, rather than the usual 28. This mode "
		L"only works correctly under PAL mode. In NTSC mode, there are not enough vertical "
		L"border lines available to make V30 mode work, and a corrupted display is "
		L"produced.";
	const std::wstring tooltipMessageM5 = 
		L"Mode 5 enable. When this bit is set, mode 5 is activated. When this bit is "
		L"cleared, mode 4 is activated. Mode 4 support is provided for backwards "
		L"compatibility with the 315-5124 VDP used in the Master System.";
	const std::wstring tooltipMessageSZ = 
		L"Sprite size, under mode 4 only. When mode 4 is enabled, setting this register "
		L"makes all sprites 8x16 pixels, instead of the usual 8x8 pixels. This register "
		L"has no apparent effect under mode 5.";
	const std::wstring tooltipMessageMAG = 
		L"Sprite zooming, under mode 4 only. This register actually has no effect on the "
		L"315-5313 VDP. On the 315-5124 VDP, when mode 4 was activated, setting this "
		L"register would make all sprites double their regular width and height, so a "
		L"single sprite pixel would occupy a region of 2x2 pixels. This setting is not "
		L"supported on the 315-5313 VDP, but this register is reserved under mode 4 for "
		L"backwards compatibility. This register has no known function under mode 5.";
	const std::wstring tooltipMessage0B7 = 
		L"Unknown. Mentioned by Charles MacDonald in c2tech.txt, described as follows: "
		L"\"VDP controls color bus. Reading or writing color RAM at any address only "
		L"affects address zero, and the data read/written will often be corrupted.\"";
	const std::wstring tooltipMessage0B6 = 
		L"Unknown. Mentioned in Accolade doc \"The Sega Development System\". Description was "
		L"\"Unknown - causes halt sometimes\". Hardware tests have shown that setting this bit "
		L"to 1 can indeed cause some kind of lockup. Just setting the register value to 1 "
		L"and trying to set it back again to 0 seems to lock up the system. If the enable "
		L"and clear of this register value happen as two halves of a long-word register "
		L"write however, no lockup occurs. The VDP doesn't request the bus, so it's not "
		L"clear what causes the lockup at this point. Additional tests required. Note that "
		L"this bit is also mentioned in c2tech.txt, also described as locking up the "
		L"hardware when set.";
	const std::wstring tooltipMessage0B5 = 
		L"Unknown. This register has no known function.";
	const std::wstring tooltipMessage0B4 = 
		L"Unknown. This register has no known function.";
	const std::wstring tooltipMessageIE2 = 
		L"External interrupt enable. When this register is set, an external interrupt is "
		L"generated when an external interrupt is flagged as pending.";
	const std::wstring tooltipMessageVSCR = 
		L"Vertical scroll mode. Full screen vertical scrolling when clear, 2-cell column "
		L"vertical scrolling when set.";
	const std::wstring tooltipMessageHSCR = 
		L"Horizontal scroll mode. If LSCR is not set, HSCR toggles between cell-based "
		L"horizontal scrolling when set, or full screen horizontal scrolling when clear.";
	const std::wstring tooltipMessageLSCR = 
		L"Enables line-based horizontal scrolling. When this bit is set, the state of HSCR "
		L"is ignored.";
	const std::wstring tooltipMessageRS0 = 
		L"When set, the VDP uses the EDCLK input to drive SC directly. When clear, the VDP "
		L"calculates SC internally, as either MCLK/5 or MCLK/4, depending on the state of "
		L"the RS1 bit.";
	const std::wstring tooltipMessageU1 = 
		L"When this bit is set, it causes the VSYNC pin to output the internal pixel clock "
		L"(dot clock) instead of the VSYNC signal.";
	const std::wstring tooltipMessageU2 = 
		L"When this bit is set, the HSYNC line is never asserted. Effectively, the HSYNC "
		L"line is disabled. It is possible that this is actually causing the HSYNC line to "
		L"output some other kind of information, which under the circumstances that have "
		L"been tested so far is always false. It should be noted that the official 315-5313 "
		L"pinout information in the Mega Drive service manuals indicate that both CSYNC and "
		L"HSYNC lines are I/O lines, with the sync information able to be input over these "
		L"lines. It's possible this register enables external HSYNC input, but this has not "
		L"been confirmed.";
	const std::wstring tooltipMessageU3 = 
		L"When set, the sprite/layer plane output line (SPA/B) is enabled. This causes that "
		L"pin to be asserted if the current pixel is a sprite pixel, and negated if it is "
		L"not.";
	const std::wstring tooltipMessageSTE = 
		L"Shadow/highlight mode enable. When set, shadow/highlight mode is active.";
	const std::wstring tooltipMessageLSM1 = 
		L"Interlace double flag. When set, double interlace mode is active if the interlace "
		L"enable bit is set. If interlacing is enabled and this bit is not set, normal "
		L"interlace mode is active.";
	const std::wstring tooltipMessageLSM0 = 
		L"Interlace enable flag. When set, interlacing is active. Whether normal or double "
		L"interlace mode is active depends on the state of LSM1.";
	const std::wstring tooltipMessageRS1 = 
		L"When set, changes the digital operation of the VDP to enable H40 mode instead of "
		L"the default H32 mode. Also switches the internally generated SC signal from the "
		L"MCLK/5 divider used for H32 mode, to an MCLK/4 divider for H40 mode. Note that "
		L"this signal is not able to be displayed on a normal TV, because the scan rate is "
		L"too fast. In the Mega Drive, the RS0 bit also needs to be set when enabling H40 "
		L"mode, to enable the external EDCLK input to drive the SC signal. EDCLK has a "
		L"varying clock rate which increases the clock rate for active scan, while slowing "
		L"it during HSYNC to keep the overall drawing time of a line in H40 mode the same as "
		L"in H32 mode, which allows a normal TV to lock onto the signal.";

	//Create tooltips for the window
	HWND hwndTooltip = CreateTooltipControl((HINSTANCE)device->GetAssemblyHandle(), hwnd);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_VSI_TT, tooltipMessageVSI, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_HSI_TT, tooltipMessageHSI, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_LCB_TT, tooltipMessageLCB, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_IE1_TT, tooltipMessageIE1, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_SS_TT, tooltipMessageSS, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_PS_TT, tooltipMessagePS, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_M2_TT, tooltipMessageM2, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_ES_TT, tooltipMessageES, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_EVRAM_TT, tooltipMessageEVRAM, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_DISP_TT, tooltipMessageDISP, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_IE0_TT, tooltipMessageIE0, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_M1_TT, tooltipMessageM1, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_M3_TT, tooltipMessageM3, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_M5_TT, tooltipMessageM5, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_SZ_TT, tooltipMessageSZ, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_MAG_TT, tooltipMessageMAG, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_0B7_TT, tooltipMessage0B7, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_0B6_TT, tooltipMessage0B6, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_0B5_TT, tooltipMessage0B5, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_0B4_TT, tooltipMessage0B4, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_IE2_TT, tooltipMessageIE2, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_VSCR_TT, tooltipMessageVSCR, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_HSCR_TT, tooltipMessageHSCR, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_LSCR_TT, tooltipMessageLSCR, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_RS0_TT, tooltipMessageRS0, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_U1_TT, tooltipMessageU1, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_U2_TT, tooltipMessageU2, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_U3_TT, tooltipMessageU3, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_STE_TT, tooltipMessageSTE, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_LSM1_TT, tooltipMessageLSM1, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_LSM0_TT, tooltipMessageLSM0, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_RS1_TT, tooltipMessageRS1, true);

	//Create a timer to refresh the register contents
	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgModeRegistersWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgModeRegistersWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

	//Mode registers
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_VSI, (device->RegGetVSI(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_HSI, (device->RegGetHSI(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_LCB, (device->RegGetLCB(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_IE1, (device->RegGetIE1(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_SS, (device->RegGetSS(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_PS, (device->RegGetPS(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_M2, (device->RegGetM2(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_ES, (device->RegGetES(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_EVRAM, (device->RegGetEVRAM(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_DISP, (device->RegGetDisplayEnabled(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_IE0, (device->RegGetIE0(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_M1, (device->RegGetDMAEnabled(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_M3, (device->RegGetM3(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_M5, (device->RegGetMode5(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_SZ, (device->RegGetSZ(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_MAG, (device->RegGetMAG(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_0B7, (device->RegGet0B7(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_0B6, (device->RegGet0B6(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_0B5, (device->RegGet0B5(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_0B4, (device->RegGet0B4(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_IE2, (device->RegGetIE2(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_VSCR, (device->RegGetVSCR(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_HSCR, (device->RegGetHSCR(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_LSCR, (device->RegGetLSCR(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_RS0, (device->RegGetRS0(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_U1, (device->RegGetU1(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_U2, (device->RegGetU2(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_U3, (device->RegGetU3(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_STE, (device->RegGetSTE(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_LSM1, (device->RegGetLSM1(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_LSM0, (device->RegGetLSM0(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_RS1, (device->RegGetRS1(accessTarget))? BST_CHECKED: BST_UNCHECKED);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgModeRegistersWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		//##TODO## Locking renderThreadMutex here is a bit heavy handed. See if we can use
		//a different locking scheme which will allow a bit more decoupling.
		boost::mutex::scoped_lock renderLock(device->renderThreadMutex);
		boost::mutex::scoped_lock accessLock(device->accessMutex);

		AccessTarget accessTarget;
		accessTarget.AccessCommitted();

		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		case IDC_VDP_REGISTERS_VSI:
			device->RegSetVSI(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_HSI:
			device->RegSetHSI(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_LCB:
			device->RegSetLCB(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_IE1:
			device->RegSetIE1(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_SS:
			device->RegSetSS(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_PS:
			device->RegSetPS(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_M2:
			device->RegSetM2(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_ES:
			device->RegSetES(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_EVRAM:
			device->RegSetEVRAM(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_DISP:
			device->RegSetDisplayEnabled(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_IE0:
			device->RegSetIE0(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_M1:
			device->RegSetDMAEnabled(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_M3:
			device->RegSetM3(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_M5:
			device->RegSetMode5(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_SZ:
			device->RegSetSZ(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_MAG:
			device->RegSetMAG(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_0B7:
			device->RegSet0B7(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_0B6:
			device->RegSet0B6(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_0B5:
			device->RegSet0B5(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_0B4:
			device->RegSet0B4(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_IE2:
			device->RegSetIE2(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_VSCR:
			device->RegSetVSCR(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_HSCR:
			device->RegSetHSCR(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_LSCR:
			device->RegSetLSCR(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_RS0:
			device->RegSetRS0(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_U1:
			device->RegSetU1(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_U2:
			device->RegSetU2(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_U3:
			device->RegSetU3(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_STE:
			device->RegSetSTE(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_LSM1:
			device->RegSetLSM1(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_LSM0:
			device->RegSetLSM0(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_RS1:
			device->RegSetRS1(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Raw registers dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK S315_5313::RegistersView::WndProcOtherRegistersStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	RegistersView* state = (RegistersView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (RegistersView*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcOtherRegisters(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			INT_PTR result = state->WndProcOtherRegisters(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = FALSE;
	if(state != 0)
	{
		result = state->WndProcOtherRegisters(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::WndProcOtherRegisters(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgOtherRegistersWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgOtherRegistersWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgOtherRegistersWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgOtherRegistersWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Raw registers dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgOtherRegistersWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Tooltip messages
	const std::wstring tooltipMessage077 = 
		L"Unknown. This bit has no known function.";
	const std::wstring tooltipMessage076 = 
		L"Unknown. This bit has no known function.";
	const std::wstring tooltipMessageBackgroundPaletteRow = 
		L"This register selects the palette row number to use when looking up the color "
		L"value to use for the background. Note that this color is also used in the border "
		L"region.";
	const std::wstring tooltipMessageBackgroundPaletteColumn = 
		L"This register selects the palette column number to use when looking up the color "
		L"value to use for the background. Note that this color is also used in the border "
		L"region.";
	const std::wstring tooltipMessageScrollX = 
		L"This register defines the horizontal scroll value to use for the layer plane in "
		L"mode 4. This register has no known effect in mode 5.";
	const std::wstring tooltipMessageScrollY = 
		L"This register defines the vertical scroll value to use for the layer plane in "
		L"mode 4. This register has no known effect in mode 5.";
	const std::wstring tooltipMessageHIntLineCounter = 
		L"This register defines a line counter value that is used to drive horizontal "
		L"interrupts. There is an internal line counter that is loaded with the value "
		L"specified in this register during vblank. In active scan, that counter is "
		L"decremented. If a decrement operation is attempted when the internal counter is "
		L"already 0, a horizontal interrupt is flagged as pending, and the line counter is "
		L"reloaded with the value in this register again. Setting this register to 0 will "
		L"cause a horizontal interrupt to be flagged on each line. Note that no actual "
		L"interrupt is generated unless horizontal interrupts are enabled using the IE1 "
		L"register.";
	const std::wstring tooltipMessageAutoIncrement = 
		L"The contents of this register are added to the current address when performing "
		L"read or write operations, so that successive reads or writes can be performed "
		L"without having to manually update the target address.";
	const std::wstring tooltipMessageScrollABase = 
		L"This register defines the base address to use for mapping data in layer A in mode "
		L"5, and the scrolling layer in mode 4. Note that some bits only have an effect when "
		L"mode 4 or extended VRAM mode is active.";
	const std::wstring tooltipMessageWindowBase = 
		L"This register defines the base address to use for mapping data in the window "
		L"region. Note that some bits only have an effect in H32 mode, or when extended "
		L"VRAM mode is active.";
	const std::wstring tooltipMessageScrollBBase = 
		L"This register defines the base address to use for mapping data in layer B. Note "
		L"that the uppermost bit only has an effect when extended VRAM mode is active.";
	const std::wstring tooltipMessageSpriteTableBase = 
		L"This register defines the base address to use for mapping data for sprites. Note "
		L"that this register has an alternate layout when mode 4 is active, and some bits "
		L"only have an effect in H32 mode, or when extended VRAM mode is active.";
	const std::wstring tooltipMessageSpritePatternBase = 
		L"This register defines the base address to use for pattern data for sprites. Note "
		L"that this register only has an effect when mode 4 is active, or when extended VRAM "
		L"mode is active.";
	const std::wstring tooltipMessageHScrollBase = 
		L"This register defines the base address to use for horizontal scroll data. Note that "
		L"some bits only have an effect when extended VRAM mode is active.";
	const std::wstring tooltipMessageDMALength = 
		L"The DMA length register defines the number of individual operations to perform when "
		L"a DMA operation is triggered. This counter is decremented by 1 when each operation is "
		L"performed. If the resulting value is 0, the DMA operation is terminated. Note that "
		L"this means that if this register is set to 0 when a DMA operation is initiated, it "
		L"will actually perform 0x10000 operations, not 0. Also note that when performing a DMA "
		L"transfer operation, 2 bytes are transferred in a single operation, so if this register "
		L"is set to 0x8000, 0x10000 bytes will actually be copied as a result. Also note that "
		L"this actual register is actively updated by DMA operations, so after a DMA operation "
		L"completes, this register will always be set to 0.";
	const std::wstring tooltipMessageDMASource = 
		L"This register defines the source address to use for DMA copy and transfer operations. "
		L"Note that for DMA transfer operations, the LSB of the source address is not included, "
		L"so a DMA source address of 0x4000 actually becomes 0x8000 on the external bus, when "
		L"the VDP is connected to the external bus in the normal manner. Also note that this "
		L"actual register is actively updated by DMA operations, however, the source address is "
		L"actually built from 3 separate physical registers, with the lower 16 bits contained in "
		L"two registers, and the upper 7 bits contained in another register. Only the lower two "
		L"registers can be successfully updated during a DMA operation. The upper 7 bits of the "
		L"DMA source address remain fixed. This also impacts on DMA transfer operations, since "
		L"it prevents DMA transfer operations from passing a 0x20000 byte boundary in the "
		L"external address space. Attempting a DMA operation which goes across a 0x20000 byte "
		L"boundary will result in the DMA source address wrapping back around to the beginning "
		L"of that 0x20000 byte block.";
	const std::wstring tooltipMessageDMD = 
		L"The DMA mode registers are used to select which DMA operation can be performed. "
		L"Either a DMA fill, DMA copy, or DMA transfer operation can be selected.";
	const std::wstring tooltipMessage0E57 = 
		L"Unknown. These bits have no known function.";
	const std::wstring tooltipMessageScrollAPatternBase = 
		L"This register defines the base address to use for pattern data for layer A. Note "
		L"that this register only has an effect when extended VRAM mode is active.";
	const std::wstring tooltipMessage0E13 = 
		L"Unknown. These bits have no known function.";
	const std::wstring tooltipMessageScrollBPatternBase = 
		L"This register defines the base address to use for pattern data for layer B. Note "
		L"that this register only has an effect when extended VRAM mode is active. Also note "
		L"that due to a possible hardware bug, the layer A pattern base address is ANDed "
		L"together with the layer B pattern address in order to determine the actual base "
		L"address for layer B.";
	const std::wstring tooltipMessage1067 = 
		L"Unknown. These bits have no known function.";
	const std::wstring tooltipMessageVSZ = 
		L"This register defines the vertical size of the layer A and B scrolling planes. "
		L"Note that the maximum total area of the scrolling region is 0x1000 (4096) cells, "
		L"which is equal to a 64x64 cell grid, or a 128x32 cell grid. Where a total area "
		L"is specified which exceeds the maximum size, the horizontal dimension limits the "
		L"vertical dimension. Also note that a register setting of 2 for either the "
		L"horizontal or vertical scrolling plane size is invalid. Refer to further "
		L"documentation for a description of the VDP behaviour where this setting is used.";
	const std::wstring tooltipMessage1023 = 
		L"Unknown. These bits have no known function.";
	const std::wstring tooltipMessageHSZ = 
		L"This register defines the horizontal size of the layer A and B scrolling planes. "
		L"Note that the maximum total area of the scrolling region is 0x1000 (4096) cells, "
		L"which is equal to a 64x64 cell grid, or a 128x32 cell grid. Where a total area "
		L"is specified which exceeds the maximum size, the horizontal dimension limits the "
		L"vertical dimension. Also note that a register setting of 2 for either the "
		L"horizontal or vertical scrolling plane size is invalid. Refer to further "
		L"documentation for a description of the VDP behaviour where this setting is used.";
	const std::wstring tooltipMessageWindowRight = 
		L"When this register is set, if a window region is defined, it is aligned to the "
		L"right of the screen.";
	const std::wstring tooltipMessage1156 = 
		L"Unknown. These bits have no known function.";
	const std::wstring tooltipMessageWindowBaseX = 
		L"Together with the window right setting, these bits define the horizontal size "
		L"and position of the window region.";
	const std::wstring tooltipMessageWindowDown = 
		L"When this register is set, if a window region is defined, it is aligned to the "
		L"bottom of the screen.";
	const std::wstring tooltipMessage1256 = 
		L"Unknown. These bits have no known function.";
	const std::wstring tooltipMessageWindowBaseY= 
		L"Together with the window down setting, these bits define the vertical size and "
		L"position of the window region.";

	//Create tooltips for the window
	HWND hwndTooltip = CreateTooltipControl((HINSTANCE)device->GetAssemblyHandle(), hwnd);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_077_TT, tooltipMessage077, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_076_TT, tooltipMessage076, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_BACKGROUNDPALETTEROW_TT, tooltipMessageBackgroundPaletteRow, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_BACKGROUNDPALETTECOLUMN_TT, tooltipMessageBackgroundPaletteColumn, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_BACKGROUNDSCROLLX_TT, tooltipMessageScrollX, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_BACKGROUNDSCROLLY_TT, tooltipMessageScrollY, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_HINTLINECOUNTER_TT, tooltipMessageHIntLineCounter, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_AUTOINCREMENT_TT, tooltipMessageAutoIncrement, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_SCROLLABASE_TT, tooltipMessageScrollABase, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_WINDOWBASE_TT, tooltipMessageWindowBase, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_SCROLLBBASE_TT, tooltipMessageScrollBBase, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_SPRITETABLE_TT, tooltipMessageSpriteTableBase, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_SPRITEPATTERN_TT, tooltipMessageSpritePatternBase, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_HSCROLLBASE_TT, tooltipMessageHScrollBase, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_DMALENGTH_TT, tooltipMessageDMALength, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_DMASOURCE_TT, tooltipMessageDMASource, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_DMD1_TT, tooltipMessageDMD, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_DMD0_TT, tooltipMessageDMD, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_0E57_TT, tooltipMessage0E57, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_SCROLLAPATTERNBASE_TT, tooltipMessageScrollAPatternBase, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_0E13_TT, tooltipMessage0E13, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_SCROLLBPATTERNBASE_TT, tooltipMessageScrollBPatternBase, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_1067_TT, tooltipMessage1067, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_VSZ_TT, tooltipMessageVSZ, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_1023_TT, tooltipMessage1023, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_HSZ_TT, tooltipMessageHSZ, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_WINDOWRIGHT_TT, tooltipMessageWindowRight, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_1156_TT, tooltipMessage1156, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_WINDOWBASEX_TT, tooltipMessageWindowBaseX, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_WINDOWDOWN_TT, tooltipMessageWindowDown, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_1256_TT, tooltipMessage1256, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_VDP_REGISTERS_WINDOWBASEY_TT, tooltipMessageWindowBaseY, true);

	//Create a timer to refresh the register contents
	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgOtherRegistersWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgOtherRegistersWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

	//Read the current state of the screen settings we need to know in order to decode the
	//register data
	bool mode4Active = !device->RegGetMode5(accessTarget);
	bool extendedVRAMModeActive = device->RegGetEVRAM(accessTarget);
	bool screenModeH40Active = device->RegGetRS1(accessTarget);

	//Other registers
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_077, (device->RegGet077(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_076, (device->RegGet076(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_VDP_REGISTERS_BACKGROUNDPALETTEROW) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_BACKGROUNDPALETTEROW, 1, device->RegGetBackgroundPaletteRow(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_BACKGROUNDPALETTECOLUMN) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_BACKGROUNDPALETTECOLUMN, 1, device->RegGetBackgroundPaletteColumn(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_BACKGROUNDSCROLLX) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_BACKGROUNDSCROLLX, 2, device->RegGetBackgroundScrollX(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_BACKGROUNDSCROLLY) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_BACKGROUNDSCROLLY, 2, device->RegGetBackgroundScrollY(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_HINTLINECOUNTER) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_HINTLINECOUNTER, 2, device->RegGetHInterruptData(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_AUTOINCREMENT) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_AUTOINCREMENT, 2, device->RegGetAutoIncrementData(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_SCROLLABASE) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SCROLLABASE, 2, device->GetRegisterData(0x02, accessTarget).GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_SCROLLABASE_E) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SCROLLABASE_E, 5, device->RegGetNameTableBaseScrollA(accessTarget, mode4Active, extendedVRAMModeActive));
	if(currentControlFocus != IDC_VDP_REGISTERS_WINDOWBASE) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_WINDOWBASE, 2, device->GetRegisterData(0x03, accessTarget).GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_WINDOWBASE_E) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_WINDOWBASE_E, 5, device->RegGetNameTableBaseWindow(accessTarget, screenModeH40Active, extendedVRAMModeActive));
	if(currentControlFocus != IDC_VDP_REGISTERS_SCROLLBBASE) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SCROLLBBASE, 2, device->GetRegisterData(0x04, accessTarget).GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_SCROLLBBASE_E) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SCROLLBBASE_E, 5, device->RegGetNameTableBaseScrollB(accessTarget, extendedVRAMModeActive));
	if(currentControlFocus != IDC_VDP_REGISTERS_SPRITEBASE) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SPRITEBASE, 2, device->GetRegisterData(0x05, accessTarget).GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_SPRITEBASE_E) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SPRITEBASE_E, 5, device->RegGetNameTableBaseSprite(accessTarget, mode4Active, screenModeH40Active, extendedVRAMModeActive));
	if(currentControlFocus != IDC_VDP_REGISTERS_SPRITEPATTERNBASE) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SPRITEPATTERNBASE, 2, device->GetRegisterData(0x06, accessTarget).GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_SPRITEPATTERNBASE_E) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SPRITEPATTERNBASE_E, 5, device->RegGetPatternBaseSprite(accessTarget, mode4Active, extendedVRAMModeActive));
	if(currentControlFocus != IDC_VDP_REGISTERS_HSCROLLBASE) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_HSCROLLBASE, 2, device->GetRegisterData(0x0D, accessTarget).GetData());
	if(currentControlFocus != IDC_VDP_REGISTERS_HSCROLLBASE_E) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_HSCROLLBASE_E, 5, device->RegGetHScrollDataBase(accessTarget, extendedVRAMModeActive));
	if(currentControlFocus != IDC_VDP_REGISTERS_DMALENGTH) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_DMALENGTH, 4, device->RegGetDMALengthCounter(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_DMASOURCE) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_DMASOURCE, 6, device->RegGetDMASourceAddress(accessTarget) >> 1);
	if(currentControlFocus != IDC_VDP_REGISTERS_DMASOURCE_E) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_DMASOURCE_E, 6, device->RegGetDMASourceAddress(accessTarget));
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_DMD1, (device->RegGetDMD1(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_DMD0, (device->RegGetDMD0(accessTarget))? BST_CHECKED: BST_UNCHECKED);

	if(currentControlFocus != IDC_VDP_REGISTERS_0E57) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_0E57, 1, device->RegGet0E57(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_SCROLLAPATTERNBASE) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SCROLLAPATTERNBASE, 1, device->GetRegisterData(0x0E, accessTarget).GetDataSegment(0, 4));
	if(currentControlFocus != IDC_VDP_REGISTERS_SCROLLAPATTERNBASE_E) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SCROLLAPATTERNBASE_E, 5, device->RegGetPatternBaseScrollA(accessTarget, extendedVRAMModeActive));
	if(currentControlFocus != IDC_VDP_REGISTERS_0E13) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_0E13, 1, device->RegGet0E13(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_SCROLLBPATTERNBASE) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SCROLLBPATTERNBASE, 1, device->GetRegisterData(0x0E, accessTarget).GetDataSegment(4, 4));
	if(currentControlFocus != IDC_VDP_REGISTERS_SCROLLBPATTERNBASE_E) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_SCROLLBPATTERNBASE_E, 5, device->RegGetPatternBaseScrollB(accessTarget, extendedVRAMModeActive));
	if(currentControlFocus != IDC_VDP_REGISTERS_1067) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_1067, 1, device->RegGet1067(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_VSZ) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_VSZ, 1, device->RegGetVSZ(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_1023) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_1023, 1, device->RegGet1023(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_HSZ) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_HSZ, 1, device->RegGetHSZ(accessTarget));
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_WINDOWRIGHT, (device->RegGetWindowRightAligned(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_VDP_REGISTERS_1156) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_1156, 1, device->RegGet1156(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_WINDOWBASEX) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_WINDOWBASEX, 1, device->RegGetWindowBasePointX(accessTarget));
	CheckDlgButton(hwnd, IDC_VDP_REGISTERS_WINDOWDOWN, (device->RegGetWindowBottomAligned(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_VDP_REGISTERS_1256) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_1256, 1, device->RegGet1256(accessTarget));
	if(currentControlFocus != IDC_VDP_REGISTERS_WINDOWBASEY) UpdateDlgItemHex(hwnd, IDC_VDP_REGISTERS_WINDOWBASEY, 1, device->RegGetWindowBasePointY(accessTarget));

	//Calculate the effective width and height of the main scroll planes based on the
	//current register settings
	unsigned int hszState = device->RegGetHSZ(accessTarget);
	unsigned int vszState = device->RegGetVSZ(accessTarget);
	unsigned int screenSizeCellsH;
	unsigned int screenSizeCellsV;
	device->CalculateEffectiveCellScrollSize(hszState, vszState, screenSizeCellsH, screenSizeCellsV);

	//Update the effective scroll plane width and height on the debug window
	UpdateDlgItemBin(hwnd, IDC_VDP_REGISTERS_HSZ_E, screenSizeCellsH);
	UpdateDlgItemBin(hwnd, IDC_VDP_REGISTERS_VSZ_E, screenSizeCellsV);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgOtherRegistersWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		//##TODO## Locking renderThreadMutex here is a bit heavy handed. See if we can use
		//a different locking scheme which will allow a bit more decoupling.
		boost::mutex::scoped_lock renderLock(device->renderThreadMutex);
		boost::mutex::scoped_lock accessLock(device->accessMutex);

		AccessTarget accessTarget;
		accessTarget.AccessCommitted();

		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		case IDC_VDP_REGISTERS_077:
			device->RegSet077(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_076:
			device->RegSet076(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_DMD1:
			device->RegSetDMD1(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_DMD0:
			device->RegSetDMD0(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_WINDOWRIGHT:
			device->RegSetWindowRightAligned(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		case IDC_VDP_REGISTERS_WINDOWDOWN:
			device->RegSetWindowBottomAligned(accessTarget, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
			break;
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		//##TODO## Locking renderThreadMutex here is a bit heavy handed. See if we can use
		//a different locking scheme which will allow a bit more decoupling.
		boost::mutex::scoped_lock renderLock(device->renderThreadMutex);
		boost::mutex::scoped_lock accessLock(device->accessMutex);

		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			AccessTarget accessTarget;
			accessTarget.AccessLatest();

			//Read the current state of the screen settings we need to know in order to decode the
			//register data
			bool mode4Active = !device->RegGetMode5(accessTarget);

			switch(LOWORD(wparam))
			{
			case IDC_VDP_REGISTERS_BACKGROUNDPALETTEROW:
				device->RegSetBackgroundPaletteRow(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_BACKGROUNDPALETTECOLUMN:
				device->RegSetBackgroundPaletteColumn(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_BACKGROUNDSCROLLX:
				device->RegSetBackgroundScrollX(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_BACKGROUNDSCROLLY:
				device->RegSetBackgroundScrollY(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_HINTLINECOUNTER:
				device->RegSetHInterruptData(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_AUTOINCREMENT:
				device->RegSetAutoIncrementData(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_SCROLLABASE:
				device->SetRegisterData(0x02, accessTarget, Data(8, GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_VDP_REGISTERS_SCROLLABASE_E:
				device->RegSetNameTableBaseScrollA(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_WINDOWBASE:
				device->SetRegisterData(0x03, accessTarget, Data(8, GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_VDP_REGISTERS_WINDOWBASE_E:
				device->RegSetNameTableBaseWindow(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_SCROLLBBASE:
				device->SetRegisterData(0x04, accessTarget, Data(8, GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_VDP_REGISTERS_SCROLLBBASE_E:
				device->RegSetNameTableBaseScrollB(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_SPRITEBASE:
				device->SetRegisterData(0x05, accessTarget, Data(8, GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_VDP_REGISTERS_SPRITEBASE_E:
				device->RegSetNameTableBaseSprite(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)), mode4Active);
				break;
			case IDC_VDP_REGISTERS_SPRITEPATTERNBASE:
				device->SetRegisterData(0x06, accessTarget, Data(8, GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_VDP_REGISTERS_SPRITEPATTERNBASE_E:
				device->RegSetPatternBaseSprite(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)), mode4Active);
				break;
			case IDC_VDP_REGISTERS_HSCROLLBASE:
				device->SetRegisterData(0x0D, accessTarget, Data(8, GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_VDP_REGISTERS_HSCROLLBASE_E:
				device->RegSetHScrollDataBase(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_DMALENGTH:
				device->RegSetDMALengthCounter(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_DMASOURCE:
				device->RegSetDMASourceAddress(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) << 1);
				break;
			case IDC_VDP_REGISTERS_DMASOURCE_E:
				device->RegSetDMASourceAddress(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_0E57:
				device->RegSet0E57(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_SCROLLAPATTERNBASE:
				device->SetRegisterData(0x0E, accessTarget, device->GetRegisterData(0x0E, accessTarget).SetDataSegment(0, 4, GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_VDP_REGISTERS_SCROLLAPATTERNBASE_E:
				device->RegSetPatternBaseScrollA(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_0E13:
				device->RegSet0E13(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_SCROLLBPATTERNBASE:
				device->SetRegisterData(0x0E, accessTarget, device->GetRegisterData(0x0E, accessTarget).SetDataSegment(4, 4, GetDlgItemHex(hwnd, LOWORD(wparam))));
				break;
			case IDC_VDP_REGISTERS_SCROLLBPATTERNBASE_E:
				device->RegSetPatternBaseScrollB(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_1067:
				device->RegSet1067(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_VSZ:
				device->RegSetVSZ(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_1023:
				device->RegSet1023(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_HSZ:
				device->RegSetHSZ(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_1156:
				device->RegSet1156(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_WINDOWBASEX:
				device->RegSetWindowBasePointX(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_1256:
				device->RegSet1256(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VDP_REGISTERS_WINDOWBASEY:
				device->RegSetWindowBasePointY(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Register locking functions
//----------------------------------------------------------------------------------------
unsigned int S315_5313::RegistersView::ControlIDToLockedRegKey(int controlID)
{
	if((controlID >= IDC_REG_0) && (controlID <= IDC_REG_23))
	{
		return (unsigned int)(controlID - IDC_REG_0);
	}
	return 0xFFFF;
}

//----------------------------------------------------------------------------------------
void S315_5313::RegistersView::ToggleRegisterLock(int controlID)
{
	unsigned int registerNo = ControlIDToLockedRegKey(controlID);
	if(registerNo < registerCount)
	{
		device->registerLocked[registerNo] = !device->registerLocked[registerNo];
	}
}
