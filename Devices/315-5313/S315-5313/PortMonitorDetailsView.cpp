#include "PortMonitorDetailsView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::PortMonitorDetailsView::PortMonitorDetailsView(S315_5313* adevice, const PortMonitorEntry& aentry)
:device(adevice), entry(aentry)
{
	hwndInternal = NULL;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Port Monitor Entry Details");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_S315_5313_PORTMONITOR_DETAILS));
}

//----------------------------------------------------------------------------------------
//Port monitor entry functions
//----------------------------------------------------------------------------------------
void S315_5313::PortMonitorDetailsView::SetPortMonitorEntry(const PortMonitorEntry& aentry)
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
INT_PTR S315_5313::PortMonitorDetailsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
INT_PTR S315_5313::PortMonitorDetailsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	hwndInternal = hwnd;
	UpdatePortMonitorDisplay(hwndInternal);

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Entry update functions
//----------------------------------------------------------------------------------------
void S315_5313::PortMonitorDetailsView::UpdatePortMonitorDisplay(HWND hwnd)
{
	UpdateDlgItemString(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_OPERATION, entry.target);
	UpdateDlgItemString(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_SOURCE, entry.source);
	UpdateDlgItemHex(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_DATA, 4, entry.data);
	UpdateDlgItemDouble(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_ACCESSTIME, entry.accessTime);
	UpdateDlgItemHex(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_XPOS, 3, entry.hcounterPos);
	UpdateDlgItemHex(hwnd, IDC_S315_5313_PORTMONITOR_DETAILS_YPOS, 3, entry.vcounterPos);
}
