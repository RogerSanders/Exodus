#include "PlaneView.h"
#include "resource.h"
#include <DataConversion/DataConversion.pkg>

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
PlaneView::PlaneView(IUIManager& uiManager, PlaneViewPresenter& presenter, IS315_5313& model)
:ViewBase(uiManager, presenter), _presenter(presenter), _model(model), _initializedDialog(false), _currentControlFocus(0)
{
	_glrc = 0;
	_buffer = 0;
	_hwndRender = NULL;
	_zoomSettings = { 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 4.5f, 5.0f, 5.5f, 6.0f, 6.5f, 7.0f, 7.5f, 8.0f, 8.5f, 9.0f, 9.5f, 10.0f };

	_selectedLayer = SELECTEDLAYER_LAYERA;
	_displayScreen = true;
	_spriteBoundaries = true;
	_layerAScrollPlaneManual = false;
	_layerBScrollPlaneManual = false;
	_windowScrollPlaneManual = false;
	_spriteScrollPlaneManual = false;
	_layerAMappingBaseManual = false;
	_layerBMappingBaseManual = false;
	_windowMappingBaseManual = false;
	_spriteMappingBaseManual = false;
	_layerAPatternBaseManual = false;
	_layerBPatternBaseManual = false;
	_windowPatternBaseManual = false;
	_spritePatternBaseManual = false;

	SetDialogTemplateSettings(presenter.GetUnqualifiedViewTitle(), GetAssemblyHandle(), MAKEINTRESOURCE(IDD_S315_5313_PLANEVIEW));
	SetDockableViewType();
}

