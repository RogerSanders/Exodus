#include "DebuggerView.h"
#include "WindowFunctions/WindowFunctions.pkg"
#include "resource.h"
#include <boost/bind.hpp>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612::DebuggerView::DebuggerView(YM2612* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	selectedChannel = 0;
	lockedColor = RGB(255,127,127);
	lockedBrush = CreateSolidBrush(lockedColor);
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Debugger");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_YM2612_DEBUGGER));
}

//----------------------------------------------------------------------------------------
YM2612::DebuggerView::~DebuggerView()
{
	DeleteObject(lockedBrush);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR YM2612::DebuggerView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
	case WM_BOUNCE:
		return msgWM_BOUNCE(hwnd, wparam, lparam);
		//Handle background colour changes for edit controls which are locked
	case WM_CTLCOLOREDIT:
		return msgWM_CTLCOLOREDIT(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR YM2612::DebuggerView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Set the channel select radio buttons to their default state
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS1, (selectedChannel == CHANNEL1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS2, (selectedChannel == CHANNEL2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS3, (selectedChannel == CHANNEL3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS4, (selectedChannel == CHANNEL4)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS5, (selectedChannel == CHANNEL5)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS6, (selectedChannel == CHANNEL6)? BST_CHECKED: BST_UNCHECKED);

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
	const std::wstring tlTooltip = 
		L"TL - Total Level (7-bit)\n"
		L"The Total Level for an operator represents the overall "
		L"volume, or in other words, the minimum attenuation level of "
		L"the operator.";
	const std::wstring slTooltip = 
		L"SL - Sustain Level (4-bit)\n"
		L"The Sustain Level for an operator represents the point at "
		L"which the envelope will cut from the decay phase to the "
		L"sustain phase.";
	const std::wstring arTooltip = 
		L"AR - Attack Rate (5-bit)\n"
		L"The Attack Rate register is used to determine the "
		L"rate at which the attack phase of the envelope progresses. "
		L"This 5-bit value is used in the Rate calculation, along "
		L"with the current key-scale value, to determine the rate.";
	const std::wstring drTooltip = 
		L"DR - Decay Rate (5-bit)\n"
		L"The Decay Rate register is used to determine the "
		L"rate at which the decay phase of the envelope progresses. "
		L"This 5-bit value is used in the Rate calculation, along "
		L"with the current key-scale value, to determine the rate.";
	const std::wstring srTooltip = 
		L"SR - Sustain Rate (5-bit)\n"
		L"The Sustain Rate register is used to determine the "
		L"rate at which the sustain phase of the envelope progresses. "
		L"This 5-bit value is used in the Rate calculation, along "
		L"with the current key-scale value, to determine the rate.";
	const std::wstring rrTooltip = 
		L"RR - Release Rate (4-bit)\n"
		L"The Release Rate register is used to determine the "
		L"rate at which the release phase of the envelope progresses. "
		L"Unlike the other rate parameters, the release rate register "
		L"is only a 4-bit value. This 4-bit value is used in the same "
		L"rate calculation as the other rate parameters, however during "
		L"the calculation, the release rate is treated as a 5-bit value "
		L"with the LSB fixed to 1.";

	//Create tooltips for the window
	HWND hwndTooltip = CreateTooltipControl((HINSTANCE)device->GetAssemblyHandle(), hwnd);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_LOCKING_TT, lockingTooltip, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_TL_TT, tlTooltip, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_SL_TT, slTooltip, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_AR_TT, arTooltip, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_DR_TT, drTooltip, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_SR_TT, srTooltip, true);
	AddTooltip((HINSTANCE)device->GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_RR_TT, rrTooltip, true);

	//Enable system message bounce-back for controls which can be locked
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TL_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TL_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TL_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TL_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SL_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SL_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SL_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SL_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_AR_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_AR_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_AR_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_AR_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DR_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DR_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DR_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DR_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SR_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SR_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SR_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SR_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_RR_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_RR_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_RR_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_RR_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DT_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DT_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DT_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DT_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_MUL_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_MUL_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_MUL_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_MUL_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KS_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KS_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KS_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KS_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_AM_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_AM_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_AM_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_AM_OP4), BounceBackSubclassProc, 0, 0);

	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_FNUM), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_BLOCK), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_ALGORITHM), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_FEEDBACK), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_AMS), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_PMS), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_LEFT), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_RIGHT), BounceBackSubclassProc, 0, 0);

	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_LFOENABLED), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_LFOFREQ), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DACENABLED), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_DACDATA), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_CH3MODE), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP4), BounceBackSubclassProc, 0, 0);

	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERA_LOADED), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERA_ENABLED), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERA_RATE), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERA_DATA), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERB_LOADED), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERB_ENABLED), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERB_RATE), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_TIMERB_DATA), BounceBackSubclassProc, 0, 0);

	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_11), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_12), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_13), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_14), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_21), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_22), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_23), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_24), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_31), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_32), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_33), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_34), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_41), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_42), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_43), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_44), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_51), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_52), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_53), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_54), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_61), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_62), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_63), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_64), BounceBackSubclassProc, 0, 0);

	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_1), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_2), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_3), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_4), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_5), BounceBackSubclassProc, 0, 0);
	SetWindowSubclass(GetDlgItem(hwnd, IDC_YM2612_DEBUGGER_KEY_6), BounceBackSubclassProc, 0, 0);

	SetTimer(hwnd, 1, 100, NULL);

	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::DebuggerView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::DebuggerView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	AccessTarget accessTarget;
	accessTarget.AccessLatest();
	unsigned int channelNo = selectedChannel;

	//Total Level
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TL_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TL_OP1, 2, device->GetTotalLevelData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TL_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TL_OP2, 2, device->GetTotalLevelData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TL_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TL_OP3, 2, device->GetTotalLevelData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TL_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TL_OP4, 2, device->GetTotalLevelData(channelNo, OPERATOR4, accessTarget));
	}

	//Sustain Level
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SL_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SL_OP1, 1, device->GetSustainLevelData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SL_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SL_OP2, 1, device->GetSustainLevelData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SL_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SL_OP3, 1, device->GetSustainLevelData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SL_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SL_OP4, 1, device->GetSustainLevelData(channelNo, OPERATOR4, accessTarget));
	}

	//Attack Rate
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AR_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AR_OP1, 2, device->GetAttackRateData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AR_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AR_OP2, 2, device->GetAttackRateData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AR_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AR_OP3, 2, device->GetAttackRateData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AR_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AR_OP4, 2, device->GetAttackRateData(channelNo, OPERATOR4, accessTarget));
	}

	//Decay Rate
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DR_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DR_OP1, 2, device->GetDecayRateData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DR_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DR_OP2, 2, device->GetDecayRateData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DR_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DR_OP3, 2, device->GetDecayRateData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DR_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DR_OP4, 2, device->GetDecayRateData(channelNo, OPERATOR4, accessTarget));
	}

	//Sustain Rate
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SR_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SR_OP1, 2, device->GetSustainRateData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SR_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SR_OP2, 2, device->GetSustainRateData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SR_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SR_OP3, 2, device->GetSustainRateData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SR_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SR_OP4, 2, device->GetSustainRateData(channelNo, OPERATOR4, accessTarget));
	}

	//Release Rate
	if(currentControlFocus != IDC_YM2612_DEBUGGER_RR_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_RR_OP1, 1, device->GetReleaseRateData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_RR_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_RR_OP2, 1, device->GetReleaseRateData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_RR_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_RR_OP3, 1, device->GetReleaseRateData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_RR_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_RR_OP4, 1, device->GetReleaseRateData(channelNo, OPERATOR4, accessTarget));
	}

	//SSG-EG Mode
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SSGEG_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP1, 1, device->GetSSGData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SSGEG_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP2, 1, device->GetSSGData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SSGEG_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP3, 1, device->GetSSGData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SSGEG_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP4, 1, device->GetSSGData(channelNo, OPERATOR4, accessTarget));
	}

	//Detune
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DT_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DT_OP1, 1, device->GetDetuneData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DT_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DT_OP2, 1, device->GetDetuneData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DT_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DT_OP3, 1, device->GetDetuneData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DT_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DT_OP4, 1, device->GetDetuneData(channelNo, OPERATOR4, accessTarget));
	}

	//Multiple
	if(currentControlFocus != IDC_YM2612_DEBUGGER_MUL_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_MUL_OP1, 1, device->GetMultipleData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_MUL_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_MUL_OP2, 1, device->GetMultipleData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_MUL_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_MUL_OP3, 1, device->GetMultipleData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_MUL_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_MUL_OP4, 1, device->GetMultipleData(channelNo, OPERATOR4, accessTarget));
	}

	//Key Scale
	if(currentControlFocus != IDC_YM2612_DEBUGGER_KS_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_KS_OP1, 1, device->GetKeyScaleData(channelNo, OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_KS_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_KS_OP2, 1, device->GetKeyScaleData(channelNo, OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_KS_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_KS_OP3, 1, device->GetKeyScaleData(channelNo, OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_KS_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_KS_OP4, 1, device->GetKeyScaleData(channelNo, OPERATOR4, accessTarget));
	}

	//AM Enable
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_AM_OP1, (device->GetAmplitudeModulationEnabled(channelNo, OPERATOR1, accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_AM_OP2, (device->GetAmplitudeModulationEnabled(channelNo, OPERATOR2, accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_AM_OP3, (device->GetAmplitudeModulationEnabled(channelNo, OPERATOR3, accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_AM_OP4, (device->GetAmplitudeModulationEnabled(channelNo, OPERATOR4, accessTarget))? BST_CHECKED: BST_UNCHECKED);

	//Channel Registers
	if(currentControlFocus != IDC_YM2612_DEBUGGER_ALGORITHM)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_ALGORITHM, 1, device->GetAlgorithmData(channelNo, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_FEEDBACK)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_FEEDBACK, 1, device->GetFeedbackData(channelNo, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_FNUM)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_FNUM, 3, device->GetFrequencyData(channelNo, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_BLOCK)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_BLOCK, 1, device->GetBlockData(channelNo, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AMS)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AMS, 1, device->GetAMSData(channelNo, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_PMS)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_PMS, 1, device->GetPMSData(channelNo, accessTarget));
	}
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_LEFT, (device->GetOutputLeft(channelNo, accessTarget))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_RIGHT, (device->GetOutputRight(channelNo, accessTarget))? BST_CHECKED: BST_UNCHECKED);

	//Channel 3 Frequency
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3MODE)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3MODE, 1, device->GetCH3Mode(accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3FNUM_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP1, 3, device->GetFrequencyDataChannel3(OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3FNUM_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP2, 3, device->GetFrequencyDataChannel3(OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3FNUM_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP3, 3, device->GetFrequencyDataChannel3(OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3FNUM_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP4, 3, device->GetFrequencyDataChannel3(OPERATOR4, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3BLOCK_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP1, 1, device->GetBlockDataChannel3(OPERATOR1, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3BLOCK_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP2, 1, device->GetBlockDataChannel3(OPERATOR2, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3BLOCK_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP3, 1, device->GetBlockDataChannel3(OPERATOR3, accessTarget));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3BLOCK_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP4, 1, device->GetBlockDataChannel3(OPERATOR4, accessTarget));
	}

	//Timers
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERA_ENABLED, (device->timerAEnable)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERB_ENABLED, (device->timerBEnable)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERA_LOADED, (device->timerALoad)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERB_LOADED, (device->timerBLoad)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW, (device->GetTimerAOverflow())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW, (device->GetTimerBOverflow())? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TIMERA_RATE)
	{
		unsigned int realTimerAInitialCounter = device->timerAInitialCounter;	//(~device->timerAInitialCounter) & 0x3FF;
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TIMERA_RATE, 3, realTimerAInitialCounter);
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TIMERB_RATE)
	{
		unsigned int realTimerBInitialCounter = device->timerBInitialCounter;	//(~device->timerBInitialCounter) & 0xFF;
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TIMERB_RATE, 2, realTimerBInitialCounter);
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TIMERA_DATA)
	{
		unsigned int realTimerACounter = device->timerACounter;	//(~device->timerACounter) & 0x3FF;
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TIMERA_DATA, 3, realTimerACounter);
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TIMERB_DATA)
	{
		unsigned int realTimerBCounter = device->timerBCounter;	//(~device->timerBCounter) & 0xFF;
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TIMERB_DATA, 2, realTimerBCounter);
	}

	//LFO
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_LFOENABLED, (device->GetLFOEnabled(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_YM2612_DEBUGGER_LFOFREQ)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_LFOFREQ, 2, device->GetLFOData(accessTarget));
	}
	std::wstring lfoFrequencyText;
	//##TODO## Update these to clock-independent calculations
	switch(device->GetLFOData(accessTarget))
	{
	case 0:
		lfoFrequencyText = L"3.98";
		break;
	case 1:
		lfoFrequencyText = L"5.56";
		break;
	case 2:
		lfoFrequencyText = L"6.02";
		break;
	case 3:
		lfoFrequencyText = L"6.37";
		break;
	case 4:
		lfoFrequencyText = L"6.88";
		break;
	case 5:
		lfoFrequencyText = L"9.63";
		break;
	case 6:
		lfoFrequencyText = L"48.1";
		break;
	case 7:
		lfoFrequencyText = L"72.2";
		break;
	}
	UpdateDlgItemString(hwnd, IDC_YM2612_DEBUGGER_LFOFREQ2, lfoFrequencyText);

	//DAC
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_DACENABLED, (device->GetDACEnabled(accessTarget))? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DACDATA)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DACDATA, 2, device->GetDACData(accessTarget));
	}

	//Key On/Off
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_11, (device->operatorData[CHANNEL1][OPERATOR1].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_12, (device->operatorData[CHANNEL1][OPERATOR2].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_13, (device->operatorData[CHANNEL1][OPERATOR3].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_14, (device->operatorData[CHANNEL1][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_21, (device->operatorData[CHANNEL2][OPERATOR1].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_22, (device->operatorData[CHANNEL2][OPERATOR2].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_23, (device->operatorData[CHANNEL2][OPERATOR3].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_24, (device->operatorData[CHANNEL2][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_31, (device->operatorData[CHANNEL3][OPERATOR1].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_32, (device->operatorData[CHANNEL3][OPERATOR2].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_33, (device->operatorData[CHANNEL3][OPERATOR3].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_34, (device->operatorData[CHANNEL3][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_41, (device->operatorData[CHANNEL4][OPERATOR1].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_42, (device->operatorData[CHANNEL4][OPERATOR2].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_43, (device->operatorData[CHANNEL4][OPERATOR3].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_44, (device->operatorData[CHANNEL4][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_51, (device->operatorData[CHANNEL5][OPERATOR1].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_52, (device->operatorData[CHANNEL5][OPERATOR2].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_53, (device->operatorData[CHANNEL5][OPERATOR3].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_54, (device->operatorData[CHANNEL5][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_61, (device->operatorData[CHANNEL6][OPERATOR1].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_62, (device->operatorData[CHANNEL6][OPERATOR2].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_63, (device->operatorData[CHANNEL6][OPERATOR3].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_64, (device->operatorData[CHANNEL6][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);

	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_1, (device->operatorData[CHANNEL1][OPERATOR1].keyon && device->operatorData[CHANNEL1][OPERATOR2].keyon && device->operatorData[CHANNEL1][OPERATOR3].keyon && device->operatorData[CHANNEL1][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_2, (device->operatorData[CHANNEL2][OPERATOR1].keyon && device->operatorData[CHANNEL2][OPERATOR2].keyon && device->operatorData[CHANNEL2][OPERATOR3].keyon && device->operatorData[CHANNEL2][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_3, (device->operatorData[CHANNEL3][OPERATOR1].keyon && device->operatorData[CHANNEL3][OPERATOR2].keyon && device->operatorData[CHANNEL3][OPERATOR3].keyon && device->operatorData[CHANNEL3][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_4, (device->operatorData[CHANNEL4][OPERATOR1].keyon && device->operatorData[CHANNEL4][OPERATOR2].keyon && device->operatorData[CHANNEL4][OPERATOR3].keyon && device->operatorData[CHANNEL4][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_5, (device->operatorData[CHANNEL5][OPERATOR1].keyon && device->operatorData[CHANNEL5][OPERATOR2].keyon && device->operatorData[CHANNEL5][OPERATOR3].keyon && device->operatorData[CHANNEL5][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_6, (device->operatorData[CHANNEL6][OPERATOR1].keyon && device->operatorData[CHANNEL6][OPERATOR2].keyon && device->operatorData[CHANNEL6][OPERATOR3].keyon && device->operatorData[CHANNEL6][OPERATOR4].keyon)? BST_CHECKED: BST_UNCHECKED);

	//Clock
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CLOCK)
	{
		UpdateDlgItemFloat(hwnd, IDC_YM2612_DEBUGGER_CLOCK, (float)device->externalClockRate);
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_FMDIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_FMDIVIDE, device->fmClockDivider);
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_EGDIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_EGDIVIDE, device->egClockDivider);
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_OUTDIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_OUTDIVIDE, device->outputClockDivider);
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TADIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_TADIVIDE, device->timerAClockDivider);
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TBDIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_TBDIVIDE, device->timerBClockDivider);
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::DebuggerView::msgWM_BOUNCE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lparam;
	int controlID = GetDlgCtrlID(bounceMessage->hwnd);
	switch(bounceMessage->uMsg)
	{
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		if((bounceMessage->wParam & MK_CONTROL) != 0)
		{
			switch(controlID)
			{
			default:{
				//If the user has control+clicked a control which supports locking, toggle
				//the lock state of the target register.
				boost::mutex::scoped_lock lock(device->registerLockMutex);
				if(device->IsRegisterLocked(ControlIDToLockedRegKey(controlID, selectedChannel)))
				{
					device->UnlockRegister(ControlIDToLockedRegKey(controlID, selectedChannel));
				}
				else
				{
					LockControl(controlID, selectedChannel);
				}
				//Force the control to redraw when the lock state is toggled
				InvalidateRect(bounceMessage->hwnd, NULL, FALSE);
				break;}

			case IDC_YM2612_DEBUGGER_TIMERA_LOADED:
			case IDC_YM2612_DEBUGGER_TIMERA_ENABLED:
			case IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW:
			case IDC_YM2612_DEBUGGER_TIMERA_RATE:
			case IDC_YM2612_DEBUGGER_TIMERA_DATA:
			case IDC_YM2612_DEBUGGER_TIMERB_LOADED:
			case IDC_YM2612_DEBUGGER_TIMERB_ENABLED:
			case IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW:
			case IDC_YM2612_DEBUGGER_TIMERB_RATE:
			case IDC_YM2612_DEBUGGER_TIMERB_DATA:{
				TimerParam timerParam = (TimerParam)(controlID - IDC_YM2612_DEBUGGER_TIMERA_LOADED);
				device->SetTimerParamLocked(timerParam, !device->GetTimerParamLocked(timerParam));
				//Force the control to redraw when the lock state is toggled
				InvalidateRect(bounceMessage->hwnd, NULL, FALSE);
				break;}

			case IDC_YM2612_DEBUGGER_KEY_11:
			case IDC_YM2612_DEBUGGER_KEY_12:
			case IDC_YM2612_DEBUGGER_KEY_13:
			case IDC_YM2612_DEBUGGER_KEY_14:
			case IDC_YM2612_DEBUGGER_KEY_21:
			case IDC_YM2612_DEBUGGER_KEY_22:
			case IDC_YM2612_DEBUGGER_KEY_23:
			case IDC_YM2612_DEBUGGER_KEY_24:
			case IDC_YM2612_DEBUGGER_KEY_31:
			case IDC_YM2612_DEBUGGER_KEY_32:
			case IDC_YM2612_DEBUGGER_KEY_33:
			case IDC_YM2612_DEBUGGER_KEY_34:
			case IDC_YM2612_DEBUGGER_KEY_41:
			case IDC_YM2612_DEBUGGER_KEY_42:
			case IDC_YM2612_DEBUGGER_KEY_43:
			case IDC_YM2612_DEBUGGER_KEY_44:
			case IDC_YM2612_DEBUGGER_KEY_51:
			case IDC_YM2612_DEBUGGER_KEY_52:
			case IDC_YM2612_DEBUGGER_KEY_53:
			case IDC_YM2612_DEBUGGER_KEY_54:
			case IDC_YM2612_DEBUGGER_KEY_61:
			case IDC_YM2612_DEBUGGER_KEY_62:
			case IDC_YM2612_DEBUGGER_KEY_63:
			case IDC_YM2612_DEBUGGER_KEY_64:{
				unsigned int channelNo = (controlID - IDC_YM2612_DEBUGGER_KEY_11) / operatorCount;
				unsigned int operatorNo = (controlID - IDC_YM2612_DEBUGGER_KEY_11) % operatorCount;
				device->SetKeyStateLocked(channelNo, operatorNo, !device->GetKeyStateLocked(channelNo, operatorNo));
				//We force the entire window to redraw instead as a special case here to
				//deal with the "All" key-state checkboxes which toggle the locked state
				//for all the operators within a channel at once. If we only redraw the
				//control which was clicked, the other checkboxes which have had their
				//state toggled won't redraw.
				InvalidateRect(hwnd, NULL, FALSE);
				break;}
			case IDC_YM2612_DEBUGGER_KEY_1:
			case IDC_YM2612_DEBUGGER_KEY_2:
			case IDC_YM2612_DEBUGGER_KEY_3:
			case IDC_YM2612_DEBUGGER_KEY_4:
			case IDC_YM2612_DEBUGGER_KEY_5:
			case IDC_YM2612_DEBUGGER_KEY_6:{
				unsigned int channelNo = (controlID - IDC_YM2612_DEBUGGER_KEY_1);
				if(!device->GetKeyStateLocked(channelNo, OPERATOR1) && !device->GetKeyStateLocked(channelNo, OPERATOR2) && !device->GetKeyStateLocked(channelNo, OPERATOR3) && !device->GetKeyStateLocked(channelNo, OPERATOR4))
				{
					device->SetKeyStateLocked(channelNo, OPERATOR1, true);
					device->SetKeyStateLocked(channelNo, OPERATOR2, true);
					device->SetKeyStateLocked(channelNo, OPERATOR3, true);
					device->SetKeyStateLocked(channelNo, OPERATOR4, true);
				}
				else
				{
					device->SetKeyStateLocked(channelNo, OPERATOR1, false);
					device->SetKeyStateLocked(channelNo, OPERATOR2, false);
					device->SetKeyStateLocked(channelNo, OPERATOR3, false);
					device->SetKeyStateLocked(channelNo, OPERATOR4, false);
				}
				//We force the entire window to redraw instead as a special case here to
				//deal with the "All" key-state checkboxes which toggle the locked state
				//for all the operators within a channel at once. If we only redraw the
				//control which was clicked, the other checkboxes which have had their
				//state toggled won't redraw.
				InvalidateRect(hwnd, NULL, FALSE);
				break;}
			}

			bounceMessage->SetResult(TRUE);
		}
		break;
	case WM_PAINT:
		switch(controlID)
		{
		case IDC_YM2612_DEBUGGER_AM_OP1:
		case IDC_YM2612_DEBUGGER_AM_OP2:
		case IDC_YM2612_DEBUGGER_AM_OP3:
		case IDC_YM2612_DEBUGGER_AM_OP4:
		case IDC_YM2612_DEBUGGER_LEFT:
		case IDC_YM2612_DEBUGGER_RIGHT:
		case IDC_YM2612_DEBUGGER_LFOENABLED:
		case IDC_YM2612_DEBUGGER_DACENABLED:
			//Handle background colour changes for checkbox controls which are locked
			if(device->IsRegisterLocked(ControlIDToLockedRegKey(controlID, selectedChannel)))
			{
				PaintCheckboxHighlight(GetDlgItem(hwnd, controlID));
				bounceMessage->SetResult(TRUE);
			}
			break;

			//Handle background colour changes for the timer checkboxes
		case IDC_YM2612_DEBUGGER_TIMERA_LOADED:
		case IDC_YM2612_DEBUGGER_TIMERA_ENABLED:
		case IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW:
		case IDC_YM2612_DEBUGGER_TIMERB_LOADED:
		case IDC_YM2612_DEBUGGER_TIMERB_ENABLED:
		case IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW:
			if(device->GetTimerParamLocked((TimerParam)(controlID - IDC_YM2612_DEBUGGER_TIMERA_LOADED)))
			{
				PaintCheckboxHighlight(GetDlgItem(hwnd, controlID));
				bounceMessage->SetResult(TRUE);
			}
			break;

			//Handle background colour changes for the key-state checkboxes
		case IDC_YM2612_DEBUGGER_KEY_11:
		case IDC_YM2612_DEBUGGER_KEY_12:
		case IDC_YM2612_DEBUGGER_KEY_13:
		case IDC_YM2612_DEBUGGER_KEY_14:
		case IDC_YM2612_DEBUGGER_KEY_21:
		case IDC_YM2612_DEBUGGER_KEY_22:
		case IDC_YM2612_DEBUGGER_KEY_23:
		case IDC_YM2612_DEBUGGER_KEY_24:
		case IDC_YM2612_DEBUGGER_KEY_31:
		case IDC_YM2612_DEBUGGER_KEY_32:
		case IDC_YM2612_DEBUGGER_KEY_33:
		case IDC_YM2612_DEBUGGER_KEY_34:
		case IDC_YM2612_DEBUGGER_KEY_41:
		case IDC_YM2612_DEBUGGER_KEY_42:
		case IDC_YM2612_DEBUGGER_KEY_43:
		case IDC_YM2612_DEBUGGER_KEY_44:
		case IDC_YM2612_DEBUGGER_KEY_51:
		case IDC_YM2612_DEBUGGER_KEY_52:
		case IDC_YM2612_DEBUGGER_KEY_53:
		case IDC_YM2612_DEBUGGER_KEY_54:
		case IDC_YM2612_DEBUGGER_KEY_61:
		case IDC_YM2612_DEBUGGER_KEY_62:
		case IDC_YM2612_DEBUGGER_KEY_63:
		case IDC_YM2612_DEBUGGER_KEY_64:{
			unsigned int channelNo = (controlID - IDC_YM2612_DEBUGGER_KEY_11) / operatorCount;
			unsigned int operatorNo = (controlID - IDC_YM2612_DEBUGGER_KEY_11) % operatorCount;
			if(device->GetKeyStateLocked(channelNo, operatorNo))
			{
				PaintCheckboxHighlight(GetDlgItem(hwnd, controlID));
				bounceMessage->SetResult(TRUE);
			}
			break;}
		case IDC_YM2612_DEBUGGER_KEY_1:
		case IDC_YM2612_DEBUGGER_KEY_2:
		case IDC_YM2612_DEBUGGER_KEY_3:
		case IDC_YM2612_DEBUGGER_KEY_4:
		case IDC_YM2612_DEBUGGER_KEY_5:
		case IDC_YM2612_DEBUGGER_KEY_6:{
			unsigned int channelNo = (controlID - IDC_YM2612_DEBUGGER_KEY_1);
			if(device->GetKeyStateLocked(channelNo, OPERATOR1) && device->GetKeyStateLocked(channelNo, OPERATOR2) && device->GetKeyStateLocked(channelNo, OPERATOR3) && device->GetKeyStateLocked(channelNo, OPERATOR4))
			{
				PaintCheckboxHighlight(GetDlgItem(hwnd, controlID));
				bounceMessage->SetResult(TRUE);
			}
			break;}
		}
		break;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::DebuggerView::msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	boost::mutex::scoped_lock lock(device->registerLockMutex);
	int controlID = GetDlgCtrlID((HWND)lparam);
	switch(controlID)
	{
	default:{
		if(device->IsRegisterLocked(ControlIDToLockedRegKey(GetDlgCtrlID((HWND)lparam), selectedChannel)))
		{
			SetBkColor((HDC)wparam, lockedColor);
			return (BOOL)HandleToLong(lockedBrush);
		}
		break;}

	//Handle background colour changes for the timer rate values
	case IDC_YM2612_DEBUGGER_TIMERA_RATE:
	case IDC_YM2612_DEBUGGER_TIMERA_DATA:
	case IDC_YM2612_DEBUGGER_TIMERB_RATE:
	case IDC_YM2612_DEBUGGER_TIMERB_DATA:
		if(device->GetTimerParamLocked((TimerParam)(controlID - IDC_YM2612_DEBUGGER_TIMERA_LOADED)))
		{
			SetBkColor((HDC)wparam, lockedColor);
			return (BOOL)HandleToLong(lockedBrush);
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::DebuggerView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	AccessTarget accessTarget;
	accessTarget.AccessLatest();
	unsigned int channelNo = selectedChannel;

	if(HIWORD(wparam) == BN_CLICKED)
	{
		//##NOTE## We take a lock here to ensure that any register changes we're about to
		//make to any locked registers aren't reverted by a write to the interface for
		//this device, before we can update the stored locked register data.
		boost::mutex::scoped_lock lock(device->registerLockMutex);

		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		case IDC_YM2612_DEBUGGER_INFO_OP1:{
			device->OpenOperatorView(channelNo, 0);
			break;}
		case IDC_YM2612_DEBUGGER_INFO_OP2:{
			device->OpenOperatorView(channelNo, 1);
			break;}
		case IDC_YM2612_DEBUGGER_INFO_OP3:{
			device->OpenOperatorView(channelNo, 2);
			break;}
		case IDC_YM2612_DEBUGGER_INFO_OP4:{
			device->OpenOperatorView(channelNo, 3);
			break;}
		//Channel Select
		case IDC_YM2612_DEBUGGER_CS1:{
			selectedChannel = 0;
			InvalidateRect(hwnd, NULL, FALSE);
			break;}
		case IDC_YM2612_DEBUGGER_CS2:{
			selectedChannel = 1;
			InvalidateRect(hwnd, NULL, FALSE);
			break;}
		case IDC_YM2612_DEBUGGER_CS3:{
			selectedChannel = 2;
			InvalidateRect(hwnd, NULL, FALSE);
			break;}
		case IDC_YM2612_DEBUGGER_CS4:{
			selectedChannel = 3;
			InvalidateRect(hwnd, NULL, FALSE);
			break;}
		case IDC_YM2612_DEBUGGER_CS5:{
			selectedChannel = 4;
			InvalidateRect(hwnd, NULL, FALSE);
			break;}
		case IDC_YM2612_DEBUGGER_CS6:{
			selectedChannel = 5;
			InvalidateRect(hwnd, NULL, FALSE);
			break;}

		//AM Enable
		case IDC_YM2612_DEBUGGER_AM_OP1:{
			device->SetAmplitudeModulationEnabled(channelNo, OPERATOR1, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_AM_OP2:{
			device->SetAmplitudeModulationEnabled(channelNo, OPERATOR2, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_AM_OP3:{
			device->SetAmplitudeModulationEnabled(channelNo, OPERATOR3, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_AM_OP4:{
			device->SetAmplitudeModulationEnabled(channelNo, OPERATOR4, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			break;}

		//Left/Right
		case IDC_YM2612_DEBUGGER_LEFT:{
			device->SetOutputLeft(channelNo, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_RIGHT:{
			device->SetOutputRight(channelNo, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			break;}

		//Timers
		case IDC_YM2612_DEBUGGER_TIMERA_ENABLED:{
			//device->SetTimerAEnable(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			device->timerAEnable = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_ENABLED:{
			//device->SetTimerBEnable(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			device->timerBEnable = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERA_LOADED:{
			//device->SetTimerALoad(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			device->timerALoad = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_LOADED:{
			//device->SetTimerBLoad(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			device->timerBLoad = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW:{
			device->SetTimerAOverflow(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW:{
			device->SetTimerBOverflow(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}

		//LFO
		case IDC_YM2612_DEBUGGER_LFOENABLED:{
			device->SetLFOEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			break;}

		//DAC
		case IDC_YM2612_DEBUGGER_DACENABLED:{
			device->SetDACEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED, accessTarget);
			break;}

		//Key On/Off
		case IDC_YM2612_DEBUGGER_KEY_11:{
			device->operatorData[CHANNEL1][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_12:{
			device->operatorData[CHANNEL1][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_13:{
			device->operatorData[CHANNEL1][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_14:{
			device->operatorData[CHANNEL1][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_21:{
			device->operatorData[CHANNEL2][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_22:{
			device->operatorData[CHANNEL2][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_23:{
			device->operatorData[CHANNEL2][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_24:{
			device->operatorData[CHANNEL2][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_31:{
			device->operatorData[CHANNEL3][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_32:{
			device->operatorData[CHANNEL3][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_33:{
			device->operatorData[CHANNEL3][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_34:{
			device->operatorData[CHANNEL3][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_41:{
			device->operatorData[CHANNEL4][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_42:{
			device->operatorData[CHANNEL4][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_43:{
			device->operatorData[CHANNEL4][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_44:{
			device->operatorData[CHANNEL4][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_51:{
			device->operatorData[CHANNEL5][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_52:{
			device->operatorData[CHANNEL5][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_53:{
			device->operatorData[CHANNEL5][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_54:{
			device->operatorData[CHANNEL5][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_61:{
			device->operatorData[CHANNEL6][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_62:{
			device->operatorData[CHANNEL6][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_63:{
			device->operatorData[CHANNEL6][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_64:{
			device->operatorData[CHANNEL6][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}

		case IDC_YM2612_DEBUGGER_KEY_1:{
			device->operatorData[CHANNEL1][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL1][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL1][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL1][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_2:{
			device->operatorData[CHANNEL2][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL2][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL2][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL2][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_3:{
			device->operatorData[CHANNEL3][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL3][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL3][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL3][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_4:{
			device->operatorData[CHANNEL4][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL4][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL4][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL4][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_5:{
			device->operatorData[CHANNEL5][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL5][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL5][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL5][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_KEY_6:{
			device->operatorData[CHANNEL6][OPERATOR1].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL6][OPERATOR2].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL6][OPERATOR3].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			device->operatorData[CHANNEL6][OPERATOR4].keyon = (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		}

		//##NOTE## If the target register is currently locked, we need to re-create
		//the lock with the updated register value. Note that we have already
		//obtained a lock before the new register value was written.
		if(device->IsRegisterLocked(ControlIDToLockedRegKey(controlID, selectedChannel)))
		{
			device->UnlockRegister(ControlIDToLockedRegKey(controlID, selectedChannel));
			LockControl(controlID, selectedChannel);
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
		return FALSE;
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(currentControlFocus == LOWORD(wparam))
		{
			currentControlFocus = 0;
		}
		if(newText == previousText)
		{
			return FALSE;
		}

		//##NOTE## We take a lock here to ensure that any register changes we're about to
		//make to any locked registers aren't reverted by a write to the interface for
		//this device, before we can update the stored locked register data.
		boost::mutex::scoped_lock lock(device->registerLockMutex);

		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		//Total Level
		case IDC_YM2612_DEBUGGER_TL_OP1:{
			device->SetTotalLevelData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_TL_OP2:{
			device->SetTotalLevelData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_TL_OP3:{
			device->SetTotalLevelData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_TL_OP4:{
			device->SetTotalLevelData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Sustain Level
		case IDC_YM2612_DEBUGGER_SL_OP1:{
			device->SetSustainLevelData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_SL_OP2:{
			device->SetSustainLevelData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_SL_OP3:{
			device->SetSustainLevelData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_SL_OP4:{
			device->SetSustainLevelData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Attack Rate
		case IDC_YM2612_DEBUGGER_AR_OP1:{
			device->SetAttackRateData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_AR_OP2:{
			device->SetAttackRateData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_AR_OP3:{
			device->SetAttackRateData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_AR_OP4:{
			device->SetAttackRateData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Decay Rate
		case IDC_YM2612_DEBUGGER_DR_OP1:{
			device->SetDecayRateData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_DR_OP2:{
			device->SetDecayRateData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_DR_OP3:{
			device->SetDecayRateData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_DR_OP4:{
			device->SetDecayRateData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Sustain Rate
		case IDC_YM2612_DEBUGGER_SR_OP1:{
			device->SetSustainRateData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_SR_OP2:{
			device->SetSustainRateData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_SR_OP3:{
			device->SetSustainRateData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_SR_OP4:{
			device->SetSustainRateData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Release Rate
		case IDC_YM2612_DEBUGGER_RR_OP1:{
			device->SetReleaseRateData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_RR_OP2:{
			device->SetReleaseRateData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_RR_OP3:{
			device->SetReleaseRateData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_RR_OP4:{
			device->SetReleaseRateData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//SSG-EG Mode
		case IDC_YM2612_DEBUGGER_SSGEG_OP1:{
			device->SetSSGData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_SSGEG_OP2:{
			device->SetSSGData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_SSGEG_OP3:{
			device->SetSSGData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_SSGEG_OP4:{
			device->SetSSGData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Detune
		case IDC_YM2612_DEBUGGER_DT_OP1:{
			device->SetDetuneData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_DT_OP2:{
			device->SetDetuneData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_DT_OP3:{
			device->SetDetuneData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_DT_OP4:{
			device->SetDetuneData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Multiple
		case IDC_YM2612_DEBUGGER_MUL_OP1:{
			device->SetMultipleData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_MUL_OP2:{
			device->SetMultipleData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_MUL_OP3:{
			device->SetMultipleData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_MUL_OP4:{
			device->SetMultipleData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Key Scale
		case IDC_YM2612_DEBUGGER_KS_OP1:{
			device->SetKeyScaleData(channelNo, 0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_KS_OP2:{
			device->SetKeyScaleData(channelNo, 1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_KS_OP3:{
			device->SetKeyScaleData(channelNo, 2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_KS_OP4:{
			device->SetKeyScaleData(channelNo, 3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Channel Registers
		case IDC_YM2612_DEBUGGER_ALGORITHM:{
			device->SetAlgorithmData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_FEEDBACK:{
			device->SetFeedbackData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_FNUM:{
			device->SetFrequencyData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_BLOCK:{
			device->SetBlockData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_AMS:{
			device->SetAMSData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_PMS:{
			device->SetPMSData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Channel 3 Frequency
		case IDC_YM2612_DEBUGGER_CH3MODE:{
			device->SetCH3Mode(GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_CH3FNUM_OP1:{
			device->SetFrequencyDataChannel3(0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_CH3FNUM_OP2:{
			device->SetFrequencyDataChannel3(1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_CH3FNUM_OP3:{
			device->SetFrequencyDataChannel3(2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_CH3FNUM_OP4:{
			device->SetFrequencyDataChannel3(3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_CH3BLOCK_OP1:{
			device->SetBlockDataChannel3(0, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_CH3BLOCK_OP2:{
			device->SetBlockDataChannel3(1, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_CH3BLOCK_OP3:{
			device->SetBlockDataChannel3(2, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}
		case IDC_YM2612_DEBUGGER_CH3BLOCK_OP4:{
			device->SetBlockDataChannel3(3, GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Timers
		case IDC_YM2612_DEBUGGER_TIMERA_RATE:{
			//				device->SetTimerAData(GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			device->timerAInitialCounter = GetDlgItemHex(hwnd, LOWORD(wparam));
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_RATE:{
			//				device->SetTimerBData(GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			device->timerBInitialCounter = GetDlgItemHex(hwnd, LOWORD(wparam));
			break;}
		case IDC_YM2612_DEBUGGER_TIMERA_DATA:{
			device->timerACounter = GetDlgItemHex(hwnd, LOWORD(wparam));
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_DATA:{
			device->timerBCounter = GetDlgItemHex(hwnd, LOWORD(wparam));
			break;}

		//LFO
		case IDC_YM2612_DEBUGGER_LFOFREQ:{
			device->SetLFOData(GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//DAC
		case IDC_YM2612_DEBUGGER_DACDATA:{
			device->SetDACData(GetDlgItemHex(hwnd, LOWORD(wparam)), accessTarget);
			break;}

		//Clock
		case IDC_YM2612_DEBUGGER_CLOCK:{
			float newClock = GetDlgItemFloat(hwnd, LOWORD(wparam));
			if(newClock > 0)
			{
				device->externalClockRate = newClock;
			}
			break;}
		case IDC_YM2612_DEBUGGER_FMDIVIDE:{
			unsigned int newClock = GetDlgItemBin(hwnd, LOWORD(wparam));
			if(newClock > 0)
			{
				device->fmClockDivider = newClock;
			}
			break;}
		case IDC_YM2612_DEBUGGER_EGDIVIDE:{
			unsigned int newClock = GetDlgItemBin(hwnd, LOWORD(wparam));
			if(newClock > 0)
			{
				device->egClockDivider = newClock;
			}
			break;}
		case IDC_YM2612_DEBUGGER_OUTDIVIDE:{
			unsigned int newClock = GetDlgItemBin(hwnd, LOWORD(wparam));
			if(newClock > 0)
			{
				device->outputClockDivider = newClock;
			}
			break;}
		case IDC_YM2612_DEBUGGER_TADIVIDE:{
			unsigned int newClock = GetDlgItemBin(hwnd, LOWORD(wparam));
			if(newClock > 0)
			{
				device->timerAClockDivider = newClock;
			}
			break;}
		case IDC_YM2612_DEBUGGER_TBDIVIDE:{
			unsigned int newClock = GetDlgItemBin(hwnd, LOWORD(wparam));
			if(newClock > 0)
			{
				device->timerBClockDivider = newClock;
			}
			break;}
		}

		//##NOTE## If the target register is currently locked, we need to re-create
		//the lock with the updated register value. Note that we have already
		//obtained a lock before the new register value was written.
		if(device->IsRegisterLocked(ControlIDToLockedRegKey(controlID, selectedChannel)))
		{
			device->UnlockRegister(ControlIDToLockedRegKey(controlID, selectedChannel));
			LockControl(controlID, selectedChannel);
		}
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Register locking functions
//----------------------------------------------------------------------------------------
YM2612::LockedRegisterKey YM2612::DebuggerView::ControlIDToLockedRegKey(int controlID, unsigned int channelNo)
{
	switch(controlID)
	{
	case IDC_YM2612_DEBUGGER_TL_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_TL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_TL_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_TL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_TL_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_TL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_TL_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_TL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SL_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_SL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SL_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_SL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SL_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_SL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SL_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_SL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_AR_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_AR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_AR_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_AR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_AR_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_AR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_AR_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_AR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_DR_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_DR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_DR_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_DR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_DR_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_DR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_DR_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_DR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SR_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_SR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SR_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_SR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SR_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_SR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SR_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_SR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_RR_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_RR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_RR_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_RR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_RR_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_RR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_RR_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_RR + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SSGEG_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_SSGEG + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SSGEG_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_SSGEG + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SSGEG_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_SSGEG + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_SSGEG_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_SSGEG + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_DT_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_DT + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_DT_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_DT + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_DT_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_DT + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_DT_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_DT + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_MUL_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_MUL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_MUL_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_MUL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_MUL_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_MUL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_MUL_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_MUL + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_KS_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_KS + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_KS_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_KS + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_KS_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_KS + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_KS_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_KS + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_AM_OP1:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP1_AM + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_AM_OP2:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP2_AM + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_AM_OP3:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP3_AM + (operatorCount * channelNo));
	case IDC_YM2612_DEBUGGER_AM_OP4:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_OP4_AM + (operatorCount * channelNo));

	case IDC_YM2612_DEBUGGER_FNUM:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_FNUM + channelNo);
	case IDC_YM2612_DEBUGGER_BLOCK:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_BLOCK + channelNo);
	case IDC_YM2612_DEBUGGER_ALGORITHM:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_ALGORITHM + channelNo);
	case IDC_YM2612_DEBUGGER_FEEDBACK:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_FEEDBACK + channelNo);
	case IDC_YM2612_DEBUGGER_AMS:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_AMS + channelNo);
	case IDC_YM2612_DEBUGGER_PMS:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_PMS + channelNo);
	case IDC_YM2612_DEBUGGER_LEFT:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_LEFT + channelNo);
	case IDC_YM2612_DEBUGGER_RIGHT:
		return (LockedRegisterKey)(YM2612::LOCKEDREG_CH1_RIGHT + channelNo);

	case IDC_YM2612_DEBUGGER_LFOENABLED:
		return YM2612::LOCKEDREG_LFOENABLED;
	case IDC_YM2612_DEBUGGER_LFOFREQ:
		return YM2612::LOCKEDREG_LFOFREQ;
	case IDC_YM2612_DEBUGGER_DACENABLED:
		return YM2612::LOCKEDREG_DACENABLED;
	case IDC_YM2612_DEBUGGER_DACDATA:
		return YM2612::LOCKEDREG_DACDATA;
	case IDC_YM2612_DEBUGGER_CH3MODE:
		return YM2612::LOCKEDREG_CH3MODE;
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP1:
		return YM2612::LOCKEDREG_CH3FNUM_OP1;
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP2:
		return YM2612::LOCKEDREG_CH3FNUM_OP2;
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP3:
		return YM2612::LOCKEDREG_CH3FNUM_OP3;
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP4:
		return YM2612::LOCKEDREG_CH3FNUM_OP4;
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP1:
		return YM2612::LOCKEDREG_CH3BLOCK_OP1;
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP2:
		return YM2612::LOCKEDREG_CH3BLOCK_OP2;
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP3:
		return YM2612::LOCKEDREG_CH3BLOCK_OP3;
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP4:
		return YM2612::LOCKEDREG_CH3BLOCK_OP4;

	case IDC_YM2612_DEBUGGER_TIMERA_ENABLED:
		return YM2612::LOCKEDREG_TIMERA_ENABLE;
	case IDC_YM2612_DEBUGGER_TIMERA_LOADED:
		return YM2612::LOCKEDREG_TIMERA_LOAD;
	case IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW:
		return YM2612::LOCKEDREG_TIMERA_OVERFLOW;
	case IDC_YM2612_DEBUGGER_TIMERA_RATE:
		return YM2612::LOCKEDREG_TIMERA_RATE;
	case IDC_YM2612_DEBUGGER_TIMERA_DATA:
		return YM2612::LOCKEDREG_TIMERA_COUNTER;
	case IDC_YM2612_DEBUGGER_TIMERB_ENABLED:
		return YM2612::LOCKEDREG_TIMERB_ENABLE;
	case IDC_YM2612_DEBUGGER_TIMERB_LOADED:
		return YM2612::LOCKEDREG_TIMERB_LOAD;
	case IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW:
		return YM2612::LOCKEDREG_TIMERB_OVERFLOW;
	case IDC_YM2612_DEBUGGER_TIMERB_RATE:
		return YM2612::LOCKEDREG_TIMERB_RATE;
	case IDC_YM2612_DEBUGGER_TIMERB_DATA:
		return YM2612::LOCKEDREG_TIMERB_COUNTER;
	}
	return YM2612::LOCKEDREG_NONE;
}

//----------------------------------------------------------------------------------------
void YM2612::DebuggerView::LockControl(int controlID, unsigned int channelNo)
{
	AccessTarget accessTarget;
	accessTarget.AccessLatest();
	switch(controlID)
	{
	case IDC_YM2612_DEBUGGER_TL_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetTotalLevelData, device, channelNo, OPERATOR1, device->GetTotalLevelData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_TL_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetTotalLevelData, device, channelNo, OPERATOR2, device->GetTotalLevelData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_TL_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetTotalLevelData, device, channelNo, OPERATOR3, device->GetTotalLevelData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_TL_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetTotalLevelData, device, channelNo, OPERATOR4, device->GetTotalLevelData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SL_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSustainLevelData, device, channelNo, OPERATOR1, device->GetSustainLevelData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SL_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSustainLevelData, device, channelNo, OPERATOR2, device->GetSustainLevelData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SL_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSustainLevelData, device, channelNo, OPERATOR3, device->GetSustainLevelData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SL_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSustainLevelData, device, channelNo, OPERATOR4, device->GetSustainLevelData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_AR_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAttackRateData, device, channelNo, OPERATOR1, device->GetAttackRateData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_AR_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAttackRateData, device, channelNo, OPERATOR2, device->GetAttackRateData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_AR_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAttackRateData, device, channelNo, OPERATOR3, device->GetAttackRateData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_AR_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAttackRateData, device, channelNo, OPERATOR4, device->GetAttackRateData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DR_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDecayRateData, device, channelNo, OPERATOR1, device->GetDecayRateData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DR_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDecayRateData, device, channelNo, OPERATOR2, device->GetDecayRateData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DR_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDecayRateData, device, channelNo, OPERATOR3, device->GetDecayRateData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DR_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDecayRateData, device, channelNo, OPERATOR4, device->GetDecayRateData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SR_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSustainRateData, device, channelNo, OPERATOR1, device->GetSustainRateData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SR_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSustainRateData, device, channelNo, OPERATOR2, device->GetSustainRateData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SR_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSustainRateData, device, channelNo, OPERATOR3, device->GetSustainRateData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SR_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSustainRateData, device, channelNo, OPERATOR4, device->GetSustainRateData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_RR_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetReleaseRateData, device, channelNo, OPERATOR1, device->GetReleaseRateData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_RR_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetReleaseRateData, device, channelNo, OPERATOR2, device->GetReleaseRateData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_RR_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetReleaseRateData, device, channelNo, OPERATOR3, device->GetReleaseRateData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_RR_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetReleaseRateData, device, channelNo, OPERATOR4, device->GetReleaseRateData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SSGEG_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSSGData, device, channelNo, OPERATOR1, device->GetSSGData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SSGEG_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSSGData, device, channelNo, OPERATOR2, device->GetSSGData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SSGEG_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSSGData, device, channelNo, OPERATOR3, device->GetSSGData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_SSGEG_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetSSGData, device, channelNo, OPERATOR4, device->GetSSGData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DT_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDetuneData, device, channelNo, OPERATOR1, device->GetDetuneData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DT_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDetuneData, device, channelNo, OPERATOR2, device->GetDetuneData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DT_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDetuneData, device, channelNo, OPERATOR3, device->GetDetuneData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DT_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDetuneData, device, channelNo, OPERATOR4, device->GetDetuneData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_MUL_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetMultipleData, device, channelNo, OPERATOR1, device->GetMultipleData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_MUL_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetMultipleData, device, channelNo, OPERATOR2, device->GetMultipleData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_MUL_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetMultipleData, device, channelNo, OPERATOR3, device->GetMultipleData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_MUL_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetMultipleData, device, channelNo, OPERATOR4, device->GetMultipleData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_KS_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetKeyScaleData, device, channelNo, OPERATOR1, device->GetKeyScaleData(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_KS_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetKeyScaleData, device, channelNo, OPERATOR2, device->GetKeyScaleData(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_KS_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetKeyScaleData, device, channelNo, OPERATOR3, device->GetKeyScaleData(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_KS_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetKeyScaleData, device, channelNo, OPERATOR4, device->GetKeyScaleData(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_AM_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAmplitudeModulationEnabled, device, channelNo, OPERATOR1, device->GetAmplitudeModulationEnabled(channelNo, OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_AM_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAmplitudeModulationEnabled, device, channelNo, OPERATOR2, device->GetAmplitudeModulationEnabled(channelNo, OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_AM_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAmplitudeModulationEnabled, device, channelNo, OPERATOR3, device->GetAmplitudeModulationEnabled(channelNo, OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_AM_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAmplitudeModulationEnabled, device, channelNo, OPERATOR4, device->GetAmplitudeModulationEnabled(channelNo, OPERATOR4, accessTarget), accessTarget));
		break;

	case IDC_YM2612_DEBUGGER_FNUM:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetFrequencyData, device, channelNo, device->GetFrequencyData(channelNo, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_BLOCK:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetBlockData, device, channelNo, device->GetBlockData(channelNo, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_ALGORITHM:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAlgorithmData, device, channelNo, device->GetAlgorithmData(channelNo, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_FEEDBACK:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetFeedbackData, device, channelNo, device->GetFeedbackData(channelNo, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_AMS:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetAMSData, device, channelNo, device->GetAMSData(channelNo, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_PMS:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetPMSData, device, channelNo, device->GetPMSData(channelNo, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_LEFT:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetOutputLeft, device, channelNo, device->GetOutputLeft(channelNo, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_RIGHT:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetOutputRight, device, channelNo, device->GetOutputRight(channelNo, accessTarget), accessTarget));
		break;

	case IDC_YM2612_DEBUGGER_LFOENABLED:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetLFOEnabled, device, device->GetLFOEnabled(accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_LFOFREQ:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetLFOData, device, device->GetLFOData(accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DACENABLED:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDACEnabled, device, device->GetDACEnabled(accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_DACDATA:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetDACData, device, device->GetDACData(accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_CH3MODE:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetCH3Mode, device, device->GetCH3Mode(accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetFrequencyDataChannel3, device, OPERATOR1, device->GetFrequencyDataChannel3(OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetFrequencyDataChannel3, device, OPERATOR2, device->GetFrequencyDataChannel3(OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetFrequencyDataChannel3, device, OPERATOR3, device->GetFrequencyDataChannel3(OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetFrequencyDataChannel3, device, OPERATOR4, device->GetFrequencyDataChannel3(OPERATOR4, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP1:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetBlockDataChannel3, device, OPERATOR1, device->GetBlockDataChannel3(OPERATOR1, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP2:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetBlockDataChannel3, device, OPERATOR2, device->GetBlockDataChannel3(OPERATOR2, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP3:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetBlockDataChannel3, device, OPERATOR3, device->GetBlockDataChannel3(OPERATOR3, accessTarget), accessTarget));
		break;
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP4:
		device->LockRegister(ControlIDToLockedRegKey(controlID, channelNo), boost::bind(&YM2612::SetBlockDataChannel3, device, OPERATOR4, device->GetBlockDataChannel3(OPERATOR4, accessTarget), accessTarget));
		break;
	}
}
