#include "LoggingView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LoggingView::LoggingView(IUIManager& auiManager, LoggingViewPresenter& apresenter, IYM2612& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_YM2612_LOGGING));
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
	SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	for(unsigned int channelNo = 0; channelNo < model.channelCount; ++channelNo)
	{
		SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C1 + channelNo), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		for(unsigned int operatorNo = 0; operatorNo < model.operatorCount; ++operatorNo)
		{
			SendMessage(GetDlgItem(hwnd, IDC_YM2612_LOGGING_SELECT_C1OP1 + (channelNo * model.operatorCount) + operatorNo), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		}
	}

	std::wstring audioLoggingPath = model.GetAudioLoggingOutputPath();
	SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH, audioLoggingPath.c_str());
	for(unsigned int channelNo = 0; channelNo < model.channelCount; ++channelNo)
	{
		std::wstring channelLoggingPath = model.GetChannelAudioLoggingOutputPath(channelNo);
		SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1 + channelNo, channelLoggingPath.c_str());
		for(unsigned int operatorNo = 0; operatorNo < model.operatorCount; ++operatorNo)
		{
			std::wstring operatorLoggingPath = model.GetOperatorAudioLoggingOutputPath(channelNo, operatorNo);
			SetDlgItemText(hwnd, IDC_YM2612_LOGGING_PATH_C1OP1 + (channelNo * model.operatorCount) + operatorNo, operatorLoggingPath.c_str());
		}
	}

	CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED, (model.IsAudioLoggingEnabled())? BST_CHECKED: BST_UNCHECKED);
	for(unsigned int channelNo = 0; channelNo < model.channelCount; ++channelNo)
	{
		CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C1 + channelNo, (model.IsChannelAudioLoggingEnabled(channelNo))? BST_CHECKED: BST_UNCHECKED);
		for(unsigned int operatorNo = 0; operatorNo < model.operatorCount; ++operatorNo)
		{
			CheckDlgButton(hwnd, IDC_YM2612_LOGGING_ENABLED_C1OP1 + (channelNo * model.operatorCount) + operatorNo, (model.IsOperatorAudioLoggingEnabled(channelNo, operatorNo))? BST_CHECKED: BST_UNCHECKED);
		}
	}

	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR LoggingView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		int controlID = (int)LOWORD(wparam);
		if(controlID == IDC_YM2612_LOGGING_ENABLED)
		{
			model.SetAudioLoggingEnabled(IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
		}
		else if((controlID >= IDC_YM2612_LOGGING_ENABLED_C1) && (controlID < (IDC_YM2612_LOGGING_ENABLED_C1 + model.channelCount)))
		{
			unsigned int channelNo = (unsigned int)(controlID - IDC_YM2612_LOGGING_ENABLED_C1);
			model.SetChannelAudioLoggingEnabled(channelNo, IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED);
		}
		else if((controlID >= IDC_YM2612_LOGGING_ENABLED_C1OP1) && (controlID < (IDC_YM2612_LOGGING_ENABLED_C1OP1 + (model.channelCount * model.operatorCount))))
		{
			unsigned int channelNo = ((unsigned int)(controlID - IDC_YM2612_LOGGING_ENABLED_C1OP1) / model.operatorCount);
			unsigned int operatorNo = ((unsigned int)(controlID - IDC_YM2612_LOGGING_ENABLED_C1OP1) % model.operatorCount);
			model.SetOperatorAudioLoggingEnabled(channelNo, operatorNo, IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED);
		}
		else if(controlID == IDC_YM2612_LOGGING_SELECT)
		{
			std::wstring newOutputPath = model.GetAudioLoggingOutputPath();
			presenter.SelectLoggingPath(newOutputPath);
			SetDlgItemText(hwnd, controlID, newOutputPath.c_str());
		}
		else if((controlID >= IDC_YM2612_LOGGING_SELECT_C1) && (controlID < (IDC_YM2612_LOGGING_SELECT_C1 + model.channelCount)))
		{
			unsigned int channelNo = (unsigned int)(controlID - IDC_YM2612_LOGGING_SELECT_C1);
			std::wstring newOutputPath = model.GetChannelAudioLoggingOutputPath(channelNo);
			presenter.SelectLoggingPath(newOutputPath);
			SetDlgItemText(hwnd, controlID, newOutputPath.c_str());
		}
		else if((controlID >= IDC_YM2612_LOGGING_SELECT_C1OP1) && (controlID < (IDC_YM2612_LOGGING_SELECT_C1OP1 + (model.channelCount * model.operatorCount))))
		{
			unsigned int channelNo = ((unsigned int)(controlID - IDC_YM2612_LOGGING_SELECT_C1OP1) / model.operatorCount);
			unsigned int operatorNo = ((unsigned int)(controlID - IDC_YM2612_LOGGING_SELECT_C1OP1) % model.operatorCount);
			std::wstring newOutputPath = model.GetOperatorAudioLoggingOutputPath(channelNo, operatorNo);
			presenter.SelectLoggingPath(newOutputPath);
			SetDlgItemText(hwnd, controlID, newOutputPath.c_str());
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
			int controlID = (int)LOWORD(wparam);
			if(controlID == IDC_YM2612_LOGGING_PATH)
			{
				model.SetAudioLoggingOutputPath(GetDlgItemString(hwnd, controlID));
			}
			else if((controlID >= IDC_YM2612_LOGGING_PATH_C1) && (controlID < (IDC_YM2612_LOGGING_PATH_C1 + model.channelCount)))
			{
				unsigned int channelNo = (unsigned int)(controlID - IDC_YM2612_LOGGING_PATH_C1);
				model.SetChannelAudioLoggingOutputPath(channelNo, GetDlgItemString(hwnd, controlID));
			}
			else if((controlID >= IDC_YM2612_LOGGING_PATH_C1OP1) && (controlID < (IDC_YM2612_LOGGING_PATH_C1OP1 + (model.channelCount * model.operatorCount))))
			{
				unsigned int channelNo = ((unsigned int)(controlID - IDC_YM2612_LOGGING_PATH_C1OP1) / model.operatorCount);
				unsigned int operatorNo = ((unsigned int)(controlID - IDC_YM2612_LOGGING_PATH_C1OP1) % model.operatorCount);
				model.SetOperatorAudioLoggingOutputPath(channelNo, operatorNo, GetDlgItemString(hwnd, controlID));
			}
		}
	}

	return TRUE;
}