//----------------------------------------------------------------------------------------------------------------------
// Member window procedure
//----------------------------------------------------------------------------------------------------------------------
INT_PTR PlaneView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch (msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_HSCROLL:
		return msgWM_HSCROLL(hwnd, wparam, lparam);
	case WM_VSCROLL:
		return msgWM_VSCROLL(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------
// Event handlers
//----------------------------------------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	// Retrieve the current width of the vertical scroll bar
	RECT scrollBarVOriginalRect;
	GetClientRect(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_VSCROLL), &scrollBarVOriginalRect);
	int scrollBarVOriginalWidth = scrollBarVOriginalRect.right - scrollBarVOriginalRect.left;
	int scrollBarVOriginalHeight = scrollBarVOriginalRect.bottom - scrollBarVOriginalRect.top;

	// If the current width of the vertical scroll bar is different to the required width,
	// resize the scroll bar to match the required size.
	int scrollBarVRequiredWidth = GetSystemMetrics(SM_CXVSCROLL);
	if (scrollBarVOriginalWidth != scrollBarVRequiredWidth)
	{
		SetWindowPos(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_VSCROLL), NULL, 0, 0, scrollBarVRequiredWidth, scrollBarVOriginalHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
	}

	// Retrieve the current height of the horizontal scroll bar
	RECT scrollBarHOriginalRect;
	GetClientRect(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_HSCROLL), &scrollBarHOriginalRect);
	int scrollBarHOriginalWidth = scrollBarHOriginalRect.right - scrollBarHOriginalRect.left;
	int scrollBarHOriginalHeight = scrollBarHOriginalRect.bottom - scrollBarHOriginalRect.top;

	// If the current height of the horizontal scroll bar is different to the required
	// height, resize the scroll bar to match the required size.
	int scrollBarHRequiredHeight = GetSystemMetrics(SM_CYHSCROLL);
	if (scrollBarHOriginalHeight != scrollBarHRequiredHeight)
	{
		SetWindowPos(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_HSCROLL), NULL, 0, 0, scrollBarHOriginalWidth, scrollBarHRequiredHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
	}

	// If we resized either the horizontal or vertical scrollbars, resize the main dialog
	// window to match.
	if ((scrollBarVOriginalWidth != scrollBarVRequiredWidth) || (scrollBarHOriginalHeight != scrollBarHRequiredHeight))
	{
		// Calculate the current size of the dialog window
		RECT mainDialogRect;
		GetWindowRect(hwnd, &mainDialogRect);
		int currentMainDialogWidth = mainDialogRect.right - mainDialogRect.left;
		int currentMainDialogHeight = mainDialogRect.bottom - mainDialogRect.top;

		// Resize the dialog window to the required size
		int newMainDialogWidth = currentMainDialogWidth + (scrollBarVRequiredWidth - scrollBarVOriginalWidth);
		int newMainDialogHeight = currentMainDialogHeight + (scrollBarHRequiredHeight - scrollBarHOriginalHeight);
		SetWindowPos(hwnd, NULL, 0, 0, newMainDialogWidth, newMainDialogHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
	}

	// Create the window class for the render window
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

	// Calculate the marked target position for the child window inside the dialog
	RECT markerRect;
	GetWindowRect(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MARKER), &markerRect);
	POINT markerPos;
	markerPos.x = markerRect.left;
	markerPos.y = markerRect.top;
	ScreenToClient(hwnd, &markerPos);
	int renderWindowPosX = markerPos.x;
	int renderWindowPosY = markerPos.y;
	int renderWindowWidth = markerRect.right - markerRect.left;
	int renderWindowHeight = markerRect.bottom - markerRect.top;

	// Create the window
	_hwndRender = CreateWindowEx(0, L"Plane Render Child", L"Plane Render Child", WS_CHILD, renderWindowPosX, renderWindowPosY, renderWindowWidth, renderWindowHeight, hwnd, NULL, GetAssemblyHandle(), this);
	ShowWindow(_hwndRender, SW_SHOWNORMAL);
	UpdateWindow(_hwndRender);

	// Update the positions of the render window and scrollbar controls
	UpdateControlPositions(hwnd);

	// Set the window controls to their default state
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_LAYERA, (_selectedLayer == SELECTEDLAYER_LAYERA)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_LAYERB, (_selectedLayer == SELECTEDLAYER_LAYERB)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_LAYERWINDOW, (_selectedLayer == SELECTEDLAYER_WINDOW)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_LAYERSPRITES, (_selectedLayer == SELECTEDLAYER_SPRITES)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_DISPLAYSCREEN, (_displayScreen)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_DISPLAYSPRITEBOUNDARIES, (_spriteBoundaries)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PLANESIZELAYERAMANUAL, (_layerAScrollPlaneManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PLANESIZELAYERBMANUAL, (_layerBScrollPlaneManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PLANESIZEWINDOWMANUAL, (_windowScrollPlaneManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PLANESIZESPRITESMANUAL, (_spriteScrollPlaneManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA, (_layerAMappingBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB, (_layerBMappingBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW, (_windowMappingBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES, (_spriteMappingBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERA, (_layerAPatternBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERB, (_layerBPatternBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNWINDOW, (_windowPatternBaseManual)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNSPRITES, (_spritePatternBaseManual)? BST_CHECKED: BST_UNCHECKED);

	// Set the initial enable state for controls
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERA), (_layerAScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERA), (_layerAScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERB), (_layerBScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERB), (_layerBScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHWINDOW), (_windowScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTWINDOW), (_windowScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHSPRITES), (_spriteScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTSPRITES), (_spriteScrollPlaneManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA), (_layerAMappingBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB), (_layerBMappingBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW), (_windowMappingBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES), (_spriteMappingBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERA), (_layerAPatternBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERB), (_layerBPatternBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNWINDOW), (_windowPatternBaseManual)? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNSPRITES), (_spritePatternBaseManual)? TRUE: FALSE);

	// Fill the zoom combobox with its list of values
	for (unsigned int i = 0; i < _zoomSettings.size(); ++i)
	{
		float zoomSetting = _zoomSettings[i];
		std::wstring zoomSettingString;
		FloatToString(zoomSetting, zoomSettingString, false, 1);
		LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_ZOOM), CB_ADDSTRING, 0, (LPARAM)zoomSettingString.c_str());
		SendMessage(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_ZOOM), CB_SETITEMDATA, newItemIndex, (LPARAM)i);
	}

	// Select the first zoom setting by default
	_imageZoomFactor = _zoomSettings[0];
	SendMessage(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_ZOOM), CB_SETCURSEL, (WPARAM)0, 0);

	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	// Note that we need to explicitly destroy the child window here, since we share state
	// with the child window, passing in the "this" pointer as its state. Since the
	// destructor for our state may be called anytime after this window is destroyed, and
	// this window is fully destroyed before child windows are destroyed, we need to
	// explicitly destroy the child window here. The child window is fully destroyed before
	// the DestroyWindow() function returns, and our state is still valid until we return
	// from handling this WM_DESTROY message.
	DestroyWindow(_hwndRender);
	_hwndRender = NULL;

	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	UpdateControlPositions(hwnd);
	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if (HIWORD(wparam) == BN_CLICKED)
	{
		int controlID = (int)LOWORD(wparam);
		switch (controlID)
		{
		case IDC_S315_5313_PLANEVIEW_LAYERA:
			_selectedLayer = SELECTEDLAYER_LAYERA;
			break;
		case IDC_S315_5313_PLANEVIEW_LAYERB:
			_selectedLayer = SELECTEDLAYER_LAYERB;
			break;
		case IDC_S315_5313_PLANEVIEW_LAYERSPRITES:
			_selectedLayer = SELECTEDLAYER_SPRITES;
			break;
		case IDC_S315_5313_PLANEVIEW_LAYERWINDOW:
			_selectedLayer = SELECTEDLAYER_WINDOW;
			break;
		case IDC_S315_5313_PLANEVIEW_DISPLAYSCREEN:
			_displayScreen = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			break;
		case IDC_S315_5313_PLANEVIEW_DISPLAYSPRITEBOUNDARIES:
			_spriteBoundaries = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			break;
		case IDC_S315_5313_PLANEVIEW_PLANESIZELAYERAMANUAL:
			_layerAScrollPlaneManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERA), (_layerAScrollPlaneManual)? TRUE: FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERA), (_layerAScrollPlaneManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PLANESIZELAYERBMANUAL:
			_layerBScrollPlaneManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERB), (_layerBScrollPlaneManual)? TRUE: FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERB), (_layerBScrollPlaneManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PLANESIZEWINDOWMANUAL:
			_windowScrollPlaneManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHWINDOW), (_windowScrollPlaneManual)? TRUE: FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTWINDOW), (_windowScrollPlaneManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PLANESIZESPRITESMANUAL:
			_spriteScrollPlaneManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEWIDTHSPRITES), (_spriteScrollPlaneManual)? TRUE: FALSE);
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PLANEHEIGHTSPRITES), (_spriteScrollPlaneManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_MAPPINGLAYERAMANUAL:
			_layerAMappingBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA), (_layerAMappingBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_MAPPINGLAYERBMANUAL:
			_layerBMappingBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB), (_layerBMappingBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_MAPPINGWINDOWMANUAL:
			_windowMappingBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW), (_windowMappingBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_MAPPINGSPRITESMANUAL:
			_spriteMappingBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES), (_spriteMappingBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PATTERNLAYERAMANUAL:
			_layerAPatternBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERA), (_layerAPatternBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PATTERNLAYERBMANUAL:
			_layerBPatternBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNLAYERB), (_layerBPatternBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PATTERNWINDOWMANUAL:
			_windowPatternBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNWINDOW), (_windowPatternBaseManual)? TRUE: FALSE);
			break;
		case IDC_S315_5313_PLANEVIEW_PATTERNSPRITESMANUAL:
			_spritePatternBaseManual = IsDlgButtonChecked(hwnd, controlID) == BST_CHECKED;
			EnableWindow(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_PATTERNSPRITES), (_spritePatternBaseManual)? TRUE: FALSE);
			break;
		}
	}
	else if ((HIWORD(wparam) == EN_SETFOCUS) && _initializedDialog)
	{
		_previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		_currentControlFocus = LOWORD(wparam);
	}
	else if ((HIWORD(wparam) == EN_KILLFOCUS) && _initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if (newText != _previousText)
		{
			int controlID = (int)LOWORD(wparam);
			switch (controlID)
			{
			case IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERA:
				_layerAScrollPlaneWidth = GetDlgItemBin(hwnd, controlID);
				if (_layerAScrollPlaneWidth <= 0)
				{
					_layerAScrollPlaneWidth = 1;
				}
				UpdateDlgItemBin(hwnd, controlID, _layerAScrollPlaneWidth);
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERA:
				_layerAScrollPlaneHeight = GetDlgItemBin(hwnd, controlID);
				if (_layerAScrollPlaneHeight <= 0)
				{
					_layerAScrollPlaneHeight = 1;
				}
				UpdateDlgItemBin(hwnd, controlID, _layerAScrollPlaneHeight);
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERB:
				_layerBScrollPlaneWidth = GetDlgItemBin(hwnd, controlID);
				if (_layerBScrollPlaneWidth <= 0)
				{
					_layerBScrollPlaneWidth = 1;
				}
				UpdateDlgItemBin(hwnd, controlID, _layerBScrollPlaneWidth);
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERB:
				_layerBScrollPlaneHeight = GetDlgItemBin(hwnd, controlID);
				if (_layerBScrollPlaneHeight <= 0)
				{
					_layerBScrollPlaneHeight = 1;
				}
				UpdateDlgItemBin(hwnd, controlID, _layerBScrollPlaneHeight);
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEWIDTHWINDOW:
				_windowScrollPlaneWidth = GetDlgItemBin(hwnd, controlID);
				if (_windowScrollPlaneWidth <= 0)
				{
					_windowScrollPlaneWidth = 1;
				}
				UpdateDlgItemBin(hwnd, controlID, _windowScrollPlaneWidth);
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEHEIGHTWINDOW:
				_windowScrollPlaneHeight = GetDlgItemBin(hwnd, controlID);
				if (_windowScrollPlaneHeight <= 0)
				{
					_windowScrollPlaneHeight = 1;
				}
				UpdateDlgItemBin(hwnd, controlID, _windowScrollPlaneHeight);
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEWIDTHSPRITES:
				_spriteScrollPlaneWidth = GetDlgItemBin(hwnd, controlID);
				if (_spriteScrollPlaneWidth <= 0)
				{
					_spriteScrollPlaneWidth = 1;
				}
				UpdateDlgItemBin(hwnd, controlID, _spriteScrollPlaneWidth);
				break;
			case IDC_S315_5313_PLANEVIEW_PLANEHEIGHTSPRITES:
				_spriteScrollPlaneHeight = GetDlgItemBin(hwnd, controlID);
				if (_spriteScrollPlaneHeight <= 0)
				{
					_spriteScrollPlaneHeight = 1;
				}
				UpdateDlgItemBin(hwnd, controlID, _spriteScrollPlaneHeight);
				break;
			case IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA:
				_layerAMappingBase = GetDlgItemHex(hwnd, controlID);
				UpdateDlgItemHex(hwnd, controlID, 5, _layerAMappingBase);
				break;
			case IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB:
				_layerBMappingBase = GetDlgItemHex(hwnd, controlID);
				UpdateDlgItemHex(hwnd, controlID, 5, _layerBMappingBase);
				break;
			case IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW:
				_windowMappingBase = GetDlgItemHex(hwnd, controlID);
				UpdateDlgItemHex(hwnd, controlID, 5, _windowMappingBase);
				break;
			case IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES:
				_spriteMappingBase = GetDlgItemHex(hwnd, controlID);
				UpdateDlgItemHex(hwnd, controlID, 5, _spriteMappingBase);
				break;
			case IDC_S315_5313_PLANEVIEW_PATTERNLAYERA:
				_layerAPatternBase = GetDlgItemHex(hwnd, controlID);
				UpdateDlgItemHex(hwnd, controlID, 5, _layerAPatternBase);
				break;
			case IDC_S315_5313_PLANEVIEW_PATTERNLAYERB:
				_layerBPatternBase = GetDlgItemHex(hwnd, controlID);
				UpdateDlgItemHex(hwnd, controlID, 5, _layerBPatternBase);
				break;
			case IDC_S315_5313_PLANEVIEW_PATTERNWINDOW:
				_windowPatternBase = GetDlgItemHex(hwnd, controlID);
				UpdateDlgItemHex(hwnd, controlID, 5, _windowPatternBase);
				break;
			case IDC_S315_5313_PLANEVIEW_PATTERNSPRITES:
				_spritePatternBase = GetDlgItemHex(hwnd, controlID);
				UpdateDlgItemHex(hwnd, controlID, 5, _spritePatternBase);
				break;
			}
		}
	}
	else if ((HIWORD(wparam) == CBN_SELCHANGE) && _initializedDialog)
	{
		switch (LOWORD(wparam))
		{
		case IDC_S315_5313_PLANEVIEW_ZOOM:{
			int itemIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_ZOOM), CB_GETCURSEL, 0, 0);
			_imageZoomFactor = _zoomSettings[(unsigned int)SendMessage(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_ZOOM), CB_GETITEMDATA, itemIndex, NULL)];
			break;}
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_HSCROLL(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	UpdateScrollbar(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_HSCROLL), wParam);
	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
