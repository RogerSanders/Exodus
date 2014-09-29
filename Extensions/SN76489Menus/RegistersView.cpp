#include "RegistersView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RegistersView::RegistersView(IUIManager& auiManager, RegistersViewPresenter& apresenter, ISN76489& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	lockedColor = RGB(255,127,127);
	lockedBrush = CreateSolidBrush(lockedColor);
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_SN76489_REGISTERS));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
RegistersView::~RegistersView()
{
	DeleteObject(lockedBrush);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR RegistersView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create tooltips for the window
	std::wstring lockingSupportedMessage = GetLockingSupportedMessage();
	HWND hwndTooltip = CreateTooltipControl(GetAssemblyHandle(), hwnd);
	AddTooltip(GetAssemblyHandle(), hwndTooltip, hwnd, IDC_SN76489_REG_LOCKING_TT, lockingSupportedMessage, true);

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
INT_PTR RegistersView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	unsigned int shiftRegisterBitCount = model.GetShiftRegisterBitCount();
	unsigned int shiftRegisterNibbleCount = ((shiftRegisterBitCount + 3) / 4);

	//##TODO## Get the presenter to trigger the updates
	//Update registers
	if(currentControlFocus != IDC_SN76489_REG_C0V)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C0V, 1, model.GetVolumeRegisterExternal(0));
	if(currentControlFocus != IDC_SN76489_REG_C0D)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C0D, 3, model.GetToneRegisterExternal(0));
	if(currentControlFocus != IDC_SN76489_REG_C1V)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C1V, 1, model.GetVolumeRegisterExternal(1));
	if(currentControlFocus != IDC_SN76489_REG_C1D)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C1D, 3, model.GetToneRegisterExternal(1));
	if(currentControlFocus != IDC_SN76489_REG_C2V)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C2V, 1, model.GetVolumeRegisterExternal(2));
	if(currentControlFocus != IDC_SN76489_REG_C2D)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C2D, 3, model.GetToneRegisterExternal(2));
	if(currentControlFocus != IDC_SN76489_REG_C3V)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C3V, 1, model.GetVolumeRegisterExternal(3));
	if(currentControlFocus != IDC_SN76489_REG_C3D)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_C3D, 1, model.GetToneRegisterExternal(3));
	if(currentControlFocus != IDC_SN76489_REG_SHIFTREGISTER)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_SHIFTREGISTER, shiftRegisterNibbleCount, model.GetNoiseShiftRegisterExternal());
	if(currentControlFocus != IDC_SN76489_REG_LATCHCHANNEL)   UpdateDlgItemHex(hwnd, IDC_SN76489_REG_LATCHCHANNEL, 1, model.GetLatchedChannelNoExternal());
	if(currentControlFocus != IDC_SN76489_REG_LATCHREGISTER)  UpdateDlgItemHex(hwnd, IDC_SN76489_REG_LATCHREGISTER, 1, (unsigned int)!model.GetVolumeRegisterLatchedExternal());

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			//Set register
			unsigned int controlID = LOWORD(wparam);
			switch(controlID)
			{
			case IDC_SN76489_REG_C0V:
				model.SetVolumeRegisterExternal(0, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_REG_C0D:
				model.SetToneRegisterExternal(0, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_REG_C1V:
				model.SetVolumeRegisterExternal(1, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_REG_C1D:
				model.SetToneRegisterExternal(1, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_REG_C2V:
				model.SetVolumeRegisterExternal(2, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_REG_C2D:
				model.SetToneRegisterExternal(2, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_REG_C3V:
				model.SetVolumeRegisterExternal(3, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_REG_C3D:
				model.SetToneRegisterExternal(3, GetDlgItemHex(hwnd, LOWORD(wparam)));
				break;
			case IDC_SN76489_REG_SHIFTREGISTER:{
				unsigned int noiseShiftRegister = GetDlgItemHex(hwnd, LOWORD(wparam));
				model.SetNoiseShiftRegisterExternal(noiseShiftRegister);
				break;}
			case IDC_SN76489_REG_LATCHCHANNEL:{
				unsigned int latchedChannel = GetDlgItemHex(hwnd, LOWORD(wparam));
				if(latchedChannel >= model.channelCount)
				{
					latchedChannel = model.channelCount - 1;
				}
				model.SetLatchedChannelNoExternal(latchedChannel);
				break;}
			case IDC_SN76489_REG_LATCHREGISTER:{
				bool volumeRegisterLatched = (GetDlgItemHex(hwnd, LOWORD(wparam)) == 0);
				model.SetVolumeRegisterLatchedExternal(volumeRegisterLatched);
				break;}
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_BOUNCE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lparam;
	int controlID = GetDlgCtrlID(bounceMessage->hwnd);
	switch(bounceMessage->uMsg)
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		if((bounceMessage->wParam & MK_CONTROL) != 0)
		{
			//If the user has control+clicked a control which supports locking, toggle the
			//lock state of the target register.
			unsigned int genericDataID;
			if(ControlIDToDataID(controlID, genericDataID))
			{
				model.SetGenericDataLocked(genericDataID, 0, !model.GetGenericDataLocked(genericDataID, 0));

				//Force the control to redraw when the lock state is toggled
				InvalidateRect(bounceMessage->hwnd, NULL, FALSE);
			}

			bounceMessage->SetResult(TRUE);
		}
		break;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR RegistersView::msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Handle background colour changes for edit controls which are locked
	int controlID = GetDlgCtrlID((HWND)lparam);
	unsigned int genericDataID;
	if(ControlIDToDataID(controlID, genericDataID))
	{
		if(model.GetGenericDataLocked(genericDataID, 0))
		{
			SetBkColor((HDC)wparam, lockedColor);
			return (BOOL)HandleToLong(lockedBrush);
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------
//Register locking functions
//----------------------------------------------------------------------------------------
bool RegistersView::ControlIDToDataID(int controlID, unsigned int& genericDataID)
{
	switch(controlID)
	{
	default:
		return false;
	case IDC_SN76489_REG_C0V:
		genericDataID = (unsigned int)ISN76489::ISN76489DataSource::Channel1VolumeRegister;
		break;
	case IDC_SN76489_REG_C0D:
		genericDataID = (unsigned int)ISN76489::ISN76489DataSource::Channel1ToneRegister;
		break;
	case IDC_SN76489_REG_C1V:
		genericDataID = (unsigned int)ISN76489::ISN76489DataSource::Channel2VolumeRegister;
		break;
	case IDC_SN76489_REG_C1D:
		genericDataID = (unsigned int)ISN76489::ISN76489DataSource::Channel2ToneRegister;
		break;
	case IDC_SN76489_REG_C2V:
		genericDataID = (unsigned int)ISN76489::ISN76489DataSource::Channel3VolumeRegister;
		break;
	case IDC_SN76489_REG_C2D:
		genericDataID = (unsigned int)ISN76489::ISN76489DataSource::Channel3ToneRegister;
		break;
	case IDC_SN76489_REG_C3V:
		genericDataID = (unsigned int)ISN76489::ISN76489DataSource::Channel4VolumeRegister;
		break;
	case IDC_SN76489_REG_C3D:
		genericDataID = (unsigned int)ISN76489::ISN76489DataSource::Channel4ToneRegister;
		break;
	}
	return true;
}
