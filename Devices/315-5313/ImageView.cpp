#include "ImageView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::ImageView::ImageView(S315_5313* adevice)
:device(adevice)
{
	glrc = NULL;
	hwndOpenGL = NULL;
	hwndStatusBar = NULL;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Image Window");
	SetWindowSettings(windowTitle, WS_SIZEBOX, 0, 420, 313, true);
	videoFixedAspectRatioCached = device->videoFixedAspectRatio;
	imageAspectRatio = 4.0 / 3.0;
	QueryPerformanceFrequency(&counterFrequency);
	QueryPerformanceCounter(&lastFPSCounterUpdateTickCount);
	framesRenderedSinceLastFPSCounterUpdate = 0;
	windowPendingClearCount = 0;
	fullScreenMode = false;
	renderPosHighlightColorIndex= 0;
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT S315_5313::ImageView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_SIZE:
		return msgWM_SIZE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_PARENTNOTIFY:
		return msgWM_PARENTNOTIFY(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT S315_5313::ImageView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Initialize common controls
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_BAR_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	//Create the status bar window
	hwndStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | SBARS_SIZEGRIP, 0, 0, 0, 0, hwnd, NULL, (HINSTANCE)device->GetAssemblyHandle(), NULL);
	ShowWindow(hwndStatusBar, SW_SHOWNORMAL);
	UpdateWindow(hwndStatusBar);

	//Create the OpenGL window class
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
	wc.lpszClassName = L"OpenGL Render Window";
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);

	//Create the OpenGL window
	hwndOpenGL = CreateWindowEx(0, L"OpenGL Render Window", L"OpenGL Render Window", WS_CHILD, 0, 0, 0, 0, hwnd, NULL, (HINSTANCE)device->GetAssemblyHandle(), this);
	ShowWindow(hwndOpenGL, SW_SHOWNORMAL);
	UpdateWindow(hwndOpenGL);

	//Start a timer to respond to image window settings changes
	SetTimer(hwnd, 1, 250, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::ImageView::msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam)
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
LRESULT S315_5313::ImageView::msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//Obtain the current size of the client area of the window
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	int windowWidth = clientRect.right - clientRect.left;
	int WindowHeight = clientRect.bottom - clientRect.top;

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
	int openGLWindowSizeY = WindowHeight - statusBarHeight;
	int openGLWindowPosX = 0;
	int openGLWindowPosY = 0;

	//Set the new window position and size for the OpenGL window
	SetWindowPos(hwndOpenGL, NULL, openGLWindowPosX, openGLWindowPosY, openGLWindowSizeX, openGLWindowSizeY, SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::ImageView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Refresh the status bar visibility setting
	if(videoShowStatusBarCached != device->videoShowStatusBar)
	{
		videoShowStatusBarCached = device->videoShowStatusBar;
		SendMessage(hwnd, WM_SIZE, 0, 0);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::ImageView::msgWM_PARENTNOTIFY(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case WM_LBUTTONDOWN:{
		//If the user has clicked on a child window within our window region, ensure that
		//the child window gets focus.
		POINT mousePos;
		mousePos.x = LOWORD(lParam);
		mousePos.y = HIWORD(lParam);
		HWND targetWindow = ChildWindowFromPoint(hwnd, mousePos);
		if(targetWindow != NULL)
		{
			SetFocus(targetWindow);
		}
		break;}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//OpenGL window procedure
//----------------------------------------------------------------------------------------
LRESULT CALLBACK S315_5313::ImageView::WndProcRenderStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	ImageView* state = (ImageView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_CREATE:
		//Set the object pointer
		state = (ImageView*)((CREATESTRUCT*)lparam)->lpCreateParams;
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
LRESULT S315_5313::ImageView::WndProcRender(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
	case WM_KEYUP:
		return msgRenderWM_KEYUP(hwnd, wparam, lparam);
	case WM_KEYDOWN:
		return msgRenderWM_KEYDOWN(hwnd, wparam, lparam);
	case WM_SYSKEYDOWN:
		return msgRenderWM_SYSKEYDOWN(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//OpenGL window event handlers
//----------------------------------------------------------------------------------------
LRESULT S315_5313::ImageView::msgRenderWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//OpenGL Initialization code
	glrc = CreateOpenGLWindow(hwnd);
	if(glrc != NULL)
	{
		//Create a texture coordinate matrix which maps texture coordinates to pixel
		//numbers within the texture
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScaled(1.0 / (double)device->imageBufferWidth, 1.0 / (double)device->imageBufferHeight, 1.0);

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
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		//Generate a texture for the image output, with blank data in the initial image.
		//We will re-populate this data later when the image is updated. Note that it
		//appears that the only way to initialize a texture to a blank state in OpenGL is
		//to actually allocate a buffer of the correct size for the texture, and set the
		//texture data to the contents of that buffer. This seems wasteful, but it's the
		//most sensible way I've found. We can pass in a null pointer, but then the
		//texture contents are undefined until real data is provided.
		unsigned char* nullBuffer = new unsigned char[device->imageBufferWidth * device->imageBufferHeight * 4];
		memset(nullBuffer, 0, device->imageBufferWidth * device->imageBufferHeight * 4);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, device->imageBufferWidth, device->imageBufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullBuffer);
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
LRESULT S315_5313::ImageView::msgRenderWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(glrc != NULL)
	{
		wglDeleteContext(glrc);
	}
	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::ImageView::msgRenderWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam)
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
LRESULT S315_5313::ImageView::msgRenderWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
				if(videoFixedAspectRatioCached != device->videoFixedAspectRatio)
				{
					//Flag two pending clear operations of the OpenGL colour buffer. We
					//need to do this when resizing the viewport, since if we make an
					//adjustment to a smaller viewport that leaves some unused border
					//regions, the previous buffer contents will be retained in those
					//areas. Since we have double buffering of the image output, we need
					//to perform the clear operation twice to clear both buffers.
					windowPendingClearCount = 2;

					//Cache the new fixed aspect ratio setting
					videoFixedAspectRatioCached = device->videoFixedAspectRatio;

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
LRESULT S315_5313::ImageView::msgRenderWM_KEYUP(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	IDeviceContext::KeyCode keyCode;
	if(device->GetDeviceContext()->TranslateKeyCode((unsigned int)wparam, keyCode))
	{
		device->GetDeviceContext()->HandleInputKeyUp(keyCode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::ImageView::msgRenderWM_KEYDOWN(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(fullScreenMode && (wparam == VK_ESCAPE))
	{
		SetFullScreen(GetParent(hwnd), !fullScreenMode);
	}
	else
	{
		IDeviceContext::KeyCode keyCode;
		if(device->GetDeviceContext()->TranslateKeyCode((unsigned int)wparam, keyCode))
		{
			device->GetDeviceContext()->HandleInputKeyDown(keyCode);
		}
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::ImageView::msgRenderWM_SYSKEYDOWN(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(wparam == VK_RETURN)
	{
		SetFullScreen(GetParent(hwnd), !fullScreenMode);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Image update functions
//----------------------------------------------------------------------------------------
void S315_5313::ImageView::UpdateImage()
{
	//If at least one pending window clear operation is flagged, clear the OpenGL colour
	//buffer, and decrement the number of pending clear operations.
	if(windowPendingClearCount > 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		--windowPendingClearCount;
	}

	//Determine the index of the current image plane that is being used for display
	unsigned int displayingImageBufferPlane = device->videoSingleBuffering? device->drawingImageBufferPlane: ((device->drawingImageBufferPlane + device->imageBufferPlanes) - 1) % device->imageBufferPlanes;

	//Obtain the number of rows in this frame
	unsigned int rowCount = device->imageBufferLineCount[displayingImageBufferPlane];
	if(rowCount <= 0)
	{
		return;
	}

	//Set the projection matrix to map the visible rows in our output image to the output
	//window region with the correct orientation and size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (double)1.0, (float)rowCount, 0.0, -1.0, 1.0);

	//If a new frame is ready to be displayed, update our image texture with the new
	//rendered image data.
	if(device->videoSingleBuffering || (device->wholeFramesRenderedToImageBufferSinceLastRefresh > 0))
	{
		//Obtain a lock on imageBufferMutex
		boost::mutex::scoped_lock lock(device->imageBufferMutex);

		//Copy the contents of the image buffer into our image texture for rendering
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, device->imageBufferWidth, rowCount, GL_RGBA, GL_UNSIGNED_BYTE, device->imageBuffer[displayingImageBufferPlane]);

		//Flag that a new frame is no longer ready in the image buffer
		unsigned int framesCompletedDrawing = device->wholeFramesRenderedToImageBufferSinceLastRefresh;
		device->wholeFramesRenderedToImageBufferSinceLastRefresh = 0;

		//Release the lock on imageBufferMutex now that we are finished with the image
		//buffer state
		lock.unlock();

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
	double rowScreenOffset = device->imageBufferOddInterlaceFrame[displayingImageBufferPlane]? 0.5: 0.0;

	//Render each line of the output image, using hardware resampling to scale the line
	//width to match the output surface width.
	for(unsigned int rowNo = 0; rowNo < rowCount; ++rowNo)
	{
		//Render this line
		unsigned int lineWidth = device->imageBufferLineWidth[displayingImageBufferPlane][rowNo];
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
		if(device->videoShowBoundaryActiveImage)
		{
			glColor3d(0.0, 1.0, 0.0);
			if(rowNo == device->imageBufferActiveScanPosYStart[displayingImageBufferPlane])
			{
				glBegin(GL_LINES);
					glVertex2d((double)device->imageBufferActiveScanPosXStart[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)device->imageBufferActiveScanPosXEnd[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)device->imageBufferActiveScanPosXStart[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)device->imageBufferActiveScanPosXStart[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)(rowNo + 1) + rowScreenOffset);
					glVertex2d((double)device->imageBufferActiveScanPosXEnd[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)device->imageBufferActiveScanPosXEnd[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)(rowNo + 1) + rowScreenOffset);
				glEnd();
			}
			else if(rowNo == device->imageBufferActiveScanPosYEnd[displayingImageBufferPlane])
			{
				glBegin(GL_LINES);
					glVertex2d((double)device->imageBufferActiveScanPosXStart[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)device->imageBufferActiveScanPosXEnd[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)rowNo + rowScreenOffset);
				glEnd();
			}
			else if((rowNo > device->imageBufferActiveScanPosYStart[displayingImageBufferPlane]) && (rowNo < device->imageBufferActiveScanPosYEnd[displayingImageBufferPlane]))
			{
				glBegin(GL_LINES);
					glVertex2d((double)device->imageBufferActiveScanPosXStart[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)device->imageBufferActiveScanPosXStart[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)(rowNo + 1) + rowScreenOffset);
					glVertex2d((double)device->imageBufferActiveScanPosXEnd[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)rowNo + rowScreenOffset);
					glVertex2d((double)device->imageBufferActiveScanPosXEnd[displayingImageBufferPlane][rowNo] / (double)lineWidth, (double)(rowNo + 1) + rowScreenOffset);
				glEnd();
			}
			glColor3d(1.0, 1.0, 1.0);
		}
	}

	//If the user has requested the current render position to be highlighted, display it.
	if(device->videoHighlightRenderPos && device->currentRenderPosOnScreen)
	{
		//Calculate the position of the render position within the OpenGL render space
		unsigned int currentRenderPosScreenXCache = device->currentRenderPosScreenX;
		unsigned int currentRenderPosScreenYCache = device->currentRenderPosScreenY;
		unsigned int currentRenderPosScreenWidth = device->imageBufferLineWidth[device->drawingImageBufferPlane][currentRenderPosScreenYCache];
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
	if(device->videoShowBoundaryTitleSafe)
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
	if(device->videoShowBoundaryActionSafe)
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
	if(device->videoEnableSpriteBoxing)
	{
		boost::mutex::scoped_lock spriteLock(device->spriteBoundaryMutex[displayingImageBufferPlane]);
		for(unsigned int i = 0; i < device->imageBufferSpriteBoundaryLines[displayingImageBufferPlane].size(); ++i)
		{
			SpriteBoundaryLineEntry& spriteBoundaryLineEntry = device->imageBufferSpriteBoundaryLines[displayingImageBufferPlane][i];
			unsigned int displayLineWidth = device->imageBufferLineWidth[displayingImageBufferPlane][spriteBoundaryLineEntry.linePosYStart];
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
void S315_5313::ImageView::UpdateOpenGLViewport()
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
	if(device->videoFixedAspectRatio && (windowAspectRatio < imageAspectRatio))
	{
		//If the window aspect ratio is less than the image aspect ratio, the image is too
		//wide to fit perfectly in the window. In this case, we need to calculate a window
		//height that will use the full width of the window and preserve the aspect ratio.
		imageRegionHeight = (unsigned int)(((double)renderWindowWidth * (1.0 / imageAspectRatio)) + 0.5);
		imageRegionPosY = (renderWindowHeight - imageRegionHeight) / 2;
	}
	else if(device->videoFixedAspectRatio && (windowAspectRatio > imageAspectRatio))
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
void S315_5313::ImageView::SetFullScreen(HWND hwnd, bool fullScreenModeNew)
{
	//If we're already in the target mode, abort any further processing.
	if(fullScreenMode == fullScreenModeNew)
	{
		return;
	}

	//Set the fullscreen mode to the requested state
	if(fullScreenModeNew)
	{
		//Save the current window style flags, position, and size.
		fullScreenSavedWindowStyle = GetWindowLong(hwnd, GWL_STYLE);
		fullScreenSavedWindowStyleEx = GetWindowLong(hwnd, GWL_EXSTYLE);
		GetWindowRect(hwnd, &fullScreenSavedWindowRect);

		//Strip any window style flags which we don't want set in fullscreen mode
		LONG modifiedWindowStyle = fullScreenSavedWindowStyle & ~(WS_CAPTION | WS_THICKFRAME);
		LONG modifiedWindowStyleEx = fullScreenSavedWindowStyleEx & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		SetWindowLong(hwnd, GWL_STYLE, modifiedWindowStyle);
		SetWindowLong(hwnd, GWL_EXSTYLE, modifiedWindowStyleEx);

		//Obtain the position and dimensions of the monitor closest to the image window
		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(monitorInfo);
		GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitorInfo);

		//Set the position and dimensions of the image window based on the position and
		//dimensions of the monitor info
		int newWindowPosX = monitorInfo.rcMonitor.left;
		int newWindowPosY = monitorInfo.rcMonitor.top;
		int newWindowSizeX = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		int newWindowSizeY = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
		SetWindowPos(hwnd, NULL, newWindowPosX, newWindowPosY, newWindowSizeX, newWindowSizeY, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}
	else
	{
		//Restore the original window style flags
		SetWindowLong(hwnd, GWL_STYLE, fullScreenSavedWindowStyle);
		SetWindowLong(hwnd, GWL_EXSTYLE, fullScreenSavedWindowStyleEx);

		//Restore the original window size and position
		int newWindowPosX = fullScreenSavedWindowRect.left;
		int newWindowPosY = fullScreenSavedWindowRect.top;
		int newWindowSizeX = fullScreenSavedWindowRect.right - fullScreenSavedWindowRect.left;
		int newWindowSizeY = fullScreenSavedWindowRect.bottom - fullScreenSavedWindowRect.top;
		SetWindowPos(hwnd, NULL, newWindowPosX, newWindowPosY, newWindowSizeX, newWindowSizeY, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	}

	//Record the new full screen mode state
	fullScreenMode = fullScreenModeNew;
}