INT_PTR PlaneView::msgWM_VSCROLL(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	UpdateScrollbar(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_VSCROLL), wParam);
	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
void PlaneView::UpdateControlPositions(HWND hwnd)
{
	// Obtain the current size of the client area of the window
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	// Retrieve the current size and position of the vertical scroll bar
	RECT scrollBarVOriginalRect;
	GetWindowRect(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_VSCROLL), &scrollBarVOriginalRect);
	int scrollBarVOriginalWidth = scrollBarVOriginalRect.right - scrollBarVOriginalRect.left;
	POINT scrollBarVPos;
	scrollBarVPos.x = scrollBarVOriginalRect.left;
	scrollBarVPos.y = scrollBarVOriginalRect.top;
	ScreenToClient(hwnd, &scrollBarVPos);
	int scrollBarVOriginalPosY = scrollBarVPos.y;

	// Retrieve the current size and position of the horizontal scroll bar
	RECT scrollBarHOriginalRect;
	GetWindowRect(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_HSCROLL), &scrollBarHOriginalRect);
	int scrollBarHOriginalHeight = scrollBarHOriginalRect.bottom - scrollBarHOriginalRect.top;
	POINT scrollBarHPos;
	scrollBarHPos.x = scrollBarHOriginalRect.left;
	scrollBarHPos.y = scrollBarHOriginalRect.top;
	ScreenToClient(hwnd, &scrollBarHPos);
	int scrollBarHOriginalPosX = scrollBarHPos.x;

	// Update the current size and position of the vertical scroll bar
	int scrollBarVNewHeight = windowHeight - (scrollBarVOriginalPosY + scrollBarHOriginalHeight);
	int scrollBarVNewPosX = windowWidth - scrollBarVOriginalWidth;
	SetWindowPos(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_VSCROLL), NULL, scrollBarVNewPosX, scrollBarVOriginalPosY, scrollBarVOriginalWidth, scrollBarVNewHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	// Update the current size and position of the horizontal scroll bar
	int scrollBarHNewWidth = windowWidth - (scrollBarHOriginalPosX + scrollBarVOriginalWidth);
	int scrollBarHNewPosY = windowHeight - scrollBarHOriginalHeight;
	SetWindowPos(GetDlgItem(hwnd, IDC_S315_5313_PLANEVIEW_HSCROLL), NULL, scrollBarHOriginalPosX, scrollBarHNewPosY, scrollBarHNewWidth, scrollBarHOriginalHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	if (_hwndRender != NULL)
	{
		// Retrieve the current position of the render window
		RECT renderWindowRect;
		GetWindowRect(_hwndRender, &renderWindowRect);
		POINT renderWindowPos;
		renderWindowPos.x = renderWindowRect.left;
		renderWindowPos.y = renderWindowRect.top;
		ScreenToClient(hwnd, &renderWindowPos);
		int renderWindowPosX = renderWindowPos.x;
		int renderWindowPosY = renderWindowPos.y;

		// Set the new window size for the render window
		int renderWindowWidth = scrollBarVNewPosX - renderWindowPosX;
		int renderWindowHeight = scrollBarHNewPosY - renderWindowPosY;
		SetWindowPos(_hwndRender, NULL, renderWindowPosX, renderWindowPosY, renderWindowWidth, renderWindowHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void PlaneView::UpdateScrollbar(HWND scrollWindow, WPARAM wParam)
{
	// Get the current scrollbar info
	SCROLLINFO currentScrollInfo;
	currentScrollInfo.cbSize = sizeof(currentScrollInfo);
	currentScrollInfo.fMask = SIF_TRACKPOS | SIF_RANGE | SIF_POS | SIF_PAGE;
	GetScrollInfo(scrollWindow, SB_CTL, &currentScrollInfo);

	// Calculate the new scrollbar position
	int newScrollPos = currentScrollInfo.nPos;
	switch (LOWORD(wParam))
	{
	case SB_THUMBTRACK:{
		newScrollPos = currentScrollInfo.nTrackPos;
		break;}
	case SB_TOP:
		newScrollPos = 0;
		break;
	case SB_BOTTOM:
		newScrollPos = currentScrollInfo.nMax;
		break;
	case SB_PAGEUP:
		newScrollPos -= currentScrollInfo.nPage;
		break;
	case SB_PAGEDOWN:
		newScrollPos += currentScrollInfo.nPage;
		break;
	case SB_LINEUP:
		newScrollPos -= 1;
		break;
	case SB_LINEDOWN:
		newScrollPos += 1;
		break;
	}
	newScrollPos = (newScrollPos < 0)? 0: newScrollPos;
	newScrollPos = (newScrollPos > currentScrollInfo.nMax)? currentScrollInfo.nMax: newScrollPos;

	// Apply the new scrollbar position
	SCROLLINFO newScrollInfo;
	newScrollInfo.cbSize = sizeof(newScrollInfo);
	newScrollInfo.nPos = newScrollPos;
	newScrollInfo.fMask = SIF_POS;
	SetScrollInfo(scrollWindow, SB_CTL, &newScrollInfo, TRUE);
}

//----------------------------------------------------------------------------------------------------------------------
// Render window procedure
//----------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK PlaneView::WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// Obtain the object pointer
	PlaneView* state = (PlaneView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	// Process the message
	switch (msg)
	{
	case WM_CREATE:
		// Set the object pointer
		state = (PlaneView*)((CREATESTRUCT*)lparam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		// Pass this message on to the member window procedure function
		if (state != 0)
		{
			return state->WndProcRender(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if (state != 0)
		{
			// Pass this message on to the member window procedure function
			LRESULT result = state->WndProcRender(hwnd, msg, wparam, lparam);

			// Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			// Return the result from processing the message
			return result;
		}
		break;
	}

	// Pass this message on to the member window procedure function
	if (state != 0)
	{
		return state->WndProcRender(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT PlaneView::WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
		return msgRenderWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgRenderWM_DESTROY(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgRenderWM_SIZE(hwnd, wparam, lparam);
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

//----------------------------------------------------------------------------------------------------------------------
// Render window event handlers
//----------------------------------------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	// Obtain the current size of the client area of the window
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	// OpenGL Initialization code
	_glrc = CreateOpenGLWindow(hwnd);
	if (_glrc != NULL)
	{
		UpdateRenderWindowSize(hwnd, windowWidth, windowHeight);
	}

	// Allocate a memory buffer for the rendered VRAM data
	_bufferWidth = 128*8;
	_bufferHeight = 128*8;
	_buffer = new unsigned char[_bufferWidth * _bufferHeight * 4];

	SetTimer(hwnd, 1, 50, NULL);
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if (_glrc != NULL)
	{
		wglDeleteContext(_glrc);
		_glrc = NULL;
	}
	if (_buffer != 0)
	{
		delete[] _buffer;
		_buffer = 0;
	}
	KillTimer(hwnd, 1);

	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	// Obtain the current size of the client area of the window
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	// Update the size of the render window
	UpdateRenderWindowSize(hwnd, windowWidth, windowHeight);
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	_initializedDialog = true;

	// Obtain the current DPI scale factors
	float dpiScaleX;
	float dpiScaleY;
	DPIGetScreenScaleFactors(dpiScaleX, dpiScaleY);

	// Calculate the scale factors for the image
	float imageScaleX = dpiScaleX * _imageZoomFactor;
	float imageScaleY = dpiScaleY * _imageZoomFactor;

	// Read the current state of all required registers
	bool h40ModeActive = _model.RegGetRS1();
	bool v30ModeActive = _model.RegGetM3();
	bool interlacingActive = _model.RegGetLSM0();
	bool interlaceMode2Active = interlacingActive && _model.RegGetLSM1();
	bool vscrState = _model.RegGetVSCR();
	bool hscrState = _model.RegGetHSCR();
	bool lscrState = _model.RegGetLSCR();
	unsigned int hscrollDataBase = _model.RegGetHScrollDataBase();
	unsigned int hszState = _model.RegGetHSZ();
	unsigned int vszState = _model.RegGetVSZ();
	unsigned int paletteRowBackground = _model.RegGetBackgroundPaletteRow();
	unsigned int paletteIndexBackground = _model.RegGetBackgroundPaletteColumn();

	// Constants
	const unsigned int blockPixelSizeX = 8;
	const unsigned int blockPixelSizeY = (interlaceMode2Active)? 16: 8;
	const unsigned int screenWidthInCells = (h40ModeActive)? 40: 32;
	const unsigned int screenWidthInPixels = screenWidthInCells * blockPixelSizeX;
	const unsigned int screenHeightInCells = (v30ModeActive)? 30: 28;
	const unsigned int screenHeightInPixels = screenHeightInCells * blockPixelSizeY;

	// Calculate the effective width and height of the main scroll planes based on the
	// current register settings
	unsigned int currentRegisterScrollPlaneWidth;
	unsigned int currentRegisterScrollPlaneHeight;
	_model.CalculateEffectiveCellScrollSize(hszState, vszState, currentRegisterScrollPlaneWidth, currentRegisterScrollPlaneHeight);

	// Latch new settings for the layer A plane where required
	if (!_layerAScrollPlaneManual)
	{
		_layerAScrollPlaneWidth = currentRegisterScrollPlaneWidth;
		_layerAScrollPlaneHeight = currentRegisterScrollPlaneHeight;
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERA, _layerAScrollPlaneWidth);
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERA, _layerAScrollPlaneHeight);
	}
	if (!_layerAMappingBaseManual)
	{
		_layerAMappingBase = _model.RegGetNameTableBaseScrollA();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_MAPPINGLAYERA, 5, _layerAMappingBase);
	}
	if (!_layerAPatternBaseManual)
	{
		_layerAPatternBase = _model.RegGetPatternBaseScrollA();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PATTERNLAYERA, 5, _layerAPatternBase);
	}

	// Latch new settings for the layer B plane where required
	if (!_layerBScrollPlaneManual)
	{
		_layerBScrollPlaneWidth = currentRegisterScrollPlaneWidth;
		_layerBScrollPlaneHeight = currentRegisterScrollPlaneHeight;
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEWIDTHLAYERB, _layerBScrollPlaneWidth);
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEHEIGHTLAYERB, _layerBScrollPlaneHeight);
	}
	if (!_layerBMappingBaseManual)
	{
		_layerBMappingBase = _model.RegGetNameTableBaseScrollB();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_MAPPINGLAYERB, 5, _layerBMappingBase);
	}
	if (!_layerBPatternBaseManual)
	{
		_layerBPatternBase = _model.RegGetPatternBaseScrollB();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PATTERNLAYERB, 5, _layerBPatternBase);
	}

	// Latch new settings for the window plane where required
	if (!_windowScrollPlaneManual)
	{
		_windowScrollPlaneWidth = h40ModeActive? 64: 32;
		_windowScrollPlaneHeight = 32;
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEWIDTHWINDOW, _windowScrollPlaneWidth);
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEHEIGHTWINDOW, _windowScrollPlaneHeight);
	}
	if (!_windowMappingBaseManual)
	{
		_windowMappingBase = _model.RegGetNameTableBaseWindow();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_MAPPINGWINDOW, 5, _windowMappingBase);
	}
	if (!_windowPatternBaseManual)
	{
		_windowPatternBase = _model.RegGetPatternBaseScrollA();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PATTERNWINDOW, 5, _windowPatternBase);
	}

	// Latch new settings for the sprite plane where required
	if (!_spriteScrollPlaneManual)
	{
		const unsigned int spritePosBitCountH = 9;
		const unsigned int spritePosBitCountV = (interlaceMode2Active)? 10: 9;
		_spriteScrollPlaneWidth = (1 << spritePosBitCountH) / blockPixelSizeX;
		_spriteScrollPlaneHeight = (1 << spritePosBitCountV) / blockPixelSizeY;
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEWIDTHSPRITES, _spriteScrollPlaneWidth);
		UpdateDlgItemBin(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PLANEHEIGHTSPRITES, _spriteScrollPlaneHeight);
	}
	if (!_spriteMappingBaseManual)
	{
		_spriteMappingBase = _model.RegGetNameTableBaseSprite();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_MAPPINGSPRITES, 5, _spriteMappingBase);
	}
	if (!_spritePatternBaseManual)
	{
		_spritePatternBase = _model.RegGetPatternBaseSprite();
		UpdateDlgItemHex(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_PATTERNSPRITES, 5, _spritePatternBase);
	}

	// Get the current horizontal scrollbar settings
	SCROLLINFO hscrollInfoCurrent;
	hscrollInfoCurrent.cbSize = sizeof(hscrollInfoCurrent);
	hscrollInfoCurrent.fMask = SIF_POS | SIF_RANGE;
	GetScrollInfo(GetDlgItem(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_HSCROLL), SB_CTL, &hscrollInfoCurrent);
	int currentScrollPosH = hscrollInfoCurrent.nPos;
	int currentScrollMaxH = hscrollInfoCurrent.nMax;

	// Get the current vertical scrollbar settings
	SCROLLINFO vscrollInfoCurrent;
	vscrollInfoCurrent.cbSize = sizeof(vscrollInfoCurrent);
	vscrollInfoCurrent.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	GetScrollInfo(GetDlgItem(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_VSCROLL), SB_CTL, &vscrollInfoCurrent);
	int currentScrollPosV = vscrollInfoCurrent.nPos;
	int currentScrollMaxV = vscrollInfoCurrent.nMax;
	unsigned int currentScrollPageSizeV = vscrollInfoCurrent.nPage;

	// Retrieve the current layer size for the target layer
	unsigned int selectedPlaneWidth = 0;
	unsigned int selectedPlaneHeight = 0;
	switch (_selectedLayer)
	{
	case SELECTEDLAYER_LAYERA:
		selectedPlaneWidth = _layerAScrollPlaneWidth;
		selectedPlaneHeight = _layerAScrollPlaneHeight;
		break;
	case SELECTEDLAYER_LAYERB:
		selectedPlaneWidth = _layerBScrollPlaneWidth;
		selectedPlaneHeight = _layerBScrollPlaneHeight;
		break;
	case SELECTEDLAYER_WINDOW:
		selectedPlaneWidth = _windowScrollPlaneWidth;
		selectedPlaneHeight = _windowScrollPlaneHeight;
		break;
	case SELECTEDLAYER_SPRITES:
		selectedPlaneWidth = _spriteScrollPlaneWidth;
		selectedPlaneHeight = _spriteScrollPlaneHeight;
		break;
	}

	// Apply the latest horizontal scrollbar settings
	if (currentScrollMaxH != (int)selectedPlaneWidth)
	{
		SCROLLINFO hscrollInfo;
		hscrollInfo.cbSize = sizeof(hscrollInfo);
		hscrollInfo.nMin = 0;
		hscrollInfo.nMax = (int)(selectedPlaneWidth > 0)? selectedPlaneWidth - 1: 0;
		hscrollInfo.nPos = currentScrollPosH;
		hscrollInfo.nPage = (unsigned int)((float)(_currentRenderWindowWidth / blockPixelSizeX) / imageScaleX);
		hscrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		SetScrollInfo(GetDlgItem(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_HSCROLL), SB_CTL, &hscrollInfo, TRUE);
	}

	// Apply the latest vertical scrollbar settings
	unsigned int newScrollPageSizeV = _currentRenderWindowHeight / blockPixelSizeY;
	if ((currentScrollMaxV != (int)selectedPlaneHeight) || (currentScrollPageSizeV != newScrollPageSizeV))
	{
		SCROLLINFO vscrollInfo;
		vscrollInfo.cbSize = sizeof(vscrollInfo);
		vscrollInfo.nMin = 0;
		vscrollInfo.nMax = (int)(selectedPlaneHeight > 0)? selectedPlaneHeight - 1: 0;
		vscrollInfo.nPos = currentScrollPosV;
		vscrollInfo.nPage = (unsigned int)((float)newScrollPageSizeV / imageScaleY);
		vscrollInfo.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		SetScrollInfo(GetDlgItem(GetParent(hwnd), IDC_S315_5313_PLANEVIEW_VSCROLL), SB_CTL, &vscrollInfo, TRUE);
	}

	// Calculate the pixel coordinates of the currently visible window region
	unsigned int renderRegionPixelStartX = currentScrollPosH * blockPixelSizeX;
	unsigned int renderRegionPixelStartY = currentScrollPosV * blockPixelSizeY;
	unsigned int renderRegionPixelEndX = renderRegionPixelStartX + _currentRenderWindowWidth;
	unsigned int renderRegionPixelEndY = renderRegionPixelStartY + _currentRenderWindowHeight;
	renderRegionPixelEndX = (renderRegionPixelEndX >= _bufferWidth) ? _bufferWidth : renderRegionPixelEndX;
	renderRegionPixelEndY = (renderRegionPixelEndY >= _bufferHeight) ? _bufferHeight : renderRegionPixelEndY;
	renderRegionPixelEndX = (renderRegionPixelEndX >= (selectedPlaneWidth * blockPixelSizeX)) ? (selectedPlaneWidth * blockPixelSizeX) : renderRegionPixelEndX;
	renderRegionPixelEndY = (renderRegionPixelEndY >= (selectedPlaneHeight * blockPixelSizeY)) ? (selectedPlaneHeight * blockPixelSizeY) : renderRegionPixelEndY;

	// Obtain a copy of the current VRAM data buffer
	_model.LockExternalBuffers();
	ITimedBufferInt* vramBuffer = _model.GetVRAMBuffer();
	std::vector<unsigned char> vramDataCopy(IS315_5313::VramSize, 0);
	if (vramBuffer != 0)
	{
		vramBuffer->GetLatestBufferCopy(vramDataCopy);
	}
	_model.UnlockExternalBuffers();

	// Fill the plane render buffer
	for (unsigned int ypos = 0; ypos < _bufferHeight; ++ypos)
	{
		for (unsigned int xpos = 0; xpos < _bufferWidth; ++xpos)
		{
			// Calculate the position of the current pixel in the currently selected layer
			unsigned int layerPixelPosX = renderRegionPixelStartX + xpos;
			unsigned int layerPixelPosY = renderRegionPixelStartY + ypos;

			// Retrieve the pixel value for the selected layer at the current position
			bool outsideSelectedPlane = true;
			unsigned int paletteRow = { };
			unsigned int paletteIndex = { };
			switch (_selectedLayer)
			{
			case SELECTEDLAYER_LAYERA:
				GetScrollPlanePaletteInfo(vramDataCopy, _layerAMappingBase, _layerAPatternBase, _layerAScrollPlaneWidth, _layerAScrollPlaneHeight, layerPixelPosX, layerPixelPosY, interlaceMode2Active, paletteRow, paletteIndex);
				outsideSelectedPlane = (layerPixelPosX >= (_layerAScrollPlaneWidth * blockPixelSizeX)) || (layerPixelPosY >= (_layerAScrollPlaneHeight * blockPixelSizeY));
				break;
			case SELECTEDLAYER_LAYERB:
				GetScrollPlanePaletteInfo(vramDataCopy, _layerBMappingBase, _layerBPatternBase, _layerBScrollPlaneWidth, _layerBScrollPlaneHeight, layerPixelPosX, layerPixelPosY, interlaceMode2Active, paletteRow, paletteIndex);
				outsideSelectedPlane = (layerPixelPosX >= (_layerBScrollPlaneWidth * blockPixelSizeX)) || (layerPixelPosY >= (_layerBScrollPlaneHeight * blockPixelSizeY));
				break;
			case SELECTEDLAYER_WINDOW:
				GetScrollPlanePaletteInfo(vramDataCopy, _windowMappingBase, _windowPatternBase, _windowScrollPlaneWidth, _windowScrollPlaneHeight, layerPixelPosX, layerPixelPosY, interlaceMode2Active, paletteRow, paletteIndex);
				outsideSelectedPlane = (layerPixelPosX >= (_windowScrollPlaneWidth * blockPixelSizeX)) || (layerPixelPosY >= (_windowScrollPlaneHeight * blockPixelSizeY));
				break;
			case SELECTEDLAYER_SPRITES:
				// If the sprite plane is selected, just fill the buffer with the backdrop
				// colour where appropriate for now. We'll fill in sprite data later.
				paletteRow = 0;
				paletteIndex = 0;
				outsideSelectedPlane = (layerPixelPosX >= (_spriteScrollPlaneWidth * blockPixelSizeX)) || (layerPixelPosY >= (_spriteScrollPlaneHeight * blockPixelSizeY));
				break;
			}

			// Set the initial state for the output colour for this pixel
			unsigned char colorR = 0;
			unsigned char colorG = 0;
			unsigned char colorB = 0;

			// If this pixel isn't outside the boundaries of the currently selected plane,
			// decode the selected palette colour value for this pixel.
			if (!outsideSelectedPlane)
			{
				// If the pixel for the selected layer is transparent, select the current
				// background colour.
				if (paletteIndex == 0)
				{
					paletteRow = paletteRowBackground;
					paletteIndex = paletteIndexBackground;
				}

				// Decode the colour for the target palette entry
				IS315_5313::DecodedPaletteColorEntry color = _model.ReadDecodedPaletteColor(paletteRow, paletteIndex);
				colorR = _model.ColorValueTo8BitValue(color.r, false, false);
				colorG = _model.ColorValueTo8BitValue(color.g, false, false);
				colorB = _model.ColorValueTo8BitValue(color.b, false, false);
			}

			// Write this pixel colour value into the data buffer
			_buffer[((xpos + (ypos * _bufferWidth)) * 4) + 0] = colorR;
			_buffer[((xpos + (ypos * _bufferWidth)) * 4) + 1] = colorG;
			_buffer[((xpos + (ypos * _bufferWidth)) * 4) + 2] = colorB;
			_buffer[((xpos + (ypos * _bufferWidth)) * 4) + 3] = 0xFF;
		}
	}

	// If the sprite plane is currently selected, render each sprite to the buffer.
	std::vector<ScreenBoundaryPrimitive> screenBoundaryPrimitives;
	if (_selectedLayer == SELECTEDLAYER_SPRITES)
	{
		// Render each sprite to the sprite plane
		unsigned int maxSpriteCount = (h40ModeActive)? 80: 64;
		unsigned int currentSpriteNo = 0;
		std::set<unsigned int> processedSprites;
		do
		{
			// Read the mapping data for this sprite
			IS315_5313::SpriteMappingTableEntry spriteMapping = _model.GetSpriteMappingTableEntry(_spriteMappingBase, currentSpriteNo);

			// Render this sprite to the buffer
			unsigned int spriteHeightInCells = spriteMapping.height + 1;
			unsigned int spriteWidthInCells = spriteMapping.width + 1;
			for (unsigned int ypos = 0; ypos < (spriteHeightInCells * blockPixelSizeY); ++ypos)
			{
				for (unsigned int xpos = 0; xpos < (spriteWidthInCells * blockPixelSizeX); ++xpos)
				{
					// If this sprite pixel lies outside the visible buffer region, skip
					// it.
					if (((spriteMapping.xpos + xpos) < renderRegionPixelStartX) || ((spriteMapping.xpos + xpos) >= renderRegionPixelEndX)
					|| ((spriteMapping.ypos + ypos) < renderRegionPixelStartY) || ((spriteMapping.ypos + ypos) >= renderRegionPixelEndY))
					{
						continue;
					}

					// Calculate the target pixel row and column number within the sprite
					unsigned int pixelRowNo = (spriteMapping.vflip)? ((spriteHeightInCells * blockPixelSizeY) - 1) - ypos: ypos;
					unsigned int pixelColumnNo = (spriteMapping.hflip)? ((spriteWidthInCells * blockPixelSizeX) - 1) - xpos: xpos;

					// Calculate the row and column numbers for the target block within the
					// sprite, and the target pattern data within that block.
					unsigned int blockRowNo = pixelRowNo / blockPixelSizeY;
					unsigned int blockColumnNo = pixelColumnNo / blockPixelSizeX;
					unsigned int blockOffset = (blockColumnNo * spriteHeightInCells) + blockRowNo;
					unsigned int patternRowNo = pixelRowNo % blockPixelSizeY;
					unsigned int patternColumnNo = pixelColumnNo % blockPixelSizeX;

					// Calculate the VRAM address of the target pattern row data
					const unsigned int patternDataRowByteSize = 4;
					const unsigned int blockPatternByteSize = blockPixelSizeY * patternDataRowByteSize;
					unsigned int patternRowDataAddress = (((spriteMapping.blockNumber + blockOffset) * blockPatternByteSize) + (patternRowNo * patternDataRowByteSize)) % (unsigned int)vramDataCopy.size();
					patternRowDataAddress = (_spritePatternBase + patternRowDataAddress) % (unsigned int)vramDataCopy.size();

					// Read the pattern data byte for the target pixel in the target block
					const unsigned int pixelsPerPatternByte = 2;
					unsigned int patternByteNo = patternColumnNo / pixelsPerPatternByte;
					bool patternDataUpperHalf = (patternColumnNo % pixelsPerPatternByte) == 0;
					Data patternData(8, vramDataCopy[patternRowDataAddress + patternByteNo]);

					// Return the target palette row and index numbers
					unsigned int paletteRow = spriteMapping.paletteLine;
					unsigned int paletteIndex = patternData.GetDataSegment((patternDataUpperHalf)? 4: 0, 4);

					// If this pixel is transparent, skip it.
					if (paletteIndex == 0)
					{
						continue;
					}

					// Decode the colour for the target palette entry
					IS315_5313::DecodedPaletteColorEntry color = _model.ReadDecodedPaletteColor(paletteRow, paletteIndex);
					unsigned char colorR = _model.ColorValueTo8BitValue(color.r, false, false);
					unsigned char colorG = _model.ColorValueTo8BitValue(color.g, false, false);
					unsigned char colorB = _model.ColorValueTo8BitValue(color.b, false, false);

					// Calculate the location of the target pixel within the data buffer
					unsigned int spritePixelPosXInBuffer = (spriteMapping.xpos + xpos) - renderRegionPixelStartX;
					unsigned int spritePixelPosYInBuffer = (spriteMapping.ypos + ypos) - renderRegionPixelStartY;

					// Write this pixel colour value into the data buffer
					_buffer[((spritePixelPosXInBuffer + (spritePixelPosYInBuffer * _bufferWidth)) * 4) + 0] = colorR;
					_buffer[((spritePixelPosXInBuffer + (spritePixelPosYInBuffer * _bufferWidth)) * 4) + 1] = colorG;
					_buffer[((spritePixelPosXInBuffer + (spritePixelPosYInBuffer * _bufferWidth)) * 4) + 2] = colorB;
					_buffer[((spritePixelPosXInBuffer + (spritePixelPosYInBuffer * _bufferWidth)) * 4) + 3] = 0xFF;
				}
			}

			// Calculate the boundaries of this sprite if requested
			if (_spriteBoundaries)
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

			// Advance to the next sprite in the list
			processedSprites.insert(currentSpriteNo);
			currentSpriteNo = spriteMapping.link;
		}
		while ((currentSpriteNo > 0) && (currentSpriteNo < maxSpriteCount) && (processedSprites.find(currentSpriteNo) == processedSprites.end()));
	}

	// Calculate the screen boundary region information for the target layer, if requested.
	if (_displayScreen)
	{
		if ((_selectedLayer == SELECTEDLAYER_LAYERA) || (_selectedLayer == SELECTEDLAYER_LAYERB))
		{
			for (unsigned int screenRow = 0; screenRow < screenHeightInPixels; ++screenRow)
			{
				unsigned int pixelsPerColumn = _model.CellsPerColumn * blockPixelSizeX;
				unsigned int screenColumnCount = (screenWidthInCells / _model.CellsPerColumn);
				for (unsigned int screenColumn = 0; screenColumn < screenColumnCount; ++screenColumn)
				{
					// Calculate the properties for the selected layer
					bool inLayerA = (_selectedLayer == SELECTEDLAYER_LAYERA);
					unsigned int layerScrollPlaneWidthInPixels = ((_selectedLayer == SELECTEDLAYER_LAYERA)? _layerAScrollPlaneWidth: _layerBScrollPlaneWidth) * blockPixelSizeX;
					unsigned int layerScrollPlaneHeightInPixels = ((_selectedLayer == SELECTEDLAYER_LAYERA)? _layerAScrollPlaneHeight: _layerBScrollPlaneHeight) * blockPixelSizeY;

					// Read the vertical scroll data for this column
					Data vsramDataCache(16);
					unsigned int layerVScrollPatternDisplacement;
					unsigned int layerVScrollMappingDisplacement;
					_model.DigitalRenderReadVscrollData(screenColumn, (inLayerA)? 0: 1, vscrState, interlaceMode2Active, layerVScrollPatternDisplacement, layerVScrollMappingDisplacement, vsramDataCache);

					// Read the horizontal scroll data for this row
					unsigned int layerHScrollPatternDisplacement;
					unsigned int layerHScrollMappingDisplacement;
					GetScrollPlaneHScrollData(vramDataCopy, screenRow, hscrollDataBase, hscrState, lscrState, inLayerA, layerHScrollPatternDisplacement, layerHScrollMappingDisplacement);

					// Calculate the screen boundaries within the selected layer using the
					// scroll data for the current column and row
					unsigned int layerPosScreenStartX = (layerScrollPlaneWidthInPixels - (((layerHScrollMappingDisplacement * pixelsPerColumn) + layerHScrollPatternDisplacement) % layerScrollPlaneWidthInPixels)) % layerScrollPlaneWidthInPixels;
					unsigned int layerPosScreenEndX = (layerPosScreenStartX + screenWidthInPixels) % layerScrollPlaneWidthInPixels;
					unsigned int layerPosScreenStartY = ((layerVScrollMappingDisplacement * blockPixelSizeY) + layerVScrollPatternDisplacement) % layerScrollPlaneHeightInPixels;
					unsigned int layerPosScreenEndY = (layerPosScreenStartY + screenHeightInPixels) % layerScrollPlaneHeightInPixels;
					unsigned int layerPosScreenLastRow = ((layerPosScreenEndY + layerScrollPlaneHeightInPixels) - 1) % layerScrollPlaneHeightInPixels;

					// Calculate the position of the current row and column within the
					// selected layer
					unsigned int layerPixelPosX = (layerPosScreenStartX + (screenColumn * pixelsPerColumn)) % layerScrollPlaneWidthInPixels;
					unsigned int layerPixelPosY = (layerPosScreenStartY + screenRow) % layerScrollPlaneHeightInPixels;

					// Draw the screen boundary line for the left or right of the screen
					// region, if required.
					if ((layerPixelPosX == layerPosScreenStartX)
					&& (((layerPosScreenStartY < layerPosScreenEndY) && (layerPixelPosY >= layerPosScreenStartY) && (layerPixelPosY < layerPosScreenEndY))
					|| ((layerPosScreenStartY > layerPosScreenEndY) && ((layerPixelPosY >= layerPosScreenStartY) || (layerPixelPosY < layerPosScreenEndY)))))
					{
						screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(layerPixelPosX, layerPixelPosX, layerPixelPosY, layerPixelPosY+1));
					}
					if ((((layerPixelPosX + pixelsPerColumn) % layerScrollPlaneWidthInPixels) == layerPosScreenEndX)
					&& (((layerPosScreenStartY < layerPosScreenEndY) && (layerPixelPosY >= layerPosScreenStartY) && (layerPixelPosY < layerPosScreenEndY))
					|| ((layerPosScreenStartY > layerPosScreenEndY) && ((layerPixelPosY >= layerPosScreenStartY) || (layerPixelPosY < layerPosScreenEndY)))))
					{
						unsigned int linePosX = (layerPixelPosX + pixelsPerColumn) % layerScrollPlaneWidthInPixels;
						screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(linePosX, linePosX, layerPixelPosY, layerPixelPosY+1));
					}

					// Draw the screen boundary line for the top or the bottom of the
					// screen region, if required.
					if (((layerPixelPosY == layerPosScreenStartY) || (layerPixelPosY == layerPosScreenLastRow))
					&& (((layerPosScreenStartX < layerPosScreenEndX) && (layerPixelPosX >= layerPosScreenStartX) && (layerPixelPosX < layerPosScreenEndX))
					|| ((layerPosScreenStartX > layerPosScreenEndX) && ((layerPixelPosX >= layerPosScreenStartX) || (layerPixelPosX < layerPosScreenEndX)))))
					{
						// If the current column wraps around to the start of the layer,
						// split the boundary line, otherwise draw a single line to the end
						// of the column.
						unsigned int linePosY = layerPixelPosY + (layerPixelPosY == layerPosScreenLastRow ? 1 : 0);
						if ((layerPixelPosX + pixelsPerColumn) > layerScrollPlaneWidthInPixels)
						{
							screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(layerPixelPosX, layerPixelPosX+pixelsPerColumn, linePosY, linePosY));
							screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(0, (layerPixelPosX+pixelsPerColumn) % layerScrollPlaneWidthInPixels, layerPixelPosY, layerPixelPosY));
						}
						else
						{
							screenBoundaryPrimitives.push_back(ScreenBoundaryPrimitive(layerPixelPosX, layerPixelPosX+pixelsPerColumn, linePosY, linePosY));
						}
					}

					// Calculate the shaded region within the window for the current row
					// and column. If the column wraps around to the start of the layer,
					// split the shaded region, otherwise draw it as a single block.
					if ((layerPixelPosX + pixelsPerColumn) > layerScrollPlaneWidthInPixels)
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
		else if (_selectedLayer == SELECTEDLAYER_WINDOW)
		{
			// Calculate the screen boundary region for the window layer
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
		else if (_selectedLayer == SELECTEDLAYER_SPRITES)
		{
			// Calculate the screen boundary region for the sprite layer
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

	// Adjust all screen boundary primitives to take scrolling and limiting of the render
	// region into account
	std::vector<ScreenBoundaryPrimitive> validScreenBoundaryLines;
	std::vector<ScreenBoundaryPrimitive> validScreenBoundaryQuads;
	for (unsigned int i = 0; i < (unsigned int)screenBoundaryPrimitives.size(); ++i)
	{
		// Take a copy of this screen boundary primitive
		ScreenBoundaryPrimitive primitive = screenBoundaryPrimitives[i];

		// If this primitive is entirely outside the visible window region, skip it.
		if ((primitive.pixelPosXBegin >= renderRegionPixelEndX) || (primitive.pixelPosXEnd < renderRegionPixelStartX)
		|| (primitive.pixelPosYBegin >= renderRegionPixelEndY) || (primitive.pixelPosYEnd < renderRegionPixelStartY))
		{
			continue;
		}

		// Clamp the primitive boundaries to constrain them to the visible render window
		// region
		primitive.pixelPosXBegin = (primitive.pixelPosXBegin < renderRegionPixelStartX)? renderRegionPixelStartX: primitive.pixelPosXBegin;
		primitive.pixelPosXEnd = (primitive.pixelPosXEnd > renderRegionPixelEndX)? renderRegionPixelEndX: primitive.pixelPosXEnd;
		primitive.pixelPosYBegin = (primitive.pixelPosYBegin < renderRegionPixelStartY)? renderRegionPixelStartY: primitive.pixelPosYBegin;
		primitive.pixelPosYEnd = (primitive.pixelPosYEnd > renderRegionPixelEndY)? renderRegionPixelEndY: primitive.pixelPosYEnd;

		// Convert this primitive to be relative to the visible render window region
		primitive.pixelPosXBegin -= renderRegionPixelStartX;
		primitive.pixelPosXEnd -= renderRegionPixelStartX;
		primitive.pixelPosYBegin -= renderRegionPixelStartY;
		primitive.pixelPosYEnd -= renderRegionPixelStartY;

		// Save the modified primitive to the appropriate list of valid primitive
		// definitions
		if (primitive.primitiveIsPolygon)
		{
			validScreenBoundaryQuads.push_back(primitive);
		}
		else
		{
			validScreenBoundaryLines.push_back(primitive);
		}
	}

	// Draw the rendered buffer data to the window
	HDC hdc = GetDC(hwnd);
	if (hdc != NULL)
	{
		if ((_glrc != NULL) && (wglMakeCurrent(hdc, _glrc) != FALSE))
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// Clear the background of the image region
			glClearColor(0.75f, 0.75f, 0.75f, 0.75f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Scale the pixel image based on the current DPI settings and zoom factor
			glPixelZoom(imageScaleX, -imageScaleY);

			// Set the raster position and pixel buffer width to account for scrolling
			glRasterPos2i(0, 0);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, _bufferWidth);

			// Draw the pixel buffer to the screen
			int drawImageWidth = renderRegionPixelEndX - renderRegionPixelStartX;
			int drawImageHeight = renderRegionPixelEndY - renderRegionPixelStartY;
			glDrawPixels(drawImageWidth, drawImageHeight, GL_RGBA, GL_UNSIGNED_BYTE, _buffer);

			// Draw our screen boundary lines
			for (unsigned int i = 0; i < (unsigned int)validScreenBoundaryLines.size(); ++i)
			{
				// Set the colour for this line
				if (validScreenBoundaryLines[i].primitiveIsScreenBoundary)
				{
					glColor3d(0.0, 1.0, 0.0);
				}
				else
				{
					glColor3d(1.0, 1.0, 1.0);
				}

				// Draw the line
				glBegin(GL_LINES);
					glVertex2f((float)validScreenBoundaryLines[i].pixelPosXBegin * imageScaleX, (float)validScreenBoundaryLines[i].pixelPosYBegin * imageScaleY);
					glVertex2f((float)validScreenBoundaryLines[i].pixelPosXEnd * imageScaleX, (float)validScreenBoundaryLines[i].pixelPosYEnd * imageScaleY);
				glEnd();
			}
			glColor3d(1.0, 1.0, 1.0);

			// Draw our screen boundary quads
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4d(0.0, 1.0, 0.0, 0.5);
			for (unsigned int i = 0; i < (unsigned int)validScreenBoundaryQuads.size(); ++i)
			{
				glBegin(GL_QUADS);
					glVertex2f((float)validScreenBoundaryQuads[i].pixelPosXBegin * imageScaleX, (float)validScreenBoundaryQuads[i].pixelPosYBegin * imageScaleY);
					glVertex2f((float)validScreenBoundaryQuads[i].pixelPosXEnd * imageScaleX, (float)validScreenBoundaryQuads[i].pixelPosYBegin * imageScaleY);
					glVertex2f((float)validScreenBoundaryQuads[i].pixelPosXEnd * imageScaleX, (float)validScreenBoundaryQuads[i].pixelPosYEnd * imageScaleY);
					glVertex2f((float)validScreenBoundaryQuads[i].pixelPosXBegin * imageScaleX, (float)validScreenBoundaryQuads[i].pixelPosYEnd * imageScaleY);
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

//----------------------------------------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_LBUTTONDOWN(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetFocus(hwnd);
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_KEYUP(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	ISystemDeviceInterface::KeyCode keyCode;
	if (_presenter.GetSystemInterface().TranslateKeyCode((unsigned int)wparam, keyCode))
	{
		_presenter.GetSystemInterface().HandleInputKeyUp(keyCode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
LRESULT PlaneView::msgRenderWM_KEYDOWN(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	ISystemDeviceInterface::KeyCode keyCode;
	if (_presenter.GetSystemInterface().TranslateKeyCode((unsigned int)wparam, keyCode))
	{
		_presenter.GetSystemInterface().HandleInputKeyDown(keyCode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void PlaneView::UpdateRenderWindowSize(HWND hwnd, int renderWindowWidth, int renderWindowHeight)
{
	_currentRenderWindowWidth = renderWindowWidth;
	_currentRenderWindowHeight = renderWindowHeight;

	HDC hdc = GetDC(hwnd);
	if (hdc != NULL)
	{
		if ((_glrc != NULL) && (wglMakeCurrent(hdc, _glrc) != FALSE))
		{
			glViewport(0, 0, _currentRenderWindowWidth, _currentRenderWindowHeight);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0, (double)_currentRenderWindowWidth, (double)_currentRenderWindowHeight, 0.0, -1.0, 1.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			wglMakeCurrent(NULL, NULL);
		}
		ReleaseDC(hwnd, hdc);
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Render helper methods
//----------------------------------------------------------------------------------------------------------------------
void PlaneView::GetScrollPlanePaletteInfo(const std::vector<unsigned char>& vramData, unsigned int mappingBaseAddress, unsigned int patternBaseAddress, unsigned int planeWidth, unsigned int planeHeight, unsigned int xpos, unsigned int ypos, bool interlaceMode2Active, unsigned int& paletteRow, unsigned int& paletteIndex) const
{
	// Constants
	const unsigned int mappingByteSize = 2;
	const unsigned int pixelsPerPatternByte = 2;
	unsigned int blockPixelSizeX = 8;
	unsigned int blockPixelSizeY = (interlaceMode2Active)? 16: 8;

	// Determine the address of the mapping data to use for this layer
	unsigned int mappingIndex = (((ypos / blockPixelSizeY) % planeHeight) * planeWidth) + ((xpos / blockPixelSizeX) % planeWidth);
	unsigned int mappingAddress = (mappingBaseAddress + (mappingIndex * mappingByteSize)) % (unsigned int)vramData.size();

	// Read the mapping data
	Data mappingData(16);
	mappingData.SetUpperHalf(vramData[mappingAddress+0]);
	mappingData.SetLowerHalf(vramData[mappingAddress+1]);

	// Determine the address of the target row in the target block
	unsigned int patternRowNumberNoFlip = ypos % blockPixelSizeY;
	unsigned int patternRowNumber = _model.CalculatePatternDataRowNumber(patternRowNumberNoFlip, interlaceMode2Active, mappingData);
	unsigned int patternRowDataAddress = _model.CalculatePatternDataRowAddress(patternRowNumber, 0, interlaceMode2Active, mappingData);
	patternRowDataAddress = (patternBaseAddress + patternRowDataAddress) % (unsigned int)vramData.size();

	// Read the pattern data byte for the target pixel in the target block
	bool patternHFlip = mappingData.GetBit(11);
	unsigned int patternColumnNo = (patternHFlip)? (blockPixelSizeX - 1) - (xpos % blockPixelSizeX): (xpos % blockPixelSizeX);
	unsigned int patternByteNo = patternColumnNo / pixelsPerPatternByte;
	bool patternDataUpperHalf = (patternColumnNo % pixelsPerPatternByte) == 0;
	Data patternData(8, vramData[patternRowDataAddress + patternByteNo]);

	// Return the target palette row and index numbers
	paletteRow = mappingData.GetDataSegment(13, 2);
	paletteIndex = patternData.GetDataSegment((patternDataUpperHalf)? 4: 0, 4);
}

//----------------------------------------------------------------------------------------------------------------------
void PlaneView::GetScrollPlaneHScrollData(const std::vector<unsigned char>& vramData, unsigned int screenRowNumber, unsigned int hscrollDataBase, bool hscrState, bool lscrState, bool layerA, unsigned int& layerHscrollPatternDisplacement, unsigned int& layerHscrollMappingDisplacement) const
{
	// Calculate the address of the hscroll data to read for this line
	unsigned int hscrollDataAddress = hscrollDataBase;
	if (hscrState)
	{
		const unsigned int hscrollDataPairSize = 4;
		const unsigned int blockPixelSizeY = 8;
		hscrollDataAddress += lscrState? (screenRowNumber * hscrollDataPairSize): (((screenRowNumber / blockPixelSizeY) * blockPixelSizeY) * hscrollDataPairSize);
	}
	if (!layerA)
	{
		hscrollDataAddress += 2;
	}

	// Read the hscroll data for this line
	unsigned int layerHscrollOffset = ((unsigned int)vramData[(hscrollDataAddress+0) % (unsigned int)vramData.size()] << 8) | (unsigned int)vramData[(hscrollDataAddress+1) % (unsigned int)vramData.size()];

	// Break the hscroll data into its two component parts. The lower 4 bits represent a
	// displacement into the 2-cell column, or in other words, the displacement of the
	// starting pixel within each column, while the upper 6 bits represent an offset for
	// the column mapping data itself.
	// -----------------------------------------
	// | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	// |---------------------------------------|
	// |  Column Shift Value   | Displacement  |
	// -----------------------------------------
	layerHscrollPatternDisplacement = (layerHscrollOffset & 0x00F);
	layerHscrollMappingDisplacement = (layerHscrollOffset & 0x3F0) >> 4;
}
