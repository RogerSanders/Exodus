#include "EmbeddedROMView.h"
#include "Stream/Stream.pkg"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
EmbeddedROMView::EmbeddedROMView(IUIManager& auiManager, EmbeddedROMViewPresenter& apresenter, ISystemGUIInterface& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	embeddedROMIDLastModifiedToken = 0;
	selectedROMID = 0;
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_EMBEDDEDROM));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR EmbeddedROMView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR EmbeddedROMView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Set the folder icon on the browse button
	HANDLE folderIconHandle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, 0, 0, LR_SHARED);
	SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_BROWSE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);

	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR EmbeddedROMView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR EmbeddedROMView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//If the embedded ROM list hasn't been modified, abort any further processing.
	unsigned int newEmbeddedROMIDLastModifiedToken = model.GetEmbeddedROMInfoLastModifiedToken();
	if(embeddedROMIDLastModifiedToken == newEmbeddedROMIDLastModifiedToken)
	{
		return TRUE;
	}

	//Obtain the current set of embedded ROM IDs
	std::list<unsigned int> embeddedROMIDList = model.GetEmbeddedROMIDs();

	//Disable visual updates to the embedded ROM list
	SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), WM_SETREDRAW, FALSE, 0);

	//Update the embedded ROM list
	bool foundCurrentlySelectedROMID = false;
	LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_GETTOPINDEX, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_RESETCONTENT, 0, NULL);
	for(std::list<unsigned int>::const_iterator i = embeddedROMIDList.begin(); i != embeddedROMIDList.end(); ++i)
	{
		//Attempt to obtain info on the next embedded ROM
		unsigned int embeddedROMID = *i;
		EmbeddedROMInfo embeddedROMInfo;
		if(!model.GetEmbeddedROMInfo(embeddedROMID, embeddedROMInfo))
		{
			continue;
		}

		//Attempt to retrieve the display name of the associated module
		std::wstring moduleDisplayName;
		if(!model.GetModuleDisplayName(embeddedROMInfo.GetModuleID(), moduleDisplayName))
		{
			continue;
		}

		//Build a string uniquely identifying this embedded ROM in the system
		std::wstring romNameString = moduleDisplayName + L" - " + embeddedROMInfo.GetDisplayName();

		//Add an entry for this embedded ROM to the list
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_ADDSTRING, 0, (LPARAM)romNameString.c_str());
		SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)embeddedROMID);

		//If this ROM entry was the previously selected ROM entry, record its index in the
		//list.
		if(embeddedROMID == selectedROMID)
		{
			std::wstring filePath = embeddedROMInfo.GetFilePath();
			SetDlgItemText(hwnd, IDC_EMBEDDEDROM_PATH, filePath.c_str());
			SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_SETCURSEL, newItemIndex, 0);
			foundCurrentlySelectedROMID = true;
		}
	}
	SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_SETTOPINDEX, top, 0);

	//If the previously selected embedded ROM item was not found, clear the current
	//selection in the list.
	if(!foundCurrentlySelectedROMID)
	{
		selectedROMID = 0;
		SetDlgItemText(hwnd, IDC_EMBEDDEDROM_PATH, L"");
		SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_SETCURSEL, (WPARAM)-1, 0);
	}

	//Enable visual updates to the embedded ROM list, and trigger a refresh.
	SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), WM_SETREDRAW, TRUE, 0);
	InvalidateRect(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), NULL, FALSE);

	//Save the new last modified token as the current last modified token
	embeddedROMIDLastModifiedToken = newEmbeddedROMIDLastModifiedToken;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR EmbeddedROMView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		case IDC_EMBEDDEDROM_BROWSE:{
			//Select a new target file
			std::wstring fileNameCurrent = GetDlgItemString(hwnd, IDC_EMBEDDEDROM_PATH);
			std::wstring selectedFilePath;
			if(presenter.GetGUIInterface().SelectExistingFile(L"Binary files|bin", L"bin", fileNameCurrent, L"", true, selectedFilePath))
			{
				SetDlgItemText(hwnd, IDC_EMBEDDEDROM_PATH, &selectedFilePath[0]);
			}
			break;}
		case IDC_EMBEDDEDROM_APPLY:
			if(selectedROMID != 0)
			{
				std::wstring filePath = GetDlgItemString(hwnd, IDC_EMBEDDEDROM_PATH);
				model.SetEmbeddedROMPath(selectedROMID, filePath);
			}
			break;
		case IDC_EMBEDDEDROM_RELOAD:
			if(selectedROMID != 0)
			{
				model.ReloadEmbeddedROMData(selectedROMID);
			}
			break;
		}
	}
	else if(HIWORD(wparam) == LBN_SELCHANGE)
	{
		switch(LOWORD(wparam))
		{
		case IDC_EMBEDDEDROM_LIST:
			int selectedItem = (int)SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_GETCURSEL, 0, NULL);
			if(selectedItem != LB_ERR)
			{
				selectedROMID = (unsigned int)SendMessage(GetDlgItem(hwnd, IDC_EMBEDDEDROM_LIST), LB_GETITEMDATA, selectedItem, 0);
				EmbeddedROMInfo embeddedROMInfo;
				if(model.GetEmbeddedROMInfo(selectedROMID, embeddedROMInfo))
				{
					std::wstring filePath = embeddedROMInfo.GetFilePath();
					SetDlgItemText(hwnd, IDC_EMBEDDEDROM_PATH, filePath.c_str());
				}
			}
			break;
		}
	}

	return TRUE;
}
