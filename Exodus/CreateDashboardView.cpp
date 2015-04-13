#include "CreateDashboardView.h"
#include "DataConversion/DataConversion.pkg"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
CreateDashboardView::CreateDashboardView(IUIManager& auiManager, CreateDashboardViewPresenter& apresenter, ExodusInterface& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_CREATEDASHBOARD));
	SetDialogViewType(DialogMode::Modeless, false, DialogPos::Center);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR CreateDashboardView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR CreateDashboardView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	EnableWindow(GetDlgItem(hwnd, IDC_CREATEDASHBOARD_OK), FALSE);
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CreateDashboardView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == EN_CHANGE)
	{
		EnableWindow(GetDlgItem(hwnd, IDC_CREATEDASHBOARD_OK), (GetDlgItemString(hwnd, IDC_CREATEDASHBOARD_NAME).empty()? FALSE: TRUE));
	}
	else if(HIWORD(wparam) == BN_CLICKED)
	{
		unsigned int controlID = LOWORD(wparam);
		switch(controlID)
		{
		case IDC_CREATEDASHBOARD_OK:{
			std::wstring dashboardTitle = GetDlgItemString(hwnd, IDC_CREATEDASHBOARD_NAME);
			model.CreateDashboard(dashboardTitle);
			CloseView();
			break;}
		case IDC_CREATEDASHBOARD_CANCEL:
			CloseView();
			break;
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
