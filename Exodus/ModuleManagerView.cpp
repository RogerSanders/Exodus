#include "ModuleManagerView.h"
#include "DataConversion/DataConversion.pkg"
#include "resource.h"
#include <functional>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ModuleManagerView::ModuleManagerView(IUIManager& auiManager, ModuleManagerViewPresenter& apresenter, ExodusInterface& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), windowHandle(NULL)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_MODULEMANAGER));
	SetDialogViewType(DialogMode::Modeless);
}

//----------------------------------------------------------------------------------------
//Refresh methods
//----------------------------------------------------------------------------------------
void ModuleManagerView::RefreshModuleList() const
{
	if(windowHandle != NULL)
	{
		SendMessage(windowHandle, WM_USER, 0, 0);
	}
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR ModuleManagerView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_USER:
		return msgWM_USER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR ModuleManagerView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Subscribe to change notifications for the loaded module list
	loadedModulesChangeSubscription.SetBoundCallback(std::bind(std::mem_fn(&ModuleManagerView::RefreshModuleList), this));
	ISystemGUIInterface& systemInterface = *model.GetSystemInterface();
	systemInterface.LoadedModulesChangeNotifyRegister(loadedModulesChangeSubscription);

	//Refresh our module list
	windowHandle = hwnd;
	RefreshModuleList();
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ModuleManagerView::msgWM_USER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), WM_SETREDRAW, FALSE, 0);

	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_GETTOPINDEX, 0, 0);
	LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_RESETCONTENT, 0, NULL);
	ISystemGUIInterface& systemInterface = *model.GetSystemInterface();
	std::list<unsigned int> loadedModuleIDs = systemInterface.GetLoadedModuleIDs();
	for(std::list<unsigned int>::const_iterator i = loadedModuleIDs.begin(); i != loadedModuleIDs.end(); ++i)
	{
		LoadedModuleInfo moduleInfo;
		if(systemInterface.GetLoadedModuleInfo(*i, moduleInfo))
		{
			std::wstringstream text;
			text << moduleInfo.GetModuleDisplayName() << " (" << moduleInfo.GetModuleClassName() << ")";
			LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_ADDSTRING, 0, (LPARAM)text.str().c_str());
			SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)moduleInfo.GetModuleID());
		}
	}
	SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_SETCURSEL, selected, 0);
	SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_SETTOPINDEX, top, 0);

	SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), NULL, FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ModuleManagerView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_MODULEMANAGER_LOAD:
			model.LoadModule(model.GetGlobalPreferencePathModules());
			break;
		case IDC_MODULEMANAGER_UNLOADSELECTED:{
			//Retrieve the selected list item
			LRESULT selectedListItem = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_GETCURSEL, 0, 0);
			if(selectedListItem != LB_ERR)
			{
				//Retrieve the module ID number of the selected module
				LRESULT listItemData = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_GETITEMDATA, selectedListItem, NULL);
				if(listItemData != LB_ERR)
				{
					//Unload the selected module
					unsigned int targetModuleID = (unsigned int)listItemData;
					model.UnloadModule(targetModuleID);
				}
			}
			break;}
		case IDC_MODULEMANAGER_UNLOADALL:
			//Clear all loaded modules
			model.UnloadAllModules();
			break;
		}
	}
	return TRUE;
}
