#include "AboutView.h"
#include "DataConversion/DataConversion.pkg"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
AboutView::AboutView(IUIManager& auiManager, AboutViewPresenter& apresenter, ExodusInterface& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	hwndLayoutGrid = NULL;
	hwndNestedDialog = NULL;
	fontHandle = NULL;
	SetWindowSettings(apresenter.GetUnqualifiedViewTitle(), 0, WS_EX_COMPOSITED, 700, 565);
	SetDialogViewType(DialogMode::Modal, true, DialogPos::Center);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT AboutView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_ERASEBKGND:
		return msgWM_ERASEBKGND(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT AboutView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the dialog control panel
	hwndNestedDialog = CreateDialogParam(GetAssemblyHandle(), MAKEINTRESOURCE(IDD_ABOUT), hwnd, WndProcPanelStatic, (LPARAM)this);
	ShowWindow(hwndNestedDialog, SW_SHOWNORMAL);
	UpdateWindow(hwndNestedDialog);

	//Calculate the width and height of the border region for the dialog
	RECT controlPanelDialogRect;
	GetWindowRect(hwndNestedDialog, &controlPanelDialogRect);
	RECT borderMeasureRect;
	GetWindowRect(GetDlgItem(hwndNestedDialog, IDC_BORDERMEASURE), &borderMeasureRect);
	int dialogBorderWidth = borderMeasureRect.left - controlPanelDialogRect.left;
	int dialogBorderHeight = borderMeasureRect.top - controlPanelDialogRect.top;

	//Restrict the width and height of the control panel to the desired size
	RECT controlPanelDialogBottomMarkerRect;
	GetWindowRect(GetDlgItem(hwndNestedDialog, IDC_LOADEDDEVICESTEXT), &controlPanelDialogBottomMarkerRect);
	int controlPanelDialogWidth = controlPanelDialogRect.right - controlPanelDialogRect.left;
	int controlPanelDialogHeight = controlPanelDialogBottomMarkerRect.top - controlPanelDialogRect.top;
	SetWindowPos(hwndNestedDialog, NULL, 0, 0, controlPanelDialogWidth, controlPanelDialogHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	//Create the default font for this window
	int fontPointSize = 8;
	HDC hdc = GetDC(hwnd);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);
	std::wstring fontTypefaceName = L"MS Shell Dlg";
	fontHandle = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &fontTypefaceName[0]);

	//Register the DataGrid window class
	WC_DataGrid::RegisterWindowClass(GetAssemblyHandle());

	//Create the DataGrid child controls
	HWND hwndDeviceList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_DataGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, NULL, GetAssemblyHandle(), NULL);
	HWND hwndExtensionList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_DataGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, NULL, GetAssemblyHandle(), NULL);

	//Set the default font for the child controls
	SendMessage(hwndDeviceList, WM_SETFONT, (WPARAM)fontHandle, (LPARAM)TRUE);
	SendMessage(hwndExtensionList, WM_SETFONT, (WPARAM)fontHandle, (LPARAM)TRUE);
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)fontHandle, (LPARAM)TRUE);
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)fontHandle, (LPARAM)TRUE);

	//Read all the device data to be shown in the data grid
	std::list<ExodusInterface::RegisteredDeviceInfo> registeredDevices = model.GetRegisteredDevices();
	unsigned int registeredDeviceCount = (unsigned int)registeredDevices.size();
	std::vector<std::wstring> deviceColumnDataName(registeredDeviceCount);
	std::vector<std::wstring> deviceColumnDataAssembly(registeredDeviceCount);
	std::vector<std::wstring> deviceColumnDataCopyright(registeredDeviceCount);
	std::vector<std::wstring> deviceColumnDataClassName(registeredDeviceCount);
	std::vector<std::wstring> deviceColumnDataVersion(registeredDeviceCount);
	std::vector<std::wstring> deviceColumnDataComments(registeredDeviceCount);
	unsigned int deviceColumnIndexNo = 0;
	for(std::list<ExodusInterface::RegisteredDeviceInfo>::const_iterator i = registeredDevices.begin(); i != registeredDevices.end(); ++i)
	{
		const ExodusInterface::RegisteredDeviceInfo& deviceInfo = *i;
		deviceColumnDataName[deviceColumnIndexNo] = deviceInfo.info.GetDeviceImplementationName();
		deviceColumnDataAssembly[deviceColumnIndexNo] = PathGetFileName(deviceInfo.assemblyPath);
		deviceColumnDataCopyright[deviceColumnIndexNo] = deviceInfo.info.GetDeviceCopyright();
		deviceColumnDataClassName[deviceColumnIndexNo] = deviceInfo.info.GetDeviceClassName();
		std::wstring versionString;
		IntToString(deviceInfo.info.GetDeviceVersionNo(), versionString);
		deviceColumnDataVersion[deviceColumnIndexNo] = versionString;
		deviceColumnDataComments[deviceColumnIndexNo] = deviceInfo.info.GetDeviceComments();
		++deviceColumnIndexNo;
	}

	//Insert our columns into the device data grid
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Name", 1));
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Assembly", 2));
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Copyright", 3));
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Class Name", 4));
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Version", 5));
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Comments", 6));
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 1, (LPARAM)&deviceColumnDataName);
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 2, (LPARAM)&deviceColumnDataAssembly);
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 3, (LPARAM)&deviceColumnDataCopyright);
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 4, (LPARAM)&deviceColumnDataClassName);
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 5, (LPARAM)&deviceColumnDataVersion);
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 6, (LPARAM)&deviceColumnDataComments);

	//Read all the extension data to be shown in the data grid
	std::list<ExodusInterface::RegisteredExtensionInfo> registeredExtensions = model.GetRegisteredExtensions();
	unsigned int registeredExtensionCount = (unsigned int)registeredExtensions.size();
	std::vector<std::wstring> extensionColumnDataName(registeredExtensionCount);
	std::vector<std::wstring> extensionColumnDataAssembly(registeredExtensionCount);
	std::vector<std::wstring> extensionColumnDataCopyright(registeredExtensionCount);
	std::vector<std::wstring> extensionColumnDataClassName(registeredExtensionCount);
	std::vector<std::wstring> extensionColumnDataVersion(registeredExtensionCount);
	std::vector<std::wstring> extensionColumnDataComments(registeredExtensionCount);
	unsigned int extensionColumnIndexNo = 0;
	for(std::list<ExodusInterface::RegisteredExtensionInfo>::const_iterator i = registeredExtensions.begin(); i != registeredExtensions.end(); ++i)
	{
		const ExodusInterface::RegisteredExtensionInfo& extensionInfo = *i;
		extensionColumnDataName[extensionColumnIndexNo] = extensionInfo.info.GetExtensionImplementationName();
		extensionColumnDataAssembly[extensionColumnIndexNo] = PathGetFileName(extensionInfo.assemblyPath);
		extensionColumnDataCopyright[extensionColumnIndexNo] = extensionInfo.info.GetExtensionCopyright();
		extensionColumnDataClassName[extensionColumnIndexNo] = extensionInfo.info.GetExtensionClassName();
		std::wstring versionString;
		IntToString(extensionInfo.info.GetExtensionVersionNo(), versionString);
		extensionColumnDataVersion[extensionColumnIndexNo] = versionString;
		extensionColumnDataComments[extensionColumnIndexNo] = extensionInfo.info.GetExtensionComments();
		++extensionColumnIndexNo;
	}

	//Insert our columns into the extension data grid
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Name", 1));
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Assembly", 2));
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Copyright", 3));
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Class Name", 4));
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Version", 5));
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::InsertColumn, 0, (LPARAM)&(const WC_DataGrid::Grid_InsertColumn&)WC_DataGrid::Grid_InsertColumn(L"Comments", 6));
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 1, (LPARAM)&extensionColumnDataName);
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 2, (LPARAM)&extensionColumnDataAssembly);
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 3, (LPARAM)&extensionColumnDataCopyright);
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 4, (LPARAM)&extensionColumnDataClassName);
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 5, (LPARAM)&extensionColumnDataVersion);
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::UpdateColumnText, 6, (LPARAM)&extensionColumnDataComments);

	//Register the LayoutGrid window class
	WC_LayoutGrid::RegisterWindowClass(GetAssemblyHandle());

	//Create the LayoutGrid child control
	layoutGridBorderWidth = dialogBorderWidth;
	layoutGridBorderHeight = dialogBorderHeight;
	hwndLayoutGrid = CreateWindowEx(WS_EX_TRANSPARENT, WC_LayoutGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE, layoutGridBorderWidth, layoutGridBorderHeight, 0, 0, hwnd, NULL, GetAssemblyHandle(), NULL);

	//Insert our rows and columns into the layout grid
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddRow, 0, (LPARAM)&(const WC_LayoutGrid::AddRowParams&)WC_LayoutGrid::AddRowParams(WC_LayoutGrid::SizeMode::Content));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddRow, 0, (LPARAM)&(const WC_LayoutGrid::AddRowParams&)WC_LayoutGrid::AddRowParams(WC_LayoutGrid::SizeMode::Content));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddRow, 0, (LPARAM)&(const WC_LayoutGrid::AddRowParams&)WC_LayoutGrid::AddRowParams(WC_LayoutGrid::SizeMode::Proportional));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddRow, 0, (LPARAM)&(const WC_LayoutGrid::AddRowParams&)WC_LayoutGrid::AddRowParams(WC_LayoutGrid::SizeMode::Content));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddRow, 0, (LPARAM)&(const WC_LayoutGrid::AddRowParams&)WC_LayoutGrid::AddRowParams(WC_LayoutGrid::SizeMode::Proportional));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddRow, 0, (LPARAM)&(const WC_LayoutGrid::AddRowParams&)WC_LayoutGrid::AddRowParams(WC_LayoutGrid::SizeMode::Content));

	//Add each child control to the layout grid
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(hwndNestedDialog, 0, 0, WC_LayoutGrid::WindowSizeMode::Fixed, WC_LayoutGrid::WindowSizeMode::Fixed, WC_LayoutGrid::HorizontalAlignment::Centre, WC_LayoutGrid::VerticalAlignment::Top));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(GetDlgItem(hwndNestedDialog, IDC_LOADEDDEVICESTEXT), 1, 0, WC_LayoutGrid::WindowSizeMode::Fixed, WC_LayoutGrid::WindowSizeMode::Fixed));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(hwndDeviceList, 2, 0, WC_LayoutGrid::WindowSizeMode::Proportional, WC_LayoutGrid::WindowSizeMode::Proportional));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(GetDlgItem(hwndNestedDialog, IDC_LOADEDEXTENSIONSTEXT), 3, 0, WC_LayoutGrid::WindowSizeMode::Fixed, WC_LayoutGrid::WindowSizeMode::Fixed));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(hwndExtensionList, 4, 0, WC_LayoutGrid::WindowSizeMode::Proportional, WC_LayoutGrid::WindowSizeMode::Proportional));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(GetDlgItem(hwndNestedDialog, IDOK), 5, 0, WC_LayoutGrid::WindowSizeMode::Fixed, WC_LayoutGrid::WindowSizeMode::Fixed, WC_LayoutGrid::HorizontalAlignment::Centre, WC_LayoutGrid::VerticalAlignment::Centre));

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT AboutView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Delete the default font object. Note that since our window has been removed from the
	//screen at this point, it should be safe to delete this object here even though child
	//controls still hold references to it, as long as we don't attempt to use the font
	//handle while the window is being destroyed.
	DeleteObject(fontHandle);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT AboutView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Read the new client size of the window
	RECT rect;
	GetClientRect(hwnd, &rect);
	int controlWidth = rect.right;
	int controlHeight = rect.bottom;

	//Resize the layout grid to the desired width and height
	SetWindowPos(hwndLayoutGrid, NULL, layoutGridBorderWidth, layoutGridBorderHeight, controlWidth - (layoutGridBorderWidth * 2), controlHeight - (layoutGridBorderHeight * 2), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT AboutView::msgWM_ERASEBKGND(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Ensure that our window background is painted with the dialog background colour
	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect((HDC)wParam, &rect, GetSysColorBrush(COLOR_3DFACE));
	return TRUE;
}

//----------------------------------------------------------------------------------------
LRESULT AboutView::msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Forward all commands to our contained dialog
	return SendMessage(hwndNestedDialog, WM_COMMAND, wParam, lParam);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK AboutView::WndProcPanelStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	AboutView* state = (AboutView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (AboutView*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcPanel(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			INT_PTR result = state->WndProcPanel(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = FALSE;
	if(state != 0)
	{
		result = state->WndProcPanel(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
INT_PTR AboutView::WndProcPanel(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgPanelWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_PARENTNOTIFY:
		return msgPanelWM_PARENTNOTIFY(hwnd, wparam, lparam);
	case WM_NOTIFY:
		return msgPanelWM_NOTIFY(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgPanelWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Panel dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR AboutView::msgPanelWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Retrieve the required information from the version info table for our program
	std::wstring mainExecutablePath = GetModuleFilePath(GetAssemblyHandle());
	std::wstring versionText;
	std::wstring copyrightText;
	GetModuleVersionInfoString(mainExecutablePath, VERSIONINFOPROPERTY_PRODUCTVERSION, versionText);
	GetModuleVersionInfoString(mainExecutablePath, VERSIONINFOPROPERTY_LEGALCOPYRIGHT, copyrightText);

	//Load the program version info into the about dialog
	SetDlgItemText(hwnd, IDC_ABOUT_VERSION, versionText.c_str());
	SetDlgItemText(hwnd, IDC_ABOUT_COPYRIGHT, copyrightText.c_str());

	//Select an appropriate size for the program icon image based on the current
	//screen DPI settings
	int iconWidth = 0;
	int iconHeight = 0;
	int iconRegionWidth = DPIScaleWidth(48);
	int iconRegionHeight = DPIScaleHeight(48);
	if     ((iconRegionWidth >= 128) && (iconRegionHeight >= 128)) { iconWidth = 128; iconHeight = 128; }
	else if((iconRegionWidth >=  84) && (iconRegionHeight >=  84)) { iconWidth =  84; iconHeight =  84; }
	else if((iconRegionWidth >=  72) && (iconRegionHeight >=  72)) { iconWidth =  72; iconHeight =  72; }
	else if((iconRegionWidth >=  60) && (iconRegionHeight >=  60)) { iconWidth =  60; iconHeight =  60; }
	else if((iconRegionWidth >=  56) && (iconRegionHeight >=  56)) { iconWidth =  56; iconHeight =  56; }
	else if((iconRegionWidth >=  48) && (iconRegionHeight >=  48)) { iconWidth =  48; iconHeight =  48; }
	else if((iconRegionWidth >=  40) && (iconRegionHeight >=  40)) { iconWidth =  40; iconHeight =  40; }
	else if((iconRegionWidth >=  32) && (iconRegionHeight >=  32)) { iconWidth =  32; iconHeight =  32; }
	else if((iconRegionWidth >=  28) && (iconRegionHeight >=  28)) { iconWidth =  28; iconHeight =  28; }
	else if((iconRegionWidth >=  24) && (iconRegionHeight >=  24)) { iconWidth =  24; iconHeight =  24; }
	else if((iconRegionWidth >=  20) && (iconRegionHeight >=  20)) { iconWidth =  20; iconHeight =  20; }
	else if((iconRegionWidth >=  16) && (iconRegionHeight >=  16)) { iconWidth =  16; iconHeight =  16; }

	//Set the program icon image
	if((iconWidth > 0) && (iconHeight > 0))
	{
		HWND logoControl = GetDlgItem(hwnd, IDC_ABOUT_LOGO);
		HANDLE programIconHandle = LoadImage(GetAssemblyHandle(), MAKEINTRESOURCE(IDI_PROGRAM), IMAGE_ICON, iconWidth, iconHeight, LR_SHARED);
		if(programIconHandle != NULL)
		{
			SendMessage(logoControl, STM_SETIMAGE, IMAGE_ICON, (LPARAM)programIconHandle);
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR AboutView::msgPanelWM_PARENTNOTIFY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	switch(LOWORD(wparam))
	{
	case WM_LBUTTONDOWN:{
		//If the user has clicked on a child window within our window region, ensure that
		//the child window gets focus.
		POINT mousePos;
		mousePos.x = (short)LOWORD(lparam);
		mousePos.y = (short)HIWORD(lparam);
		HWND targetWindow = ChildWindowFromPoint(hwnd, mousePos);
		if(targetWindow != NULL)
		{
			SetFocus(targetWindow);
		}
		break;}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR AboutView::msgPanelWM_NOTIFY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	LPNMHDR message = (LPNMHDR)lparam;
	switch(message->code)
	{
	case NM_CLICK:
	case NM_RETURN:{
		//If the user has clicked a hyperlink, open the target in the default browser.
		HWND linkControl = GetDlgItem(hwnd, IDC_ABOUT_URL);
		if(message->hwndFrom == linkControl)
		{
			PNMLINK linkInfo = (PNMLINK)message;
			ShellExecute(hwnd, L"open", linkInfo->item.szUrl, NULL, NULL, SW_SHOW);
		}
		break;}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR AboutView::msgPanelWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(LOWORD(wparam) == IDOK)
	{
		CloseView();
	}
	return TRUE;
}
