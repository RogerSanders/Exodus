#include "VRAMView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::VRAMView::VRAMView(S315_5313* adevice)
:device(adevice)
{
	glrc = 0;
	blockSize = BLOCKSIZE_AUTO;
	selectedPalette = PALETTE_LOWHIGH;
	shadow = false;
	highlight = false;
	buffer = 0;
	hwndDetails = NULL;
	hwndDetails16 = NULL;
	detailsVisible = false;

	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"VRAM Pattern Viewer");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_VRAM));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::VRAMView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::VRAMView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the window class
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProcRenderStatic;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = (HINSTANCE)device->GetAssemblyHandle();
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"VRAM Render Child";
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);

	//Calculate the marked target position for the child window inside the dialog
	RECT markerRect;
	GetWindowRect(GetDlgItem(hwnd, IDC_VDP_VRAM_MARKER), &markerRect);
	POINT markerPos;
	unsigned int width = DPIScaleWidth(256);	//markerRect.right - markerRect.left;
	unsigned int height = DPIScaleHeight(512);	//markerRect.bottom - markerRect.top;
	markerPos.x = markerRect.left;
	markerPos.y = markerRect.top;
	ScreenToClient(hwnd, &markerPos);

	//Create the window
	HWND hwndOpenGL;
	hwndOpenGL = CreateWindowEx(0, L"VRAM Render Child", L"VRAM Render Child", WS_CHILD, markerPos.x, markerPos.y, width, height, hwnd, NULL, (HINSTANCE)device->GetAssemblyHandle(), this);
	ShowWindow(hwndOpenGL, SW_SHOWNORMAL);
	UpdateWindow(hwndOpenGL);

	//Set the palette selection controls to their default state
	CheckDlgButton(hwnd, IDC_VDP_VRAM_BLOCKAUTO, (blockSize == BLOCKSIZE_AUTO)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_BLOCK8X8, (blockSize == BLOCKSIZE_8X8)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_BLOCK8X16, (blockSize == BLOCKSIZE_8X16)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_PALETTE1, (selectedPalette == PALETTE_LINE1)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_PALETTE2, (selectedPalette == PALETTE_LINE2)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_PALETTE3, (selectedPalette == PALETTE_LINE3)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_PALETTE4, (selectedPalette == PALETTE_LINE4)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_LOWHIGH, (selectedPalette == PALETTE_LOWHIGH)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_HIGHLOW, (selectedPalette == PALETTE_HIGHLOW)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_SHADOW, (shadow)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_VRAM_HIGHLIGHT, (highlight)? BST_CHECKED: BST_UNCHECKED);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::VRAMView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::VRAMView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
