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
		SetFullScreen(hwnd, false);
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
		SetFullScreen(hwnd, !fullScreenMode);
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
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//OpenGL window event handlers
//----------------------------------------------------------------------------------------
LRESULT ImageView::msgRenderWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
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
	renderWindowWidth = (unsigned int)LOWORD(lParam);
	renderWindowHeight = (unsigned int)HIWORD(lParam);
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
	unsigned int imageRegionPosX = 0;
	unsigned int imageRegionPosY = 0;
	unsigned int imageRegionWidth = renderWindowWidth;
	unsigned int imageRegionHeight = renderWindowHeight;
	double windowAspectRatio = (double)renderWindowWidth / (double)renderWindowHeight;
	if(videoFixedAspectRatioCached && (windowAspectRatio < imageAspectRatio))
	{
		//If the window aspect ratio is less than the image aspect ratio, the image is too
		//wide to fit perfectly in the window. In this case, we need to calculate a window
		//height that will use the full width of the window and preserve the aspect ratio.
		imageRegionHeight = (unsigned int)(((double)renderWindowWidth * (1.0 / imageAspectRatio)) + 0.5);
		imageRegionPosY = (renderWindowHeight - imageRegionHeight) / 2;
	}
	else if(videoFixedAspectRatioCached && (windowAspectRatio > imageAspectRatio))
	{
		//If the window aspect ratio is greater than the image aspect ratio, the image is
		//too high to fit perfectly in the window. In this case, we need to calculate a
		//window width that will use the full height of the window and preserve the aspect
		//ratio.
		imageRegionWidth = (unsigned int)(((double)renderWindowHeight * imageAspectRatio) + 0.5);
		imageRegionPosX = (renderWindowWidth - imageRegionWidth) / 2;
	}

	//Set the viewport dimensions based on the current window size
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
	ShowWindow(hwndStatusBar, videoShowStatusBarCached? SW_SHOWNOACTIVATE: SW_HIDE);

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
		const int framesPerSecondSegmentPreferredWidth = DPIScaleWidth(75);
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
void ImageView::SetFullScreen(HWND hwnd, bool fullScreenModeNew)
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
		GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitorInfo);

		//Save the width and height of our image window before entering fullscreen mode
		RECT rect;
		GetClientRect(hwnd, &rect);
		savedWindowWidth = rect.right - rect.left;
		savedWindowHeight = rect.bottom - rect.top;

		//Make our image window into a top-level window, and save the previous parent
		//window.
		savedParent = SetWindowParent(hwnd, NULL);

		//Set the position and dimensions of the image window based on the position and
		//dimensions of the monitor
		int newWindowPosX = monitorInfo.rcMonitor.left;
		int newWindowPosY = monitorInfo.rcMonitor.top;
		int newWindowSizeX = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		int newWindowSizeY = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
		SetWindowPos(hwnd, NULL, newWindowPosX, newWindowPosY, newWindowSizeX, newWindowSizeY, SWP_NOZORDER);
	}
	else
	{
		//Restore the saved parent window of the image window
		SetWindowParent(hwnd, savedParent);

		//Ensure that the parent window is correctly set as the foreground window, and
		//that our image window retains focus.
		SetForegroundWindow(GetAncestor(savedParent, GA_ROOT));
		SetFocus(hwnd);

		//Restore the image window size and position
		SetWindowPos(hwnd, NULL, 0, 0, savedWindowWidth, savedWindowHeight, SWP_NOZORDER | SWP_NOMOVE);
	}

	//Record the new full screen mode state
	fullScreenMode = fullScreenModeNew;
}
