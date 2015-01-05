#include <Stream/Stream.pkg>
#include <random>
#include <cstdint>
#include "PaletteView.h"
#include "resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PaletteView::PaletteView(IUIManager& auiManager, PaletteViewPresenter& apresenter, IS315_5313& amodel)
:ViewBase(auiManager, apresenter), presenter(apresenter), model(amodel)
{
	//Set the window settings for this view
	static const unsigned int paletteRows = 4;
	static const unsigned int paletteColumns = 16;
	static const unsigned int paletteSquareDesiredSize = 15;
	int width = DPIScaleWidth(paletteSquareDesiredSize) * paletteColumns;
	int height = DPIScaleHeight(paletteSquareDesiredSize) * paletteRows;
	SetWindowSettings(apresenter.GetUnqualifiedViewTitle(), 0, 0, width, height);
	SetDockableViewType(false);

	//Set the format for our palette image
	paletteImage.SetImageFormat(paletteColumns, paletteRows, IImage::PIXELFORMAT_RGB, IImage::DATAFORMAT_8BIT);
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
	case WM_PAINT:
		return msgWM_PAINT(hwnd, wparam, lparam);
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
	SetTimer(hwnd, 1, 1000/30, NULL);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT PaletteView::msgWM_PAINT(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Begin the paint operation
	PAINTSTRUCT paintInfo;
	HDC hdc = BeginPaint(hwnd, &paintInfo);

	//Obtain the current width and height of the client region of the window
	RECT rect;
	GetClientRect(hwnd, &rect);
	int clientWidth = rect.right;
	int clientHeight = rect.bottom;

	//Build a set of palette columns with an extra pixel in their width
	static const unsigned int paletteColumns = 16;
	std::minstd_rand widthExtraPixelDistributionGenerator;
	std::set<unsigned int> widthExtraPixelFoundEntrySet;
	for(unsigned int paletteEntry = 0; paletteEntry < (clientWidth % paletteColumns); ++paletteEntry)
	{
		unsigned int nextEntry;
		do
		{
			nextEntry = (unsigned int)widthExtraPixelDistributionGenerator() % paletteColumns;
		}
		while(widthExtraPixelFoundEntrySet.find(nextEntry) != widthExtraPixelFoundEntrySet.end());
		widthExtraPixelFoundEntrySet.insert(nextEntry);
	}

	//Build a set of palette rows with an extra pixel in their height
	static const unsigned int paletteRows = 4;
	std::minstd_rand heightExtraPixelDistributionGenerator;
	std::set<unsigned int> heightExtraPixelFoundEntrySet;
	for(unsigned int paletteLine = 0; paletteLine < (clientHeight % paletteRows); ++paletteLine)
	{
		unsigned int nextEntry;
		do
		{
			nextEntry = (unsigned int)heightExtraPixelDistributionGenerator() % paletteRows;
		}
		while(heightExtraPixelFoundEntrySet.find(nextEntry) != heightExtraPixelFoundEntrySet.end());
		heightExtraPixelFoundEntrySet.insert(nextEntry);
	}

	//Draw each entry in the palette
	unsigned int nextCellPosY = 0;
	for(unsigned int paletteLine = 0; paletteLine < paletteRows; ++paletteLine)
	{
		//Calculate the height of this palette row in the window
		unsigned int cellHeight = (clientHeight / paletteRows) + ((heightExtraPixelFoundEntrySet.find(paletteLine) != heightExtraPixelFoundEntrySet.end())? 1: 0);

		unsigned int nextCellPosX = 0;
		for(unsigned int paletteEntry = 0; paletteEntry < paletteColumns; ++paletteEntry)
		{
			//Calculate the width of this palette entry in the window
			unsigned int cellWidth = (clientWidth / paletteColumns) + ((widthExtraPixelFoundEntrySet.find(paletteEntry) != widthExtraPixelFoundEntrySet.end())? 1: 0);

			//Retrieve the colour of this palette entry
			unsigned char r, g, b;
			paletteImage.ReadPixelData(paletteEntry, paletteLine, 0, r);
			paletteImage.ReadPixelData(paletteEntry, paletteLine, 1, g);
			paletteImage.ReadPixelData(paletteEntry, paletteLine, 2, b);

			//Populate the rectangle with the coordinates of this palette entry in the
			//window
			rect.left = (int)nextCellPosX;
			rect.right = rect.left + (int)cellWidth;
			rect.top = (int)nextCellPosY;
			rect.bottom = rect.top + (int)cellHeight;

			//Draw the rectangle for this palette entry
			HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
			FillRect(hdc, &rect, brush);
			DeleteObject(brush);

			//Advance to the next horizontal cell position
			nextCellPosX += cellWidth;
		}

		//Advance to the next vertical cell position
		nextCellPosY += cellHeight;
	}

	//Complete the paint operation
	EndPaint(hwnd, &paintInfo);
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT PaletteView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	//Retrieve the latest colour values for the palette
	unsigned int paletteRows = 4;
	unsigned int paletteColumns = 16;
	for(unsigned int paletteLine = 0; paletteLine < paletteRows; ++paletteLine)
	{
		for(unsigned int paletteEntry = 0; paletteEntry < paletteColumns; ++paletteEntry)
		{
			IS315_5313::DecodedPaletteColorEntry color = model.ReadDecodedPaletteColor(paletteLine, paletteEntry);
			paletteImage.WritePixelData(paletteEntry, paletteLine, 0, model.ColorValueTo8BitValue(color.r, false, false));
			paletteImage.WritePixelData(paletteEntry, paletteLine, 1, model.ColorValueTo8BitValue(color.g, false, false));
			paletteImage.WritePixelData(paletteEntry, paletteLine, 2, model.ColorValueTo8BitValue(color.b, false, false));
		}
	}

	//Invalidate the window to trigger a redraw operation
	InvalidateRect(hwnd, NULL, FALSE);
	return 0;
}
