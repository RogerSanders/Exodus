#include "DebuggerView.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "resource.h"
#include <boost/bind.hpp>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebuggerView::DebuggerView(IUIManager& auiManager, DebuggerViewPresenter& apresenter, IYM2612& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	selectedChannel = 0;
	lockedColor = RGB(255,127,127);
	lockedBrush = CreateSolidBrush(lockedColor);
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_YM2612_DEBUGGER));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
DebuggerView::~DebuggerView()
{
	DeleteObject(lockedBrush);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR DebuggerView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR DebuggerView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Set the channel select radio buttons to their default state
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS1, (selectedChannel == IYM2612::CHANNEL1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS2, (selectedChannel == IYM2612::CHANNEL2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS3, (selectedChannel == IYM2612::CHANNEL3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS4, (selectedChannel == IYM2612::CHANNEL4)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS5, (selectedChannel == IYM2612::CHANNEL5)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_CS6, (selectedChannel == IYM2612::CHANNEL6)? BST_CHECKED: BST_UNCHECKED);

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
	HWND hwndTooltip = CreateTooltipControl(GetAssemblyHandle(), hwnd);
	AddTooltip(GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_LOCKING_TT, lockingTooltip, true);
	AddTooltip(GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_TL_TT, tlTooltip, true);
	AddTooltip(GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_SL_TT, slTooltip, true);
	AddTooltip(GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_AR_TT, arTooltip, true);
	AddTooltip(GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_DR_TT, drTooltip, true);
	AddTooltip(GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_SR_TT, srTooltip, true);
	AddTooltip(GetAssemblyHandle(), hwndTooltip, hwnd, IDC_YM2612_DEBUGGER_RR_TT, rrTooltip, true);

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
INT_PTR DebuggerView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR DebuggerView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	unsigned int channelNo = selectedChannel;

	//Total Level
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TL_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TL_OP1, 2, model.GetTotalLevelData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TL_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TL_OP2, 2, model.GetTotalLevelData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TL_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TL_OP3, 2, model.GetTotalLevelData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TL_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TL_OP4, 2, model.GetTotalLevelData(channelNo, IYM2612::OPERATOR4));
	}

	//Sustain Level
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SL_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SL_OP1, 1, model.GetSustainLevelData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SL_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SL_OP2, 1, model.GetSustainLevelData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SL_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SL_OP3, 1, model.GetSustainLevelData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SL_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SL_OP4, 1, model.GetSustainLevelData(channelNo, IYM2612::OPERATOR4));
	}

	//Attack Rate
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AR_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AR_OP1, 2, model.GetAttackRateData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AR_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AR_OP2, 2, model.GetAttackRateData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AR_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AR_OP3, 2, model.GetAttackRateData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AR_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AR_OP4, 2, model.GetAttackRateData(channelNo, IYM2612::OPERATOR4));
	}

	//Decay Rate
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DR_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DR_OP1, 2, model.GetDecayRateData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DR_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DR_OP2, 2, model.GetDecayRateData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DR_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DR_OP3, 2, model.GetDecayRateData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DR_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DR_OP4, 2, model.GetDecayRateData(channelNo, IYM2612::OPERATOR4));
	}

	//Sustain Rate
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SR_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SR_OP1, 2, model.GetSustainRateData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SR_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SR_OP2, 2, model.GetSustainRateData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SR_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SR_OP3, 2, model.GetSustainRateData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SR_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SR_OP4, 2, model.GetSustainRateData(channelNo, IYM2612::OPERATOR4));
	}

	//Release Rate
	if(currentControlFocus != IDC_YM2612_DEBUGGER_RR_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_RR_OP1, 1, model.GetReleaseRateData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_RR_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_RR_OP2, 1, model.GetReleaseRateData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_RR_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_RR_OP3, 1, model.GetReleaseRateData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_RR_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_RR_OP4, 1, model.GetReleaseRateData(channelNo, IYM2612::OPERATOR4));
	}

	//SSG-EG Mode
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SSGEG_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP1, 1, model.GetSSGData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SSGEG_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP2, 1, model.GetSSGData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SSGEG_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP3, 1, model.GetSSGData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_SSGEG_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_SSGEG_OP4, 1, model.GetSSGData(channelNo, IYM2612::OPERATOR4));
	}

	//Detune
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DT_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DT_OP1, 1, model.GetDetuneData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DT_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DT_OP2, 1, model.GetDetuneData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DT_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DT_OP3, 1, model.GetDetuneData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DT_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DT_OP4, 1, model.GetDetuneData(channelNo, IYM2612::OPERATOR4));
	}

	//Multiple
	if(currentControlFocus != IDC_YM2612_DEBUGGER_MUL_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_MUL_OP1, 1, model.GetMultipleData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_MUL_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_MUL_OP2, 1, model.GetMultipleData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_MUL_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_MUL_OP3, 1, model.GetMultipleData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_MUL_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_MUL_OP4, 1, model.GetMultipleData(channelNo, IYM2612::OPERATOR4));
	}

	//Key Scale
	if(currentControlFocus != IDC_YM2612_DEBUGGER_KS_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_KS_OP1, 1, model.GetKeyScaleData(channelNo, IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_KS_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_KS_OP2, 1, model.GetKeyScaleData(channelNo, IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_KS_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_KS_OP3, 1, model.GetKeyScaleData(channelNo, IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_KS_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_KS_OP4, 1, model.GetKeyScaleData(channelNo, IYM2612::OPERATOR4));
	}

	//AM Enable
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_AM_OP1, (model.GetAmplitudeModulationEnabled(channelNo, IYM2612::OPERATOR1))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_AM_OP2, (model.GetAmplitudeModulationEnabled(channelNo, IYM2612::OPERATOR2))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_AM_OP3, (model.GetAmplitudeModulationEnabled(channelNo, IYM2612::OPERATOR3))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_AM_OP4, (model.GetAmplitudeModulationEnabled(channelNo, IYM2612::OPERATOR4))? BST_CHECKED: BST_UNCHECKED);

	//Channel Registers
	if(currentControlFocus != IDC_YM2612_DEBUGGER_ALGORITHM)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_ALGORITHM, 1, model.GetAlgorithmData(channelNo));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_FEEDBACK)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_FEEDBACK, 1, model.GetFeedbackData(channelNo));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_FNUM)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_FNUM, 3, model.GetFrequencyData(channelNo));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_BLOCK)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_BLOCK, 1, model.GetBlockData(channelNo));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_AMS)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_AMS, 1, model.GetAMSData(channelNo));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_PMS)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_PMS, 1, model.GetPMSData(channelNo));
	}
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_LEFT, (model.GetOutputLeft(channelNo))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_RIGHT, (model.GetOutputRight(channelNo))? BST_CHECKED: BST_UNCHECKED);

	//Channel 3 Frequency
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3MODE)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3MODE, 1, model.GetCH3Mode());
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3FNUM_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP1, 3, model.GetFrequencyDataChannel3(IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3FNUM_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP2, 3, model.GetFrequencyDataChannel3(IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3FNUM_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP3, 3, model.GetFrequencyDataChannel3(IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3FNUM_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3FNUM_OP4, 3, model.GetFrequencyDataChannel3(IYM2612::OPERATOR4));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3BLOCK_OP1)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP1, 1, model.GetBlockDataChannel3(IYM2612::OPERATOR1));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3BLOCK_OP2)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP2, 1, model.GetBlockDataChannel3(IYM2612::OPERATOR2));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3BLOCK_OP3)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP3, 1, model.GetBlockDataChannel3(IYM2612::OPERATOR3));
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CH3BLOCK_OP4)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_CH3BLOCK_OP4, 1, model.GetBlockDataChannel3(IYM2612::OPERATOR4));
	}

	//Timers
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERA_ENABLED, (model.GetTimerAEnable())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERB_ENABLED, (model.GetTimerBEnable())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERA_LOADED, (model.GetTimerALoad())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERB_LOADED, (model.GetTimerBLoad())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW, (model.GetTimerAOverflow())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW, (model.GetTimerBOverflow())? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TIMERA_RATE)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TIMERA_RATE, 3, model.GetTimerAData());
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TIMERB_RATE)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TIMERB_RATE, 2, model.GetTimerBData());
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TIMERA_DATA)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TIMERA_DATA, 3, model.GetTimerACurrentCounter());
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TIMERB_DATA)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_TIMERB_DATA, 2, model.GetTimerBCurrentCounter());
	}

	//LFO
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_LFOENABLED, (model.GetLFOEnabled())? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_YM2612_DEBUGGER_LFOFREQ)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_LFOFREQ, 2, model.GetLFOData());
	}
	std::wstring lfoFrequencyText;
	//##TODO## Update these to clock-independent calculations
	switch(model.GetLFOData())
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
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_DACENABLED, (model.GetDACEnabled())? BST_CHECKED: BST_UNCHECKED);
	if(currentControlFocus != IDC_YM2612_DEBUGGER_DACDATA)
	{
		UpdateDlgItemHex(hwnd, IDC_YM2612_DEBUGGER_DACDATA, 2, model.GetDACData());
	}

	//Key On/Off
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_11, model.GetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_12, model.GetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_13, model.GetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_14, model.GetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR4)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_21, model.GetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_22, model.GetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_23, model.GetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_24, model.GetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR4)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_31, model.GetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_32, model.GetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_33, model.GetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_34, model.GetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR4)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_41, model.GetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_42, model.GetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_43, model.GetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_44, model.GetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR4)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_51, model.GetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_52, model.GetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_53, model.GetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_54, model.GetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR4)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_61, model.GetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_62, model.GetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_63, model.GetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_64, model.GetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR4)? BST_CHECKED: BST_UNCHECKED);

	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_1, (model.GetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR1) && model.GetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR2) && model.GetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR3) && model.GetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR4))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_2, (model.GetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR1) && model.GetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR2) && model.GetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR3) && model.GetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR4))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_3, (model.GetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR1) && model.GetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR2) && model.GetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR3) && model.GetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR4))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_4, (model.GetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR1) && model.GetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR2) && model.GetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR3) && model.GetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR4))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_5, (model.GetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR1) && model.GetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR2) && model.GetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR3) && model.GetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR4))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_DEBUGGER_KEY_6, (model.GetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR1) && model.GetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR2) && model.GetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR3) && model.GetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR4))? BST_CHECKED: BST_UNCHECKED);

	//Clock
	if(currentControlFocus != IDC_YM2612_DEBUGGER_CLOCK)
	{
		UpdateDlgItemFloat(hwnd, IDC_YM2612_DEBUGGER_CLOCK, (float)model.GetExternalClockRate());
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_FMDIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_FMDIVIDE, model.GetFMClockDivider());
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_EGDIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_EGDIVIDE, model.GetEGClockDivider());
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_OUTDIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_OUTDIVIDE, model.GetOutputClockDivider());
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TADIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_TADIVIDE, model.GetTimerAClockDivider());
	}
	if(currentControlFocus != IDC_YM2612_DEBUGGER_TBDIVIDE)
	{
		UpdateDlgItemBin(hwnd, IDC_YM2612_DEBUGGER_TBDIVIDE, model.GetTimerBClockDivider());
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR DebuggerView::msgWM_BOUNCE(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
			switch(controlID)
			{
			default:{
				unsigned int dataID;
				IYM2612::RegisterDataContext registerDataContext;
				IYM2612::ChannelDataContext channelDataContext;
				IYM2612::OperatorDataContext operatorDataContext;
				const IYM2612::DataContext* dataContext;
				if(ControlIDToDataID(controlID, dataID, registerDataContext, channelDataContext, operatorDataContext, &dataContext))
				{
					model.SetGenericDataLocked(dataID, dataContext, !model.GetGenericDataLocked(dataID, dataContext));
				}

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
				unsigned int dataID;
				IYM2612::RegisterDataContext registerDataContext;
				IYM2612::ChannelDataContext channelDataContext;
				IYM2612::OperatorDataContext operatorDataContext;
				const IYM2612::DataContext* dataContext;
				if(ControlIDToDataID(controlID, dataID, registerDataContext, channelDataContext, operatorDataContext, &dataContext))
				{
					model.SetGenericDataLocked(dataID, dataContext, !model.GetGenericDataLocked(dataID, dataContext));
				}

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
				IYM2612::OperatorDataContext operator1KeyStateDataContext(channelNo, IYM2612::OPERATOR1);
				IYM2612::OperatorDataContext operator2KeyStateDataContext(channelNo, IYM2612::OPERATOR2);
				IYM2612::OperatorDataContext operator3KeyStateDataContext(channelNo, IYM2612::OPERATOR3);
				IYM2612::OperatorDataContext operator4KeyStateDataContext(channelNo, IYM2612::OPERATOR4);
				bool newKeyLockingState = (!model.GetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator1KeyStateDataContext) && !model.GetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator2KeyStateDataContext) && !model.GetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator3KeyStateDataContext) && !model.GetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator4KeyStateDataContext));
				model.SetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator1KeyStateDataContext, newKeyLockingState);
				model.SetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator2KeyStateDataContext, newKeyLockingState);
				model.SetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator3KeyStateDataContext, newKeyLockingState);
				model.SetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator4KeyStateDataContext, newKeyLockingState);

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
		case IDC_YM2612_DEBUGGER_TIMERA_LOADED:
		case IDC_YM2612_DEBUGGER_TIMERA_ENABLED:
		case IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW:
		case IDC_YM2612_DEBUGGER_TIMERB_LOADED:
		case IDC_YM2612_DEBUGGER_TIMERB_ENABLED:
		case IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW:
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
			//Handle background colour changes for checkbox controls which are locked
			unsigned int dataID;
			IYM2612::RegisterDataContext registerDataContext;
			IYM2612::ChannelDataContext channelDataContext;
			IYM2612::OperatorDataContext operatorDataContext;
			const IYM2612::DataContext* dataContext;
			if(ControlIDToDataID(controlID, dataID, registerDataContext, channelDataContext, operatorDataContext, &dataContext))
			{
				if(model.GetGenericDataLocked(dataID, dataContext))
				{
					PaintCheckboxHighlight(GetDlgItem(hwnd, controlID));
					bounceMessage->SetResult(TRUE);
				}
			}
			break;}
		case IDC_YM2612_DEBUGGER_KEY_1:
		case IDC_YM2612_DEBUGGER_KEY_2:
		case IDC_YM2612_DEBUGGER_KEY_3:
		case IDC_YM2612_DEBUGGER_KEY_4:
		case IDC_YM2612_DEBUGGER_KEY_5:
		case IDC_YM2612_DEBUGGER_KEY_6:{
			unsigned int channelNo = (controlID - IDC_YM2612_DEBUGGER_KEY_1);
			IYM2612::OperatorDataContext operator1KeyStateDataContext(channelNo, IYM2612::OPERATOR1);
			IYM2612::OperatorDataContext operator2KeyStateDataContext(channelNo, IYM2612::OPERATOR2);
			IYM2612::OperatorDataContext operator3KeyStateDataContext(channelNo, IYM2612::OPERATOR3);
			IYM2612::OperatorDataContext operator4KeyStateDataContext(channelNo, IYM2612::OPERATOR4);
			bool keyLockingState = (model.GetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator1KeyStateDataContext) && model.GetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator2KeyStateDataContext) && model.GetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator3KeyStateDataContext) && model.GetGenericDataLocked(IYM2612::DATASOURCE_KEYSTATE, &operator4KeyStateDataContext));
			if(keyLockingState)
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
INT_PTR DebuggerView::msgWM_CTLCOLOREDIT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Handle background colour changes for edit controls which are locked
	int controlID = GetDlgCtrlID((HWND)lparam);
	unsigned int dataID;
	IYM2612::RegisterDataContext registerDataContext;
	IYM2612::ChannelDataContext channelDataContext;
	IYM2612::OperatorDataContext operatorDataContext;
	const IYM2612::DataContext* dataContext;
	if(ControlIDToDataID(controlID, dataID, registerDataContext, channelDataContext, operatorDataContext, &dataContext))
	{
		if(model.GetGenericDataLocked(dataID, dataContext))
		{
			SetBkColor((HDC)wparam, lockedColor);
			return (BOOL)HandleToLong(lockedBrush);
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR DebuggerView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	unsigned int channelNo = selectedChannel;

	if(HIWORD(wparam) == BN_CLICKED)
	{
		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		case IDC_YM2612_DEBUGGER_INFO_OP1:{
			presenter.OpenOperatorView(channelNo, IYM2612::OPERATOR1);
			break;}
		case IDC_YM2612_DEBUGGER_INFO_OP2:{
			presenter.OpenOperatorView(channelNo, IYM2612::OPERATOR2);
			break;}
		case IDC_YM2612_DEBUGGER_INFO_OP3:{
			presenter.OpenOperatorView(channelNo, IYM2612::OPERATOR3);
			break;}
		case IDC_YM2612_DEBUGGER_INFO_OP4:{
			presenter.OpenOperatorView(channelNo, IYM2612::OPERATOR4);
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
			model.SetAmplitudeModulationEnabled(channelNo, IYM2612::OPERATOR1, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_AM_OP2:{
			model.SetAmplitudeModulationEnabled(channelNo, IYM2612::OPERATOR2, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_AM_OP3:{
			model.SetAmplitudeModulationEnabled(channelNo, IYM2612::OPERATOR3, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_AM_OP4:{
			model.SetAmplitudeModulationEnabled(channelNo, IYM2612::OPERATOR4, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}

		//Left/Right
		case IDC_YM2612_DEBUGGER_LEFT:{
			model.SetOutputLeft(channelNo, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_RIGHT:{
			model.SetOutputRight(channelNo, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}

		//Timers
		case IDC_YM2612_DEBUGGER_TIMERA_ENABLED:{
			model.SetTimerAEnable(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_ENABLED:{
			model.SetTimerBEnable(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERA_LOADED:{
			model.SetTimerALoad(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_LOADED:{
			model.SetTimerBLoad(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW:{
			model.SetTimerAOverflow(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW:{
			model.SetTimerBOverflow(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}

		//LFO
		case IDC_YM2612_DEBUGGER_LFOENABLED:{
			model.SetLFOEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}

		//DAC
		case IDC_YM2612_DEBUGGER_DACENABLED:{
			model.SetDACEnabled(IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
			break;}

		//Key On/Off
		case IDC_YM2612_DEBUGGER_KEY_11:{
			model.SetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_12:{
			model.SetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_13:{
			model.SetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_14:{
			model.SetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_21:{
			model.SetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_22:{
			model.SetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_23:{
			model.SetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_24:{
			model.SetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_31:{
			model.SetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_32:{
			model.SetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_33:{
			model.SetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_34:{
			model.SetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_41:{
			model.SetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_42:{
			model.SetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_43:{
			model.SetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_44:{
			model.SetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_51:{
			model.SetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_52:{
			model.SetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_53:{
			model.SetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_54:{
			model.SetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_61:{
			model.SetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_62:{
			model.SetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_63:{
			model.SetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_64:{
			model.SetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}

		case IDC_YM2612_DEBUGGER_KEY_1:{
			model.SetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL1, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_2:{
			model.SetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL2, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_3:{
			model.SetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL3, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_4:{
			model.SetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL4, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_5:{
			model.SetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL5, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		case IDC_YM2612_DEBUGGER_KEY_6:{
			model.SetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			model.SetKeyState(IYM2612::CHANNEL6, IYM2612::OPERATOR4, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
			break;}
		}

		//If the target register is currently locked, we need to re-create the lock with
		//the updated register value. Note that we have already obtained a lock before the
		//new register value was written.
		unsigned int dataID;
		IYM2612::RegisterDataContext registerDataContext;
		IYM2612::ChannelDataContext channelDataContext;
		IYM2612::OperatorDataContext operatorDataContext;
		const IYM2612::DataContext* dataContext;
		if(ControlIDToDataID(controlID, dataID, registerDataContext, channelDataContext, operatorDataContext, &dataContext))
		{
			model.SetGenericDataLocked(dataID, dataContext, model.GetGenericDataLocked(dataID, dataContext));
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

		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		//Total Level
		case IDC_YM2612_DEBUGGER_TL_OP1:{
			model.SetTotalLevelData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_TL_OP2:{
			model.SetTotalLevelData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_TL_OP3:{
			model.SetTotalLevelData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_TL_OP4:{
			model.SetTotalLevelData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Sustain Level
		case IDC_YM2612_DEBUGGER_SL_OP1:{
			model.SetSustainLevelData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_SL_OP2:{
			model.SetSustainLevelData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_SL_OP3:{
			model.SetSustainLevelData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_SL_OP4:{
			model.SetSustainLevelData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Attack Rate
		case IDC_YM2612_DEBUGGER_AR_OP1:{
			model.SetAttackRateData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_AR_OP2:{
			model.SetAttackRateData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_AR_OP3:{
			model.SetAttackRateData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_AR_OP4:{
			model.SetAttackRateData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Decay Rate
		case IDC_YM2612_DEBUGGER_DR_OP1:{
			model.SetDecayRateData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_DR_OP2:{
			model.SetDecayRateData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_DR_OP3:{
			model.SetDecayRateData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_DR_OP4:{
			model.SetDecayRateData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Sustain Rate
		case IDC_YM2612_DEBUGGER_SR_OP1:{
			model.SetSustainRateData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_SR_OP2:{
			model.SetSustainRateData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_SR_OP3:{
			model.SetSustainRateData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_SR_OP4:{
			model.SetSustainRateData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Release Rate
		case IDC_YM2612_DEBUGGER_RR_OP1:{
			model.SetReleaseRateData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_RR_OP2:{
			model.SetReleaseRateData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_RR_OP3:{
			model.SetReleaseRateData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_RR_OP4:{
			model.SetReleaseRateData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//SSG-EG Mode
		case IDC_YM2612_DEBUGGER_SSGEG_OP1:{
			model.SetSSGData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_SSGEG_OP2:{
			model.SetSSGData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_SSGEG_OP3:{
			model.SetSSGData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_SSGEG_OP4:{
			model.SetSSGData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Detune
		case IDC_YM2612_DEBUGGER_DT_OP1:{
			model.SetDetuneData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_DT_OP2:{
			model.SetDetuneData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_DT_OP3:{
			model.SetDetuneData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_DT_OP4:{
			model.SetDetuneData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Multiple
		case IDC_YM2612_DEBUGGER_MUL_OP1:{
			model.SetMultipleData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_MUL_OP2:{
			model.SetMultipleData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_MUL_OP3:{
			model.SetMultipleData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_MUL_OP4:{
			model.SetMultipleData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Key Scale
		case IDC_YM2612_DEBUGGER_KS_OP1:{
			model.SetKeyScaleData(channelNo, IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_KS_OP2:{
			model.SetKeyScaleData(channelNo, IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_KS_OP3:{
			model.SetKeyScaleData(channelNo, IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_KS_OP4:{
			model.SetKeyScaleData(channelNo, IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Channel Registers
		case IDC_YM2612_DEBUGGER_ALGORITHM:{
			model.SetAlgorithmData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_FEEDBACK:{
			model.SetFeedbackData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_FNUM:{
			model.SetFrequencyData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_BLOCK:{
			model.SetBlockData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_AMS:{
			model.SetAMSData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_PMS:{
			model.SetPMSData(channelNo, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Channel 3 Frequency
		case IDC_YM2612_DEBUGGER_CH3MODE:{
			model.SetCH3Mode(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_CH3FNUM_OP1:{
			model.SetFrequencyDataChannel3(IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_CH3FNUM_OP2:{
			model.SetFrequencyDataChannel3(IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_CH3FNUM_OP3:{
			model.SetFrequencyDataChannel3(IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_CH3FNUM_OP4:{
			model.SetFrequencyDataChannel3(IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_CH3BLOCK_OP1:{
			model.SetBlockDataChannel3(IYM2612::OPERATOR1, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_CH3BLOCK_OP2:{
			model.SetBlockDataChannel3(IYM2612::OPERATOR2, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_CH3BLOCK_OP3:{
			model.SetBlockDataChannel3(IYM2612::OPERATOR3, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_CH3BLOCK_OP4:{
			model.SetBlockDataChannel3(IYM2612::OPERATOR4, GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Timers
		case IDC_YM2612_DEBUGGER_TIMERA_RATE:{
			model.SetTimerAData(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_RATE:{
			model.SetTimerBData(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_TIMERA_DATA:{
			model.SetTimerACurrentCounter(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_TIMERB_DATA:{
			model.SetTimerBCurrentCounter(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//LFO
		case IDC_YM2612_DEBUGGER_LFOFREQ:{
			model.SetLFOData(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//DAC
		case IDC_YM2612_DEBUGGER_DACDATA:{
			model.SetDACData(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;}

		//Clock
		case IDC_YM2612_DEBUGGER_CLOCK:{
			model.SetExternalClockRate(GetDlgItemDouble(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_FMDIVIDE:{
			model.SetFMClockDivider(GetDlgItemBin(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_EGDIVIDE:{
			model.SetEGClockDivider(GetDlgItemBin(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_OUTDIVIDE:{
			model.SetOutputClockDivider(GetDlgItemBin(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_TADIVIDE:{
			model.SetTimerAClockDivider(GetDlgItemBin(hwnd, LOWORD(wparam)));
			break;}
		case IDC_YM2612_DEBUGGER_TBDIVIDE:{
			model.SetTimerBClockDivider(GetDlgItemBin(hwnd, LOWORD(wparam)));
			break;}
		}

		//If the target register is currently locked, we need to re-create the lock with
		//the updated register value. Note that we have already obtained a lock before the
		//new register value was written.
		unsigned int dataID;
		IYM2612::RegisterDataContext registerDataContext;
		IYM2612::ChannelDataContext channelDataContext;
		IYM2612::OperatorDataContext operatorDataContext;
		const IYM2612::DataContext* dataContext;
		if(ControlIDToDataID(controlID, dataID, registerDataContext, channelDataContext, operatorDataContext, &dataContext))
		{
			model.SetGenericDataLocked(dataID, dataContext, model.GetGenericDataLocked(dataID, dataContext));
		}
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Register locking functions
//----------------------------------------------------------------------------------------
bool DebuggerView::ControlIDToDataID(int controlID, unsigned int& genericDataID, IYM2612::RegisterDataContext& registerDataContext, IYM2612::ChannelDataContext& channelDataContext, IYM2612::OperatorDataContext& operatorDataContext, const IGenericAccess::DataContext** dataContext)
{
	*dataContext = 0;
	switch(controlID)
	{
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
		genericDataID = IYM2612::DATASOURCE_KEYSTATE;
		operatorDataContext.channelNo = (controlID - IDC_YM2612_DEBUGGER_KEY_11) / IYM2612::operatorCount;
		operatorDataContext.operatorNo = (controlID - IDC_YM2612_DEBUGGER_KEY_11) % IYM2612::operatorCount;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;}

	case IDC_YM2612_DEBUGGER_TL_OP1:
	case IDC_YM2612_DEBUGGER_TL_OP2:
	case IDC_YM2612_DEBUGGER_TL_OP3:
	case IDC_YM2612_DEBUGGER_TL_OP4:
		genericDataID = IYM2612::DATASOURCE_TOTALLEVELDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_TL_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_SL_OP1:
	case IDC_YM2612_DEBUGGER_SL_OP2:
	case IDC_YM2612_DEBUGGER_SL_OP3:
	case IDC_YM2612_DEBUGGER_SL_OP4:
		genericDataID = IYM2612::DATASOURCE_SUSTAINLEVELDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_SL_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_AR_OP1:
	case IDC_YM2612_DEBUGGER_AR_OP2:
	case IDC_YM2612_DEBUGGER_AR_OP3:
	case IDC_YM2612_DEBUGGER_AR_OP4:
		genericDataID = IYM2612::DATASOURCE_ATTACKRATEDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_AR_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_DR_OP1:
	case IDC_YM2612_DEBUGGER_DR_OP2:
	case IDC_YM2612_DEBUGGER_DR_OP3:
	case IDC_YM2612_DEBUGGER_DR_OP4:
		genericDataID = IYM2612::DATASOURCE_DECAYRATEDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_DR_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_SR_OP1:
	case IDC_YM2612_DEBUGGER_SR_OP2:
	case IDC_YM2612_DEBUGGER_SR_OP3:
	case IDC_YM2612_DEBUGGER_SR_OP4:
		genericDataID = IYM2612::DATASOURCE_SUSTAINRATEDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_SR_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_RR_OP1:
	case IDC_YM2612_DEBUGGER_RR_OP2:
	case IDC_YM2612_DEBUGGER_RR_OP3:
	case IDC_YM2612_DEBUGGER_RR_OP4:
		genericDataID = IYM2612::DATASOURCE_RELEASERATEDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_RR_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_SSGEG_OP1:
	case IDC_YM2612_DEBUGGER_SSGEG_OP2:
	case IDC_YM2612_DEBUGGER_SSGEG_OP3:
	case IDC_YM2612_DEBUGGER_SSGEG_OP4:
		genericDataID = IYM2612::DATASOURCE_SSGDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_SSGEG_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_DT_OP1:
	case IDC_YM2612_DEBUGGER_DT_OP2:
	case IDC_YM2612_DEBUGGER_DT_OP3:
	case IDC_YM2612_DEBUGGER_DT_OP4:
		genericDataID = IYM2612::DATASOURCE_DETUNEDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_DT_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_MUL_OP1:
	case IDC_YM2612_DEBUGGER_MUL_OP2:
	case IDC_YM2612_DEBUGGER_MUL_OP3:
	case IDC_YM2612_DEBUGGER_MUL_OP4:
		genericDataID = IYM2612::DATASOURCE_MULTIPLEDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_MUL_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_KS_OP1:
	case IDC_YM2612_DEBUGGER_KS_OP2:
	case IDC_YM2612_DEBUGGER_KS_OP3:
	case IDC_YM2612_DEBUGGER_KS_OP4:
		genericDataID = IYM2612::DATASOURCE_KEYSCALEDATA;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_KS_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_AM_OP1:
	case IDC_YM2612_DEBUGGER_AM_OP2:
	case IDC_YM2612_DEBUGGER_AM_OP3:
	case IDC_YM2612_DEBUGGER_AM_OP4:
		genericDataID = IYM2612::DATASOURCE_AMPLITUDEMODULATIONENABLED;
		operatorDataContext.channelNo = selectedChannel;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_AM_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;

	case IDC_YM2612_DEBUGGER_FNUM:
		genericDataID = IYM2612::DATASOURCE_FREQUENCYDATA;
		channelDataContext.channelNo = selectedChannel;
		*dataContext = (IGenericAccess::DataContext*)&channelDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_BLOCK:
		genericDataID = IYM2612::DATASOURCE_BLOCKDATA;
		channelDataContext.channelNo = selectedChannel;
		*dataContext = (IGenericAccess::DataContext*)&channelDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_ALGORITHM:
		genericDataID = IYM2612::DATASOURCE_ALGORITHMDATA;
		channelDataContext.channelNo = selectedChannel;
		*dataContext = (IGenericAccess::DataContext*)&channelDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_FEEDBACK:
		genericDataID = IYM2612::DATASOURCE_FEEDBACKDATA;
		channelDataContext.channelNo = selectedChannel;
		*dataContext = (IGenericAccess::DataContext*)&channelDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_AMS:
		genericDataID = IYM2612::DATASOURCE_AMSDATA;
		channelDataContext.channelNo = selectedChannel;
		*dataContext = (IGenericAccess::DataContext*)&channelDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_PMS:
		genericDataID = IYM2612::DATASOURCE_PMSDATA;
		channelDataContext.channelNo = selectedChannel;
		*dataContext = (IGenericAccess::DataContext*)&channelDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_LEFT:
		genericDataID = IYM2612::DATASOURCE_OUTPUTLEFT;
		channelDataContext.channelNo = selectedChannel;
		*dataContext = (IGenericAccess::DataContext*)&channelDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_RIGHT:
		genericDataID = IYM2612::DATASOURCE_OUTPUTRIGHT;
		channelDataContext.channelNo = selectedChannel;
		*dataContext = (IGenericAccess::DataContext*)&channelDataContext;
		return true;

	case IDC_YM2612_DEBUGGER_LFOENABLED:
		genericDataID = IYM2612::DATASOURCE_LFOENABLED;
		return true;
	case IDC_YM2612_DEBUGGER_LFOFREQ:
		genericDataID = IYM2612::DATASOURCE_LFODATA;
		return true;
	case IDC_YM2612_DEBUGGER_DACENABLED:
		genericDataID = IYM2612::DATASOURCE_DACENABLED;
		return true;
	case IDC_YM2612_DEBUGGER_DACDATA:
		genericDataID = IYM2612::DATASOURCE_DACDATA;
		return true;
	case IDC_YM2612_DEBUGGER_CH3MODE:
		genericDataID = IYM2612::DATASOURCE_CH3MODE;
		return true;

	case IDC_YM2612_DEBUGGER_CH3FNUM_OP1:
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP2:
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP3:
	case IDC_YM2612_DEBUGGER_CH3FNUM_OP4:
		genericDataID = IYM2612::DATASOURCE_FREQUENCYDATACHANNEL3;
		operatorDataContext.channelNo = IYM2612::CHANNEL3;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_CH3FNUM_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP1:
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP2:
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP3:
	case IDC_YM2612_DEBUGGER_CH3BLOCK_OP4:
		genericDataID = IYM2612::DATASOURCE_BLOCKDATACHANNEL3;
		operatorDataContext.channelNo = IYM2612::CHANNEL3;
		operatorDataContext.operatorNo = controlID - IDC_YM2612_DEBUGGER_CH3BLOCK_OP1;
		*dataContext = (IGenericAccess::DataContext*)&operatorDataContext;
		return true;

	case IDC_YM2612_DEBUGGER_TIMERA_ENABLED:
		genericDataID = IYM2612::DATASOURCE_TIMERAENABLE;
		return true;
	case IDC_YM2612_DEBUGGER_TIMERA_LOADED:
		genericDataID = IYM2612::DATASOURCE_TIMERALOAD;
		return true;
	case IDC_YM2612_DEBUGGER_TIMERA_OVERFLOW:
		genericDataID = IYM2612::DATASOURCE_TIMERAOVERFLOW;
		return true;
	case IDC_YM2612_DEBUGGER_TIMERA_RATE:
		genericDataID = IYM2612::DATASOURCE_TIMERADATA;
		return true;
	case IDC_YM2612_DEBUGGER_TIMERA_CURRENT:
		genericDataID = IYM2612::DATASOURCE_TIMERACURRENTCOUNTER;
		return true;
	case IDC_YM2612_DEBUGGER_TIMERB_ENABLED:
		genericDataID = IYM2612::DATASOURCE_TIMERBENABLE;
		return true;
	case IDC_YM2612_DEBUGGER_TIMERB_LOADED:
		genericDataID = IYM2612::DATASOURCE_TIMERBLOAD;
		return true;
	case IDC_YM2612_DEBUGGER_TIMERB_OVERFLOW:
		genericDataID = IYM2612::DATASOURCE_TIMERBOVERFLOW;
		return true;
	case IDC_YM2612_DEBUGGER_TIMERB_RATE:
		genericDataID = IYM2612::DATASOURCE_TIMERBDATA;
		return true;
	case IDC_YM2612_DEBUGGER_TIMERB_CURRENT:
		genericDataID = IYM2612::DATASOURCE_TIMERBCURRENTCOUNTER;
		return true;
	}
	return false;
}
