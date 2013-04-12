#include "LoggingView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489::LoggingView::LoggingView(SN76489* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"WAV File Logging");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_SN76489_LOGGING));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR SN76489::LoggingView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR SN76489::LoggingView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	HANDLE folderIconHandle = LoadImage((HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, 0, 0, LR_SHARED);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT_C0), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT_C1), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT_C2), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT_C3), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);

	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH, &device->wavLoggingPath[0]);
	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C0, &device->wavLoggingChannelPath[0][0]);
	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C1, &device->wavLoggingChannelPath[1][0]);
	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C2, &device->wavLoggingChannelPath[2][0]);
	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C3, &device->wavLoggingChannelPath[3][0]);

	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED, (device->wavLoggingEnabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED_C0, (device->wavLoggingChannelEnabled[0])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED_C1, (device->wavLoggingChannelEnabled[1])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED_C2, (device->wavLoggingChannelEnabled[2])? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED_C3, (device->wavLoggingChannelEnabled[3])? BST_CHECKED: BST_UNCHECKED);
	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::LoggingView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SN76489::LoggingView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		double outputFrequency = device->externalClockRate / device->externalClockDivider;

		if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED)
		{
			device->wavLoggingEnabled = ToggleLoggingEnabledState(device->wavLog, device->wavLoggingPath, device->wavLoggingEnabled, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)outputFrequency);
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED_C0)
		{
			device->wavLoggingChannelEnabled[0] = ToggleLoggingEnabledState(device->wavLogChannel[0], device->wavLoggingChannelPath[0], device->wavLoggingChannelEnabled[0], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)outputFrequency);
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED_C1)
		{
			device->wavLoggingChannelEnabled[1] = ToggleLoggingEnabledState(device->wavLogChannel[1], device->wavLoggingChannelPath[1], device->wavLoggingChannelEnabled[1], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)outputFrequency);
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED_C2)
		{
			device->wavLoggingChannelEnabled[2] = ToggleLoggingEnabledState(device->wavLogChannel[2], device->wavLoggingChannelPath[2], device->wavLoggingChannelEnabled[2], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)outputFrequency);
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED_C3)
		{
			device->wavLoggingChannelEnabled[3] = ToggleLoggingEnabledState(device->wavLogChannel[3], device->wavLoggingChannelPath[3], device->wavLoggingChannelEnabled[3], (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED), 1, 16, (unsigned int)outputFrequency);
		}

		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT)
		{
			SelectLoggingPath(hwnd, device->wavLoggingPath);
			SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH, &device->wavLoggingPath[0]);
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT_C0)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[0]);
			SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C0, &device->wavLoggingChannelPath[0][0]);
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT_C1)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[1]);
			SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C1, &device->wavLoggingChannelPath[1][0]);
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT_C2)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[2]);
			SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C2, &device->wavLoggingChannelPath[2][0]);
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT_C3)
		{
			SelectLoggingPath(hwnd, device->wavLoggingChannelPath[3]);
			SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C3, &device->wavLoggingChannelPath[3][0]);
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
		if(newText != previousText)
		{
			if(LOWORD(wparam) == IDC_SN76489_LOGGING_PATH)
			{
				device->wavLoggingPath = GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH);
			}
			else if(LOWORD(wparam) == IDC_SN76489_LOGGING_PATH_C0)
			{
				device->wavLoggingChannelPath[0] = GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C0);
			}
			else if(LOWORD(wparam) == IDC_SN76489_LOGGING_PATH_C1)
			{
				device->wavLoggingChannelPath[1] = GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C1);
			}
			else if(LOWORD(wparam) == IDC_SN76489_LOGGING_PATH_C2)
			{
				device->wavLoggingChannelPath[2] = GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C2);
			}
			else if(LOWORD(wparam) == IDC_SN76489_LOGGING_PATH_C3)
			{
				device->wavLoggingChannelPath[3] = GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C3);
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Logging functions
//----------------------------------------------------------------------------------------
void SN76489::LoggingView::SelectLoggingPath(HWND hwnd, std::wstring& fileName) const
{
	std::wstring selectedFilePath;
	if(SelectNewFile(hwnd, L"Wave file|wav", L"wav", fileName, L"", selectedFilePath))
	{
		fileName = selectedFilePath;
	}
}

//----------------------------------------------------------------------------------------
bool SN76489::LoggingView::ToggleLoggingEnabledState(Stream::WAVFile& wavFile, const std::wstring& fileName, bool currentState, bool newState, unsigned int channelCount, unsigned int bitsPerSample, unsigned int samplesPerSec) const
{
	boost::mutex::scoped_lock lock(device->waveLoggingMutex);
	if(newState != currentState)
	{
		if(newState)
		{
			wavFile.Open(fileName, Stream::WAVFile::OPENMODE_WRITEONLY, Stream::WAVFile::CREATEMODE_CREATE, channelCount, bitsPerSample, samplesPerSec);
		}
		else
		{
			wavFile.Close();
		}
	}
	return newState;
}
