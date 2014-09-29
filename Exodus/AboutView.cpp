#include "AboutView.h"
#include "DataConversion/DataConversion.pkg"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
AboutView::AboutView(IUIManager& auiManager, AboutViewPresenter& apresenter, ExodusInterface& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	fontHandle = NULL;
	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_ABOUT));
	SetDialogViewType(DialogMode::Modal, false, DialogPos::Center);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR AboutView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_PARENTNOTIFY:
		return msgWM_PARENTNOTIFY(hwnd, wparam, lparam);
	case WM_NOTIFY:
		return msgWM_NOTIFY(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR AboutView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Retrieve the required information from the version info table for our program
	std::wstring mainExecutablePath = GetModuleFilePath(GetModuleHandle(NULL));
	std::wstring versionText;
	std::wstring copyrightText;
	GetModuleVersionInfoString(mainExecutablePath, VERSIONINFOPROPERTY_PRODUCTVERSION, versionText);
	GetModuleVersionInfoString(mainExecutablePath, VERSIONINFOPROPERTY_LEGALCOPYRIGHT, copyrightText);

	//Load the program version info into the about dialog
	SetDlgItemText(hwnd, IDC_ABOUT_VERSION, versionText.c_str());
	SetDlgItemText(hwnd, IDC_ABOUT_COPYRIGHT, copyrightText.c_str());

	//Create the default font for this window
	int fontPointSize = 8;
	HDC hdc = GetDC(hwnd);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);
	std::wstring fontTypefaceName = L"MS Shell Dlg";
	fontHandle = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &fontTypefaceName[0]);

	//Calculate the marked target position for the device list inside the dialog
	RECT deviceListMarkerRect;
	GetWindowRect(GetDlgItem(hwnd, IDC_ABOUT_DEVICELIST_PLACEHOLDER), &deviceListMarkerRect);
	POINT deviceListMarkerPos;
	deviceListMarkerPos.x = deviceListMarkerRect.left;
	deviceListMarkerPos.y = deviceListMarkerRect.top;
	ScreenToClient(hwnd, &deviceListMarkerPos);
	int deviceListPosX = deviceListMarkerPos.x;
	int deviceListPosY = deviceListMarkerPos.y;
	int deviceListWidth = (int)(deviceListMarkerRect.right - deviceListMarkerRect.left);
	int deviceListHeight = (int)(deviceListMarkerRect.bottom - deviceListMarkerRect.top);

	//Calculate the marked target position for the extension list inside the dialog
	RECT extensionListMarkerRect;
	GetWindowRect(GetDlgItem(hwnd, IDC_ABOUT_EXTENSIONLIST_PLACEHOLDER), &extensionListMarkerRect);
	POINT extensionListMarkerPos;
	extensionListMarkerPos.x = extensionListMarkerRect.left;
	extensionListMarkerPos.y = extensionListMarkerRect.top;
	ScreenToClient(hwnd, &extensionListMarkerPos);
	int extensionListPosX = extensionListMarkerPos.x;
	int extensionListPosY = extensionListMarkerPos.y;
	int extensionListWidth = (int)(extensionListMarkerRect.right - extensionListMarkerRect.left);
	int extensionListHeight = (int)(extensionListMarkerRect.bottom - extensionListMarkerRect.top);

	//Register the DataGrid window class
	WC_DataGrid::RegisterWindowClass((HINSTANCE)GetModuleHandle(NULL));

	//Create the DataGrid child controls
	const unsigned int deviceListControlID = 50000;
	const unsigned int extensionListControlID = 50001;
	HWND hwndDeviceList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_DataGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, deviceListPosX, deviceListPosY, deviceListWidth, deviceListHeight, hwnd, (HMENU)((UINT_PTR)deviceListControlID), (HINSTANCE)GetModuleHandle(NULL), NULL);
	HWND hwndExtensionList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_DataGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, extensionListPosX, extensionListPosY, extensionListWidth, extensionListHeight, hwnd, (HMENU)((UINT_PTR)extensionListControlID), (HINSTANCE)GetModuleHandle(NULL), NULL);

	//Set the default font for the child controls
	SendMessage(hwndDeviceList, WM_SETFONT, (WPARAM)fontHandle, (LPARAM)TRUE);
	SendMessage(hwndExtensionList, WM_SETFONT, (WPARAM)fontHandle, (LPARAM)TRUE);
	SendMessage(hwndDeviceList, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)fontHandle, (LPARAM)TRUE);
	SendMessage(hwndExtensionList, (UINT)WC_DataGrid::WindowMessages::SetDataAreaFont, (WPARAM)fontHandle, (LPARAM)TRUE);

	//Read all the device data to be shown in the grid control
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

	//Insert our columns into the device DataGrid control
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

	//Read all the extension data to be shown in the grid control
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

	//Insert our columns into the device DataGrid control
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

	//Build the list of third party libraries
	std::list<std::wstring> thirdPartyLibraryList;
	thirdPartyLibraryList.push_back(L"expat");
	thirdPartyLibraryList.push_back(L"zlib");
	thirdPartyLibraryList.push_back(L"libpng");
	thirdPartyLibraryList.push_back(L"libjpeg");
	thirdPartyLibraryList.push_back(L"libtiff");
	thirdPartyLibraryList.push_back(L"glew");
	for(std::list<std::wstring>::const_iterator i = thirdPartyLibraryList.begin(); i != thirdPartyLibraryList.end(); ++i)
	{
		SendMessage(GetDlgItem(hwnd, IDC_ABOUT_THIRDLIBRARIES_LIST), LB_ADDSTRING, 0, (LPARAM)i->c_str());
	}

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
		HANDLE programIconHandle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM), IMAGE_ICON, iconWidth, iconHeight, LR_SHARED);
		if(programIconHandle != NULL)
		{
			SendMessage(logoControl, STM_SETIMAGE, IMAGE_ICON, (LPARAM)programIconHandle);
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR AboutView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Delete the default font object
	SendMessage(hwnd, WM_SETFONT, (WPARAM)NULL, (LPARAM)FALSE);
	DeleteObject(fontHandle);
	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR AboutView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//##TODO## Make this dialog resizable
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR AboutView::msgWM_PARENTNOTIFY(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
INT_PTR AboutView::msgWM_NOTIFY(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
INT_PTR AboutView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(LOWORD(wparam) == IDOK)
	{
		CloseView();
	}
	return TRUE;
}
