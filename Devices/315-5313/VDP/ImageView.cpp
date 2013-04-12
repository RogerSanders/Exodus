#include "ImageView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::ImageView::ImageView(VDP* adevice)
:device(adevice)
{
	glrc = NULL;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Image");
	SetWindowSettings(windowTitle, WS_SIZEBOX, 0, 320, 272, false);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT VDP::ImageView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_KEYUP:
		return msgWM_KEYUP(hwnd, wparam, lparam);
	case WM_KEYDOWN:
		return msgWM_KEYDOWN(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT VDP::ImageView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//OpenGL Initialization code
	int width = 320;
	int height = 272;
	glrc = CreateOpenGLWindow(hwnd);
	if(glrc != NULL)
	{
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, (float)width, (float)height, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//glDrawBuffer(GL_FRONT);

		//Texture initialization
		//glEnable(GL_TEXTURE_2D);
		//glTexImage2D(GL_TEXTURE_2D, 0, 4, vdp->imageWidth, vdp->imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, vdp->image);
	}

	//##FIX## This should be based on the screen refresh rate
	//##FIX## Rather than use a timer-based system at all, make our VDP core post a
	//message to our image window when a new frame is ready to draw. This will eliminate
	//any drawing delays, and update at the correct rate regardless of how quickly new
	//frames are being rendered.
	SetTimer(hwnd, 1, 1000/50, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VDP::ImageView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(glrc != NULL)
	{
		wglDeleteContext(glrc);
	}
	DestroyWindow(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VDP::ImageView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	HDC hdc = GetDC(hwnd);
	if(hdc != NULL)
	{
		if((glrc != NULL) && (wglMakeCurrent(hdc, glrc) != FALSE))
		{
			UpdateImage();
			SwapBuffers(hdc);
			wglMakeCurrent(NULL, NULL);
		}
		ReleaseDC(hwnd, hdc);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VDP::ImageView::msgWM_KEYUP(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	IDeviceContext::KeyCode keyCode;
	if(device->GetDeviceContext()->TranslateKeyCode((unsigned int)wparam, keyCode))
	{
		device->GetDeviceContext()->HandleInputKeyUp(keyCode);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT VDP::ImageView::msgWM_KEYDOWN(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	IDeviceContext::KeyCode keyCode;
	if(device->GetDeviceContext()->TranslateKeyCode((unsigned int)wparam, keyCode))
	{
		device->GetDeviceContext()->HandleInputKeyDown(keyCode);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
//Image update functions
//----------------------------------------------------------------------------------------
void VDP::ImageView::UpdateImage()
{
	boost::mutex::scoped_lock lock(device->imageBufferMutex);
	unsigned int displayingImageBufferPlane = ((device->drawingImageBufferPlane + device->imageBufferPlanes) - 1) % device->imageBufferPlanes;
	glDrawPixels(device->imageWidth, device->imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, device->image[displayingImageBufferPlane]);
	glFlush();
}
