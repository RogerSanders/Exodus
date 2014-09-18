#include "LoggingView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LoggingView::LoggingView(IUIManager& auiManager, LoggingViewPresenter& apresenter, ISN76489& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_SN76489_LOGGING));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR LoggingView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR LoggingView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	HANDLE folderIconHandle = LoadImage(GetAssemblyHandle(), MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, 0, 0, LR_SHARED);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT_C0), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT_C1), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT_C2), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SN76489_LOGGING_SELECT_C3), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);

	std::wstring tempPathString;
	tempPathString = model.GetAudioLoggingOutputPath();
	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH, tempPathString.c_str());
	tempPathString = model.GetChannelAudioLoggingOutputPath(0);
	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C0, tempPathString.c_str());
	tempPathString = model.GetChannelAudioLoggingOutputPath(1);
	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C1, tempPathString.c_str());
	tempPathString = model.GetChannelAudioLoggingOutputPath(2);
	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C2, tempPathString.c_str());
	tempPathString = model.GetChannelAudioLoggingOutputPath(3);
	SetDlgItemText(hwnd, IDC_SN76489_LOGGING_PATH_C3, tempPathString.c_str());

	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED, (model.IsAudioLoggingEnabled())? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED_C0, (model.IsChannelAudioLoggingEnabled(0))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED_C1, (model.IsChannelAudioLoggingEnabled(1))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED_C2, (model.IsChannelAudioLoggingEnabled(2))? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SN76489_LOGGING_ENABLED_C3, (model.IsChannelAudioLoggingEnabled(3))? BST_CHECKED: BST_UNCHECKED);
	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR LoggingView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED)
		{
			model.SetAudioLoggingEnabled((IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED_C0)
		{
			model.SetChannelAudioLoggingEnabled(0, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED_C1)
		{
			model.SetChannelAudioLoggingEnabled(1, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED_C2)
		{
			model.SetChannelAudioLoggingEnabled(2, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_ENABLED_C3)
		{
			model.SetChannelAudioLoggingEnabled(3, (IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED));
		}

		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT)
		{
			std::wstring newOutputPath = model.GetAudioLoggingOutputPath();
			if(presenter.SelectLoggingPath(newOutputPath))
			{
				UpdateDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH, newOutputPath);
			}
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT_C0)
		{
			std::wstring newOutputPath = model.GetChannelAudioLoggingOutputPath(0);
			if(presenter.SelectLoggingPath(newOutputPath))
			{
				UpdateDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C0, newOutputPath);
			}
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT_C1)
		{
			std::wstring newOutputPath = model.GetChannelAudioLoggingOutputPath(1);
			if(presenter.SelectLoggingPath(newOutputPath))
			{
				UpdateDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C1, newOutputPath);
			}
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT_C2)
		{
			std::wstring newOutputPath = model.GetChannelAudioLoggingOutputPath(2);
			if(presenter.SelectLoggingPath(newOutputPath))
			{
				UpdateDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C2, newOutputPath);
			}
		}
		else if(LOWORD(wparam) == IDC_SN76489_LOGGING_SELECT_C3)
		{
			std::wstring newOutputPath = model.GetChannelAudioLoggingOutputPath(3);
			if(presenter.SelectLoggingPath(newOutputPath))
			{
				UpdateDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C3, newOutputPath);
			}
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
				model.SetAudioLoggingOutputPath(GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH));
			}
			else if(LOWORD(wparam) == IDC_SN76489_LOGGING_PATH_C0)
			{
				model.SetChannelAudioLoggingOutputPath(0, GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C0));
			}
			else if(LOWORD(wparam) == IDC_SN76489_LOGGING_PATH_C1)
			{
				model.SetChannelAudioLoggingOutputPath(1, GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C1));
			}
			else if(LOWORD(wparam) == IDC_SN76489_LOGGING_PATH_C2)
			{
				model.SetChannelAudioLoggingOutputPath(2, GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C2));
			}
			else if(LOWORD(wparam) == IDC_SN76489_LOGGING_PATH_C3)
			{
				model.SetChannelAudioLoggingOutputPath(3, GetDlgItemString(hwnd, IDC_SN76489_LOGGING_PATH_C3));
			}
		}
	}

	return TRUE;
}