//Render window procedure
//----------------------------------------------------------------------------------------
LRESULT CALLBACK S315_5313::VRAMView::WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	VRAMView* state = (VRAMView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_CREATE:
		//Set the object pointer
		state = (VRAMView*)((CREATESTRUCT*)lparam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcRender(hwnd, msg, wparam, lparam);
		}
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			state->WndProcRender(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
		}
		return TRUE;
	}

	//Pass this message on to the member window procedure function
	if(state != 0)
	{
		return state->WndProcRender(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::VRAMView::WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgRenderWM_CREATE(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgRenderWM_CLOSE(hwnd, wparam, lparam);
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
LRESULT S315_5313::VRAMView::msgRenderWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the popup tile viewer window
	hwndDetails = CreateDialogParam((HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_VRAM_DETAILS), hwnd, WndProcDetailsStatic, (LPARAM)this);
	hwndDetails16 = CreateDialogParam((HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_VRAM_DETAILS16), hwnd, WndProcDetailsStatic, (LPARAM)this);

	//OpenGL Initialization code
	int screenWidth = DPIScaleWidth(256);
	int screenHeight = DPIScaleHeight(512);
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
	int bufferWidth = 256;
	int bufferHeight = 512;
	buffer = new unsigned char[bufferWidth * bufferHeight * 0x20 * 4];

	SetTimer(hwnd, 1, 200, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::VRAMView::msgRenderWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwndDetails);
	DestroyWindow(hwndDetails16);
	hwndDetails = NULL;
	hwndDetails16 = NULL;
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
	DestroyWindow(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::VRAMView::msgRenderWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Obtain a lock on external references, and ensure that the required buffers exists.
	boost::mutex::scoped_lock lock(device->externalReferenceMutex);
	if((device->vram == 0) || (device->cram == 0))
	{
		return 0;
	}

	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

	//Constants
	const unsigned int width = 256;
	const unsigned int height = 512;
	unsigned int blockPixelSizeX = 8;
	unsigned int blockPixelSizeY = 8;
	unsigned int blockSizeForDisplay = 0x20;
	if((blockSize == BLOCKSIZE_8X16) || ((blockSize == BLOCKSIZE_AUTO) && (device->M5GetLSM0(accessTarget)) && (device->M5GetLSM1(accessTarget))))
	{
		blockPixelSizeY = 16;
		blockSizeForDisplay = 0x40;
	}

	//Fill the VRAM render buffer
	std::vector<unsigned char> vramDataCopy;
	device->vram->GetLatestBufferCopy(vramDataCopy);
	for(unsigned int line = 0; line < height; ++line)
	{
		for(unsigned int xpos = 0; xpos < width; ++xpos)
		{
			unsigned int blockNumber = ((line / blockPixelSizeY) * (width / blockPixelSizeX)) + (xpos / blockPixelSizeX);
			unsigned int blockStart = blockNumber * blockSizeForDisplay;
			unsigned int blockOffset = ((line % blockPixelSizeY) * blockPixelSizeX) + (xpos % blockPixelSizeX);
			Data blockData(8);
			blockData = vramDataCopy[blockStart + (blockOffset / 2)];
			unsigned int index = ((blockOffset % 2) == 0)? blockData.GetDataSegment(4, 4): blockData.GetDataSegment(0, 4);

			Color color;
			color.shadow = shadow;
			color.highlight = highlight;
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
				PaletteIndex paletteIndex;
				paletteIndex.entry = index;
				paletteIndex.line = selectedPalette;
				paletteIndex.highlight = highlight;
				paletteIndex.shadow = shadow;

//				color = state->device->GetPaletteColor(paletteIndex);

				//Assumed constants
				unsigned int paletteEntriesPerLine = 16;
				//Decode palette color
				Data paletteEntry(16);
				unsigned char byte1 = device->cram->ReadCommitted((paletteIndex.entry + (paletteIndex.line * paletteEntriesPerLine)) * 2);
				unsigned char byte2 = device->cram->ReadCommitted(((paletteIndex.entry + (paletteIndex.line * paletteEntriesPerLine)) * 2) + 1);
				paletteEntry.SetUpperHalf(byte1);
				paletteEntry.SetLowerHalf(byte2);
				//##TODO## Add some notes here showing the bit structure of a palette entry in a
				//pretty table.
				//##TODO## Use GetDataSegment
				color.r = (paletteEntry.GetData() & 0x000E) >> 1;
				color.g = (paletteEntry.GetData() & 0x00E0) >> 5;
				color.b = (paletteEntry.GetData() & 0x0E00) >> 9;
				color.shadow = paletteIndex.shadow;
				color.highlight = paletteIndex.highlight;

				r = color.GetR8Bit();
				g = color.GetG8Bit();
				b = color.GetB8Bit();
			}

			//Copy the data into the details popup
			if(blockNumber == tileNumber)
			{
				tileDetails[(line % blockPixelSizeY)][(xpos % blockPixelSizeX)].r = r;
				tileDetails[(line % blockPixelSizeY)][(xpos % blockPixelSizeX)].g = g;
				tileDetails[(line % blockPixelSizeY)][(xpos % blockPixelSizeX)].b = b;
				tileDetails[(line % blockPixelSizeY)][(xpos % blockPixelSizeX)].value = index;
			}

			buffer[((xpos + (((height - 1) * width) - (line * width))) * 4) + 0] = r;
			buffer[((xpos + (((height - 1) * width) - (line * width))) * 4) + 1] = g;
			buffer[((xpos + (((height - 1) * width) - (line * width))) * 4) + 2] = b;
			buffer[((xpos + (((height - 1) * width) - (line * width))) * 4) + 3] = 0xFF;
		}
	}

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

			glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			glFlush();
			SwapBuffers(hdc);
			wglMakeCurrent(NULL, NULL);
		}
		ReleaseDC(hwnd, hdc);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::VRAMView::msgRenderWM_MOUSEMOVE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(!detailsVisible)
	{
		//Enable the WM_MOUSELEAVE notification
		TRACKMOUSEEVENT mouseEvent;
		mouseEvent.cbSize = sizeof(mouseEvent);
		mouseEvent.hwndTrack = hwnd;
		mouseEvent.dwHoverTime = HOVER_DEFAULT;
		mouseEvent.dwFlags = TME_LEAVE;
		TrackMouseEvent(&mouseEvent);
	}

	AccessTarget accessTarget;
	accessTarget.AccessCommitted();

	//Calculate the new data contents for the details popup
	const unsigned int width = 256;
	const unsigned int height = 512;
	unsigned int blockPixelSizeX = 8;
	unsigned int blockPixelSizeY = 8;
	unsigned int blockSizeForDisplay = 0x20;
	detailsBlock16 = (blockSize == BLOCKSIZE_8X16) || ((blockSize == BLOCKSIZE_AUTO) && (device->M5GetLSM0(accessTarget)) && (device->M5GetLSM1(accessTarget)));
	if(detailsBlock16)
	{
		blockPixelSizeY = 16;
		blockSizeForDisplay = 0x40;
	}

	int xpos = LOWORD(lparam);
	int ypos = HIWORD(lparam);
	unsigned int blockPosX = xpos / blockPixelSizeX;
	unsigned int blockPosY = ypos / blockPixelSizeY;
	tileNumber = (blockPosY * (width / blockPixelSizeX)) + blockPosX;
	tileAddress = tileNumber * blockSizeForDisplay;

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

	//Calculate the position for the details popup
	RECT windowPos;
	RECT detailsRect;
	GetWindowRect(hwnd, &windowPos);
	if(!detailsBlock16)
	{
		GetWindowRect(hwndDetails, &detailsRect);
	}
	else
	{
		GetWindowRect(hwndDetails16, &detailsRect);
	}
	int detailsWidth = detailsRect.right - detailsRect.left;
	int detailsHeight = detailsRect.bottom - detailsRect.top;

	//Try positioning the window down and to the right of the cursor position
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

	//Display the details popup
	if(!detailsBlock16)
	{
		ShowWindow(hwndDetails16, SW_HIDE);
		SetWindowPos(hwndDetails, 0, xposDetails, yposDetails, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
	else
	{
		ShowWindow(hwndDetails, SW_HIDE);
		SetWindowPos(hwndDetails16, 0, xposDetails, yposDetails, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::VRAMView::msgRenderWM_MOUSELEAVE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	detailsVisible = false;

	//Hide the details popup
	ShowWindow(hwndDetails, SW_HIDE);
	ShowWindow(hwndDetails16, SW_HIDE);

	return 0;
}

//----------------------------------------------------------------------------------------
//Details dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK S315_5313::VRAMView::WndProcDetailsStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	VRAMView* state = (VRAMView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (VRAMView*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcDetails(hwnd, msg, wparam, lparam);
		}
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			state->WndProcDetails(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);
		}
		return TRUE;
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = TRUE;
	if(state != 0)
	{
		result = state->WndProcDetails(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::VRAMView::WndProcDetails(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgDetailsWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgDetailsWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgDetailsWM_TIMER(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Details dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR S315_5313::VRAMView::msgDetailsWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 200, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::VRAMView::msgDetailsWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR S315_5313::VRAMView::msgDetailsWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
