#include "PlaneView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PlaneView::PlaneView(IUIManager& auiManager, PlaneViewPresenter& apresenter, IS315_5313& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	glrc = 0;
	buffer = 0;
	hwndRender = NULL;

	selectedLayer = SELECTEDLAYER_LAYERA;
	displayScreen = true;
	spriteBoundaries = true;
	layerAScrollPlaneManual = false;
	layerBScrollPlaneManual = false;
	windowScrollPlaneManual = false;
	spriteScrollPlaneManual = false;
	layerAMappingBaseManual = false;
	layerBMappingBaseManual = false;
	windowMappingBaseManual = false;
	spriteMappingBaseManual = false;
	layerAPatternBaseManual = false;
	layerBPatternBaseManual = false;
	windowPatternBaseManual = false;
	spritePatternBaseManual = false;

	SetDialogTemplateSettings(apresenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_S315_5313_PLANEVIEW));
	SetDialogViewType();
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR PlaneView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_HSCROLL:
		return msgWM_HSCROLL(hwnd, wparam, lparam);
	case WM_VSCROLL:
		return msgWM_VSCROLL(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Retrieve the current width of the vertical scroll bar
	RECT scrollBarVOriginalRect;
	GetClientRect(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_VSCROLL), &scrollBarVOriginalRect);
	int scrollBarVOriginalWidth = scrollBarVOriginalRect.right - scrollBarVOriginalRect.left;
	int scrollBarVOriginalHeight = scrollBarVOriginalRect.bottom - scrollBarVOriginalRect.top;

	//If the current width of the vertical scroll bar is different to the required width,
	//resize the scroll bar to match the required size.
	int scrollBarVRequiredWidth = GetSystemMetrics(SM_CXVSCROLL);
	if(scrollBarVOriginalWidth != scrollBarVRequiredWidth)
	{
		SetWindowPos(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_VSCROLL), NULL, 0, 0, scrollBarVRequiredWidth, scrollBarVOriginalHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
	}

	//Retrieve the current height of the horizontal scroll bar
	RECT scrollBarHOriginalRect;
	GetClientRect(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_HSCROLL), &scrollBarHOriginalRect);
	int scrollBarHOriginalWidth = scrollBarHOriginalRect.right - scrollBarHOriginalRect.left;
	int scrollBarHOriginalHeight = scrollBarHOriginalRect.bottom - scrollBarHOriginalRect.top;

	//If the current height of the horizontal scroll bar is different to the required
	//height, resize the scroll bar to match the required size.
	int scrollBarHRequiredHeight = GetSystemMetrics(SM_CYHSCROLL);
	if(scrollBarHOriginalHeight != scrollBarHRequiredHeight)
	{
		SetWindowPos(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_HSCROLL), NULL, 0, 0, scrollBarHOriginalWidth, scrollBarHRequiredHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
	}

	//If we resized either the horizontal or vertical scrollbars, resize the main dialog
	//window to match.
	if((scrollBarVOriginalWidth != scrollBarVRequiredWidth) || (scrollBarHOriginalHeight != scrollBarHRequiredHeight))
	{
		//Calculate the current size of the dialog window
		RECT mainDialogRect;
		GetWindowRect(hwnd, &mainDialogRect);
		int currentMainDialogWidth = mainDialogRect.right - mainDialogRect.left;
		int currentMainDialogHeight = mainDialogRect.bottom - mainDialogRect.top;

		//Resize the dialog window to the required size
		int newMainDialogWidth = currentMainDialogWidth + (scrollBarVRequiredWidth - scrollBarVOriginalWidth);
		int newMainDialogHeight = currentMainDialogHeight + (scrollBarHRequiredHeight - scrollBarHOriginalHeight);
		SetWindowPos(hwnd, NULL, 0, 0, newMainDialogWidth, newMainDialogHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
	}

	//Create the window class for the render window
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProcRenderStatic;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetAssemblyHandle();
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"Plane Render Child";
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);

	//Calculate the marked target position for the child window inside the dialog
	RECT markerRect;
	GetWindowRect(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MARKER), &markerRect);
	POINT markerPos;
	unsigned int width = DPIScaleWidth(64*8);   //markerRect.right - markerRect.left;
	unsigned int height = DPIScaleHeight(64*8); //markerRect.bottom - markerRect.top;
	markerPos.x = markerRect.left;
	markerPos.y = markerRect.top;
	ScreenToClient(hwnd, &markerPos);

	//Create the window
	hwndRender = CreateWindowEx(0, L"Plane Render Child", L"Plane Render Child", WS_CHILD, markerPos.x, markerPos.y, width, height, hwnd, NULL, GetAssemblyHandle(), this);
	ShowWindow(hwndRender, SW_SHOWNORMAL);
	UpdateWindow(hwndRender);

	//Set the window controls to their default state
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_LAYERA, (selectedLayer == SELECTEDLAYER_LAYERA)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_LAYERB, (selectedLayer == SELECTEDLAYER_LAYERB)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_LAYERWINDOW, (selectedLayer == SELECTEDLAYER_WINDOW)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_LAYERSPRITES, (selectedLayer == SELECTEDLAYER_SPRITES)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_DISPLAYSCREEN, (displayScreen)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_DISPLAYSPRITEBOUNDARIES, (spriteBoundaries)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PLANESIZELAYERAMANUAL, (layerAScrollPlaneManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PLANESIZELAYERBMANUAL, (layerBScrollPlaneManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PLANESIZEWINDOWMANUAL, (windowScrollPlaneManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PLANESIZESPRITESMANUAL, (spriteScrollPlaneManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA, (layerAMappingBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB, (layerBMappingBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW, (windowMappingBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES, (spriteMappingBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERA, (layerAPatternBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERB, (layerBPatternBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNWINDOW, (windowPatternBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNSPRITES, (spritePatternBaseManual)? BST_CHECKED: BST_UNCHECKED);

	//Set the initial enable state for controls
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERA), (layerAScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERA), (layerAScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERB), (layerBScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERB), (layerBScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHWINDOW), (windowScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTWINDOW), (windowScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHSPRITES), (spriteScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTSPRITES), (spriteScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA), (layerAMappingBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB), (layerBMappingBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW), (windowMappingBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES), (spriteMappingBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERA), (layerAPatternBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERB), (layerBPatternBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNWINDOW), (windowPatternBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNSPRITES), (spritePatternBaseManual)? TRUE: FALSE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Note that we need to explicitly destroy the child window here, since we share state
	//with the child window, passing in the "this" pointer as its state. Since the
	//destructor for our state may be called anytime after this window is destroyed, and
	//this window is fully destroyed before child windows are destroyed, we need to
	//explicitly destroy the child window here. The child window is fully destroyed before
	//the DestroyWindow() function returns, and our state is still valid until we return
	//from handling this WM_DESTROY message.
	DestroyWindow(hwndRender);
	hwndRender = NULL;

	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		int controlID = (int)LOWORD(wparam);
		switch(controlID)
		{
		case IDC_S315_5313_PLANEVIEW_LAYERA:
			selectedLayer = SELECTEDLAYER_LAYERA;
			break;
		case IDC_S315_5313_PLANEVIEW_LAYERB:
			selectedLayer = SELECTEDLAYER_LAYERB;
			break;
		case IDC_S315_5313_PLANEVIEW_LAYERSPRITES:
			selectedLayer = SELECTEDLAYER_SPRITES;
			break;
		case IDC_S315_5313_PLANEVIEW_LAYERWINDOW:
			selectedLayer = SELECTEDLAYER_WINDOW;
			break;
		case IDC_S315_5313_PLANEVIEW_DISPLAYSCREEN:
			displayScreen = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			break;
		case IDC_S315_5313_PLANEVIEW_DISPLAYSPRITEBOUNDARIES:
			spriteBoundaries = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			break;
		case IDC_S315_5313_PLANEVIEW_PLANESIZELAYERAMANUAL:
			layerAScrollPlaneManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERA), (layerAScrollPlaneManual)? TRUE: FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERA), (layerAScrollPlaneManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PLANESIZELAYERBMANUAL:
			layerBScrollPlaneManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERB), (layerBScrollPlaneManual)? TRUE: FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERB), (layerBScrollPlaneManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PLANESIZEWINDOWMANUAL:
			windowScrollPlaneManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHWINDOW), (windowScrollPlaneManual)? TRUE: FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTWINDOW), (windowScrollPlaneManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PLANESIZESPRITESMANUAL:
			spriteScrollPlaneManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHSPRITES), (spriteScrollPlaneManual)? TRUE: FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTSPRITES), (spriteScrollPlaneManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_MAPPINGLAYERAMANUAL:
			layerAMappingBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA), (layerAMappingBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_MAPPINGLAYERBMANUAL:
			layerBMappingBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB), (layerBMappingBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_MAPPINGWINDOWMANUAL:
			windowMappingBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW), (windowMappingBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_MAPPINGSPRITESMANUAL:
			spriteMappingBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES), (spriteMappingBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PATTERNLAYERAMANUAL:
			layerAPatternBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERA), (layerAPatternBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PATTERNLAYERBMANUAL:
			layerBPatternBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERB), (layerBPatternBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PATTERNWINDOWMANUAL:
			windowPatternBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNWINDOW), (windowPatternBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PATTERNSPRITESMANUAL:
			spritePatternBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNSPRITES), (spritePatternBaseManual)? TRUE: FALSE);
			break;
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
		if(newText != previousText)
		{
			int controlID = (int)LOWORD(wparam);
			switch(controlID)
			{
			case IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERA:
				layerAScrollPlaneWidth = GetDlgItemBin(hwnd, controlID);
				if(layerAScrollPlaneWidth <= 0)
				{
					layerAScrollPlaneWidth = 1;
					UpdateDlgItemBin(hwnd, controlID, layerAScrollPlaneWidth);
				}
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERA:
				layerAScrollPlaneHeight = GetDlgItemBin(hwnd, controlID);
				if(layerAScrollPlaneHeight <= 0)
				{
					layerAScrollPlaneHeight = 1;
					UpdateDlgItemBin(hwnd, controlID, layerAScrollPlaneHeight);
				}
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERB:
				layerBScrollPlaneWidth = GetDlgItemBin(hwnd, controlID);
				if(layerBScrollPlaneWidth <= 0)
				{
					layerBScrollPlaneWidth = 1;
					UpdateDlgItemBin(hwnd, controlID, layerBScrollPlaneWidth);
				}
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERB:
				layerBScrollPlaneHeight = GetDlgItemBin(hwnd, controlID);
				if(layerBScrollPlaneHeight <= 0)
				{
					layerBScrollPlaneHeight = 1;
					UpdateDlgItemBin(hwnd, controlID, layerBScrollPlaneHeight);
				}
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEWIDTHWINDOW:
				windowScrollPlaneWidth = GetDlgItemBin(hwnd, controlID);
				if(windowScrollPlaneWidth <= 0)
				{
					windowScrollPlaneWidth = 1;
					UpdateDlgItemBin(hwnd, controlID, windowScrollPlaneWidth);
				}
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEHEIGHTWINDOW:
				windowScrollPlaneHeight = GetDlgItemBin(hwnd, controlID);
				if(windowScrollPlaneHeight <= 0)
				{
					windowScrollPlaneHeight = 1;
					UpdateDlgItemBin(hwnd, controlID, windowScrollPlaneHeight);
				}
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEWIDTHSPRITES:
				spriteScrollPlaneWidth = GetDlgItemBin(hwnd, controlID);
				if(spriteScrollPlaneWidth <= 0)
				{
					spriteScrollPlaneWidth = 1;
					UpdateDlgItemBin(hwnd, controlID, spriteScrollPlaneWidth);
				}
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEHEIGHTSPRITES:
				spriteScrollPlaneHeight = GetDlgItemBin(hwnd, controlID);
				if(spriteScrollPlaneHeight <= 0)
				{
					spriteScrollPlaneHeight = 1;
					UpdateDlgItemBin(hwnd, controlID, spriteScrollPlaneHeight);
				}
				break;
			case IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA:
				layerAMappingBase = GetDlgItemHex(hwnd, controlID);
				break;
			case IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB:
				layerBMappingBase = GetDlgItemHex(hwnd, controlID);
				break;
			case IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW:
				windowMappingBase = GetDlgItemHex(hwnd, controlID);
				break;
			case IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES:
				spriteMappingBase = GetDlgItemHex(hwnd, controlID);
				break;
			case IDC_S315_5313_PLANEVIEW_PATTERNLAYERA:
				layerAPatternBase = GetDlgItemHex(hwnd, controlID);
				break;
			case IDC_S315_5313_PLANEVIEW_PATTERNLAYERB:
				layerBPatternBase = GetDlgItemHex(hwnd, controlID);
				break;
			case IDC_S315_5313_PLANEVIEW_PATTERNWINDOW:
				windowPatternBase = GetDlgItemHex(hwnd, controlID);
				break;
			case IDC_S315_5313_PLANEVIEW_PATTERNSPRITES:
				spritePatternBase = GetDlgItemHex(hwnd, controlID);
				break;
			}
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_HSCROLL(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Get the current horizontal scrollbar track position
	SCROLLINFO hscrollInfoCurrent;
	hscrollInfoCurrent.cbSize = sizeof(hscrollInfoCurrent);
	hscrollInfoCurrent.fMask = SIF_TRACKPOS;
	GetScrollInfo(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_HSCROLL), SB_CTL, &hscrollInfoCurrent);
	int currentScrollPosH = hscrollInfoCurrent.nTrackPos;

	//Apply the new horizontal scroll track position as the current scroll position
	SCROLLINFO hscrollInfo;
	hscrollInfo.cbSize = sizeof(hscrollInfo);
	hscrollInfo.nPos = currentScrollPosH;
	hscrollInfo.fMask = SIF_POS;
	SetScrollInfo(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_HSCROLL), SB_CTL, &hscrollInfo, TRUE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_VSCROLL(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Get the current vertical scrollbar track position
	SCROLLINFO vscrollInfoCurrent;
	vscrollInfoCurrent.cbSize = sizeof(vscrollInfoCurrent);
	vscrollInfoCurrent.fMask = SIF_TRACKPOS;
	GetScrollInfo(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_VSCROLL), SB_CTL, &vscrollInfoCurrent);
	int currentScrollPosV = vscrollInfoCurrent.nTrackPos;

	//Apply the new vertical scroll track position as the current scroll position
	SCROLLINFO vscrollInfo;
	vscrollInfo.cbSize = sizeof(vscrollInfo);
	vscrollInfo.nPos = currentScrollPosV;
	vscrollInfo.fMask = SIF_POS;
	SetScrollInfo(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_VSCROLL), SB_CTL, &vscrollInfo, TRUE);

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Render window procedure
//----------------------------------------------------------------------------------------
LRESULT CALLBACK PlaneView::WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	PlaneView* state = (PlaneView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_CREATE:
		//Set the object pointer
		state = (PlaneView*)((CREATESTRUCT*)lparam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcRender(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			LRESULT result = state->WndProcRender(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	if(state != 0)
	{
		return state->WndProcRender(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT PlaneView::WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgRenderWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgRenderWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgRenderWM_TIMER(hwnd, wparam, lparam);
	case WM_LBUTTONDOWN:
		return msgRenderWM_LBUTTONDOWN(hwnd, wparam, lparam);
	case WM_KEYUP:
		return msgRenderWM_KEYUP(hwnd, wparam, lparam);
	case WM_KEYDOWN:
		return msgRenderWM_KEYDOWN(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Render window event handlers
//----------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//OpenGL Initialization code
	int screenWidth = DPIScaleWidth(64*8);
	int screenHeight = DPIScaleHeight(64*8);
	glrc = CreateOpenGLWindow(hwnd);
	if(glrc != NULL)
	{
		glViewport(0, 0, screenWidth, screenHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, (float)screenWidth, (float)screenHeight, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	//Allocate a memory buffer for the rendered VRAM data
	int bufferWidth = 64*8;
	int bufferHeight = 64*8;
	buffer = new unsigned char[bufferWidth * bufferHeight * 4];

	SetTimer(hwnd, 1, 50, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(glrc != NULL)
	{
		wglDeleteContext(glrc);
		glrc = NULL;
	}
	if(buffer != 0)
	{
		delete[] buffer;
		buffer = 0;
	}
	KillTimer(hwnd, 1);

	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	//Read the current state of all required registers
	bool h40ModeActive = model.RegGetRS1();
	bool v30ModeActive = model.RegGetM3();
	bool interlacingActive = model.RegGetLSM0();
	bool interlaceMode2Active = interlacingActive && model.RegGetLSM1();
	bool vscrState = model.RegGetVSCR();
	bool hscrState = model.RegGetHSCR();
	bool lscrState = model.RegGetLSCR();
	unsigned int hscrollDataBase = model.RegGetHScrollDataBase();
	unsigned int hszState = model.RegGetHSZ();
	unsigned int vszState = model.RegGetVSZ();
	unsigned int paletteRowBackground = model.RegGetBackgroundPaletteRow();
	unsigned int paletteIndexBackground = model.RegGetBackgroundPaletteColumn();

	//Constants
	const unsigned int blockPixelSizeX = 8;
	const unsigned int blockPixelSizeY = (interlaceMode2Active)? 16: 8;
	const unsigned int width = 64*8;
	const unsigned int height = 64*8;
	const unsigned int screenWidthInCells = (h40ModeActive)? 40: 32;
	const unsigned int screenWidthInPixels = screenWidthInCells * blockPixelSizeX;
	const unsigned int screenHeightInCells = (v30ModeActive)? 30: 28;
	const unsigned int screenHeightInPixels = screenHeightInCells * blockPixelSizeY;

	//Calculate the effective width and height of the main scroll planes based on the
	//current register settings
	unsigned int currentRegisterScrollPlaneWidth;
	unsigned int currentRegisterScrollPlaneHeight;
	model.CalculateEffectiveCellScrollSize(hszState, vszState, currentRegisterScrollPlaneWidth, currentRegisterScrollPlaneHeight);

	//Latch new settings for the layer A plane where required
	if(!layerAScrollPlaneManual)
	{
		layerAScrollPlaneWidth = currentRegisterScrollPlaneWidth;
		layerAScrollPlaneHeight = currentRegisterScrollPlaneHeight;
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERA, layerAScrollPlaneWidth);
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERA, layerAScrollPlaneHeight);
	}
	if(!layerAMappingBaseManual)
	{
		layerAMappingBase = model.RegGetNameTableBaseScrollA();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA, 5, layerAMappingBase);
	}
	if(!layerAPatternBaseManual)
	{
		layerAPatternBase = model.RegGetPatternBaseScrollA();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PATTERNLAYERA, 5, layerAPatternBase);
	}

	//Latch new settings for the layer B plane where required
	if(!layerBScrollPlaneManual)
	{
		layerBScrollPlaneWidth = currentRegisterScrollPlaneWidth;
		layerBScrollPlaneHeight = currentRegisterScrollPlaneHeight;
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERB, layerBScrollPlaneWidth);
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERB, layerBScrollPlaneHeight);
	}
	if(!layerBMappingBaseManual)
	{
		layerBMappingBase = model.RegGetNameTableBaseScrollB();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB, 5, layerBMappingBase);
	}
	if(!layerBPatternBaseManual)
	{
		layerBPatternBase = model.RegGetPatternBaseScrollB();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PATTERNLAYERB, 5, layerBPatternBase);
	}

	//Latch new settings for the window plane where required
	if(!windowScrollPlaneManual)
	{
		windowScrollPlaneWidth = h40ModeActive? 64: 32;
		windowScrollPlaneHeight = 32;
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEWIDTHWINDOW, windowScrollPlaneWidth);
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEHEIGHTWINDOW, windowScrollPlaneHeight);
	}
	if(!windowMappingBaseManual)
	{
		windowMappingBase = model.RegGetNameTableBaseWindow();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW, 5, windowMappingBase);
	}
	if(!windowPatternBaseManual)
	{
		windowPatternBase = model.RegGetPatternBaseScrollA();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PATTERNWINDOW, 5, windowPatternBase);
	}

	//Latch new settings for the sprite plane where required
	if(!spriteScrollPlaneManual)
	{
		const unsigned int spritePosBitCountH = 9;
		const unsigned int spritePosBitCountV = (interlaceMode2Active)? 10: 9;
		spriteScrollPlaneWidth = (1 << spritePosBitCountH) / blockPixelSizeX;
		spriteScrollPlaneHeight = (1 << spritePosBitCountV) / blockPixelSizeY;
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEWIDTHSPRITES, spriteScrollPlaneWidth);
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEHEIGHTSPRITES, spriteScrollPlaneHeight);
	}
	if(!spriteMappingBaseManual)
	{
		spriteMappingBase = model.RegGetNameTableBaseSprite();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES, 5, spriteMappingBase);
	}
	if(!spritePatternBaseManual)
	{
		spritePatternBase = model.RegGetPatternBaseSprite();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PATTERNSPRITES, 5, spritePatternBase);
	}

	//Get the current horizontal scrollbar settings
	SCROLLINFO hscrollInfoCurrent;
	hscrollInfoCurrent.cbSize = sizeof(hscrollInfoCurrent);
	hscrollInfoCurrent.fMask = SIF_POS | SIF_RANGE;
	GetScrollInfo(GetDlgItem(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_HSCROLL), SB_CTL, &hscrollInfoCurrent);
	int currentScrollPosH = hscrollInfoCurrent.nPos;
	int currentScrollMaxH = hscrollInfoCurrent.nMax;

	//Get the current vertical scrollbar settings
	SCROLLINFO vscrollInfoCurrent;
	vscrollInfoCurrent.cbSize = sizeof(vscrollInfoCurrent);
	vscrollInfoCurrent.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	GetScrollInfo(GetDlgItem(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_VSCROLL), SB_CTL, &vscrollInfoCurrent);
	int currentScrollPosV = vscrollInfoCurrent.nPos;
	int currentScrollMaxV = vscrollInfoCurrent.nMax;
	unsigned int currentScrollPageSizeV = vscrollInfoCurrent.nPage;

	//Retrieve the current layer size for the target layer
	unsigned int selectedPlaneWidth = 0;
	unsigned int selectedPlaneHeight = 0;
	switch(selectedLayer)
	{
	case SELECTEDLAYER_LAYERA:
		selectedPlaneWidth = layerAScrollPlaneWidth;
		selectedPlaneHeight = layerAScrollPlaneHeight;
		break;
	case SELECTEDLAYER_LAYERB:
		selectedPlaneWidth = layerBScrollPlaneWidth;
		selectedPlaneHeight = layerBScrollPlaneHeight;
		break;
	case SELECTEDLAYER_WINDOW:
		selectedPlaneWidth = windowScrollPlaneWidth;
		selectedPlaneHeight = windowScrollPlaneHeight;
		break;
	case SELECTEDLAYER_SPRITES:
		selectedPlaneWidth = spriteScrollPlaneWidth;
		selectedPlaneHeight = spriteScrollPlaneHeight;
		break;
	}

	//Apply the latest horizontal scrollbar settings
	if(currentScrollMaxH != (int)selectedPlaneWidth)
	{
		SCROLLINFO hscrollInfo;
		hscrollInfo.cbSize = sizeof(hscrollInfo);
		hscrollInfo.nMin = 0;
		hscrollInfo.nMax = (int)(selectedPlaneWidth > 0)? selectedPlaneWidth - 1: 0;
		hscrollInfo.nPos = currentScrollPosH;
		hscrollInfo.nPage = 64;
		hscrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		SetScrollInfo(GetDlgItem(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_HSCROLL), SB_CTL, &hscrollInfo, TRUE);
	}

	//Apply the latest vertical scrollbar settings
	unsigned int newScrollPageSizeV = (interlaceMode2Active)? 32: 64;
	if((currentScrollMaxV != (int)selectedPlaneHeight) || (currentScrollPageSizeV != newScrollPageSizeV))
	{
		SCROLLINFO vscrollInfo;
		vscrollInfo.cbSize = sizeof(vscrollInfo);
		vscrollInfo.nMin = 0;
		vscrollInfo.nMax = (int)(selectedPlaneHeight > 0)? selectedPlaneHeight - 1: 0;
		vscrollInfo.nPos = currentScrollPosV;
		vscrollInfo.nPage = newScrollPageSizeV;
		vscrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		SetScrollInfo(GetDlgItem(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_VSCROLL), SB_CTL, &vscrollInfo, TRUE);
	}

	//Calculate the pixel coordinates of the currently visible window region
	unsigned int renderRegionPixelStartX = currentScrollPosH * blockPixelSizeX;
	unsigned int renderRegionPixelStartY = currentScrollPosV * blockPixelSizeY;
	unsigned int renderRegionPixelEndX = (currentScrollPosH * blockPixelSizeX) + width;
	unsigned int renderRegionPixelEndY = (currentScrollPosV * blockPixelSizeY) + height;

	//Obtain a copy of the current VRAM data buffer
	model.LockExternalBuffers();
	ITimedBufferInt* vramBuffer = model.GetVRAMBuffer();
	std::vector<unsigned char> vramDataCopy(IS315_5313::vramSize, 0);
	if(vramBuffer != 0)
	{
		vramBuffer->GetLatestBufferCopy(vramDataCopy);
	}
	model.UnlockExternalBuffers();

	//Fill the plane render buffer
	for(unsigned int ypos = 0; ypos < height; ++ypos)
	{
		for(unsigned int xpos = 0; xpos < width; ++xpos)
		{
			//Calculate the position of the current pixel in the currently selected layer
			unsigned int layerPixelPosX = renderRegionPixelStartX + xpos;
			unsigned int layerPixelPosY = renderRegionPixelStartY + ypos;

			//Retrieve the pixel value for the selected layer at the current position
			bool outsideSelectedPlane = true;
			unsigned int paletteRow;
			unsigned int paletteIndex;
			switch(selectedLayer)
			{
			case SELECTEDLAYER_LAYERA:
				GetScrollPlanePaletteInfo(vramDataCopy, layerAMappingBase, layerAPatternBase, layerAScrollPlaneWidth, layerAScrollPlaneHeight, layerPixelPosX, layerPixelPosY, interlaceMode2Active, paletteRow, paletteIndex);
				outsideSelectedPlane = (layerPixelPosX >= (layerAScrollPlaneWidth * blockPixelSizeX)) || (layerPixelPosY >= (layerAScrollPlaneHeight * blockPixelSizeY));
				break;
			case SELECTEDLAYER_LAYERB:
				GetScrollPlanePaletteInfo(vramDataCopy, layerBMappingBase, layerBPatternBase, layerBScrollPlaneWidth, layerBScrollPlaneHeight, layerPixelPosX, layerPixelPosY, interlaceMode2Active, paletteRow, paletteIndex);
				outsideSelectedPlane = (layerPixelPosX >= (layerBScrollPlaneWidth * blockPixelSizeX)) || (layerPixelPosY >= (layerBScrollPlaneHeight * blockPixelSizeY));
				break;
			case SELECTEDLAYER_WINDOW:
				GetScrollPlanePaletteInfo(vramDataCopy, windowMappingBase, windowPatternBase, windowScrollPlaneWidth, windowScrollPlaneHeight, layerPixelPosX, layerPixelPosY, interlaceMode2Active, paletteRow, paletteIndex);
				outsideSelectedPlane = (layerPixelPosX >= (windowScrollPlaneWidth * blockPixelSizeX)) || (layerPixelPosY >= (windowScrollPlaneHeight * blockPixelSizeY));
				break;
			case SELECTEDLAYER_SPRITES:
				//If the sprite plane is selected, just fill the buffer with the backdrop
				//colour where appropriate for now. We'll fill in sprite data later.
				paletteRow = 0;
				paletteIndex = 0;
				outsideSelectedPlane = (layerPixelPosX >= (spriteScrollPlaneWidth * blockPixelSizeX)) || (layerPixelPosY >= (spriteScrollPlaneHeight * blockPixelSizeY));
				break;
			}

			//Set the initial state for the output colour for this pixel
			unsigned char colorR = 0;
			unsigned char colorG = 0;
			unsigned char colorB = 0;

			//If this pixel isn't outside the boundaries of the currently selected plane,
			//decode the selected palette colour value for this pixel.
			if(!outsideSelectedPlane)
			{
				//If the pixel for the selected layer is transparent, select the current
				//background colour.
				if(paletteIndex == 0)
				{
					paletteRow = paletteRowBackground;
					paletteIndex = paletteIndexBackground;
				}

				//Decode the colour for the target palette entry
				IS315_5313::DecodedPaletteColorEntry color = model.ReadDecodedPaletteColor(paletteRow, paletteIndex);
				colorR = model.ColorValueTo8BitValue(color.r, false, false);
				colorG = model.ColorValueTo8BitValue(color.g, false, false);
				colorB = model.ColorValueTo8BitValue(color.b, false, false);
			}

			//Write this pixel colour value into the data buffer
			buffer[((xpos + (((height-1)-ypos) * width)) * 4) + 0] = colorR;
			buffer[((xpos + (((height-1)-ypos) * width)) * 4) + 1] = colorG;
			buffer[((xpos + (((height-1)-ypos) * width)) * 4) + 2] = colorB;
			buffer[((xpos + (((height-1)-ypos) * width)) * 4) + 3] = 0xFF;
		}
	}

	//If the sprite plane is currently selected, render each sprite to the buffer.
	std::vector<ScreenBoundaryPrimitive> screenBoundaryPrimitives;
	if(selectedLayer == SELECTEDLAYER_SPRITES)
	{
		//Render each sprite to the sprite plane
		unsigned int maxSpriteCount = (h40ModeActive)? 80: 64;
		unsigned int currentSpriteNo = 0;
		do
		{
			//Read the mapping data for this sprite
			IS315_5313::SpriteMappingTableEntry spriteMapping = model.GetSpriteMappingTableEntry(currentSpriteNo);

			//Render this sprite to the buffer
			unsigned int spriteHeightInCells = spriteMapping.height + 1;
			unsigned int spriteWidthInCells = spriteMapping.width + 1;
			for(unsigned int ypos = 0; ypos < (spriteHeightInCells * blockPixelSizeY); ++ypos)
			{
				for(unsigned int xpos = 0; xpos < (spriteWidthInCells * blockPixelSizeX); ++xpos)
				{
					//If this sprite pixel lies outside the visible buffer region, skip
					//it.
					if(((spriteMapping.xpos + xpos) < renderRegionPixelStartX) || ((spriteMapping.xpos + xpos) >= renderRegionPixelEndX)
					|| ((spriteMapping.ypos + ypos) < renderRegionPixelStartY) || ((spriteMapping.ypos + ypos) >= renderRegionPixelEndY))
					{
						continue;
					}

					//Calculate the target pixel row and column number within the sprite
					unsigned int pixelRowNo = (spriteMapping.vflip)? ((spriteHeightInCells * blockPixelSizeY) - 1) - ypos: ypos;
					unsigned int pixelColumnNo = (spriteMapping.hflip)? ((spriteWidthInCells * blockPixelSizeX) - 1) - xpos: xpos;

					//Calculate the row and column numbers for the target block within the
					//sprite, and the target pattern data within that block.
					unsigned int blockRowNo = pixelRowNo / blockPixelSizeY;
					unsigned int blockColumnNo = pixelColumnNo / blockPixelSizeX;
					unsigned int blockOffset = (blockColumnNo * spriteHeightInCells) + blockRowNo;
					unsigned int patternRowNo = pixelRowNo % blockPixelSizeY;
					unsigned int patternColumnNo = pixelColumnNo % blockPixelSizeX;

					//Calculate the VRAM address of the target pattern row data
					const unsigned int patternDataRowByteSize = 4;
					const unsigned int blockPatternByteSize = blockPixelSizeY * patternDataRowByteSize;
					unsigned int patternRowDataAddress = (((spriteMapping.blockNumber + blockOffset) * blockPatternByteSize) + (patternRowNo * patternDataRowByteSize)) % (unsigned int)vramDataCopy.size();
					patternRowDataAddress = (spritePatternBase + patternRowDataAddress) % (unsigned int)vramDataCopy.size();

					//Read the pattern data byte for the target pixel in the target block
					const unsigned int pixelsPerPatternByte = 2;
					unsigned int patternByteNo = patternColumnNo / pixelsPerPatternByte;
					bool patternDataUpperHalf = (patternColumnNo % pixelsPerPatternByte) == 0;
					Data patternData(8, vramDataCopy[patternRowDataAddress + patternByteNo]);

					//Return the target palette row and index numbers
					unsigned int paletteRow = spriteMapping.paletteLine;
					unsigned int paletteIndex = patternData.GetDataSegment((patternDataUpperHalf)? 4: 0, 4);

					//If this pixel is transparent, skip it.
					if(paletteIndex == 0)
					{
						continue;
					}

					//Decode the colour for the target palette entry
					IS315_5313::DecodedPaletteColorEntry color = model.ReadDecodedPaletteColor(paletteRow, paletteIndex);
					unsigned char colorR = model.ColorValueTo8BitValue(color.r, false, false);
					unsigned char colorG = model.ColorValueTo8BitValue(color.g, false, false);
					unsigned char colorB = model.ColorValueTo8BitValue(color.b, false, false);

					//Calculate the location of the target pixel within the data buffer
					unsigned int spritePixelPosXInBuffer = (spriteMapping.xpos + xpos) - renderRegionPixelStartX;
					unsigned int spritePixelPosYInBuffer = (spriteMapping.ypos + ypos) - renderRegionPixelStartY;

					//Write this pixel colour value into the data buffer
					buffer[((spritePixelPosXInBuffer + (((height-1)-spritePixelPosYInBuffer) * width)) * 4) + 0] = colorR;
					buffer[((spritePixelPosXInBuffer + (((height-1)-spritePixelPosYInBuffer) * width)) * 4) + 1] = colorG;
					buffer[((spritePixelPosXInBuffer + (((height-1)-spritePixelPosYInBuffer) * width)) * 4) + 2] = colorB;
					buffer[((spritePixelPosXInBuffer + (((height-1)-spritePixelPosYInBuffer) * width)) * 4) + 3] = 0xFF;
				}
			}

			//Calculate the boundaries of this sprite if requested
			if(spriteBoundaries)
			{
				unsigned int spritePosStartX = spriteMapping.xpos;
				unsigned int spritePosStartY = spriteMapping.ypos;
				unsigned int spritePosEndX = spriteMapping.xpos + (spriteWidthInCells * blockPixelSizeX);
				unsigned int spritePosEndY = spriteMapping.ypos + (spriteHeightInCells * blockPixelSizeY);
				screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(spritePosStartX, spritePosStartX, spritePosStartY, spritePosEndY, false, false));
				screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(spritePosEndX, spritePosEndX, spritePosStartY, spritePosEndY, false, false));
				screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(spritePosStartX, spritePosEndX, spritePosStartY, spritePosStartY, false, false));
				screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(spritePosStartX, spritePosEndX, spritePosEndY, spritePosEndY, false, false));
			}

			//Advance to the next sprite in the list
			currentSpriteNo = spriteMapping.link;
		}
		while((currentSpriteNo > 0) && (currentSpriteNo < maxSpriteCount));
	}

	//Calculate the screen boundary region information for the target layer, if requested.
	if(displayScreen)
	{
		if((selectedLayer == SELECTEDLAYER_LAYERA) || (selectedLayer == SELECTEDLAYER_LAYERB))
		{
			for(unsigned int screenRow = 0; screenRow < screenHeightInPixels; ++screenRow)
			{
				unsigned int pixelsPerColumn = model.cellsPerColumn * blockPixelSizeX;
				unsigned int screenColumnCount = (screenWidthInCells / model.cellsPerColumn);
				for(unsigned int screenColumn = 0; screenColumn < screenColumnCount; ++screenColumn)
				{
					//Calculate the properties for the selected layer
					bool inLayerA = (selectedLayer == SELECTEDLAYER_LAYERA);
					unsigned int layerScrollPlaneWidthInPixels = ((selectedLayer == SELECTEDLAYER_LAYERA)? layerAScrollPlaneWidth: layerBScrollPlaneWidth) * blockPixelSizeX;
					unsigned int layerScrollPlaneHeightInPixels = ((selectedLayer == SELECTEDLAYER_LAYERA)? layerAScrollPlaneHeight: layerBScrollPlaneHeight) * blockPixelSizeY;

					//Read the vertical scroll data for this column
					Data vsramDataCache(16);
					unsigned int layerVScrollPatternDisplacement;
					unsigned int layerVScrollMappingDisplacement;
					model.DigitalRenderReadVscrollData(screenColumn, (inLayerA)? 0: 1, vscrState, interlaceMode2Active, layerVScrollPatternDisplacement, layerVScrollMappingDisplacement, vsramDataCache);

					//Read the horizontal scroll data for this row
					unsigned int layerHScrollPatternDisplacement;
					unsigned int layerHScrollMappingDisplacement;
					GetScrollPlaneHScrollData(vramDataCopy, screenRow, hscrollDataBase, hscrState, lscrState, inLayerA, layerHScrollPatternDisplacement, layerHScrollMappingDisplacement);

					//Calculate the screen boundaries within the selected layer using the
					//scroll data for the current column and row
					unsigned int layerPosScreenStartX = (layerScrollPlaneWidthInPixels - (((layerHScrollMappingDisplacement * pixelsPerColumn) + layerHScrollPatternDisplacement) % layerScrollPlaneWidthInPixels)) % layerScrollPlaneWidthInPixels;
					unsigned int layerPosScreenEndX = (layerPosScreenStartX + screenWidthInPixels) % layerScrollPlaneWidthInPixels;
					unsigned int layerPosScreenStartY = ((layerVScrollMappingDisplacement * blockPixelSizeY) + layerVScrollPatternDisplacement) % layerScrollPlaneHeightInPixels;
					unsigned int layerPosScreenEndY = (layerPosScreenStartY + screenHeightInPixels) % layerScrollPlaneHeightInPixels;
					unsigned int layerPosScreenLastRow = ((layerPosScreenEndY + layerScrollPlaneHeightInPixels) - 1) % layerScrollPlaneHeightInPixels;

					//Calculate the position of the current row and column within the
					//selected layer
					unsigned int layerPixelPosX = (layerPosScreenStartX + (screenColumn * pixelsPerColumn)) % layerScrollPlaneWidthInPixels;
					unsigned int layerPixelPosY = (layerPosScreenStartY + screenRow) % layerScrollPlaneHeightInPixels;

					//Draw the screen boundary line for the left or right of the screen
					//region, if required.
					if((layerPixelPosX == layerPosScreenStartX)
					&& (((layerPosScreenStartY < layerPosScreenEndY) && (layerPixelPosY >= layerPosScreenStartY) && (layerPixelPosY < layerPosScreenEndY))
					|| ((layerPosScreenStartY > layerPosScreenEndY) && ((layerPixelPosY >= layerPosScreenStartY) || (layerPixelPosY < layerPosScreenEndY)))))
					{
						screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(layerPixelPosX+1, layerPixelPosX+1, layerPixelPosY, layerPixelPosY+1));
					}
					if((((layerPixelPosX + pixelsPerColumn) % layerScrollPlaneWidthInPixels) == layerPosScreenEndX)
					&& (((layerPosScreenStartY < layerPosScreenEndY) && (layerPixelPosY >= layerPosScreenStartY) && (layerPixelPosY < layerPosScreenEndY))
					|| ((layerPosScreenStartY > layerPosScreenEndY) && ((layerPixelPosY >= layerPosScreenStartY) || (layerPixelPosY < layerPosScreenEndY)))))
					{
						unsigned int linePosX = (layerPixelPosX + pixelsPerColumn + 1) % layerScrollPlaneWidthInPixels;
						screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(linePosX, linePosX, layerPixelPosY, layerPixelPosY+1));
					}

					//Draw the screen boundary line for the top or the bottom of the
					//screen region, if required.
					if(((layerPixelPosY == layerPosScreenStartY) || (layerPixelPosY == layerPosScreenLastRow))
					&& (((layerPosScreenStartX < layerPosScreenEndX) && (layerPixelPosX >= layerPosScreenStartX) && (layerPixelPosX < layerPosScreenEndX))
					|| ((layerPosScreenStartX > layerPosScreenEndX) && ((layerPixelPosX >= layerPosScreenStartX) || (layerPixelPosX < layerPosScreenEndX)))))
					{
						//If the current column wraps around to the start of the layer,
						//split the boundary line, otherwise draw a single line to the end
						//of the column.
						if((layerPixelPosX + pixelsPerColumn) > layerScrollPlaneWidthInPixels)
						{
							screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(layerPixelPosX, layerPixelPosX+pixelsPerColumn, layerPixelPosY, layerPixelPosY));
							screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(0, (layerPixelPosX+pixelsPerColumn) % layerScrollPlaneWidthInPixels, layerPixelPosY, layerPixelPosY));
						}
						else
						{
							screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(layerPixelPosX, layerPixelPosX+pixelsPerColumn, layerPixelPosY, layerPixelPosY));
						}
					}

					//Calculate the shaded region within the window for the current row
					//and column. If the column wraps around to the start of the layer,
					//split the shaded region, otherwise draw it as a single block.
					if((layerPixelPosX + pixelsPerColumn) > layerScrollPlaneWidthInPixels)
					{
						screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(layerPixelPosX, layerPixelPosX+pixelsPerColumn, layerPixelPosY, layerPixelPosY+1, true));
						screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(0, (layerPixelPosX+pixelsPerColumn) % layerScrollPlaneWidthInPixels, layerPixelPosY, layerPixelPosY+1, true));
					}
					else
					{
						screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(layerPixelPosX, layerPixelPosX+pixelsPerColumn, layerPixelPosY, layerPixelPosY+1, true));
					}
				}
			}
		}
		else if(selectedLayer == SELECTEDLAYER_WINDOW)
		{
			//Calculate the screen boundary region for the window layer
			unsigned int windowPosScreenStartX = 0;
			unsigned int windowPosScreenStartY = 0;
			unsigned int windowPosScreenEndX = (windowPosScreenStartX + screenWidthInPixels);
			unsigned int windowPosScreenEndY = (windowPosScreenStartY + screenHeightInPixels);
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(windowPosScreenStartX, windowPosScreenStartX, windowPosScreenStartY, windowPosScreenEndY));
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(windowPosScreenEndX, windowPosScreenEndX, windowPosScreenStartY, windowPosScreenEndY));
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(windowPosScreenStartX, windowPosScreenEndX, windowPosScreenStartY, windowPosScreenStartY));
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(windowPosScreenStartX, windowPosScreenEndX, windowPosScreenEndY, windowPosScreenEndY));
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(windowPosScreenStartX, windowPosScreenEndX, windowPosScreenStartY, windowPosScreenEndY, true));
		}
		else if(selectedLayer == SELECTEDLAYER_SPRITES)
		{
			//Calculate the screen boundary region for the sprite layer
			unsigned int spritePosScreenStartX = 0x80;
			unsigned int spritePosScreenStartY = (interlaceMode2Active)? 0x100: 0x80;
			unsigned int spritePosScreenEndX = (spritePosScreenStartX + screenWidthInPixels);
			unsigned int spritePosScreenEndY = (spritePosScreenStartY + screenHeightInPixels);
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(spritePosScreenStartX, spritePosScreenStartX, spritePosScreenStartY, spritePosScreenEndY));
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(spritePosScreenEndX, spritePosScreenEndX, spritePosScreenStartY, spritePosScreenEndY));
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(spritePosScreenStartX, spritePosScreenEndX, spritePosScreenStartY, spritePosScreenStartY));
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(spritePosScreenStartX, spritePosScreenEndX, spritePosScreenEndY, spritePosScreenEndY));
			screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(spritePosScreenStartX, spritePosScreenEndX, spritePosScreenStartY, spritePosScreenEndY, true));
		}
	}

	//Adjust all screen boundary primitives to take scrolling and limiting of the render
	//region into account
	std::vector<ScreenBoundaryPrimitive> validScreenBoundaryLines;
	std::vector<ScreenBoundaryPrimitive> validScreenBoundaryQuads;
	for(unsigned int i = 0; i < (unsigned int)screenBoundaryPrimitives.size(); ++i)
	{
		//Take a copy of this screen boundary primitive
		ScreenBoundaryPrimitive primitive = screenBoundaryPrimitives[i];

		//If this primitive is entirely outside the visible window region, skip it.
		if((primitive.pixelPosXBegin >= renderRegionPixelEndX) || (primitive.pixelPosXEnd < renderRegionPixelStartX)
		|| (primitive.pixelPosYBegin >= renderRegionPixelEndY) || (primitive.pixelPosYEnd < renderRegionPixelStartY))
		{
			continue;
		}

		//Clamp the primitive boundaries to constrain them to the visible render window
		//region
		primitive.pixelPosXBegin = (primitive.pixelPosXBegin < renderRegionPixelStartX)? renderRegionPixelStartX: primitive.pixelPosXBegin;
		primitive.pixelPosXEnd = (primitive.pixelPosXEnd > renderRegionPixelEndX)? renderRegionPixelEndX: primitive.pixelPosXEnd;
		primitive.pixelPosYBegin = (primitive.pixelPosYBegin < renderRegionPixelStartY)? renderRegionPixelStartY: primitive.pixelPosYBegin;
		primitive.pixelPosYEnd = (primitive.pixelPosYEnd > renderRegionPixelEndY)? renderRegionPixelEndY: primitive.pixelPosYEnd;

		//Convert this primitive to be relative to the visible render window region
		primitive.pixelPosXBegin -= renderRegionPixelStartX;
		primitive.pixelPosXEnd -= renderRegionPixelStartX;
		primitive.pixelPosYBegin -= renderRegionPixelStartY;
		primitive.pixelPosYEnd -= renderRegionPixelStartY;

		//Save the modified primitive to the appropriate list of valid primitive
		//definitions
		if(primitive.primitiveIsPolygon)
		{
			validScreenBoundaryQuads.push_back(primitive);
		}
		else
		{
			validScreenBoundaryLines.push_back(primitive);
		}
	}

	//Draw the rendered buffer data to the window
	HDC hdc = GetDC(hwnd);
	if(hdc != NULL)
	{
		if((glrc != NULL) && (wglMakeCurrent(hdc, glrc) != FALSE))
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			//If a nonstandard DPI mode is active, scale the pixel image based on the
			//current DPI settings.
			float dpiScaleX;
			float dpiScaleY;
			DPIGetScreenScaleFactors(dpiScaleX, dpiScaleY);
			glPixelZoom(dpiScaleX, dpiScaleY);

			//Draw the pixel buffer to the screen
			glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

			//Draw our screen boundary lines
			for(unsigned int i = 0; i < (unsigned int)validScreenBoundaryLines.size(); ++i)
			{
				//Set the colour for this line
				if(validScreenBoundaryLines[i].primitiveIsScreenBoundary)
				{
					glColor3d(0.0, 1.0, 0.0);
				}
				else
				{
					glColor3d(1.0, 1.0, 1.0);
				}

				//Draw the line
				glBegin(GL_LINES);
					glVertex2i(validScreenBoundaryLines[i].pixelPosXBegin, validScreenBoundaryLines[i].pixelPosYBegin);
					glVertex2i(validScreenBoundaryLines[i].pixelPosXEnd, validScreenBoundaryLines[i].pixelPosYEnd);
				glEnd();
			}
			glColor3d(1.0, 1.0, 1.0);

			//Draw our screen boundary quads
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4d(0.0, 1.0, 0.0, 0.5);
			for(unsigned int i = 0; i < (unsigned int)validScreenBoundaryQuads.size(); ++i)
			{
				glBegin(GL_QUADS);
					glVertex2i(validScreenBoundaryQuads[i].pixelPosXBegin, validScreenBoundaryQuads[i].pixelPosYBegin);
					glVertex2i(validScreenBoundaryQuads[i].pixelPosXEnd, validScreenBoundaryQuads[i].pixelPosYBegin);
					glVertex2i(validScreenBoundaryQuads[i].pixelPosXEnd, validScreenBoundaryQuads[i].pixelPosYEnd);
					glVertex2i(validScreenBoundaryQuads[i].pixelPosXBegin, validScreenBoundaryQuads[i].pixelPosYEnd);
				glEnd();
			}
			glColor4d(1.0, 1.0, 1.0, 1.0);
			glDisable(GL_BLEND);

			glFlush();
			SwapBuffers(hdc);
			wglMakeCurrent(NULL, NULL);
		}
		ReleaseDC(hwnd, hdc);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_LBUTTONDOWN(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetFocus(hwnd);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_KEYUP(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	ISystemDeviceInterface::KeyCode keyCode;
	if(presenter.GetSystemInterface().TranslateKeyCode((unsigned int)wparam, keyCode))
	{
		presenter.GetSystemInterface().HandleInputKeyUp(keyCode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_KEYDOWN(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	ISystemDeviceInterface::KeyCode keyCode;
	if(presenter.GetSystemInterface().TranslateKeyCode((unsigned int)wparam, keyCode))
	{
		presenter.GetSystemInterface().HandleInputKeyDown(keyCode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Render helper methods
//----------------------------------------------------------------------------------------
void PlaneView::GetScrollPlanePaletteInfo(const std::vector<unsigned char>& vramData, unsigned int mappingBaseAddress, unsigned int patternBaseAddress, unsigned int planeWidth, unsigned int planeHeight, unsigned int xpos, unsigned int ypos, bool interlaceMode2Active, unsigned int& paletteRow, unsigned int& paletteIndex) const
{
	//Constants
	const unsigned int mappingByteSize = 2;
	const unsigned int pixelsPerPatternByte = 2;
	unsigned int blockPixelSizeX = 8;
	unsigned int blockPixelSizeY = (interlaceMode2Active)? 16: 8;

	//Determine the address of the mapping data to use for this layer
	unsigned int mappingIndex = (((ypos / blockPixelSizeY) % planeHeight) * planeWidth) + ((xpos / blockPixelSizeX) % planeWidth);
	unsigned int mappingAddress = (mappingBaseAddress + (mappingIndex * mappingByteSize)) % (unsigned int)vramData.size();

	//Read the mapping data
	Data mappingData(16);
	mappingData.SetUpperHalf(vramData[mappingAddress+0]);
	mappingData.SetLowerHalf(vramData[mappingAddress+1]);

	//Determine the address of the target row in the target block
	unsigned int patternRowNumber = ypos % blockPixelSizeY;
	unsigned int patternRowDataAddress = model.CalculatePatternDataRowAddress(patternRowNumber, 0, interlaceMode2Active, mappingData);
	patternRowDataAddress = (patternBaseAddress + patternRowDataAddress) % (unsigned int)vramData.size();

	//Read the pattern data byte for the target pixel in the target block
	bool patternHFlip = mappingData.GetBit(11);
	unsigned int patternColumnNo = (patternHFlip)? (blockPixelSizeX - 1) - (xpos % blockPixelSizeX): (xpos % blockPixelSizeX);
	unsigned int patternByteNo = patternColumnNo / pixelsPerPatternByte;
	bool patternDataUpperHalf = (patternColumnNo % pixelsPerPatternByte) == 0;
	Data patternData(8, vramData[patternRowDataAddress + patternByteNo]);

	//Return the target palette row and index numbers
	paletteRow = mappingData.GetDataSegment(13, 2);
	paletteIndex = patternData.GetDataSegment((patternDataUpperHalf)? 4: 0, 4);
}

//----------------------------------------------------------------------------------------
void PlaneView::GetScrollPlaneHScrollData(const std::vector<unsigned char>& vramData, unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, bool layerA, unsigned int& layerHscrollPatternDisplacement, unsigned int& layerHscrollMappingDisplacement) const
{
	//Calculate the address of the hscroll data to read for this line
	unsigned int hscrollDataAddress = hscrollDataBase;
	if(hscrState)
	{
		const unsigned int hscrollDataPairSize = 4;
		const unsigned int blockPixelSizeY = 8;
		hscrollDataAddress += lscrState? (screenRowNumber * hscrollDataPairSize): (((screenRowNumber / blockPixelSizeY) * blockPixelSizeY) * hscrollDataPairSize);
	}
	if(!layerA)
	{
		hscrollDataAddress += 2;
	}

	//Read the hscroll data for this line
	unsigned int layerHscrollOffset = ((unsigned int)vramData[(hscrollDataAddress+0) % (unsigned int)vramData.size()] << 8) | (unsigned int)vramData[(hscrollDataAddress+1) % (unsigned int)vramData.size()];

	//Break the hscroll data into its two component parts. The lower 4 bits represent a
	//displacement into the 2-cell column, or in other words, the displacement of the
	//starting pixel within each column, while the upper 6 bits represent an offset for
	//the column mapping data itself.
	//-----------------------------------------
	//| 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------|
	//|  Column Shift Value   | Displacement  |
	//-----------------------------------------
	layerHscrollPatternDisplacement = (layerHscrollOffset & 0x00F);
	layerHscrollMappingDisplacement = (layerHscrollOffset & 0x3F0) >> 4;
}
