#include "PortMonitorDetailsView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PortMonitorDetailsView::PortMonitorDetailsView(IUIManager& auiManager, PortMonitorDetailsViewPresenter& apresenter, IS315_5313& amodel, const IS315_5313::PortMonitorEntry& aentry)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), entry(aentry)
{
	hwndInternal = NULL;
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_S315_5313_PORTMONITOR_DETAILS));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Port monitor entry functions
//----------------------------------------------------------------------------------------
void PortMonitorDetailsView::SetPortMonitorEntry(const IS315_5313::PortMonitorEntry& aentry)
{
	entry = aentry;
	if(hwndInternal != NULL)
	{
		UpdatePortMonitorDisplay(hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR PortMonitorDetailsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR PortMonitorDetailsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	hwndInternal = hwnd;
	UpdatePortMonitorDisplay(hwndInternal);

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Entry update functions
//----------------------------------------------------------------------------------------
void PortMonitorDetailsView::UpdatePortMonitorDisplay(HWND hwnd)
{
	UpdateDlgItemString(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_OPERATION, entry.target);
	UpdateDlgItemString(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_SOURCE, entry.source);
	UpdateDlgItemHex(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_DATA, 4, entry.data);
	UpdateDlgItemDouble(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_ACCESSTIME, entry.accessTime);
	UpdateDlgItemHex(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_XPOS, 3, entry.hcounterPos);
	UpdateDlgItemHex(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_YPOS, 3, entry.vcounterPos);
}
