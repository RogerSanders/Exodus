#include "SettingsView.h"
#include "DataConversion/DataConversion.pkg"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SettingsView::SettingsView(IUIManager& auiManager, SettingsViewPresenter& apresenter, ExodusInterface& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_SETTINGS));
	SetDialogViewType(DialogMode::Modal, false, DialogPos::Center);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR SettingsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR SettingsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CoInitializeEx(0, COINIT_APARTMENTTHREADED);

	HANDLE folderIconHandle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, 0, 0, LR_SHARED);
	SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHMODULESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHSAVESTATESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHPERSISTENTSTATECHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHDEBUGSESSIONSCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHWORKSPACESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHCAPTURESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHASSEMBLIESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_LOADSYSTEMCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_LOADWORKSPACECHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
	std::wstring preferenceDirPath = model.GetPreferenceDirectoryPath();
	SetDlgItemText(hwnd, IDC_SETTINGS_PATHMODULES, PathRemoveBasePath(preferenceDirPath, model.GetGlobalPreferencePathModules()).c_str());
	SetDlgItemText(hwnd, IDC_SETTINGS_PATHSAVESTATES, PathRemoveBasePath(preferenceDirPath, model.GetGlobalPreferencePathSavestates()).c_str());
	SetDlgItemText(hwnd, IDC_SETTINGS_PATHPERSISTENTSTATE, PathRemoveBasePath(preferenceDirPath, model.GetGlobalPreferencePathPersistentState()).c_str());
	SetDlgItemText(hwnd, IDC_SETTINGS_PATHWORKSPACES, PathRemoveBasePath(preferenceDirPath, model.GetGlobalPreferencePathWorkspaces()).c_str());
	SetDlgItemText(hwnd, IDC_SETTINGS_PATHCAPTURES, PathRemoveBasePath(preferenceDirPath, model.GetGlobalPreferencePathCaptures()).c_str());
	SetDlgItemText(hwnd, IDC_SETTINGS_PATHASSEMBLIES, PathRemoveBasePath(preferenceDirPath, model.GetGlobalPreferencePathAssemblies()).c_str());
	SetDlgItemText(hwnd, IDC_SETTINGS_LOADSYSTEM, PathRemoveBasePath(model.GetGlobalPreferencePathModules(), model.GetGlobalPreferenceInitialSystem()).c_str());
	SetDlgItemText(hwnd, IDC_SETTINGS_LOADWORKSPACE, PathRemoveBasePath(model.GetGlobalPreferencePathWorkspaces(), model.GetGlobalPreferenceInitialWorkspace()).c_str());
	CheckDlgButton(hwnd, IDC_SETTINGS_ENABLETHROTTLE, model.GetGlobalPreferenceEnableThrottling()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SETTINGS_RUNWHENPROGRAMLOADED, model.GetGlobalPreferenceRunWhenProgramModuleLoaded()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SETTINGS_ENABLEPERSISTENTSTATE, model.GetGlobalPreferenceEnablePersistentState()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SETTINGS_LOADWORKSPACEWITHDEBUGSTATE, model.GetGlobalPreferenceLoadWorkspaceWithDebugState()? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_SETTINGS_SHOWDEBUGCONSOLE, model.GetGlobalPreferenceShowDebugConsole()? BST_CHECKED: BST_UNCHECKED);

	EnableWindow(GetDlgItem(hwnd, IDC_SETTINGS_APPLY), FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR SettingsView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	CoUninitialize();
	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR SettingsView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == EN_CHANGE)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_SETTINGS_APPLY), TRUE);
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		case IDC_SETTINGS_ENABLETHROTTLE:
		case IDC_SETTINGS_RUNWHENPROGRAMLOADED:
		case IDC_SETTINGS_ENABLEPERSISTENTSTATE:
		case IDC_SETTINGS_LOADWORKSPACEWITHDEBUGSTATE:
		case IDC_SETTINGS_SHOWDEBUGCONSOLE:
			EnableWindow(GetDlgItem(hwnd, IDC_SETTINGS_APPLY), TRUE);
			break;
		case IDC_SETTINGS_OK:
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_SETTINGS_APPLY, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_SETTINGS_APPLY));
			CloseView();
			break;
		case IDC_SETTINGS_CANCEL:
			CloseView();
			break;
		case IDC_SETTINGS_APPLY:
			model.SetGlobalPreferencePathModules(GetDlgItemString(hwnd, IDC_SETTINGS_PATHMODULES));
			model.SetGlobalPreferencePathSavestates(GetDlgItemString(hwnd, IDC_SETTINGS_PATHSAVESTATES));
			model.SetGlobalPreferencePathPersistentState(GetDlgItemString(hwnd, IDC_SETTINGS_PATHPERSISTENTSTATE));
			model.SetGlobalPreferencePathWorkspaces(GetDlgItemString(hwnd, IDC_SETTINGS_PATHWORKSPACES));
			model.SetGlobalPreferencePathCaptures(GetDlgItemString(hwnd, IDC_SETTINGS_PATHCAPTURES));
			model.SetGlobalPreferencePathAssemblies(GetDlgItemString(hwnd, IDC_SETTINGS_PATHASSEMBLIES));
			model.SetGlobalPreferenceInitialSystem(GetDlgItemString(hwnd, IDC_SETTINGS_LOADSYSTEM));
			model.SetGlobalPreferenceInitialWorkspace(GetDlgItemString(hwnd, IDC_SETTINGS_LOADWORKSPACE));
			model.SetGlobalPreferenceEnableThrottling(IsDlgButtonChecked(hwnd, IDC_SETTINGS_ENABLETHROTTLE) == BST_CHECKED);
			model.SetGlobalPreferenceRunWhenProgramModuleLoaded(IsDlgButtonChecked(hwnd, IDC_SETTINGS_RUNWHENPROGRAMLOADED) == BST_CHECKED);
			model.SetGlobalPreferenceEnablePersistentState(IsDlgButtonChecked(hwnd, IDC_SETTINGS_ENABLEPERSISTENTSTATE) == BST_CHECKED);
			model.SetGlobalPreferenceLoadWorkspaceWithDebugState(IsDlgButtonChecked(hwnd, IDC_SETTINGS_LOADWORKSPACEWITHDEBUGSTATE) == BST_CHECKED);
			model.SetGlobalPreferenceShowDebugConsole(IsDlgButtonChecked(hwnd, IDC_SETTINGS_SHOWDEBUGCONSOLE) == BST_CHECKED);
			EnableWindow(GetDlgItem(hwnd, IDC_SETTINGS_APPLY), FALSE);
			break;
		case IDC_SETTINGS_LOADSYSTEMCHANGE:{
			//Get the fully resolved path to the current target file
			std::wstring moduleFolderPath = model.GetGlobalPreferencePathModules();
			std::wstring fileNameCurrent = GetDlgItemString(hwnd, IDC_SETTINGS_LOADSYSTEM);
			if(!fileNameCurrent.empty() && PathIsRelativePath(fileNameCurrent))
			{
				fileNameCurrent = PathCombinePaths(moduleFolderPath, fileNameCurrent);
			}

			//Select a new target file
			std::wstring selectedFilePath;
			if(model.SelectExistingFile(L"System Definitions|xml", L"xml", fileNameCurrent, moduleFolderPath, true, selectedFilePath))
			{
				selectedFilePath = PathRemoveBasePath(moduleFolderPath, selectedFilePath);
				SetDlgItemText(hwnd, IDC_SETTINGS_LOADSYSTEM, selectedFilePath.c_str());
			}
			break;}
		case IDC_SETTINGS_LOADWORKSPACECHANGE:{
			//Get the fully resolved path to the current target file
			std::wstring workspaceFolderPath = model.GetGlobalPreferencePathWorkspaces();
			std::wstring fileNameCurrent = GetDlgItemString(hwnd, IDC_SETTINGS_LOADWORKSPACE);
			if(!fileNameCurrent.empty() && PathIsRelativePath(fileNameCurrent))
			{
				fileNameCurrent = PathCombinePaths(workspaceFolderPath, fileNameCurrent);
			}

			//Select a new target file
			std::wstring selectedFilePath;
			if(model.SelectExistingFile(L"Workspace Files|xml", L"xml", fileNameCurrent, workspaceFolderPath, true, selectedFilePath))
			{
				selectedFilePath = PathRemoveBasePath(workspaceFolderPath, selectedFilePath);
				SetDlgItemText(hwnd, IDC_SETTINGS_LOADWORKSPACE, selectedFilePath.c_str());
			}
			break;}
		case IDC_SETTINGS_PATHMODULESCHANGE:
		case IDC_SETTINGS_PATHSAVESTATESCHANGE:
		case IDC_SETTINGS_PATHPERSISTENTSTATECHANGE:
		case IDC_SETTINGS_PATHDEBUGSESSIONSCHANGE:
		case IDC_SETTINGS_PATHWORKSPACESCHANGE:
		case IDC_SETTINGS_PATHCAPTURESCHANGE:
		case IDC_SETTINGS_PATHASSEMBLIESCHANGE:{
			unsigned int textboxControlID = 0;
			switch(LOWORD(wparam))
			{
			case IDC_SETTINGS_PATHMODULESCHANGE:
				textboxControlID = IDC_SETTINGS_PATHMODULES;
				break;
			case IDC_SETTINGS_PATHPERSISTENTSTATECHANGE:
				textboxControlID = IDC_SETTINGS_PATHPERSISTENTSTATE;
				break;
			case IDC_SETTINGS_PATHSAVESTATESCHANGE:
				textboxControlID = IDC_SETTINGS_PATHSAVESTATES;
				break;
			case IDC_SETTINGS_PATHDEBUGSESSIONSCHANGE:
				textboxControlID = IDC_SETTINGS_PATHDEBUGSESSIONS;
				break;
			case IDC_SETTINGS_PATHWORKSPACESCHANGE:
				textboxControlID = IDC_SETTINGS_PATHWORKSPACES;
				break;
			case IDC_SETTINGS_PATHCAPTURESCHANGE:
				textboxControlID = IDC_SETTINGS_PATHCAPTURES;
				break;
			case IDC_SETTINGS_PATHASSEMBLIESCHANGE:
				textboxControlID = IDC_SETTINGS_PATHASSEMBLIES;
				break;
			}

			//Retrieve the current target directory
			std::wstring currentSelectedDir = GetDlgItemString(hwnd, textboxControlID);
			if(PathIsRelativePath(currentSelectedDir))
			{
				currentSelectedDir = PathCombinePaths(model.GetPreferenceDirectoryPath(), currentSelectedDir);
			}

			//Select a new target directory
			std::wstring newSelectedDir;
			if(SelectExistingDirectory((HWND)model.GetMainWindowHandle(), currentSelectedDir, newSelectedDir))
			{
				newSelectedDir = PathRemoveBasePath(model.GetPreferenceDirectoryPath(), newSelectedDir);
				SetDlgItemText(hwnd, textboxControlID, newSelectedDir.c_str());
			}
			break;}
		}
	}
	return TRUE;
}
