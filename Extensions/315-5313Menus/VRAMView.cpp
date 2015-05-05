#include <WindowsControls/WindowsControls.pkg>
#include <Stream/Stream.pkg>
#include "VRAMView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VRAMView::VRAMView(IUIManager& auiManager, VRAMViewPresenter& apresenter, IS315_5313& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel), initializedDialog(false), currentControlFocus(0)
{
	blockSize = BLOCKSIZE_AUTO;
	selectedPalette = PALETTE_LOWHIGH;
	shadow = false;
	highlight = false;
	hwndLayoutGrid = NULL;
	hwndScrollViewer = NULL;
	hwndControlDialog1 = NULL;
	hwndControlDialog2 = NULL;
	hwndRender = NULL;
	hwndDetails = NULL;
	hwndDetails16 = NULL;
	detailsVisible = false;

	blocksPerRenderRowSetting = 0;
	blockMagnificationFactorSetting = 0;
	renderWindowAvailableWidth = 0;
	renderWindowAvailableHeight = 0;

	SetWindowSettings(apresenter.GetUnqualifiedViewTitle(), WS_CLIPCHILDREN, WS_EX_COMPOSITED, 8 * 0x20, 800);
	SetDockableViewType(true, DockPos::Left);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT VRAMView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
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
	case WM_BOUNCE:
		return msgWM_BOUNCE(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the dialog control panel
	hwndControlDialog1 = CreateChildDialog(hwnd, GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VRAMPATTERN_CONTROL1), std::bind(&VRAMView::WndProcControlDialog1, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	ShowWindow(hwndControlDialog1, SW_SHOWNORMAL);
	UpdateWindow(hwndControlDialog1);
	hwndControlDialog2 = CreateChildDialog(hwnd, GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VRAMPATTERN_CONTROL2), std::bind(&VRAMView::WndProcControlDialog2, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	ShowWindow(hwndControlDialog2, SW_SHOWNORMAL);
	UpdateWindow(hwndControlDialog2);

	//Create the render window
	hwndRender = CreateChildWindow(WS_CHILD | WS_VISIBLE, WS_EX_TRANSPARENT, 0, 0, 0, 0, hwnd, std::bind(&VRAMView::WndProcRender, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	//Register the ScrollViewer window class
	WC_ScrollViewer::RegisterWindowClass(GetAssemblyHandle());

	//Create the ScrollViewer child control
	hwndScrollViewer = CreateWindowEx(WS_EX_TRANSPARENT, WC_ScrollViewer::windowClassName, L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, GetAssemblyHandle(), NULL);

	//Subclass the scroll viewer window so we can intercept position change events
	SetWindowSubclass(hwndScrollViewer, BounceBackSubclassProc, 0, 0);

	//Add each control to the scroll viewer
	SendMessage(hwndScrollViewer, (UINT)WC_ScrollViewer::WindowMessages::AddWindow, 0, (LPARAM)hwndRender);

	//Register the LayoutGrid window class
	WC_LayoutGrid::RegisterWindowClass(GetAssemblyHandle());

	//Create the LayoutGrid child control
	hwndLayoutGrid = CreateWindowEx(WS_EX_TRANSPARENT, WC_LayoutGrid::windowClassName, L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, GetAssemblyHandle(), NULL);

	//Register the StackPanel window class
	WC_StackPanel::RegisterWindowClass(GetAssemblyHandle());

	//Create the StackPanel child control
	HWND hwndStackPanel = CreateWindowEx(WS_EX_TRANSPARENT, WC_StackPanel::windowClassName, L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, GetAssemblyHandle(), NULL);
	SendMessage(hwndStackPanel, (UINT)WC_StackPanel::WindowMessages::SetWrappingEnabled, 1, 0);
	SendMessage(hwndStackPanel, (UINT)WC_StackPanel::WindowMessages::SetHorizontalPadding, DPIScaleWidth(7), 0);
	SendMessage(hwndStackPanel, (UINT)WC_StackPanel::WindowMessages::SetVerticalPadding, DPIScaleHeight(7), 0);

	//Add each child control to the layout grid
	SendMessage(hwndStackPanel, (UINT)WC_StackPanel::WindowMessages::AddWindow, 0, (LPARAM)hwndControlDialog1);
	SendMessage(hwndStackPanel, (UINT)WC_StackPanel::WindowMessages::AddWindow, 0, (LPARAM)hwndControlDialog2);

	//Insert our rows and columns into the layout grid
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddRow, 0, (LPARAM)&(const WC_LayoutGrid::AddRowParams&)WC_LayoutGrid::AddRowParams(WC_LayoutGrid::SizeMode::Content));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddRow, 0, (LPARAM)&(const WC_LayoutGrid::AddRowParams&)WC_LayoutGrid::AddRowParams(WC_LayoutGrid::SizeMode::Proportional));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddColumn, 0, (LPARAM)&(const WC_LayoutGrid::AddColumnParams&)WC_LayoutGrid::AddColumnParams(WC_LayoutGrid::SizeMode::Proportional));

	//Add each child control to the layout grid
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(hwndStackPanel, 0, 0).SetSizeMode(WC_LayoutGrid::WindowSizeMode::Proportional, WC_LayoutGrid::WindowSizeMode::Fixed).SetPadding(DPIScaleWidth(7), DPIScaleHeight(7)));
	SendMessage(hwndLayoutGrid, (UINT)WC_LayoutGrid::WindowMessages::AddWindow, 0, (LPARAM)&(const WC_LayoutGrid::AddWindowParams&)WC_LayoutGrid::AddWindowParams(hwndScrollViewer, 1, 0).SetSizeMode(WC_LayoutGrid::WindowSizeMode::Proportional));

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Note that we need to explicitly destroy the child window here, since we share state
	//with the child window, passing in the "this" pointer as its state. Since the
	//destructor for our state may be called anytime after this window is destroyed, and
	//this window is fully destroyed before child windows are destroyed, we need to
	//explicitly destroy the child window here. The child window is fully destroyed before
	//the DestroyWindow() function returns, and our state is still valid until we return
	//from handling this WM_DESTROY message.
	DestroyWindow(hwndLayoutGrid);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgWM_SIZE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Read the new client size of the window
	RECT rect;
	GetClientRect(hwnd, &rect);
	int controlWidth = rect.right;
	int controlHeight = rect.bottom;

	//Resize the layout grid to the desired width and height
	SetWindowPos(hwndLayoutGrid, NULL, 0, 0, controlWidth, controlHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgWM_ERASEBKGND(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Ensure that our window background is painted with the dialog background colour
	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect((HDC)wParam, &rect, GetSysColorBrush(COLOR_3DFACE));
	return TRUE;
}

//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgWM_BOUNCE(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	BounceMessage* bounceMessage = (BounceMessage*)lParam;
	if(bounceMessage->hwnd == hwndScrollViewer)
	{
		if(bounceMessage->uMsg == WM_SIZE)
		{
			renderWindowAvailableWidth = LOWORD(bounceMessage->lParam);
			renderWindowAvailableHeight = HIWORD(bounceMessage->lParam);
			UpdateRenderWindowSizeIfRequired();
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Control dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR VRAMView::WndProcControlDialog1(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgControlDialog1WM_INITDIALOG(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgControlDialog1WM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
INT_PTR VRAMView::WndProcControlDialog2(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgControlDialog2WM_INITDIALOG(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgControlDialog2WM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Control dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR VRAMView::msgControlDialog1WM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Set the window controls to their default state
	CheckDlgButton(hwnd, IDC_VDP_VRAM_PALETTE1, (selectedPalette == PALETTE_LINE1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_PALETTE2, (selectedPalette == PALETTE_LINE2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_PALETTE3, (selectedPalette == PALETTE_LINE3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_PALETTE4, (selectedPalette == PALETTE_LINE4)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_LOWHIGH, (selectedPalette == PALETTE_LOWHIGH)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_HIGHLOW, (selectedPalette == PALETTE_HIGHLOW)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_SHADOW, (shadow)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_HIGHLIGHT, (highlight)? BST_CHECKED: BST_UNCHECKED);
	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VRAMView::msgControlDialog1WM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_VDP_VRAM_PALETTE1:
			selectedPalette = PALETTE_LINE1;
			break;
		case IDC_VDP_VRAM_PALETTE2:
			selectedPalette = PALETTE_LINE2;
			break;
		case IDC_VDP_VRAM_PALETTE3:
			selectedPalette = PALETTE_LINE3;
			break;
		case IDC_VDP_VRAM_PALETTE4:
			selectedPalette = PALETTE_LINE4;
			break;
		case IDC_VDP_VRAM_LOWHIGH:
			selectedPalette = PALETTE_LOWHIGH;
			break;
		case IDC_VDP_VRAM_HIGHLOW:
			selectedPalette = PALETTE_HIGHLOW;
			break;
		case IDC_VDP_VRAM_SHADOW:
			shadow = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		case IDC_VDP_VRAM_HIGHLIGHT:
			highlight = IsDlgButtonChecked(hwnd, LOWORD(wparam)) == BST_CHECKED;
			break;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VRAMView::msgControlDialog2WM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Set the window controls to their default state
	CheckDlgButton(hwnd, IDC_VDP_VRAM_BLOCKAUTO, (blockSize == BLOCKSIZE_AUTO)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_BLOCK8X8, (blockSize == BLOCKSIZE_8X8)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_BLOCK8X16, (blockSize == BLOCKSIZE_8X16)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemBin(hwnd, IDC_VDP_VRAM_MAGINIFICATION, blockMagnificationFactorSetting);
	UpdateDlgItemBin(hwnd, IDC_VDP_VRAM_BLOCKSPERROW, blocksPerRenderRowSetting);
	initializedDialog = true;

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VRAMView::msgControlDialog2WM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(HIWORD(wparam) == BN_CLICKED)
	{
		switch(LOWORD(wparam))
		{
		case IDC_VDP_VRAM_BLOCKAUTO:
			blockSize = BLOCKSIZE_AUTO;
			break;
		case IDC_VDP_VRAM_BLOCK8X8:
			blockSize = BLOCKSIZE_8X8;
			break;
		case IDC_VDP_VRAM_BLOCK8X16:
			blockSize = BLOCKSIZE_8X16;
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
			case IDC_VDP_VRAM_MAGINIFICATION:
				blockMagnificationFactorSetting = GetDlgItemBin(hwnd, controlID);
				UpdateDlgItemBin(hwnd, controlID, blockMagnificationFactorSetting);
				UpdateRenderWindowSizeIfRequired();
				break;
			case IDC_VDP_VRAM_BLOCKSPERROW:
				blocksPerRenderRowSetting = GetDlgItemBin(hwnd, controlID);
				UpdateDlgItemBin(hwnd, controlID, blocksPerRenderRowSetting);
				UpdateRenderWindowSizeIfRequired();
				break;
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------------------
//Render window procedure
//----------------------------------------------------------------------------------------
LRESULT VRAMView::WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgRenderWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgRenderWM_DESTROY(hwnd, wparam, lparam);
	case WM_PAINT:
		return msgRenderWM_PAINT(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgRenderWM_TIMER(hwnd, wparam, lparam);
	case WM_MOUSEMOVE:
		return msgRenderWM_MOUSEMOVE(hwnd, wparam, lparam);
	case WM_MOUSELEAVE:
		return msgRenderWM_MOUSELEAVE(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Render window event handlers
//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgRenderWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the popup tile viewer window
	hwndDetails = CreateChildDialog(hwnd, GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_VRAM_DETAILS), std::bind(&VRAMView::WndProcDetails, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	hwndDetails16 = CreateChildDialog(hwnd, GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_VRAM_DETAILS16), std::bind(&VRAMView::WndProcDetails, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	//Process the initial size of the render window
	UpdateRenderWindowSizeIfRequired();

	//Start the refresh timer
	SetTimer(hwnd, 1, 200, NULL);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgRenderWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwndDetails);
	DestroyWindow(hwndDetails16);
	hwndDetails = NULL;
	hwndDetails16 = NULL;
	KillTimer(hwnd, 1);

	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgRenderWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintInfo;
	HDC hdc = BeginPaint(hwnd, &paintInfo);

	Stream::Buffer ddbData(0);
	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	vramImage.SaveDIBImage(ddbData, &bitmapInfo.bmiHeader);

	unsigned int imageWidth = (unsigned int)bitmapInfo.bmiHeader.biWidth;
	unsigned int imageHeight = (unsigned int)bitmapInfo.bmiHeader.biHeight;
	HBITMAP hbitmap = CreateCompatibleBitmap(hdc, (int)imageWidth, (int)imageHeight);
	SetDIBits(hdc, hbitmap, 0, imageHeight, ddbData.GetRawBuffer(), &bitmapInfo, 0);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbitmap);

	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchBlt(hdc, 0, 0, (int)(imageWidth * blockMagnificationFactor), (int)(imageHeight * blockMagnificationFactor), hdcMem, 0, 0, (int)imageWidth, (int)imageHeight, SRCCOPY);

	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
	DeleteObject(hbitmap);

	EndPaint(hwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgRenderWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Obtain a copy of the current VRAM data buffer
	bool obtainedVRAMData = false;
	model.LockExternalBuffers();
	ITimedBufferInt* vramBuffer = model.GetVRAMBuffer();
	std::vector<unsigned char> vramDataCopy;
	if(vramBuffer != 0)
	{
		vramBuffer->GetLatestBufferCopy(vramDataCopy);
		obtainedVRAMData = true;
	}
	model.UnlockExternalBuffers();
	if(!obtainedVRAMData)
	{
		return 0;
	}

	//Constants
	static const unsigned int vramSize = 0x10000;
	static const unsigned int pixelsPerByte = 2;
	static const unsigned int pixelsInVRAM = vramSize * pixelsPerByte;
	static const unsigned int blockPixelSizeX = 8;
	unsigned int blockPixelSizeY = ((blockSize == BLOCKSIZE_8X16) || ((blockSize == BLOCKSIZE_AUTO) && (model.RegGetLSM0()) && (model.RegGetLSM1())))? 16: 8;
	unsigned int pixelsPerBufferRow = blocksPerRenderRow * blockPixelSizeX;
	unsigned int bufferVisibleBlockRowCount = ((pixelsInVRAM / (pixelsPerBufferRow * blockPixelSizeY)) + (((pixelsInVRAM % (pixelsPerBufferRow * blockPixelSizeY)) > 0)? 1: 0));
	unsigned int bufferVisiblePixelRowCount = bufferVisibleBlockRowCount * blockPixelSizeY;

	vramImage.SetImageFormat(pixelsPerBufferRow, bufferVisiblePixelRowCount, IImage::PIXELFORMAT_RGB, IImage::DATAFORMAT_8BIT);

	//Fill the VRAM render buffer
	for(unsigned int byteNo = 0; byteNo < vramSize; ++byteNo)
	{
		unsigned char nextByte = vramDataCopy[byteNo];
		for(unsigned int pixelNo = 0; pixelNo < pixelsPerByte; ++pixelNo)
		{
			//Decode the colour for the selected pixel
			unsigned int index = (nextByte >> (((pixelsPerByte - 1) - pixelNo) * 4)) & 0x0F;
			unsigned char r = 0;
			unsigned char g = 0;
			unsigned char b = 0;
			if(selectedPalette == PALETTE_LOWHIGH)
			{
				r = (unsigned char)index * 17;
				g = (unsigned char)index * 17;
				b = (unsigned char)index * 17;
				if(shadow && !highlight)
				{
					r /= 2;
					g /= 2;
					b /= 2;
				}
				else if(highlight && !shadow)
				{
					r = (r / 2) + 0x80;
					g = (g / 2) + 0x80;
					b = (b / 2) + 0x80;
				}
			}
			else if(selectedPalette == PALETTE_HIGHLOW)
			{
				r = 0xFF - ((unsigned char)index * 17);
				g = 0xFF - ((unsigned char)index * 17);
				b = 0xFF - ((unsigned char)index * 17);
				if(shadow && !highlight)
				{
					r /= 2;
					g /= 2;
					b /= 2;
				}
				else if(highlight && !shadow)
				{
					r = (r / 2) + 0x80;
					g = (g / 2) + 0x80;
					b = (b / 2) + 0x80;
				}
			}
			else
			{
				//Decode the colour for the target palette entry
				IS315_5313::DecodedPaletteColorEntry color = model.ReadDecodedPaletteColor(selectedPalette, index);
				r = model.ColorValueTo8BitValue(color.r, shadow, highlight);
				g = model.ColorValueTo8BitValue(color.g, shadow, highlight);
				b = model.ColorValueTo8BitValue(color.b, shadow, highlight);
			}

			//Calculate the position in the data buffer to store the decoded colour. Note
			//that a lot of math is required here. We've got to convert a raw byte index
			//for a tile-based image buffer into a raw byte index for a scanline-based
			//image buffer, so this is a non-trivial conversion.
			unsigned int blockNo = (byteNo * pixelsPerByte) / (blockPixelSizeX * blockPixelSizeY);
			unsigned int pixelNoWithinBlock = ((byteNo * pixelsPerByte) + pixelNo) % (blockPixelSizeX * blockPixelSizeY);
			unsigned int blockRowNo = pixelNoWithinBlock / blockPixelSizeX;
			unsigned int blockColumnNo = pixelNoWithinBlock % blockPixelSizeX;
			unsigned int blockBufferRowNo = ((blockNo / blocksPerRenderRow) * blockPixelSizeY) + blockRowNo;
			unsigned int blockBufferColumnNo = ((blockNo % blocksPerRenderRow) * blockPixelSizeX) + blockColumnNo;

			//Copy the decoded colour data into the buffer
			vramImage.WritePixelData(blockBufferColumnNo, blockBufferRowNo, 0, r);
			vramImage.WritePixelData(blockBufferColumnNo, blockBufferRowNo, 1, g);
			vramImage.WritePixelData(blockBufferColumnNo, blockBufferRowNo, 2, b);

			//Copy the data into the details popup
			if(blockNo == tileNumber)
			{
				tileDetails[blockRowNo][blockColumnNo].r = r;
				tileDetails[blockRowNo][blockColumnNo].g = g;
				tileDetails[blockRowNo][blockColumnNo].b = b;
				tileDetails[blockRowNo][blockColumnNo].value = index;
			}

			//If we've just output the last pixel at the end of a row within the last
			//block, pad this scanline in the data buffer out to the end of the line.
			unsigned int lastBlockNo = ((pixelsInVRAM / (blockPixelSizeX * blockPixelSizeY)) - 1);
			if((blockNo == lastBlockNo) && (blockColumnNo == (blockPixelSizeX - 1)))
			{
				unsigned int paddingPixelsAfterLastBlock = ((blocksPerRenderRow - 1) - (lastBlockNo % blocksPerRenderRow)) * blockPixelSizeX;
				for(unsigned int i = 0; i < paddingPixelsAfterLastBlock; ++i)
				{
					vramImage.WritePixelData(blockBufferColumnNo + 1 + i, blockBufferRowNo, 0, (unsigned char)0);
					vramImage.WritePixelData(blockBufferColumnNo + 1 + i, blockBufferRowNo, 1, (unsigned char)0);
					vramImage.WritePixelData(blockBufferColumnNo + 1 + i, blockBufferRowNo, 2, (unsigned char)0);
				}
			}
		}
	}

	InvalidateRect(hwnd, NULL, FALSE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgRenderWM_MOUSEMOVE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Constants
	static const unsigned int vramSize = 0x10000;
	static const unsigned int pixelsPerByte = 2;
	detailsBlock16 = (blockSize == BLOCKSIZE_8X16) || ((blockSize == BLOCKSIZE_AUTO) && (model.RegGetLSM0()) && (model.RegGetLSM1()));
	unsigned int blockPixelSizeX = 8;
	unsigned int blockPixelSizeY = (detailsBlock16)? 16: 8;

	//Calculate the new target tile number for the details popup
	int xpos = (short)LOWORD(lparam);
	int ypos = (short)HIWORD(lparam);
	unsigned int blockPosX = xpos / (blockPixelSizeX * blockMagnificationFactor);
	unsigned int blockPosY = ypos / (blockPixelSizeY * blockMagnificationFactor);
	unsigned int newTileAddress = (blockPosX + (blockPosY * blocksPerRenderRow)) * ((blockPixelSizeX * blockPixelSizeY) / pixelsPerByte);
	tileAddress = newTileAddress;
	tileNumber = (newTileAddress * pixelsPerByte) / (blockPixelSizeX * blockPixelSizeY);

	//If the new mouse position is outside the region of the render window which contains
	//tile content, hide the details popup.
	if((blockPosX >= blocksPerRenderRow) || (newTileAddress >= vramSize))
	{
		detailsVisible = false;
		ShowWindow(hwndDetails, SW_HIDE);
		ShowWindow(hwndDetails16, SW_HIDE);
		return 0;
	}

	//Retrieve the rectangle representing the work area of the target monitor
	POINT cursorPoint;
	cursorPoint.x = xpos;
	cursorPoint.y = ypos;
	ClientToScreen(hwnd, &cursorPoint);
	HMONITOR hmonitor = MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfo(hmonitor, &monitorInfo);
	RECT screenRect = monitorInfo.rcWork;

	//Calculate the size of the details popup
	RECT detailsRect;
	GetWindowRect((detailsBlock16)? hwndDetails16: hwndDetails, &detailsRect);
	int detailsWidth = detailsRect.right - detailsRect.left;
	int detailsHeight = detailsRect.bottom - detailsRect.top;

	//Try positioning the details popup down and to the right of the cursor position
	RECT windowPos;
	GetWindowRect(hwnd, &windowPos);
	int xposDetails = windowPos.left + xpos + 10;
	int yposDetails = windowPos.top + ypos + 10;

	//Snap to the screen boundaries, jumping to the other side if the cursor moves
	//past the center of the window.
	if((xposDetails + detailsWidth) > screenRect.right)
	{
		xposDetails = screenRect.right - detailsWidth;
		if(((windowPos.left + xpos) - xposDetails) > (detailsWidth / 2))
		{
			xposDetails = (windowPos.left + xpos) - (detailsWidth + 10);
			if(xposDetails < screenRect.left)
			{
				xposDetails = screenRect.left;
			}
		}
	}
	if((yposDetails + detailsHeight) > screenRect.bottom)
	{
		yposDetails = screenRect.bottom - detailsHeight;
		if(((windowPos.top + ypos) - yposDetails) > (detailsHeight / 2))
		{
			yposDetails = (windowPos.top + ypos) - (detailsHeight + 10);
			if(yposDetails < screenRect.top)
			{
				yposDetails = screenRect.top;
			}
		}
	}

	//Prevent the window being trapped by the cursor in the bottom righthand corner
	if((((windowPos.left + xpos) >= xposDetails) && ((windowPos.left + xpos) < (xposDetails + detailsWidth)))
	&& (((windowPos.top + ypos) >= yposDetails) && ((windowPos.top + ypos) < (yposDetails + detailsHeight))))
	{
		xposDetails = (windowPos.left + xpos) - (detailsWidth + 10);
	}

	//Enable the WM_MOUSELEAVE notification if the details popup isn't currently visible
	if(!detailsVisible)
	{
		TRACKMOUSEEVENT mouseEvent;
		mouseEvent.cbSize = sizeof(mouseEvent);
		mouseEvent.hwndTrack = hwnd;
		mouseEvent.dwHoverTime = HOVER_DEFAULT;
		mouseEvent.dwFlags = TME_LEAVE;
		TrackMouseEvent(&mouseEvent);
	}

	//Position the details popup, and show it if necessary.
	ShowWindow((detailsBlock16)? hwndDetails: hwndDetails16, SW_HIDE);
	SetWindowPos((detailsBlock16)? hwndDetails16: hwndDetails, NULL, xposDetails, yposDetails, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	if(!detailsVisible)
	{
		detailsVisible = true;
		ShowWindow((detailsBlock16)? hwndDetails16: hwndDetails, SW_SHOWNA);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VRAMView::msgRenderWM_MOUSELEAVE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Hide the details popup
	detailsVisible = false;
	ShowWindow(hwndDetails, SW_HIDE);
	ShowWindow(hwndDetails16, SW_HIDE);
	return 0;
}

//----------------------------------------------------------------------------------------
//Render window helper methods
//----------------------------------------------------------------------------------------
void VRAMView::UpdateRenderWindowSizeIfRequired()
{
	//Constants
	static const unsigned int vramSize = 0x10000;
	static const unsigned int pixelsPerByte = 2;
	static const unsigned int pixelsInVRAM = vramSize * pixelsPerByte;
	unsigned int blockPixelSizeX = 8;
	unsigned int blockPixelSizeY = ((blockSize == BLOCKSIZE_8X16) || ((blockSize == BLOCKSIZE_AUTO) && (model.RegGetLSM0()) && (model.RegGetLSM1())))? 16: 8;
	unsigned int blocksInVRAM = pixelsInVRAM / (blockPixelSizeX * blockPixelSizeY);

	//Calculate the effective block magnification factor
	if(blockMagnificationFactorSetting == 0)
	{
		if(blocksPerRenderRowSetting == 0)
		{
			//Make a rough estimate of the block magnification factor to use. This
			//estimate is based on pixel area alone, and doesn't take into account
			//available pixels that cannot be used due to the minimum cell size, so it may
			//be one step too large.
			double availableSpaceToPixelRatio = (double)(renderWindowAvailableWidth * renderWindowAvailableHeight) / (double)pixelsInVRAM;
			blockMagnificationFactor = (unsigned int)sqrt(availableSpaceToPixelRatio);
			blockMagnificationFactor = (blockMagnificationFactor <= 0)? 1: blockMagnificationFactor;

			//Take the minimum cell size into account with the selected block
			//magnification factor, and if the selected magnification factor is too large,
			//take it back one step.
			unsigned int displayedBlockColumns = (renderWindowAvailableWidth / (blockMagnificationFactor * blockPixelSizeX));
			displayedBlockColumns = (displayedBlockColumns <= 0)? 1: displayedBlockColumns;
			unsigned int displayedBlockRows = (blocksInVRAM + (displayedBlockColumns - 1)) / displayedBlockColumns;
			blockMagnificationFactor = ((displayedBlockRows * blockMagnificationFactor * blockPixelSizeY) > (unsigned int)renderWindowAvailableHeight)? ((blockMagnificationFactor > 1)? (blockMagnificationFactor - 1): 1): blockMagnificationFactor;
		}
		else
		{
			//Calculate the block magnification factor to use based on the specified
			//number of blocks to display per row
			unsigned int displayedBlockColumns = blocksPerRenderRowSetting;
			unsigned int displayedBlockRows = (blocksInVRAM + (displayedBlockColumns - 1)) / displayedBlockColumns;
			blockMagnificationFactor = renderWindowAvailableHeight / (displayedBlockRows * blockPixelSizeY);
			blockMagnificationFactor = (blockMagnificationFactor <= 0)? 1: blockMagnificationFactor;
		}
	}
	else
	{
		blockMagnificationFactor = blockMagnificationFactorSetting;
	}

	//Calculate the effective number of blocks per row
	if(blocksPerRenderRowSetting == 0)
	{
		blocksPerRenderRow = renderWindowAvailableWidth / (blockPixelSizeX * blockMagnificationFactor);
		blocksPerRenderRow = (blocksPerRenderRow <= 0)? 1: blocksPerRenderRow;
	}
	else
	{
		blocksPerRenderRow = blocksPerRenderRowSetting;
	}

	//Calculate the required width and height of the render window
	unsigned int visibleBlockRowCount = ((pixelsInVRAM / (blocksPerRenderRow * blockPixelSizeX * blockPixelSizeY)) + (((pixelsInVRAM % (blocksPerRenderRow * blockPixelSizeX * blockPixelSizeY)) > 0)? 1: 0));
	int newRenderWindowWidth = (int)(blocksPerRenderRow * blockPixelSizeX * blockMagnificationFactor);
	int newRenderWindowHeight = (int)(visibleBlockRowCount * blockPixelSizeY * blockMagnificationFactor);

	//Read the client size of the render window
	RECT rect;
	GetClientRect(hwndRender, &rect);
	int renderWindowWidth = rect.right;
	int renderWindowHeight = rect.bottom;

	//Update the render window size, if required.
	if((renderWindowWidth != newRenderWindowWidth) || (renderWindowHeight != newRenderWindowHeight))
	{
		SetWindowPos(hwndRender, NULL, 0, 0, newRenderWindowWidth, newRenderWindowHeight, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

//----------------------------------------------------------------------------------------
//Details dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR VRAMView::WndProcDetails(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgDetailsWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgDetailsWM_TIMER(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Details dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR VRAMView::msgDetailsWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 1000/20, NULL);
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VRAMView::msgDetailsWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Update address and block numbers
	UpdateDlgItemHex(hwnd, IDC_VDP_VRAM_DETAILS_ADDRESS, 4, tileAddress);
	UpdateDlgItemHex(hwnd, IDC_VDP_VRAM_DETAILS_NUMBER, 4, tileNumber);

	//Update the cell contents
	unsigned int blockWidth = 8;
	unsigned int blockHeight = (detailsBlock16)? 16: 8;
	for(unsigned int x = 0; x < blockWidth; ++x)
	{
		for(unsigned int y = 0; y < blockHeight; ++y)
		{
			HWND hwndCell = GetDlgItem(hwnd, IDC_VDP_VRAM_DETAILS_DATA00 + (y * blockWidth) + x);
			if(hwndCell != NULL)
			{
				HDC dc = GetDC(hwndCell);
				if(dc != NULL)
				{
					HBRUSH brush = CreateSolidBrush(RGB(tileDetails[y][x].r, tileDetails[y][x].g, tileDetails[y][x].b));
					if(brush != NULL)
					{
						RECT rect;
						GetClientRect(hwndCell, &rect);
						FillRect(dc, &rect, brush);
						std::wstringstream stream;
						stream << std::hex << std::uppercase << tileDetails[y][x].value;
						DrawText(dc, stream.str().c_str(), (int)stream.str().size(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						DeleteObject(brush);
					}
					ReleaseDC(hwndCell, dc);
				}
			}
		}
	}

	return TRUE;
}
