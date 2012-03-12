#include "PortMonitorDetailsView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::PortMonitorDetailsView::PortMonitorDetailsView(VDP* adevice, const PortMonitorEntry& aentry)
:device(adevice), entry(aentry)
{
	hwndInternal = NULL;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Port Monitor Entry Details");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_PORTMONITOR_DETAILS));
}

//----------------------------------------------------------------------------------------
//Port monitor entry functions
//----------------------------------------------------------------------------------------
void VDP::PortMonitorDetailsView::SetPortMonitorEntry(const PortMonitorEntry& aentry)
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
INT_PTR VDP::PortMonitorDetailsView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR VDP::PortMonitorDetailsView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	hwndInternal = hwnd;
	UpdatePortMonitorDisplay(hwndInternal);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::PortMonitorDetailsView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Entry update functions
//----------------------------------------------------------------------------------------
void VDP::PortMonitorDetailsView::UpdatePortMonitorDisplay(HWND hwnd)
{
	UpdateDlgItemString(hwnd, IDC_VDP_PORTMONITOR_DETAILS_OPERATION, entry.target);
	UpdateDlgItemString(hwnd, IDC_VDP_PORTMONITOR_DETAILS_SOURCE, entry.source);
	UpdateDlgItemHex(hwnd, IDC_VDP_PORTMONITOR_DETAILS_DATA, 4, entry.data);
	UpdateDlgItemBin(hwnd, IDC_VDP_PORTMONITOR_DETAILS_XPOS, entry.accessPos.line);
	UpdateDlgItemBin(hwnd, IDC_VDP_PORTMONITOR_DETAILS_YPOS, entry.accessPos.pixel);
	UpdateDlgItemBin(hwnd, IDC_VDP_PORTMONITOR_DETAILS_HBLANK, entry.accessPos.hblank);
	UpdateDlgItemBin(hwnd, IDC_VDP_PORTMONITOR_DETAILS_VBLANK, entry.accessPos.vblank);
}
