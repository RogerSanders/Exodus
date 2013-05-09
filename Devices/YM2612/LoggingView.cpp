#include "LoggingView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612::LoggingView::LoggingView(YM2612* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"WAV File Logging");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_YM2612_LOGGING));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR YM2612::LoggingView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR YM2612::LoggingView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	HANDLE folderIconHandle = LoadImage((HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, 0, 0, LR_SHARED);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C1), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C1OP1), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C1OP2), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C1OP3), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C1OP4), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C2), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C2OP1), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C2OP2), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C2OP3), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C2OP4), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C3), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C3OP1), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C3OP2), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C3OP3), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C3OP4), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C4), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C4OP1), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C4OP2), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C4OP3), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C4OP4), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C5), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C5OP1), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C5OP2), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C5OP3), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C5OP4), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C6), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C6OP1), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C6OP2), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C6OP3), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C6OP4), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);

	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH, &device->wavLoggingPath[CHANNEL1]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1, &device->wavLoggingChannelPath[CHANNEL1][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1OP1, &device->wavLoggingOperatorPath[CHANNEL1][OPERATOR1][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1OP2, &device->wavLoggingOperatorPath[CHANNEL1][OPERATOR2][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1OP3, &device->wavLoggingOperatorPath[CHANNEL1][OPERATOR3][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1OP4, &device->wavLoggingOperatorPath[CHANNEL1][OPERATOR4][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2, &device->wavLoggingChannelPath[CHANNEL2][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2OP1, &device->wavLoggingOperatorPath[CHANNEL2][OPERATOR1][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2OP2, &device->wavLoggingOperatorPath[CHANNEL2][OPERATOR2][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2OP3, &device->wavLoggingOperatorPath[CHANNEL2][OPERATOR3][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2OP4, &device->wavLoggingOperatorPath[CHANNEL2][OPERATOR4][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3, &device->wavLoggingChannelPath[CHANNEL3][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3OP1, &device->wavLoggingOperatorPath[CHANNEL3][OPERATOR1][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3OP2, &device->wavLoggingOperatorPath[CHANNEL3][OPERATOR2][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3OP3, &device->wavLoggingOperatorPath[CHANNEL3][OPERATOR3][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3OP4, &device->wavLoggingOperatorPath[CHANNEL3][OPERATOR4][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4, &device->wavLoggingChannelPath[CHANNEL4][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4OP1, &device->wavLoggingOperatorPath[CHANNEL4][OPERATOR1][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4OP2, &device->wavLoggingOperatorPath[CHANNEL4][OPERATOR2][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4OP3, &device->wavLoggingOperatorPath[CHANNEL4][OPERATOR3][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4OP4, &device->wavLoggingOperatorPath[CHANNEL4][OPERATOR4][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5, &device->wavLoggingChannelPath[CHANNEL5][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5OP1, &device->wavLoggingOperatorPath[CHANNEL5][OPERATOR1][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5OP2, &device->wavLoggingOperatorPath[CHANNEL5][OPERATOR2][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5OP3, &device->wavLoggingOperatorPath[CHANNEL5][OPERATOR3][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5OP4, &device->wavLoggingOperatorPath[CHANNEL5][OPERATOR4][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6, &device->wavLoggingChannelPath[CHANNEL6][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6OP1, &device->wavLoggingOperatorPath[CHANNEL6][OPERATOR1][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6OP2, &device->wavLoggingOperatorPath[CHANNEL6][OPERATOR2][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6OP3, &device->wavLoggingOperatorPath[CHANNEL6][OPERATOR3][0]);
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6OP4, &device->wavLoggingOperatorPath[CHANNEL6][OPERATOR4][0]);

	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED, (device->wavLoggingEnabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C1, (device->wavLoggingChannelEnabled[CHANNEL1])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C1OP1, (device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR1])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C1OP2, (device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR2])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C1OP3, (device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR3])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C1OP4, (device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR4])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C2, (device->wavLoggingChannelEnabled[CHANNEL2])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C2OP1, (device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR1])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C2OP2, (device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR2])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C2OP3, (device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR3])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C2OP4, (device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR4])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C3, (device->wavLoggingChannelEnabled[CHANNEL3])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C3OP1, (device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR1])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C3OP2, (device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR2])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C3OP3, (device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR3])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C3OP4, (device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR4])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C4, (device->wavLoggingChannelEnabled[CHANNEL4])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C4OP1, (device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR1])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C4OP2, (device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR2])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C4OP3, (device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR3])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C4OP4, (device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR4])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C5, (device->wavLoggingChannelEnabled[CHANNEL5])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C5OP1, (device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR1])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C5OP2, (device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR2])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C5OP3, (device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR3])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C5OP4, (device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR4])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C6, (device->wavLoggingChannelEnabled[CHANNEL6])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C6OP1, (device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR1])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C6OP2, (device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR2])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C6OP3, (device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR3])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C6OP4, (device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR4])? BST_CHECKED: BST_UNCHECKED);
	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::LoggingView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR YM2612::LoggingView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		//Calculate the FM clock period
		double fmClock = (device->externalClockRate / device->fmClockDivider) / device->outputClockDivider;

		if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED)
		{
			device->wavLoggingEnabled = ToggleLoggingEnabledState(device->wavLog, device->wavLoggingPath, device->wavLoggingEnabled, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 2, 16, (unsigned int)fmClock);
//			device->wavLoggingEnabled = ToggleLoggingEnabledState(device->wavLog, device->wavLoggingPath, device->wavLoggingEnabled, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 2, 16, (unsigned int)fmClock * channelCount);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C1)
		{
			device->wavLoggingChannelEnabled[CHANNEL1] = ToggleLoggingEnabledState(device->wavLogChannel[CHANNEL1], device->wavLoggingChannelPath[CHANNEL1], device->wavLoggingChannelEnabled[CHANNEL1], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 2, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C1OP1)
		{
			device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR1] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL1][OPERATOR1], device->wavLoggingOperatorPath[CHANNEL1][OPERATOR1], device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR1], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C1OP2)
		{
			device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR2] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL1][OPERATOR2], device->wavLoggingOperatorPath[CHANNEL1][OPERATOR2], device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR2], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C1OP3)
		{
			device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR3] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL1][OPERATOR3], device->wavLoggingOperatorPath[CHANNEL1][OPERATOR3], device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR3], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C1OP4)
		{
			device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR4] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL1][OPERATOR4], device->wavLoggingOperatorPath[CHANNEL1][OPERATOR4], device->wavLoggingOperatorEnabled[CHANNEL1][OPERATOR4], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C2)
		{
			device->wavLoggingChannelEnabled[CHANNEL2] = ToggleLoggingEnabledState(device->wavLogChannel[CHANNEL2], device->wavLoggingChannelPath[CHANNEL2], device->wavLoggingChannelEnabled[CHANNEL2], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 2, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C2OP1)
		{
			device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR1] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL2][OPERATOR1], device->wavLoggingOperatorPath[CHANNEL2][OPERATOR1], device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR1], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C2OP2)
		{
			device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR2] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL2][OPERATOR2], device->wavLoggingOperatorPath[CHANNEL2][OPERATOR2], device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR2], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C2OP3)
		{
			device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR3] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL2][OPERATOR3], device->wavLoggingOperatorPath[CHANNEL2][OPERATOR3], device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR3], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C2OP4)
		{
			device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR4] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL2][OPERATOR4], device->wavLoggingOperatorPath[CHANNEL2][OPERATOR4], device->wavLoggingOperatorEnabled[CHANNEL2][OPERATOR4], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C3)
		{
			device->wavLoggingChannelEnabled[CHANNEL3] = ToggleLoggingEnabledState(device->wavLogChannel[CHANNEL3], device->wavLoggingChannelPath[CHANNEL3], device->wavLoggingChannelEnabled[CHANNEL3], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 2, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C3OP1)
		{
			device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR1] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL3][OPERATOR1], device->wavLoggingOperatorPath[CHANNEL3][OPERATOR1], device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR1], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C3OP2)
		{
			device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR2] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL3][OPERATOR2], device->wavLoggingOperatorPath[CHANNEL3][OPERATOR2], device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR2], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C3OP3)
		{
			device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR3] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL3][OPERATOR3], device->wavLoggingOperatorPath[CHANNEL3][OPERATOR3], device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR3], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C3OP4)
		{
			device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR4] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL3][OPERATOR4], device->wavLoggingOperatorPath[CHANNEL3][OPERATOR4], device->wavLoggingOperatorEnabled[CHANNEL3][OPERATOR4], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C4)
		{
			device->wavLoggingChannelEnabled[CHANNEL4] = ToggleLoggingEnabledState(device->wavLogChannel[CHANNEL4], device->wavLoggingChannelPath[CHANNEL4], device->wavLoggingChannelEnabled[CHANNEL4], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 2, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C4OP1)
		{
			device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR1] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL4][OPERATOR1], device->wavLoggingOperatorPath[CHANNEL4][OPERATOR1], device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR1], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C4OP2)
		{
			device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR2] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL4][OPERATOR2], device->wavLoggingOperatorPath[CHANNEL4][OPERATOR2], device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR2], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C4OP3)
		{
			device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR3] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL4][OPERATOR3], device->wavLoggingOperatorPath[CHANNEL4][OPERATOR3], device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR3], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C4OP4)
		{
			device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR4] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL4][OPERATOR4], device->wavLoggingOperatorPath[CHANNEL4][OPERATOR4], device->wavLoggingOperatorEnabled[CHANNEL4][OPERATOR4], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C5)
		{
			device->wavLoggingChannelEnabled[CHANNEL5] = ToggleLoggingEnabledState(device->wavLogChannel[CHANNEL5], device->wavLoggingChannelPath[CHANNEL5], device->wavLoggingChannelEnabled[CHANNEL5], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 2, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C5OP1)
		{
			device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR1] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL5][OPERATOR1], device->wavLoggingOperatorPath[CHANNEL5][OPERATOR1], device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR1], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C5OP2)
		{
			device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR2] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL5][OPERATOR2], device->wavLoggingOperatorPath[CHANNEL5][OPERATOR2], device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR2], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C5OP3)
		{
			device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR3] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL5][OPERATOR3], device->wavLoggingOperatorPath[CHANNEL5][OPERATOR3], device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR3], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C5OP4)
		{
			device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR4] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL5][OPERATOR4], device->wavLoggingOperatorPath[CHANNEL5][OPERATOR4], device->wavLoggingOperatorEnabled[CHANNEL5][OPERATOR4], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C6)
		{
			device->wavLoggingChannelEnabled[CHANNEL6] = ToggleLoggingEnabledState(device->wavLogChannel[CHANNEL6], device->wavLoggingChannelPath[CHANNEL6], device->wavLoggingChannelEnabled[CHANNEL6], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 2, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C6OP1)
		{
			device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR1] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL6][OPERATOR1], device->wavLoggingOperatorPath[CHANNEL6][OPERATOR1], device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR1], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C6OP2)
		{
			device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR2] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL6][OPERATOR2], device->wavLoggingOperatorPath[CHANNEL6][OPERATOR2], device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR2], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C6OP3)
		{
			device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR3] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL6][OPERATOR3], device->wavLoggingOperatorPath[CHANNEL6][OPERATOR3], device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR3], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_ENABLED_C6OP4)
		{
			device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR4] = ToggleLoggingEnabledState(device->wavLogOperator[CHANNEL6][OPERATOR4], device->wavLoggingOperatorPath[CHANNEL6][OPERATOR4], device->wavLoggingOperatorEnabled[CHANNEL6][OPERATOR4], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)fmClock);
		}

		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT)
		{
			SelectLoggingPath(hwnd, device->wavLoggingPath);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH, &device->wavLoggingPath[0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C1)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[CHANNEL1]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1, &device->wavLoggingChannelPath[CHANNEL1][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C1OP1)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL1][OPERATOR1]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1OP1, &device->wavLoggingOperatorPath[CHANNEL1][OPERATOR1][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C1OP2)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL1][OPERATOR2]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1OP2, &device->wavLoggingOperatorPath[CHANNEL1][OPERATOR2][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C1OP3)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL1][OPERATOR3]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1OP3, &device->wavLoggingOperatorPath[CHANNEL1][OPERATOR3][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C1OP4)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL1][OPERATOR4]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1OP4, &device->wavLoggingOperatorPath[CHANNEL1][OPERATOR4][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C2)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[CHANNEL2]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2, &device->wavLoggingChannelPath[CHANNEL2][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C2OP1)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL2][OPERATOR1]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2OP1, &device->wavLoggingOperatorPath[CHANNEL2][OPERATOR1][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C2OP2)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL2][OPERATOR2]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2OP2, &device->wavLoggingOperatorPath[CHANNEL2][OPERATOR2][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C2OP3)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL2][OPERATOR3]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2OP3, &device->wavLoggingOperatorPath[CHANNEL2][OPERATOR3][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C2OP4)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL2][OPERATOR4]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C2OP4, &device->wavLoggingOperatorPath[CHANNEL2][OPERATOR4][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C3)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[CHANNEL3]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3, &device->wavLoggingChannelPath[CHANNEL3][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C3OP1)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL3][OPERATOR1]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3OP1, &device->wavLoggingOperatorPath[CHANNEL3][OPERATOR1][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C3OP2)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL3][OPERATOR2]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3OP2, &device->wavLoggingOperatorPath[CHANNEL3][OPERATOR2][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C3OP3)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL3][OPERATOR3]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3OP3, &device->wavLoggingOperatorPath[CHANNEL3][OPERATOR3][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C3OP4)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL3][OPERATOR4]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C3OP4, &device->wavLoggingOperatorPath[CHANNEL3][OPERATOR4][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C4)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[CHANNEL4]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4, &device->wavLoggingChannelPath[CHANNEL4][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C4OP1)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL4][OPERATOR1]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4OP1, &device->wavLoggingOperatorPath[CHANNEL4][OPERATOR1][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C4OP2)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL4][OPERATOR2]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4OP2, &device->wavLoggingOperatorPath[CHANNEL4][OPERATOR2][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C4OP3)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL4][OPERATOR3]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4OP3, &device->wavLoggingOperatorPath[CHANNEL4][OPERATOR3][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C4OP4)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL4][OPERATOR4]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C4OP4, &device->wavLoggingOperatorPath[CHANNEL4][OPERATOR4][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C5)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[CHANNEL5]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5, &device->wavLoggingChannelPath[CHANNEL5][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C5OP1)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL5][OPERATOR1]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5OP1, &device->wavLoggingOperatorPath[CHANNEL5][OPERATOR1][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C5OP2)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL5][OPERATOR2]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5OP2, &device->wavLoggingOperatorPath[CHANNEL5][OPERATOR2][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C5OP3)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL5][OPERATOR3]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5OP3, &device->wavLoggingOperatorPath[CHANNEL5][OPERATOR3][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C5OP4)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL5][OPERATOR4]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C5OP4, &device->wavLoggingOperatorPath[CHANNEL5][OPERATOR4][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C6)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[CHANNEL6]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6, &device->wavLoggingChannelPath[CHANNEL6][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C6OP1)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL6][OPERATOR1]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6OP1, &device->wavLoggingOperatorPath[CHANNEL6][OPERATOR1][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C6OP2)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL6][OPERATOR2]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6OP2, &device->wavLoggingOperatorPath[CHANNEL6][OPERATOR2][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C6OP3)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL6][OPERATOR3]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6OP3, &device->wavLoggingOperatorPath[CHANNEL6][OPERATOR3][0]);
		}
		else if(LOWORD(wparam) == IDC_YM2612_LOGGING_SELECT_C6OP4)
		{
			SelectLoggingPath(hwnd, device->wavLoggingOperatorPath[CHANNEL6][OPERATOR4]);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C6OP4, &device->wavLoggingOperatorPath[CHANNEL6][OPERATOR4][0]);
		}
	}
	else if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(currentControlFocus == LOWORD(wparam))
		{
			currentControlFocus = 0;
		}
		if(newText != previousText)
		{
			if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH)
			{
				device->wavLoggingPath = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C1)
			{
				device->wavLoggingChannelPath[CHANNEL1] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C1);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C1OP1)
			{
				device->wavLoggingOperatorPath[CHANNEL1][OPERATOR1] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C1OP1);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C1OP2)
			{
				device->wavLoggingOperatorPath[CHANNEL1][OPERATOR2] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C1OP2);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C1OP3)
			{
				device->wavLoggingOperatorPath[CHANNEL1][OPERATOR3] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C1OP3);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C1OP4)
			{
				device->wavLoggingOperatorPath[CHANNEL1][OPERATOR4] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C1OP4);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C2)
			{
				device->wavLoggingChannelPath[CHANNEL2] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C2);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C2OP1)
			{
				device->wavLoggingOperatorPath[CHANNEL2][OPERATOR1] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C2OP1);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C2OP2)
			{
				device->wavLoggingOperatorPath[CHANNEL2][OPERATOR2] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C2OP2);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C2OP3)
			{
				device->wavLoggingOperatorPath[CHANNEL2][OPERATOR3] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C2OP3);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C2OP4)
			{
				device->wavLoggingOperatorPath[CHANNEL2][OPERATOR4] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C2OP4);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C3)
			{
				device->wavLoggingChannelPath[CHANNEL3] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C3);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C3OP1)
			{
				device->wavLoggingOperatorPath[CHANNEL3][OPERATOR1] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C3OP1);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C3OP2)
			{
				device->wavLoggingOperatorPath[CHANNEL3][OPERATOR2] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C3OP2);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C3OP3)
			{
				device->wavLoggingOperatorPath[CHANNEL3][OPERATOR3] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C3OP3);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C3OP4)
			{
				device->wavLoggingOperatorPath[CHANNEL3][OPERATOR4] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C3OP4);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C4)
			{
				device->wavLoggingChannelPath[CHANNEL4] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C4);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C4OP1)
			{
				device->wavLoggingOperatorPath[CHANNEL4][OPERATOR1] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C4OP1);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C4OP2)
			{
				device->wavLoggingOperatorPath[CHANNEL4][OPERATOR2] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C4OP2);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C4OP3)
			{
				device->wavLoggingOperatorPath[CHANNEL4][OPERATOR3] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C4OP3);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C4OP4)
			{
				device->wavLoggingOperatorPath[CHANNEL4][OPERATOR4] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C4OP4);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C5)
			{
				device->wavLoggingChannelPath[CHANNEL5] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C5);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C5OP1)
			{
				device->wavLoggingOperatorPath[CHANNEL5][OPERATOR1] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C5OP1);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C5OP2)
			{
				device->wavLoggingOperatorPath[CHANNEL5][OPERATOR2] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C5OP2);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C5OP3)
			{
				device->wavLoggingOperatorPath[CHANNEL5][OPERATOR3] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C5OP3);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C5OP4)
			{
				device->wavLoggingOperatorPath[CHANNEL5][OPERATOR4] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C5OP4);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C6)
			{
				device->wavLoggingChannelPath[CHANNEL6] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C6);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C6OP1)
			{
				device->wavLoggingOperatorPath[CHANNEL6][OPERATOR1] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C6OP1);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C6OP2)
			{
				device->wavLoggingOperatorPath[CHANNEL6][OPERATOR2] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C6OP2);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C6OP3)
			{
				device->wavLoggingOperatorPath[CHANNEL6][OPERATOR3] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C6OP3);
			}
			else if(LOWORD(wparam) == IDC_YM2612_LOGGING_PATH_C6OP4)
			{
				device->wavLoggingOperatorPath[CHANNEL6][OPERATOR4] = GetDlgItemString(hwnd, IDC_YM2612_LOGGING_PATH_C6OP4);
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Logging functions
//----------------------------------------------------------------------------------------
void YM2612::LoggingView::SelectLoggingPath(HWND hwnd, std::wstring& fileName) const
{
	std::wstring selectedFilePath;
	if(SelectNewFile(hwnd, L"Wave file|wav", L"wav", fileName, L"", selectedFilePath))
	{
		fileName = selectedFilePath;
	}
}

//----------------------------------------------------------------------------------------
bool YM2612::LoggingView::ToggleLoggingEnabledState(Stream::WAVFile& wavFile, const std::wstring& fileName, bool currentState, bool newState, unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec) const
{
	boost::mutex::scoped_lock lock(device->waveLoggingMutex);
	if(newState != currentState)
	{
		if(newState)
		{
			wavFile.SetDataFormat(channelCount, bitsPerSample, samplesPerSec);
			wavFile.Open(fileName, Stream::WAVFile::OPENMODE_WRITEONLY, Stream::WAVFile::CREATEMODE_CREATE);
		}
		else
		{
			wavFile.Close();
		}
	}
	return newState;
}
