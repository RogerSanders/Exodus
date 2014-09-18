#include "PaletteView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PaletteView::PaletteView(IUIManager& auiManager, PaletteViewPresenter& apresenter, IS315_5313& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	unsigned int paletteSquareSizeX = DPIScaleWidth(15);
	unsigned int paletteSquareSizeY = DPIScaleHeight(15);
	unsigned int paletteRows = 4;
	unsigned int paletteColumns = 16;
	int width = paletteSquareSizeX * paletteColumns;
	int height = paletteSquareSizeY * paletteRows;

	glrc = NULL;
	SetWindowSettings(apresenter.GetUnqualifiedViewTitle(), 0, 0, DPIReverseScaleWidth(width), DPIReverseScaleHeight(height));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
LRESULT PaletteView::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		return msgWM_CREATE(hwnd, wparam, lparam);
	case WM_DESTROY:
		return msgWM_DESTROY(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
LRESULT PaletteView::msgWM_CREATE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//##FIX## Make this dialog resizable. We should maintain aspect ratio, but scale to
	//fill the available space.
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
LRESULT PaletteView::msgWM_DESTROY(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if(glrc != NULL)
	{
		wglDeleteContext(glrc);
	}

	return DefWindowProc(hwnd, WM_DESTROY, wparam, lparam);
}

//----------------------------------------------------------------------------------------
LRESULT PaletteView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
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
void PaletteView::UpdatePalette()
{
	model.LockExternalBuffers();

	ITimedBufferInt* cramBuffer = model.GetCRAMBuffer();
	if(cramBuffer != 0)
	{
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
				paletteData.SetByteFromBottomUp(1, cramBuffer->ReadLatest(((paletteLine * 16) + paletteEntry) * 2));
				paletteData.SetByteFromBottomUp(0, cramBuffer->ReadLatest((((paletteLine * 16) + paletteEntry) * 2) + 1));
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

	model.UnlockExternalBuffers();
}
