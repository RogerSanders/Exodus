#include "PaletteView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::PaletteView::PaletteView(S315_5313* adevice)
:device(adevice)
{
	unsigned int paletteSquareSizeX = DPIScaleWidth(15);
	unsigned int paletteSquareSizeY = DPIScaleHeight(15);
	unsigned int paletteRows = 4;
	unsigned int paletteColumns = 16;
	int width = paletteSquareSizeX * paletteColumns;
	int height = paletteSquareSizeY * paletteRows;

	glrc = NULL;
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceInstanceName(), L"Palette");
	SetWindowSettings(windowTitle, WS_BORDER | WS_CAPTION | WS_OVERLAPPED | WS_POPUP | WS_POPUPWINDOW | WS_SYSMENU | WS_TILED, 0, DPIReverseScaleWidth(width), DPIReverseScaleHeight(height), false);
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT S315_5313::PaletteView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT S315_5313::PaletteView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//OpenGL Initialization code
	unsigned int paletteSquareSizeX = DPIScaleWidth(15);
	unsigned int paletteSquareSizeY = DPIScaleHeight(15);
	unsigned int paletteRows = 4;
	unsigned int paletteColumns = 16;
	int width = paletteSquareSizeX * paletteColumns;
	int height = paletteSquareSizeY * paletteRows;
	glrc = CreateOpenGLWindow(hwnd);
	if(glrc != NULL)
	{
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, (float)width, (float)height, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	SetTimer(hwnd, 1, 100, NULL);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::PaletteView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(glrc != NULL)
	{
		wglDeleteContext(glrc);
	}
	DestroyWindow(hwnd);

	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT S315_5313::PaletteView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	HDC hdc = GetDC(hwnd);
	if(hdc != NULL)
	{
		if((glrc != NULL) && (wglMakeCurrent(hdc, glrc) != FALSE))
		{
			UpdatePalette();
			SwapBuffers(hdc);
			wglMakeCurrent(NULL, NULL);
		}
		ReleaseDC(hwnd, hdc);
	}

	return 0;
}

//----------------------------------------------------------------------------------------
//Palette update functions
//----------------------------------------------------------------------------------------
void S315_5313::PaletteView::UpdatePalette()
{
	//Obtain a lock on external references, and ensure that the required buffers exists.
	boost::mutex::scoped_lock lock(device->externalReferenceMutex);
	if(device->cram == 0)
	{
		return;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	unsigned int paletteSquareSizeX = DPIScaleWidth(15);
	unsigned int paletteSquareSizeY = DPIScaleHeight(15);
	float paletteX = 0.0;
	float paletteY = 0.0;
	for(unsigned int paletteLine = 0; paletteLine < 4; ++paletteLine)
	{
		for(unsigned int paletteEntry = 0; paletteEntry < 16; ++paletteEntry)
		{
			Data paletteData(16);
			paletteData.SetByte(1, device->cram->ReadLatest(((paletteLine * 16) + paletteEntry) * 2));
			paletteData.SetByte(0, device->cram->ReadLatest((((paletteLine * 16) + paletteEntry) * 2) + 1));
			float r = (float)paletteData.GetDataSegment(1, 3) / 7;
			float g = (float)paletteData.GetDataSegment(5, 3) / 7;
			float b = (float)paletteData.GetDataSegment(9, 3) / 7;
			glColor3f(r, g, b);
			glBegin(GL_POLYGON);
			glVertex3f(paletteX + (paletteEntry * paletteSquareSizeX), paletteY + (paletteLine * paletteSquareSizeY), 0.0);
			glVertex3f(paletteX + (paletteEntry * paletteSquareSizeX) + paletteSquareSizeX, paletteY + (paletteLine * paletteSquareSizeY), 0.0);
			glVertex3f(paletteX + (paletteEntry * paletteSquareSizeX) + paletteSquareSizeX, paletteY + (paletteLine * paletteSquareSizeY) + paletteSquareSizeY, 0.0);
			glVertex3f(paletteX + (paletteEntry * paletteSquareSizeX), paletteY + (paletteLine * paletteSquareSizeY) + paletteSquareSizeY, 0.0);
			glEnd();
		}
	}
	glFlush();
}
