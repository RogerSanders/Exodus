#include <WindowsSupport/WindowsSupport.pkg>
#include "ImageView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ImageView::ImageView(IUIManager& auiManager, ImageViewPresenter& apresenter, IS315_5313& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	glrc = NULL;
	hwndChildGroup = NULL;
	hwndOpenGL = NULL;
	hwndStatusBar = NULL;
	savedParent = NULL;
	SetWindowSettings(apresenter.GetUnqualifiedViewTitle(), 0, 0, 420, 313);
	SetDockableViewType(true, DockPos::Center);
	videoFixedAspectRatioCached = model.GetVideoFixedAspectRatio();
	imageAspectRatio = 4.0 / 3.0;
	QueryPerformanceFrequency(&counterFrequency);
	QueryPerformanceCounter(&lastFPSCounterUpdateTickCount);
	framesRenderedSinceLastFPSCounterUpdate = 0;
	windowPendingClearCount = 0;
	fullScreenMode = false;
	renderPosHighlightColorIndex = 0;
	lastRenderedFrameTokenCached = 0;

	hwndPixelInfo = NULL;
	pixelInfoVisible = false;
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT ImageView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementGiveFocusToChildWindowOnClick(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_NCHITTEST:
		return msgWM_NCHITTEST(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT ImageView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Initialize common controls
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_BAR_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	//Save a copy of the main window handle
	hwndMain = hwnd;

	//Create the render window
	hwndChildGroup = CreateChildWindow(WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, 0, hwnd, std::bind(&ImageView::WndProcChildGroup, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	//Create the status bar window
	hwndStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwndChildGroup, NULL, GetAssemblyHandle(), NULL);

	//Create the render window
	hwndOpenGL = CreateChildWindow(WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, 0, hwndChildGroup, std::bind(&ImageView::WndProcRender, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	//Start a timer to respond to image window settings changes
	SetTimer(hwnd, 1, 250, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Note that we need to explicitly destroy the child window here, since we share state
	//with the child window, passing in the "this" pointer as its state. Since the
	//destructor for our state may be called anytime after this window is destroyed, and
	//this window is fully destroyed before child windows are destroyed, we need to
	//explicitly destroy the child window here. The child window is fully destroyed before
	//the DestroyWindow() function returns, and our state is still valid until we return
	//from handling this WM_DESTROY message.
	DestroyWindow(hwndOpenGL);
	hwndOpenGL = NULL;

	return DefWindowProc(hwnd, WM_DESTROY, wParam, lParam);
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Obtain the current size of the client area of the window
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	//Set the new window position and size for the child window group
	SetWindowPos(hwndChildGroup, NULL, 0, 0, windowWidth, windowHeight, SWP_NOZORDER | SWP_NOACTIVATE);

	//Update the size of the render window
	UpdateRenderWindowSize();
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgWM_NCHITTEST(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Make this control transparent in the client area for hit testing
	LRESULT result = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
	return (result == HTCLIENT)? HTTRANSPARENT: result;
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Refresh the status bar visibility setting
	if(videoShowStatusBarCached != model.GetVideoShowStatusBar())
	{
		videoShowStatusBarCached = model.GetVideoShowStatusBar();
		UpdateRenderWindowSize();
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Child window procedure
//----------------------------------------------------------------------------------------
LRESULT ImageView::WndProcChildGroup(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_KEYUP:
		return msgChildWM_KEYUP(hwnd, wparam, lparam);
	case WM_KEYDOWN:
		return msgChildWM_KEYDOWN(hwnd, wparam, lparam);
	case WM_SYSKEYDOWN:
		return msgChildWM_SYSKEYDOWN(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Child event handlers
//----------------------------------------------------------------------------------------
LRESULT ImageView::msgChildWM_KEYUP(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	ISystemDeviceInterface::KeyCode keyCode;
	if(presenter.GetSystemInterface().TranslateKeyCode((unsigned int)wparam, keyCode))
	{
		presenter.GetSystemInterface().HandleInputKeyUp(keyCode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgChildWM_KEYDOWN(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(fullScreenMode && (wparam == VK_ESCAPE))
	{
		SetFullScreen(false);
	}
	else
	{
		ISystemDeviceInterface::KeyCode keyCode;
		if(presenter.GetSystemInterface().TranslateKeyCode((unsigned int)wparam, keyCode))
		{
			presenter.GetSystemInterface().HandleInputKeyDown(keyCode);
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgChildWM_SYSKEYDOWN(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(wparam == VK_RETURN)
	{
		SetFullScreen(!fullScreenMode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//OpenGL window procedure
//----------------------------------------------------------------------------------------
LRESULT ImageView::WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgRenderWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgRenderWM_DESTROY(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgRenderWM_SIZE(hwnd, wparam, lparam);
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
//OpenGL window event handlers
//----------------------------------------------------------------------------------------
LRESULT ImageView::msgRenderWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Create the popup pixel info viewer window
	hwndPixelInfo = CreateChildDialog(hwnd, GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_IMAGE_PIXELINFO), std::bind(&ImageView::WndProcPixelInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	//OpenGL Initialization code
	glrc = CreateOpenGLWindow(hwnd);
	if(glrc != NULL)
	{
		//Create a texture coordinate matrix which maps texture coordinates to pixel
		//numbers within the texture
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScaled(1.0 / (double)model.imageBufferWidth, 1.0 / (double)model.imageBufferHeight, 1.0);

		//Set our basic render properties
		glEnable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);

		//Generate a texture number for the image texture
		GLuint textureNo;
		glGenTextures(1, &textureNo);

		//Bind the image texture number to the 2D texture unit
		glBindTexture(GL_TEXTURE_2D, textureNo);

		//Set our texture settings
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		//Generate a texture for the image output, with blank data in the initial image.
		//We will re-populate this data later when the image is updated. Note that it
		//appears that the only way to initialize a texture to a blank state in OpenGL is
		//to actually allocate a buffer of the correct size for the texture, and set the
		//texture data to the contents of that buffer. This seems wasteful, but it's the
		//most sensible way I've found. We can pass in a null pointer, but then the
		//texture contents are undefined until real data is provided.
		unsigned char* nullBuffer = new unsigned char[model.imageBufferWidth * model.imageBufferHeight * 4];
		memset(nullBuffer, 0, model.imageBufferWidth * model.imageBufferHeight * 4);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, model.imageBufferWidth, model.imageBufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullBuffer);
		delete[] nullBuffer;

		//Release the rendering context
		wglMakeCurrent(NULL, NULL);
	}

	//Start a timer to update the output image
	SetTimer(hwnd, 1, 10, NULL);

	//Start a timer to rotate our highlighted colours
	SetTimer(hwnd, 2, 100, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgRenderWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwndPixelInfo);
	hwndPixelInfo = NULL;
	KillTimer(hwnd, 1);

	if(glrc != NULL)
	{
		wglDeleteContext(glrc);
	}
	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgRenderWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Obtain the current window size. If the window size is invalid, force a minimum
	//window size of 1x1, so that we can always assume the window has a valid size from
	//this point on.
	renderWindowWidth = (int)LOWORD(lParam);
	renderWindowHeight = (int)HIWORD(lParam);
	if((renderWindowWidth <= 0) || (renderWindowHeight <= 0))
	{
		renderWindowWidth = 1;
		renderWindowHeight = 1;
	}

	//Now that the window size has changed, update the OpenGL viewport.
	HDC hdc = GetDC(hwnd);
	if(hdc != NULL)
	{
		if((glrc != NULL) && (wglMakeCurrent(hdc, glrc) != FALSE))
		{
			//Flag two pending clear operations of the OpenGL colour buffer. We need to do
			//this when resizing the viewport, since if we make an adjustment to a smaller
			//viewport that leaves some unused border regions, the previous buffer
			//contents will be retained in those areas. Since we have double buffering of
			//the image output, we need to perform the clear operation twice to clear both
			//buffers.
			windowPendingClearCount = 2;

			//Update the OpenGL viewport
			UpdateOpenGLViewport();
		}
		ReleaseDC(hwnd, hdc);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgRenderWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	switch(wparam)
	{
	case 1:{
		HDC hdc = GetDC(hwnd);
		if(hdc != NULL)
		{
			if((glrc != NULL) && (wglMakeCurrent(hdc, glrc) != FALSE))
			{
				//If the fixed aspect ratio setting has changed, update the OpenGL
				//viewport.
				if(videoFixedAspectRatioCached != model.GetVideoFixedAspectRatio())
				{
					//Flag two pending clear operations of the OpenGL colour buffer. We
					//need to do this when resizing the viewport, since if we make an
					//adjustment to a smaller viewport that leaves some unused border
					//regions, the previous buffer contents will be retained in those
					//areas. Since we have double buffering of the image output, we need
					//to perform the clear operation twice to clear both buffers.
					windowPendingClearCount = 2;

					//Cache the new fixed aspect ratio setting
					videoFixedAspectRatioCached = model.GetVideoFixedAspectRatio();

					//Update the OpenGL viewport
					UpdateOpenGLViewport();
				}

				//Draw the latest image to the viewport
				UpdateImage();

				//Make the render buffer we just drew to visible on the screen
				SwapBuffers(hdc);

				//Release the OpenGL render context
				wglMakeCurrent(NULL, NULL);
			}
			ReleaseDC(hwnd, hdc);
		}
		break;}
	case 2:
		++renderPosHighlightColorIndex;
		break;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgRenderWM_MOUSEMOVE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//If full image buffer info isn't being generated by the VDP core, hide the pixel info
	//window if it is currently visible, and abort any further processing.
	if(!model.GetVideoEnableFullImageBufferInfo())
	{
		if(pixelInfoVisible)
		{
			HidePixelInfoWindow();
			return 0;
		}
	}

	//Obtain the mouse coordinates within the render window
	int xpos = (short)LOWORD(lparam);
	int ypos = (short)HIWORD(lparam);

	//If the new mouse position is outside the region of the render window which contains
	//image content, hide the details popup.
	if((xpos < imageRegionPosX) || (xpos >= (imageRegionPosX + imageRegionWidth)) || (ypos < imageRegionPosY) || (ypos >= (imageRegionPosY + imageRegionHeight)))
	{
		HidePixelInfoWindow();
		return 0;
	}

	//Determine the index of the current image plane that is being used for display
	unsigned int displayingImageBufferPlane = model.GetImageCompletedBufferPlaneNo();

	//Obtain the number of rows in this frame
	unsigned int rowCount = model.GetImageBufferLineCount(displayingImageBufferPlane);
	if(rowCount <= 0)
	{
		HidePixelInfoWindow();
		return 0;
	}

	//Calculate the vertical image buffer position of the target pixel
	pixelInfoTargetBufferPosY = (int)((float)(ypos - imageRegionPosY) * ((float)rowCount / imageRegionHeight));

	//Calculate the horizontal image buffer position of the target pixel
	unsigned int lineWidth = model.GetImageBufferLineWidth(displayingImageBufferPlane, pixelInfoTargetBufferPosY);
	pixelInfoTargetBufferPosX = (int)((float)(xpos - imageRegionPosX) * ((float)lineWidth / imageRegionWidth));

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
	GetWindowRect(hwndPixelInfo, &detailsRect);
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
	if(!pixelInfoVisible)
	{
		TRACKMOUSEEVENT mouseEvent;
		mouseEvent.cbSize = sizeof(mouseEvent);
		mouseEvent.hwndTrack = hwnd;
		mouseEvent.dwHoverTime = HOVER_DEFAULT;
		mouseEvent.dwFlags = TME_LEAVE;
		TrackMouseEvent(&mouseEvent);
	}

	//Position the details popup, and show it if necessary.
	SetWindowPos(hwndPixelInfo, NULL, xposDetails, yposDetails, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	if(!pixelInfoVisible)
	{
		pixelInfoVisible = true;
		ShowWindow(hwndPixelInfo, SW_SHOWNA);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT ImageView::msgRenderWM_MOUSELEAVE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	HidePixelInfoWindow();
	return 0;
}

//----------------------------------------------------------------------------------------
void ImageView::HidePixelInfoWindow()
{
	//Hide the pixel info popup
	pixelInfoVisible = false;
	ShowWindow(hwndPixelInfo, SW_HIDE);
}

//----------------------------------------------------------------------------------------
//Image update functions
//----------------------------------------------------------------------------------------
void ImageView::UpdateImage()
{
	//If at least one pending window clear operation is flagged, clear the OpenGL colour
	//buffer, and decrement the number of pending clear operations.
	if(windowPendingClearCount > 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		--windowPendingClearCount;
	}

	//Determine the index of the current image plane that is being used for display
	unsigned int displayingImageBufferPlane = model.GetImageCompletedBufferPlaneNo();

	//Obtain the number of rows in this frame
	unsigned int rowCount = model.GetImageBufferLineCount(displayingImageBufferPlane);
	if(rowCount <= 0)
	{
		return;
	}

	//Set the projection matrix to map the visible rows in our output image to the output
	//window region with the correct orientation and size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, (double)rowCount, 0.0, -1.0, 1.0);

	//If a new frame is ready to be displayed, update our image texture with the new
	//rendered image data.
	unsigned int latestLastRenderedFrameToken = model.GetImageLastRenderedFrameToken();
	if(model.GetVideoSingleBuffering() || (lastRenderedFrameTokenCached != latestLastRenderedFrameToken))
	{
		//Copy the contents of the image buffer into our image texture for rendering
		model.LockImageBufferData(displayingImageBufferPlane);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, model.imageBufferWidth, rowCount, GL_RGBA, GL_UNSIGNED_BYTE, model.GetImageBufferData(displayingImageBufferPlane));
		model.UnlockImageBufferData(displayingImageBufferPlane);

		//Update our cached last rendered frame token
		unsigned int framesCompletedDrawing = latestLastRenderedFrameToken - lastRenderedFrameTokenCached;
		lastRenderedFrameTokenCached = latestLastRenderedFrameToken;

		//Update the FPS counter if a new frame has been completed
		if(framesCompletedDrawing > 0)
		{
			//Obtain the current timer value
			LARGE_INTEGER newFrameTickCount;
			QueryPerformanceCounter(&newFrameTickCount);

			//Update the count of frames that have been rendered since the last FPS
			//counter update
			framesRenderedSinceLastFPSCounterUpdate += framesCompletedDrawing;

			//If a second or more has passed since we last updated the FPS counter, update
			//it now.
			if((newFrameTickCount.QuadPart - lastFPSCounterUpdateTickCount.QuadPart) >= counterFrequency.QuadPart)
			{
				//Calculate the current FPS
				LONGLONG ticksSinceLastFPSCounterUpdate = newFrameTickCount.QuadPart - lastFPSCounterUpdateTickCount.QuadPart;
				double framesPerSecond = (double)framesRenderedSinceLastFPSCounterUpdate / ((double)counterFrequency.QuadPart / (double)ticksSinceLastFPSCounterUpdate);

				//Display a string value representing the current FPS on the status bar
				std::wstringstream framesPerSecondStringStream;
				framesPerSecondStringStream << L"FPS: " << std::fixed << std::setprecision(2) << framesPerSecond;
				SendMessage(hwndStatusBar, SB_SETTEXT, 1, (LPARAM)framesPerSecondStringStream.str().c_str());

				//Record this frame time as the last time we updated the FPS counter, and
				//reset the rendered frame counter.
				lastFPSCounterUpdateTickCount.QuadPart = newFrameTickCount.QuadPart;
				framesRenderedSinceLastFPSCounterUpdate = 0;
			}
		}
	}

	//Initialize the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//If this frame was the odd field in an interlaced display, offset the row number by
	//0.5.
	double rowScreenOffset = model.GetImageBufferOddInterlaceFrame(displayingImageBufferPlane)? 0.5: 0.0;

	//Set the current filter mode for our pixels based on whether smoothing is enabled or
	//not
	int textureFilterMode = (model.GetVideoEnableLineSmoothing())? GL_LINEAR: GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilterMode);

	//Render each line of the output image, using hardware resampling to scale the line
	//width to match the output surface width.
	for(unsigned int rowNo = 0; rowNo < rowCount; ++rowNo)
	{
		//Render this line
		unsigned int lineWidth = model.GetImageBufferLineWidth(displayingImageBufferPlane, rowNo);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			//Top-Left
			glTexCoord2d(0.0, (double)rowNo + rowScreenOffset);
			glVertex2d(0.0, (double)rowNo + rowScreenOffset);

			//Top-Right
			glTexCoord2d((double)lineWidth, (double)rowNo + rowScreenOffset);
			glVertex2d(1.0, (double)rowNo + rowScreenOffset);

			//Bottom-Right
			glTexCoord2d((double)lineWidth, (double)(rowNo+1) + rowScreenOffset);
			glVertex2d(1.0, (double)(rowNo+1) + rowScreenOffset);

			//Bottom-Left
			glTexCoord2d(0.0, (double)(rowNo+1) + rowScreenOffset);
			glVertex2d(0.0, (double)(rowNo+1) + rowScreenOffset);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		//Draw the active image start and end positions on this line if requested
		if(model.GetVideoShowBoundaryActiveImage())
		{
			glColor3d(0.0, 1.0, 0.0);
			unsigned int activeScanStartPosX;
			unsigned int activeScanStartPosY;
			unsigned int activeScanEndPosX;
			unsigned int activeScanEndPosY;
			model.GetImageBufferActiveScanPosX(displayingImageBufferPlane, rowNo, activeScanStartPosX, activeScanEndPosX);
			model.GetImageBufferActiveScanPosY(displayingImageBufferPlane, activeScanStartPosY, activeScanEndPosY);
			if(rowNo == activeScanStartPosY)
			{
				glBegin(GL_LINES);
					glVertex2d((double)activeScanStartPosX / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)activeScanEndPosX / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)activeScanStartPosX / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)activeScanStartPosX / (double)lineWidth, (double)(rowNo + 1) + rowScreenOffset);
					glVertex2d((double)activeScanEndPosX / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)activeScanEndPosX / (double)lineWidth, (double)(rowNo + 1) + rowScreenOffset);
				glEnd();
			}
			else if(rowNo == activeScanEndPosY)
			{
				glBegin(GL_LINES);
					glVertex2d((double)activeScanStartPosX / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)activeScanEndPosX / (double)lineWidth, (double)rowNo + rowScreenOffset);
				glEnd();
			}
			else if((rowNo > activeScanStartPosY) && (rowNo < activeScanEndPosY))
			{
				glBegin(GL_LINES);
					glVertex2d((double)activeScanStartPosX / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)activeScanStartPosX / (double)lineWidth, (double)(rowNo + 1) + rowScreenOffset);
					glVertex2d((double)activeScanEndPosX / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)activeScanEndPosX / (double)lineWidth, (double)(rowNo + 1) + rowScreenOffset);
				glEnd();
			}
			glColor3d(1.0, 1.0, 1.0);
		}
	}

	//If the user has requested the current render position to be highlighted, display it.
	if(model.GetVideoHighlightRenderPos() && model.GetCurrentRenderPosOnScreen())
	{
		//Calculate the position of the render position within the OpenGL render space
		unsigned int currentRenderPosScreenXCache = model.GetCurrentRenderPosScreenX();
		unsigned int currentRenderPosScreenYCache = model.GetCurrentRenderPosScreenY();
		unsigned int currentRenderPosScreenWidth = model.GetImageBufferLineWidth(model.GetImageDrawingBufferPlaneNo(), currentRenderPosScreenYCache);
		double currentRenderPosScreenXStart = (double)currentRenderPosScreenXCache / (double)currentRenderPosScreenWidth;
		double currentRenderPosScreenXEnd = (double)(currentRenderPosScreenXCache + 1) / (double)currentRenderPosScreenWidth;
		double currentRenderPosScreenYStart = (double)currentRenderPosScreenYCache;
		double currentRenderPosScreenYEnd = (double)(currentRenderPosScreenYCache + 1);

		//Set the current colour for the render position marker
		switch(renderPosHighlightColorIndex % 3)
		{
		case 0:
			glColor3d(1.0, 0.0, 0.0);
			break;
		case 1:
			glColor3d(0.0, 1.0, 0.0);
			break;
		case 2:
			glColor3d(0.0, 0.0, 1.0);
			break;
		}

		//Display the render position marker
		glRectd((double)currentRenderPosScreenXStart, currentRenderPosScreenYStart + rowScreenOffset, currentRenderPosScreenXEnd, currentRenderPosScreenYEnd + rowScreenOffset);

		//Restore the OpenGL colour setting
		glColor3d(1.0, 1.0, 1.0);
	}

	//Display the title safe image boundary if requested
	if(model.GetVideoShowBoundaryTitleSafe())
	{
		double screenRegionPercentage = 0.9;
		double screenRegionXStart = (1.0 - screenRegionPercentage);
		double screenRegionXEnd = screenRegionPercentage;
		double screenRegionYStart = (double)rowCount * (1.0 - screenRegionPercentage);
		double screenRegionYEnd = (double)rowCount * screenRegionPercentage;
		glColor3d(0.8, 0.8, 0.0);
		glBegin(GL_LINE_LOOP);
			glVertex2d(screenRegionXStart, screenRegionYStart);
			glVertex2d(screenRegionXEnd, screenRegionYStart);
			glVertex2d(screenRegionXEnd, screenRegionYEnd);
			glVertex2d(screenRegionXStart, screenRegionYEnd);
		glEnd();
		glColor3d(1.0, 1.0, 1.0);
	}

	//Display the action safe image boundary if requested
	if(model.GetVideoShowBoundaryActionSafe())
	{
		double screenRegionPercentage = 0.95;
		double screenRegionXStart = (1.0 - screenRegionPercentage);
		double screenRegionXEnd = screenRegionPercentage;
		double screenRegionYStart = (double)rowCount * (1.0 - screenRegionPercentage);
		double screenRegionYEnd = (double)rowCount * screenRegionPercentage;
		glColor3d(0.8, 0.0, 0.0);
		glBegin(GL_LINE_LOOP);
			glVertex2d(screenRegionXStart, screenRegionYStart);
			glVertex2d(screenRegionXEnd, screenRegionYStart);
			glVertex2d(screenRegionXEnd, screenRegionYEnd);
			glVertex2d(screenRegionXStart, screenRegionYEnd);
		glEnd();
		glColor3d(1.0, 1.0, 1.0);
	}

	//Display sprite boundary information if requested
	if(model.GetVideoEnableSpriteBoxing())
	{
		std::list<IS315_5313::SpriteBoundaryLineEntry> spriteBoundaryLines = model.GetSpriteBoundaryLines(displayingImageBufferPlane);
		for(std::list<IS315_5313::SpriteBoundaryLineEntry>::const_iterator i = spriteBoundaryLines.begin(); i != spriteBoundaryLines.end(); ++i)
		{
			const IS315_5313::SpriteBoundaryLineEntry& spriteBoundaryLineEntry = *i;
			unsigned int displayLineWidth = model.GetImageBufferLineWidth(displayingImageBufferPlane, spriteBoundaryLineEntry.linePosYStart);
			if(displayLineWidth <= 0)
			{
				continue;
			}
			double linePosXStart = (double)spriteBoundaryLineEntry.linePosXStart / (double)displayLineWidth;
			double linePosXEnd = (double)spriteBoundaryLineEntry.linePosXEnd / (double)displayLineWidth;
			double linePosYStart = (double)spriteBoundaryLineEntry.linePosYStart;
			double linePosYEnd = (double)spriteBoundaryLineEntry.linePosYEnd;
			glBegin(GL_LINES);
				glVertex2d(linePosXStart, linePosYStart + rowScreenOffset);
				glVertex2d(linePosXEnd, linePosYEnd + rowScreenOffset);
			glEnd();
		}
	}

	//Signal the OpenGL drawing operations to start as quickly as possible
	glFlush();
}

//----------------------------------------------------------------------------------------
//Screen mode functions
//----------------------------------------------------------------------------------------
void ImageView::UpdateOpenGLViewport()
{
	//Calculate the size and position of the visible image drawing region within the
	//window surface. If the image output is set to preserve the aspect ratio of the
	//original image, and the window surface doesn't match the image aspect ratio, a
	//border region will be added, with the image centered in the viewport. This is very
	//important for fullscreen mode, where the image is being shown on a display which has
	//a different aspect ratio to the one used by the original system.
	int newImageRegionPosX = 0;
	int newImageRegionPosY = 0;
	int newImageRegionWidth = renderWindowWidth;
	int newImageRegionHeight = renderWindowHeight;
	double windowAspectRatio = (double)renderWindowWidth / (double)renderWindowHeight;
	if(videoFixedAspectRatioCached && (windowAspectRatio < imageAspectRatio))
	{
		//If the window aspect ratio is less than the image aspect ratio, the image is too
		//wide to fit perfectly in the window. In this case, we need to calculate a window
		//height that will use the full width of the window and preserve the aspect ratio.
		newImageRegionHeight = (int)(((double)renderWindowWidth * (1.0 / imageAspectRatio)) + 0.5);
		newImageRegionPosY = (renderWindowHeight - newImageRegionHeight) / 2;
	}
	else if(videoFixedAspectRatioCached && (windowAspectRatio > imageAspectRatio))
	{
		//If the window aspect ratio is greater than the image aspect ratio, the image is
		//too high to fit perfectly in the window. In this case, we need to calculate a
		//window width that will use the full height of the window and preserve the aspect
		//ratio.
		newImageRegionWidth = (int)(((double)renderWindowHeight * imageAspectRatio) + 0.5);
		newImageRegionPosX = (renderWindowWidth - newImageRegionWidth) / 2;
	}

	//Set the viewport dimensions based on the current window size
	imageRegionPosX = newImageRegionPosX;
	imageRegionPosY = newImageRegionPosY;
	imageRegionWidth = newImageRegionWidth;
	imageRegionHeight = newImageRegionHeight;
	glViewport(imageRegionPosX, imageRegionPosY, imageRegionWidth, imageRegionHeight);
}

//----------------------------------------------------------------------------------------
void ImageView::UpdateRenderWindowSize()
{
	//Obtain the current size of the client area of the window
	RECT clientRect;
	GetClientRect(hwndChildGroup, &clientRect);
	int windowWidth = clientRect.right - clientRect.left;
	int windowHeight = clientRect.bottom - clientRect.top;

	//Set the visibility state of the status bar
	ShowWindow(hwndStatusBar, (videoShowStatusBarCached? SW_SHOWNOACTIVATE: SW_HIDE));

	//If the status bar is currently visible, position and size it correctly within the
	//window.
	int statusBarHeight = 0;
	if(videoShowStatusBarCached)
	{
		//Send a size message to the status bar. Note that the status bar auto-sizes and
		//positions itself based on the size of its parent window. Simply sending a
		//WM_SIZE message will cause the status bar to recalculate its size and position.
		SendMessage(hwndStatusBar, WM_SIZE, 0, 0);

		//Retrieve the height of the status bar
		RECT statusBarRect;
		GetWindowRect(hwndStatusBar, &statusBarRect);
		int statusBarWidth = statusBarRect.right - statusBarRect.left;
		statusBarHeight = statusBarRect.bottom - statusBarRect.top;

		//Set the sizes and positions of the segments in the status bar
		const int framesPerSecondSegmentPreferredWidth = DPIScaleWidth(100);
		int framesPerSecondSegmentWidth = (framesPerSecondSegmentPreferredWidth <= statusBarWidth)? framesPerSecondSegmentPreferredWidth: statusBarWidth;
		int notificationSegmentWidth = statusBarWidth - framesPerSecondSegmentWidth;
		int statusBarSegmentWidths[] = {notificationSegmentWidth, -1};
		SendMessage(hwndStatusBar, SB_SETPARTS, 2, (LPARAM)statusBarSegmentWidths);
	}

	//Calculate the position and size of the OpenGL window
	int openGLWindowSizeX = windowWidth;
	int openGLWindowSizeY = windowHeight - statusBarHeight;
	int openGLWindowPosX = 0;
	int openGLWindowPosY = 0;

	//Set the new window position and size for the OpenGL window
	SetWindowPos(hwndOpenGL, NULL, openGLWindowPosX, openGLWindowPosY, openGLWindowSizeX, openGLWindowSizeY, SWP_NOZORDER | SWP_NOACTIVATE);
}

//----------------------------------------------------------------------------------------
void ImageView::SetFullScreen(bool fullScreenModeNew)
{
	//If we're already in the target mode, abort any further processing.
	if(fullScreenMode == fullScreenModeNew)
	{
		return;
	}

	//Set the fullscreen mode to the requested state
	if(fullScreenModeNew)
	{
		//Obtain the position and dimensions of the monitor closest to the image window
		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(monitorInfo);
		GetMonitorInfo(MonitorFromWindow(hwndMain, MONITOR_DEFAULTTONEAREST), &monitorInfo);

		//Save the position and size of our image window before entering fullscreen mode
		RECT rect;
		GetWindowRect(hwndMain, &rect);
		savedWindowPosX = rect.left;
		savedWindowPosY = rect.top;
		savedWindowWidth = rect.right - rect.left;
		savedWindowHeight = rect.bottom - rect.top;

		//Make our image window into a top-level window, and save the previous parent
		//window.
		savedParent = SetWindowParent(hwndMain, NULL);

		//If we had a parent window previously, convert the saved window position into
		//coordinates relative to the parent window.
		if(savedParent != NULL)
		{
			POINT windowPos;
			windowPos.x = savedWindowPosX;
			windowPos.y = savedWindowPosY;
			ScreenToClient(savedParent, &windowPos);
			savedWindowPosX = (int)windowPos.x;
			savedWindowPosY = (int)windowPos.y;
		}

		//Ensure our detached image window is set as the foreground window, and that the
		//input window has focus.
		SetForegroundWindow(hwndMain);
		SetFocus(hwndChildGroup);

		//Set the position and dimensions of the image window based on the position and
		//dimensions of the monitor
		int newWindowPosX = monitorInfo.rcMonitor.left;
		int newWindowPosY = monitorInfo.rcMonitor.top;
		int newWindowSizeX = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		int newWindowSizeY = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
		SetWindowPos(hwndMain, NULL, newWindowPosX, newWindowPosY, newWindowSizeX, newWindowSizeY, SWP_NOZORDER);
	}
	else
	{
		//Restore the image window size
		SetWindowPos(hwndMain, NULL, 0, 0, savedWindowWidth, savedWindowHeight, SWP_NOZORDER | SWP_NOMOVE);

		//Restore the saved parent window of the image window
		SetWindowParent(hwndMain, savedParent);

		//Restore the image window position
		SetWindowPos(hwndMain, NULL, savedWindowPosX, savedWindowPosY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		//Ensure that the parent window is correctly set as the foreground window, and
		//that our input window retains focus.
		SetForegroundWindow(GetAncestor(savedParent, GA_ROOT));
		SetFocus(hwndChildGroup);
	}

	//Record the new full screen mode state
	fullScreenMode = fullScreenModeNew;
}

//----------------------------------------------------------------------------------------
//Details dialog window procedure
//----------------------------------------------------------------------------------------
INT_PTR ImageView::WndProcPixelInfo(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgPixelInfoWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgPixelInfoWM_TIMER(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Details dialog event handlers
//----------------------------------------------------------------------------------------
INT_PTR ImageView::msgPixelInfoWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 1000/20, NULL);
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR ImageView::msgPixelInfoWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//If the pixel info window isn't currently visible, abort any further processing.
	if(!pixelInfoVisible)
	{
		return TRUE;
	}

	//If full image buffer info has been disabled on the VDP core, hide the pixel info
	//window, and abort any further processing.
	if(!model.GetVideoEnableFullImageBufferInfo())
	{
		HidePixelInfoWindow();
		return TRUE;
	}

	//Determine the index of the current image plane that is being used for display
	unsigned int displayingImageBufferPlane = model.GetImageCompletedBufferPlaneNo();

	//Retrieve info for the target pixel
	const IS315_5313::ImageBufferInfo* pixelInfo = model.GetImageBufferInfo(displayingImageBufferPlane, pixelInfoTargetBufferPosY, pixelInfoTargetBufferPosX);
	if(pixelInfo == 0)
	{
		return TRUE;
	}

	//Retrieve source-specific settings for this pixel info
	std::wstring pixelSourceString;
	bool mappingDataPresent = false;
	bool spriteDataPresent = false;
	switch(pixelInfo->pixelSource)
	{
	case IS315_5313::PixelSource::Sprite:
		pixelSourceString = L"Sprite";
		mappingDataPresent = true;
		spriteDataPresent = true;
		break;
	case IS315_5313::PixelSource::LayerA:
		pixelSourceString = L"Layer A";
		mappingDataPresent = true;
		break;
	case IS315_5313::PixelSource::LayerB:
		pixelSourceString = L"Layer B";
		mappingDataPresent = true;
		break;
	case IS315_5313::PixelSource::Background:
		pixelSourceString = L"Background";
		break;
	case IS315_5313::PixelSource::Window:
		pixelSourceString = L"Window";
		mappingDataPresent = true;
		break;
	case IS315_5313::PixelSource::CRAMWrite:
		pixelSourceString = L"CRAM Write";
		break;
	case IS315_5313::PixelSource::Border:
		pixelSourceString = L"Border";
		break;
	case IS315_5313::PixelSource::Blanking:
		pixelSourceString = L"Blanking";
		break;
	}

	//Pixel info
	UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_SOURCE, pixelSourceString);
	UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_HCOUNTER, 3, pixelInfo->hcounter);
	UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_VCOUNTER, 3, pixelInfo->vcounter);
	UpdateDlgItemBin(hwnd, IDC_VDP_IMAGE_PIXELINFO_PALETTEROW, pixelInfo->paletteRow);
	UpdateDlgItemBin(hwnd, IDC_VDP_IMAGE_PIXELINFO_PALETTECOLUMN, pixelInfo->paletteEntry);
	CheckDlgButton(hwnd, IDC_VDP_IMAGE_PIXELINFO_SH_ENABLE, (pixelInfo->shadowHighlightEnabled)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_IMAGE_PIXELINFO_SHADOW, (pixelInfo->pixelIsShadowed)? BST_CHECKED: BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_VDP_IMAGE_PIXELINFO_HIGHLIGHT, (pixelInfo->pixelIsHighlighted)? BST_CHECKED: BST_UNCHECKED);
	UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_R, 1, pixelInfo->colorComponentR);
	UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_G, 1, pixelInfo->colorComponentG);
	UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_B, 1, pixelInfo->colorComponentB);
	HWND hwndColor = GetDlgItem(hwnd, IDC_VDP_IMAGE_PIXELINFO_COLOR);
	if(hwndColor != NULL)
	{
		HDC dc = GetDC(hwndColor);
		if(dc != NULL)
		{
			unsigned char r = model.ColorValueTo8BitValue(pixelInfo->colorComponentR, pixelInfo->pixelIsShadowed, pixelInfo->pixelIsHighlighted);
			unsigned char g = model.ColorValueTo8BitValue(pixelInfo->colorComponentG, pixelInfo->pixelIsShadowed, pixelInfo->pixelIsHighlighted);
			unsigned char b = model.ColorValueTo8BitValue(pixelInfo->colorComponentB, pixelInfo->pixelIsShadowed, pixelInfo->pixelIsHighlighted);
			HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
			if(brush != NULL)
			{
				RECT rect;
				GetClientRect(hwndColor, &rect);
				FillRect(dc, &rect, brush);
				DeleteObject(brush);
			}
			ReleaseDC(hwndColor, dc);
		}
	}

	//Mapping (Pattern Name) data format:
	//-----------------------------------------------------------------
	//|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	//|---------------------------------------------------------------|
	//|Pri|PalRow |VF |HF |              Pattern Number               |
	//-----------------------------------------------------------------
	//Pri:    Priority Bit
	//PalRow: The palette row number to use when displaying the pattern data
	//VF:     Vertical Flip
	//HF:     Horizontal Flip
	if(mappingDataPresent)
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_MAPPINGADDRESS, 5, pixelInfo->mappingVRAMAddress);
		UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_MAPPINGDATA, 4, pixelInfo->mappingData.GetData());
		UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_PATTERNNUMBER, 3, pixelInfo->mappingData.GetDataSegment(0, 11));
		CheckDlgButton(hwnd, IDC_VDP_IMAGE_PIXELINFO_HORIZONTALFLIP, pixelInfo->mappingData.GetBit(11)? BST_CHECKED: BST_UNCHECKED);
		CheckDlgButton(hwnd, IDC_VDP_IMAGE_PIXELINFO_VERTICALFLIP, pixelInfo->mappingData.GetBit(12)? BST_CHECKED: BST_UNCHECKED);
		CheckDlgButton(hwnd, IDC_VDP_IMAGE_PIXELINFO_PRIORITY, pixelInfo->mappingData.GetBit(15)? BST_CHECKED: BST_UNCHECKED);
		UpdateDlgItemBin(hwnd, IDC_VDP_IMAGE_PIXELINFO_PATTERNROW, pixelInfo->patternRowNo);
		UpdateDlgItemBin(hwnd, IDC_VDP_IMAGE_PIXELINFO_PATTERNCOLUMN, pixelInfo->patternColumnNo);
	}
	else
	{
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_MAPPINGADDRESS, L"");
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_MAPPINGDATA, L"");
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_PATTERNNUMBER, L"");
		CheckDlgButton(hwnd, IDC_VDP_IMAGE_PIXELINFO_HORIZONTALFLIP, BST_UNCHECKED);
		CheckDlgButton(hwnd, IDC_VDP_IMAGE_PIXELINFO_VERTICALFLIP, BST_UNCHECKED);
		CheckDlgButton(hwnd, IDC_VDP_IMAGE_PIXELINFO_PRIORITY, BST_UNCHECKED);
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_PATTERNROW, L"");
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_PATTERNCOLUMN, L"");
	}

	//Sprite data
	if(spriteDataPresent)
	{
		UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITEENTRYNO, 2, pixelInfo->spriteTableEntryNo);
		UpdateDlgItemHex(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITEENTRYADDRESS, 5, pixelInfo->spriteTableEntryAddress);
		UpdateDlgItemBin(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITECELLSIZEX, pixelInfo->spriteCellWidth);
		UpdateDlgItemBin(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITECELLSIZEY, pixelInfo->spriteCellHeight);
		UpdateDlgItemBin(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITECELLPOSX, pixelInfo->spriteCellPosX);
		UpdateDlgItemBin(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITECELLPOSY, pixelInfo->spriteCellPosY);
	}
	else
	{
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITEENTRYNO, L"");
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITEENTRYADDRESS, L"");
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITECELLSIZEX, L"");
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITECELLSIZEY, L"");
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITECELLPOSX, L"");
		UpdateDlgItemString(hwnd, IDC_VDP_IMAGE_PIXELINFO_SPRITECELLPOSY, L"");
	}

	return TRUE;
}
