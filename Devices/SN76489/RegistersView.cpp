#include "RegistersView.h"
#include "resource.h"
#include <boost/bind.hpp>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489::RegistersView::RegistersView(SN76489* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	lockedColor = RGB(255,127,127);
	lockedBrush = CreateSolidBrush(lockedColor);
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Registers");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_SN76489_REGISTERS));
}

//----------------------------------------------------------------------------------------
SN76489::RegistersView::~RegistersView()
{
	DeleteObject(lockedBrush);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR SN76489::RegistersView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR SN76489::RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create tooltips for the window
	std::wstring lockingSupportedMessage = GetLockingSupportedMessage();
	HWND hwndTooltip = CreateTooltipControl((HINSTANCE)device->GetAssemblyHandle(), hwnd);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_SN76489_REG_LOCKING_TT, lockingSupportedMessage, true);

	//Enable system message bounce-back for controls which can be locked
	SetWindowSubclass(GetDlgItem(hwnd, IDC_SN76489_REG_C0V), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_SN76489_REG_C0D), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_SN76489_REG_C1V), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_SN76489_REG_C1D), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_SN76489_REG_C2V), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_SN76489_REG_C2D), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_SN76489_REG_C3V), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_SN76489_REG_C3D), BounceBackSubclassProc, 0, 0);

	//Create our refresh timer
	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::RegistersView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::RegistersView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	AccessTarget accessTarget;
	accessTarget.AccessLatest();
	unsigned int shiftRegisterNibbleCount = (device->shiftRegisterBitCount / 4) + ((device->shiftRegisterBitCount % 4) != 0? 1: 0);

	//Update registers
	if(currentControlFocus != IDC_SN76489_REG_C0V)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C0V, 1, device->GetVolumeRegister(0, accessTarget).GetData());
	if(currentControlFocus != IDC_SN76489_REG_C0D)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C0D, 3, device->GetToneRegister(0, accessTarget).GetData());
	if(currentControlFocus != IDC_SN76489_REG_C1V)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C1V, 1, device->GetVolumeRegister(1, accessTarget).GetData());
	if(currentControlFocus != IDC_SN76489_REG_C1D)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C1D, 3, device->GetToneRegister(1, accessTarget).GetData());
	if(currentControlFocus != IDC_SN76489_REG_C2V)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C2V, 1, device->GetVolumeRegister(2, accessTarget).GetData());
	if(currentControlFocus != IDC_SN76489_REG_C2D)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C2D, 3, device->GetToneRegister(2, accessTarget).GetData());
	if(currentControlFocus != IDC_SN76489_REG_C3V)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C3V, 1, device->GetVolumeRegister(3, accessTarget).GetData());
	if(currentControlFocus != IDC_SN76489_REG_C3D)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C3D, 1, device->GetToneRegister(3, accessTarget).GetData());
	if(currentControlFocus != IDC_SN76489_REG_SHIFTREGISTER)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_SHIFTREGISTER, shiftRegisterNibbleCount, device->noiseShiftRegister);

	if(currentControlFocus != IDC_SN76489_REG_LATCHCHANNEL)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_LATCHCHANNEL, 1, device->latchedChannel);
	if(currentControlFocus != IDC_SN76489_REG_LATCHREGISTER)	UpdateDlgItemHex(hwnd, IDC_SN76489_REG_LATCHREGISTER, 1, (unsigned int)!device->latchedVolume);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::RegistersView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if(HIWORD(wparam) == EN_KILLFOCUS)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			//Note that we take a lock here to ensure that any register changes we're
			//about to make to any locked registers aren't reverted by a write to the
			//interface for this device, before we can update the stored locked register
			//data.
			boost::mutex::scoped_lock lock(device->registerLockMutex);

			//Set register
			AccessTarget accessTarget;
			accessTarget.AccessLatest();
			unsigned int controlID = LOWORD(wparam);
			switch(controlID)
			{
			case IDC_SN76489_REG_C0V:
				device->SetVolumeRegister(0, Data(volumeRegisterBitCount, GetDlgItemHex(hwnd, LOWORD(wparam))), accessTarget);
				break;
			case IDC_SN76489_REG_C0D:
				device->SetToneRegister(0, Data(toneRegisterBitCount, GetDlgItemHex(hwnd, LOWORD(wparam))), accessTarget);
				break;
			case IDC_SN76489_REG_C1V:
				device->SetVolumeRegister(1, Data(volumeRegisterBitCount, GetDlgItemHex(hwnd, LOWORD(wparam))), accessTarget);
				break;
			case IDC_SN76489_REG_C1D:
				device->SetToneRegister(1, Data(toneRegisterBitCount, GetDlgItemHex(hwnd, LOWORD(wparam))), accessTarget);
				break;
			case IDC_SN76489_REG_C2V:
				device->SetVolumeRegister(2, Data(volumeRegisterBitCount, GetDlgItemHex(hwnd, LOWORD(wparam))), accessTarget);
				break;
			case IDC_SN76489_REG_C2D:
				device->SetToneRegister(2, Data(toneRegisterBitCount, GetDlgItemHex(hwnd, LOWORD(wparam))), accessTarget);
				break;
			case IDC_SN76489_REG_C3V:
				device->SetVolumeRegister(3, Data(volumeRegisterBitCount, GetDlgItemHex(hwnd, LOWORD(wparam))), accessTarget);
				break;
			case IDC_SN76489_REG_C3D:
				device->SetToneRegister(3, Data(noiseRegisterBitCount, GetDlgItemHex(hwnd, LOWORD(wparam))), accessTarget);
				break;
			case IDC_SN76489_REG_SHIFTREGISTER:{
				device->noiseShiftRegister = GetDlgItemHex(hwnd, LOWORD(wparam));
				unsigned int shiftRegisterMaxValue = ((1 << device->shiftRegisterBitCount) - 1);
				if(device->noiseShiftRegister > shiftRegisterMaxValue)
				{
					device->noiseShiftRegister = shiftRegisterMaxValue;
				}
				break;}

			case IDC_SN76489_REG_LATCHCHANNEL:
				device->latchedChannel = GetDlgItemHex(hwnd, LOWORD(wparam));
				if(device->latchedChannel >= device->channelCount)
				{
					device->latchedChannel = device->channelCount - 1;
				}
				break;
			case IDC_SN76489_REG_LATCHREGISTER:
				device->latchedVolume = (GetDlgItemHex(hwnd, LOWORD(wparam)) == 0);
				break;
			}

			//##NOTE## If the target register is currently locked, we need to re-create
			//the lock with the updated register value. Note that we have already
			//obtained a lock before the new register value was written.
			if(device->IsRegisterLocked(ControlIDToLockedRegKey(controlID)))
			{
				device->UnlockRegister(ControlIDToLockedRegKey(controlID));
				LockControl(controlID);
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::RegistersView::msgWM_BOUNCE(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			boost::mutex::scoped_lock lock(device->registerLockMutex);
			if(device->IsRegisterLocked(ControlIDToLockedRegKey(controlID)))
			{
				device->UnlockRegister(ControlIDToLockedRegKey(controlID));
			}
			else
			{
				LockControl(controlID);
			}
			//Force the control to redraw when the lock state is toggled
			InvalidateRect(bounceMessage->hwnd, NULL, FALSE);

			bounceMessage->SetResult(TRUE);
		}
		break;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::RegistersView::msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Handle background colour changes for edit controls which are locked
	boost::mutex::scoped_lock lock(device->registerLockMutex);
	if(device->IsRegisterLocked(ControlIDToLockedRegKey(GetDlgCtrlID((HWND)lparam))))
	{
		SetBkColor((HDC)wparam, lockedColor);
		return (BOOL)HandleToLong(lockedBrush);
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------
//Register locking functions
//----------------------------------------------------------------------------------------
SN76489::LockedRegisterKey SN76489::RegistersView::ControlIDToLockedRegKey(int controlID)
{
	switch(controlID)
	{
	case IDC_SN76489_REG_C0V:
		return SN76489::LOCKEDREG_C0V;
	case IDC_SN76489_REG_C0D:
		return SN76489::LOCKEDREG_C0D;
	case IDC_SN76489_REG_C1V:
		return SN76489::LOCKEDREG_C1V;
	case IDC_SN76489_REG_C1D:
		return SN76489::LOCKEDREG_C1D;
	case IDC_SN76489_REG_C2V:
		return SN76489::LOCKEDREG_C2V;
	case IDC_SN76489_REG_C2D:
		return SN76489::LOCKEDREG_C2D;
	case IDC_SN76489_REG_C3V:
		return SN76489::LOCKEDREG_C3V;
	case IDC_SN76489_REG_C3D:
		return SN76489::LOCKEDREG_C3D;
	}
	return SN76489::LOCKEDREG_NONE;
}

//----------------------------------------------------------------------------------------
void SN76489::RegistersView::LockControl(int controlID)
{
	AccessTarget accessTarget;
	accessTarget.AccessLatest();
	switch(controlID)
	{
	case IDC_SN76489_REG_C0V:
		device->LockRegister(ControlIDToLockedRegKey(controlID), boost::bind(&SN76489::SetVolumeRegister, device, 0, device->GetVolumeRegister(0, accessTarget), accessTarget));
		break;
	case IDC_SN76489_REG_C0D:
		device->LockRegister(ControlIDToLockedRegKey(controlID), boost::bind(&SN76489::SetToneRegister, device, 0, device->GetToneRegister(0, accessTarget), accessTarget));
		break;
	case IDC_SN76489_REG_C1V:
		device->LockRegister(ControlIDToLockedRegKey(controlID), boost::bind(&SN76489::SetVolumeRegister, device, 1, device->GetVolumeRegister(1, accessTarget), accessTarget));
		break;
	case IDC_SN76489_REG_C1D:
		device->LockRegister(ControlIDToLockedRegKey(controlID), boost::bind(&SN76489::SetToneRegister, device, 1, device->GetToneRegister(1, accessTarget), accessTarget));
		break;
	case IDC_SN76489_REG_C2V:
		device->LockRegister(ControlIDToLockedRegKey(controlID), boost::bind(&SN76489::SetVolumeRegister, device, 2, device->GetVolumeRegister(2, accessTarget), accessTarget));
		break;
	case IDC_SN76489_REG_C2D:
		device->LockRegister(ControlIDToLockedRegKey(controlID), boost::bind(&SN76489::SetToneRegister, device, 2, device->GetToneRegister(2, accessTarget), accessTarget));
		break;
	case IDC_SN76489_REG_C3V:
		device->LockRegister(ControlIDToLockedRegKey(controlID), boost::bind(&SN76489::SetVolumeRegister, device, 3, device->GetVolumeRegister(3, accessTarget), accessTarget));
		break;
	case IDC_SN76489_REG_C3D:
		device->LockRegister(ControlIDToLockedRegKey(controlID), boost::bind(&SN76489::SetToneRegister, device, 3, device->GetToneRegister(3, accessTarget), accessTarget));
		break;
	}
}
