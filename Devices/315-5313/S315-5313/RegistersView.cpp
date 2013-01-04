#include "RegistersView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::RegistersView::RegistersView(S315_5313* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	lockedColor = RGB(255,127,127);
	lockedBrush = CreateSolidBrush(lockedColor);
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Registers");
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
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_BOUNCE:
		return msgWM_BOUNCE(hwnd, wparam, lparam);
	case WM_CTLCOLOREDIT:
		return msgWM_CTLCOLOREDIT(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
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

	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

	//Update raw registers
	for(unsigned int i = 0; i < registerCount; ++i)
	{
		if(currentControlFocus != (IDC_REG_0 + i))	UpdateDlgItemHex(hwnd, IDC_REG_0 + i, 2, device->GetRegisterData(i, accessTarget).GetData());
	}

	//Base address registers
	if(currentControlFocus != IDC_SCROLLABASE)	UpdateDlgItemHex(hwnd, IDC_SCROLLABASE, 4, device->M5GetNameTableBaseScrollA(accessTarget));
	if(currentControlFocus != IDC_SCROLLBBASE)	UpdateDlgItemHex(hwnd, IDC_SCROLLBBASE, 4, device->M5GetNameTableBaseScrollB(accessTarget));
	if(currentControlFocus != IDC_WINDOWBASE)	UpdateDlgItemHex(hwnd, IDC_WINDOWBASE, 4, device->M5GetNameTableBaseWindow(accessTarget));
	if(currentControlFocus != IDC_SPRITEBASE)	UpdateDlgItemHex(hwnd, IDC_SPRITEBASE, 4, device->M5GetNameTableBaseSprite(accessTarget));
	if(currentControlFocus != IDC_HSCROLLBASE)	UpdateDlgItemHex(hwnd, IDC_HSCROLLBASE, 4, device->M5GetHScrollDataBase(accessTarget));

	//DMA registers
	if(currentControlFocus != IDC_DMASOURCE)	UpdateDlgItemHex(hwnd, IDC_DMASOURCE, 6, device->M5GetDMASourceAddress(accessTarget));
	if(currentControlFocus != IDC_DMALENGTH)	UpdateDlgItemHex(hwnd, IDC_DMALENGTH, 4, device->M5GetDMALengthCounter(accessTarget));
	if(currentControlFocus != IDC_DMAENABLED)	UpdateDlgItemHex(hwnd, IDC_DMAENABLED, 1, device->M5GetDMAEnabled(accessTarget));
	if(currentControlFocus != IDC_ADDRESSINCREMENT)	UpdateDlgItemHex(hwnd, IDC_ADDRESSINCREMENT, 2, device->M5GetAutoIncrementData(accessTarget));

	//Field size registers
//	if(currentControlFocus != IDC_HFIELDSIZE)	UpdateDlgItemHex(hwnd, IDC_HFIELDSIZE, 2, device->GetScrollSizeH());
//	if(currentControlFocus != IDC_VFIELDSIZE)	UpdateDlgItemHex(hwnd, IDC_VFIELDSIZE, 2, device->GetScrollSizeV());
//	if(currentControlFocus != IDC_HSCREENSIZE)	UpdateDlgItemHex(hwnd, IDC_HSCREENSIZE, 2, device->GetVisibleCellWidth());
//	if(currentControlFocus != IDC_VSCREENSIZE)	UpdateDlgItemHex(hwnd, IDC_VSCREENSIZE, 2, device->GetVisibleCellHeight());
//	if(currentControlFocus != IDC_HSCROLLMODE)	UpdateDlgItemHex(hwnd, IDC_HSCROLLMODE, 1, device->GetScrollModeH());
//	if(currentControlFocus != IDC_VSCROLLMODE)	UpdateDlgItemHex(hwnd, IDC_VSCROLLMODE, 1, device->GetScrollModeV());

	//Interrupt registers
	if(currentControlFocus != IDC_HINTERRUPTENABLED)	UpdateDlgItemHex(hwnd, IDC_HINTERRUPTENABLED, 1, device->M5GetHInterruptEnabled(accessTarget));
	if(currentControlFocus != IDC_VINTERRUPTENABLED)	UpdateDlgItemHex(hwnd, IDC_VINTERRUPTENABLED, 1, device->M5GetVInterruptEnabled(accessTarget));
	if(currentControlFocus != IDC_HINTERRUPTCOUNTER)	UpdateDlgItemHex(hwnd, IDC_HINTERRUPTCOUNTER, 2, device->M5GetHInterruptData(accessTarget));
	if(currentControlFocus != IDC_HVCOUNTERSTOP)	UpdateDlgItemHex(hwnd, IDC_HVCOUNTERSTOP, 1, device->M5GetHVCounterLatchEnabled(accessTarget));

	//Background color
	if(currentControlFocus != IDC_BACKGROUNDPALETTELINE)	UpdateDlgItemHex(hwnd, IDC_BACKGROUNDPALETTELINE, 1, device->M5GetBackgroundColorPalette(accessTarget));
	if(currentControlFocus != IDC_BACKGROUNDPALETTEENTRY)	UpdateDlgItemHex(hwnd, IDC_BACKGROUNDPALETTEENTRY, 1, device->M5GetBackgroundColorIndex(accessTarget));

	//Miscellaneous
	if(currentControlFocus != IDC_DISPLAYENABLED)	UpdateDlgItemHex(hwnd, IDC_DISPLAYENABLED, 1, device->M5GetDisplayEnabled(accessTarget));
	if(currentControlFocus != IDC_SHADOWHIGHLIGHTENABLED)	UpdateDlgItemHex(hwnd, IDC_SHADOWHIGHLIGHTENABLED, 1, device->M5GetShadowHighlightEnabled(accessTarget));
//	if(currentControlFocus != IDC_STATUSREGISTER)	UpdateDlgItemHex(hwnd, IDC_STATUSREGISTER, 4, device->GetStatusRegister(RasterPos()));
//	if(currentControlFocus != IDC_HVCOUNTER)	UpdateDlgItemHex(hwnd, IDC_HVCOUNTER, 4, device->GetHVCounter());
//	if(currentControlFocus != IDC_INTERLACEMODE)	UpdateDlgItemHex(hwnd, IDC_INTERLACEMODE, 1, device->GetInterlaceMode());
//	if(currentControlFocus != IDC_ODDFRAMEININTERLACE)	UpdateDlgItemHex(hwnd, IDC_ODDFRAMEININTERLACE, 1, device->GetOddInterlaceFrame());

	//Window settings
	if(currentControlFocus != IDC_WINDOWRIGHT)	UpdateDlgItemHex(hwnd, IDC_WINDOWRIGHT, 1, device->M5GetWindowRightAligned(accessTarget));
	if(currentControlFocus != IDC_WINDOWDOWN)	UpdateDlgItemHex(hwnd, IDC_WINDOWDOWN, 1, device->M5GetWindowBottomAligned(accessTarget));
	if(currentControlFocus != IDC_WINDOWBASEX)	UpdateDlgItemHex(hwnd, IDC_WINDOWBASEX, 2, device->M5GetWindowBasePointX(accessTarget));
	if(currentControlFocus != IDC_WINDOWBASEY)	UpdateDlgItemHex(hwnd, IDC_WINDOWBASEY, 2, device->M5GetWindowBasePointY(accessTarget));

	//Control port registers
	if(currentControlFocus != IDC_CODE)	UpdateDlgItemHex(hwnd, IDC_CODE, 1, device->commandCode.GetData());
	if(currentControlFocus != IDC_ADDRESS)	UpdateDlgItemHex(hwnd, IDC_ADDRESS, 4, device->commandAddress.GetData());
//	if(currentControlFocus != IDC_WRITEPENDING)	UpdateDlgItemHex(hwnd, IDC_WRITEPENDING, 1, device->writePending);
//	if(currentControlFocus != IDC_DMAPENDING)	UpdateDlgItemHex(hwnd, IDC_DMAPENDING, 1, device->dmaPending);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
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
			//Data table addresses
			case IDC_SCROLLABASE:
				device->M5SetNameTableBaseScrollA(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x02, device->GetRegisterData(0x02, accessTarget));
				break;
			case IDC_SCROLLBBASE:
				device->M5SetNameTableBaseScrollB(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x04, device->GetRegisterData(0x04, accessTarget));
				break;
			case IDC_WINDOWBASE:
				device->M5SetNameTableBaseWindow(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x03, device->GetRegisterData(0x03, accessTarget));
				break;
			case IDC_SPRITEBASE:
				device->M5SetNameTableBaseSprite(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x05, device->GetRegisterData(0x05, accessTarget));
				break;
			case IDC_HSCROLLBASE:
				device->M5SetHScrollDataBase(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x0D, device->GetRegisterData(0x0D, accessTarget));
				break;

			//Size and movement
			case IDC_HFIELDSIZE:
//				device->SetScrollSizeH(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VFIELDSIZE:
//				device->SetScrollSizeV(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_HSCREENSIZE:
//				device->SetVisibleCellWidth(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VSCREENSIZE:
//				device->SetVisibleCellHeight(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_HSCROLLMODE:
//				device->SetScrollModeH(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_VSCROLLMODE:
//				device->SetScrollModeV(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;

			//DMA registers
			case IDC_DMASOURCE:
				device->M5SetDMASourceAddress(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x15, device->GetRegisterData(0x15, accessTarget));
				device->TransparentRegisterSpecialUpdateFunction(0x16, device->GetRegisterData(0x16, accessTarget));
				device->TransparentRegisterSpecialUpdateFunction(0x17, device->GetRegisterData(0x17, accessTarget));
				break;
			case IDC_DMALENGTH:
				device->M5SetDMALengthCounter(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x13, device->GetRegisterData(0x13, accessTarget));
				device->TransparentRegisterSpecialUpdateFunction(0x14, device->GetRegisterData(0x14, accessTarget));
				break;
			case IDC_DMAENABLED:
				device->M5SetDMAEnabled(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				device->TransparentRegisterSpecialUpdateFunction(0x01, device->GetRegisterData(0x01, accessTarget));
				break;
			case IDC_ADDRESSINCREMENT:
				device->M5SetAutoIncrementData(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x0F, device->GetRegisterData(0x0F, accessTarget));
				break;

			//Window settings
			case IDC_WINDOWRIGHT:
				device->M5SetWindowRightAligned(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				device->TransparentRegisterSpecialUpdateFunction(0x11, device->GetRegisterData(0x11, accessTarget));
				break;
			case IDC_WINDOWDOWN:
				device->M5SetWindowBottomAligned(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				device->TransparentRegisterSpecialUpdateFunction(0x12, device->GetRegisterData(0x12, accessTarget));
				break;
			case IDC_WINDOWBASEX:
				device->M5SetWindowBasePointX(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x11, device->GetRegisterData(0x11, accessTarget));
				break;
			case IDC_WINDOWBASEY:
				device->M5SetWindowBasePointY(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x12, device->GetRegisterData(0x12, accessTarget));
				break;

			//VBlank and HBlank
			case IDC_HINTERRUPTENABLED:
				device->M5SetHInterruptEnabled(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				device->TransparentRegisterSpecialUpdateFunction(0x00, device->GetRegisterData(0x00, accessTarget));
				break;
			case IDC_VINTERRUPTENABLED:
				device->M5SetVInterruptEnabled(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				device->TransparentRegisterSpecialUpdateFunction(0x01, device->GetRegisterData(0x01, accessTarget));
				break;
			case IDC_HINTERRUPTCOUNTER:
				device->M5SetHInterruptData(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x0A, device->GetRegisterData(0x0A, accessTarget));
				break;

			//HV counter functions
			case IDC_HVCOUNTERSTOP:
				device->M5SetHVCounterLatchEnabled(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				device->TransparentRegisterSpecialUpdateFunction(0x00, device->GetRegisterData(0x00, accessTarget));
				break;

			//Interlace functions
			case IDC_INTERLACEMODE:
//				device->SetInterlaceMode(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_ODDFRAMEININTERLACE:
//				device->SetOddInterlaceFrame(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;

			//Miscellaneous settings
			case IDC_DISPLAYENABLED:
				device->M5SetDisplayEnabled(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				device->TransparentRegisterSpecialUpdateFunction(0x01, device->GetRegisterData(0x01, accessTarget));
				break;
			case IDC_SHADOWHIGHLIGHTENABLED:
				device->M5SetShadowHighlightEnabled(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				device->TransparentRegisterSpecialUpdateFunction(0x0C, device->GetRegisterData(0x0C, accessTarget));
				break;
			case IDC_BACKGROUNDPALETTELINE:
				device->M5SetBackgroundColorPalette(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x07, device->GetRegisterData(0x07, accessTarget));
				break;
			case IDC_BACKGROUNDPALETTEENTRY:
				device->M5SetBackgroundColorIndex(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)));
				device->TransparentRegisterSpecialUpdateFunction(0x07, device->GetRegisterData(0x07, accessTarget));
				break;

			//Control port registers
			case IDC_CODE:
				device->commandCode.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_ADDRESS:
				device->commandAddress.SetData(GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_WRITEPENDING:
//				device->SetWritePending(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			case IDC_DMAPENDING:
//				device->SetDMAPending(accessTarget, GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
				break;
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::RegistersView::msgWM_BOUNCE(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
INT_PTR S315_5313::RegistersView::msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
