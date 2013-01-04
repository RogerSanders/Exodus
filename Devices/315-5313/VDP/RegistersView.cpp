#include "RegistersView.h"
#include "..\resource.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::RegistersView::RegistersView(VDP* adevice)
:device(adevice), initializedDialog(false), currentControlFocus(0)
{
	std::wstring windowTitle = BuildWindowTitle(device->GetModuleDisplayName(), device->GetDeviceClassName(), device->GetDeviceInstanceName(), L"Registers");
	SetDialogTemplateSettings(windowTitle, (HINSTANCE)device->GetAssemblyHandle(), MAKEINTRESOURCE(IDD_VDP_REGISTERS));
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR VDP::RegistersView::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	WndProcDialogImplementSaveFieldWhenLostFocus(hwnd, msg, wparam, lparam);
	switch(msg)
	{
	case WM_INITDIALOG:
		return msgWM_INITDIALOG(hwnd, wparam, lparam);
	case WM_CLOSE:
		return msgWM_CLOSE(hwnd, wparam, lparam);
	case WM_TIMER:
		return msgWM_TIMER(hwnd, wparam, lparam);
	case WM_COMMAND:
		return msgWM_COMMAND(hwnd, wparam, lparam);
	}
	return FALSE;
}

//----------------------------------------------------------------------------------------
//Event handlers
//----------------------------------------------------------------------------------------
INT_PTR VDP::RegistersView::msgWM_INITDIALOG(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	SetTimer(hwnd, 1, 50, NULL);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::RegistersView::msgWM_CLOSE(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	KillTimer(hwnd, 1);
	DestroyWindow(hwnd);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::RegistersView::msgWM_TIMER(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	initializedDialog = true;

	//Update raw registers
	for(unsigned int i = 0; i < registerCount; ++i)
	{
		if(currentControlFocus != (IDC_REG_0 + i))	UpdateDlgItemHex(hwnd, IDC_REG_0 + i, 2, device->reg.ReadLatest(i));
	}

	//Base address registers
	if(currentControlFocus != IDC_SCROLLABASE)	UpdateDlgItemHex(hwnd, IDC_SCROLLABASE, 4, device->GetMappingAddressA());
	if(currentControlFocus != IDC_SCROLLBBASE)	UpdateDlgItemHex(hwnd, IDC_SCROLLBBASE, 4, device->GetMappingAddressB());
	if(currentControlFocus != IDC_WINDOWBASE)	UpdateDlgItemHex(hwnd, IDC_WINDOWBASE, 4, device->GetMappingAddressWindow());
	if(currentControlFocus != IDC_SPRITEBASE)	UpdateDlgItemHex(hwnd, IDC_SPRITEBASE, 4, device->GetSpriteTableAddress());
	if(currentControlFocus != IDC_HSCROLLBASE)	UpdateDlgItemHex(hwnd, IDC_HSCROLLBASE, 4, device->GetHScrollTableA());

	//DMA registers
	if(currentControlFocus != IDC_DMASOURCE)	UpdateDlgItemHex(hwnd, IDC_DMASOURCE, 6, device->GetDMASourceAddress());
	if(currentControlFocus != IDC_DMALENGTH)	UpdateDlgItemHex(hwnd, IDC_DMALENGTH, 4, device->GetDMATransferCount());
	if(currentControlFocus != IDC_DMAENABLED)	UpdateDlgItemHex(hwnd, IDC_DMAENABLED, 1, device->DMAEnabled());
	if(currentControlFocus != IDC_ADDRESSINCREMENT)	UpdateDlgItemHex(hwnd, IDC_ADDRESSINCREMENT, 2, device->GetAddressIncrement());

	//Field size registers
	if(currentControlFocus != IDC_HFIELDSIZE)	UpdateDlgItemHex(hwnd, IDC_HFIELDSIZE, 2, device->GetScrollSizeH());
	if(currentControlFocus != IDC_VFIELDSIZE)	UpdateDlgItemHex(hwnd, IDC_VFIELDSIZE, 2, device->GetScrollSizeV());
	if(currentControlFocus != IDC_HSCREENSIZE)	UpdateDlgItemHex(hwnd, IDC_HSCREENSIZE, 2, device->GetVisibleCellWidth());
	if(currentControlFocus != IDC_VSCREENSIZE)	UpdateDlgItemHex(hwnd, IDC_VSCREENSIZE, 2, device->GetVisibleCellHeight());
	if(currentControlFocus != IDC_HSCROLLMODE)	UpdateDlgItemHex(hwnd, IDC_HSCROLLMODE, 1, device->GetScrollModeH());
	if(currentControlFocus != IDC_VSCROLLMODE)	UpdateDlgItemHex(hwnd, IDC_VSCROLLMODE, 1, device->GetScrollModeV());

	//Interrupt registers
	if(currentControlFocus != IDC_HINTERRUPTENABLED)	UpdateDlgItemHex(hwnd, IDC_HINTERRUPTENABLED, 1, device->GetHInterruptEnabled());
	if(currentControlFocus != IDC_VINTERRUPTENABLED)	UpdateDlgItemHex(hwnd, IDC_VINTERRUPTENABLED, 1, device->GetVInterruptEnabled());
	if(currentControlFocus != IDC_HINTERRUPTCOUNTER)	UpdateDlgItemHex(hwnd, IDC_HINTERRUPTCOUNTER, 2, device->GetHInterruptCounter());
	if(currentControlFocus != IDC_HVCOUNTERSTOP)	UpdateDlgItemHex(hwnd, IDC_HVCOUNTERSTOP, 1, device->GetHVCounterStop());

	//Background color
	if(currentControlFocus != IDC_BACKGROUNDPALETTELINE)	UpdateDlgItemHex(hwnd, IDC_BACKGROUNDPALETTELINE, 1, device->GetBackgroundPaletteIndex().line);
	if(currentControlFocus != IDC_BACKGROUNDPALETTEENTRY)	UpdateDlgItemHex(hwnd, IDC_BACKGROUNDPALETTEENTRY, 1, device->GetBackgroundPaletteIndex().entry);

	//Miscellaneous
	if(currentControlFocus != IDC_DISPLAYENABLED)	UpdateDlgItemHex(hwnd, IDC_DISPLAYENABLED, 1, device->GetDisplayEnabled());
	if(currentControlFocus != IDC_SHADOWHIGHLIGHTENABLED)	UpdateDlgItemHex(hwnd, IDC_SHADOWHIGHLIGHTENABLED, 1, device->GetShadowHighlightEnabled());
	//			if(currentControlFocus != IDC_STATUSREGISTER)	UpdateDlgItemHex(hwnd, IDC_STATUSREGISTER, 4, device->GetStatusRegister(RasterPos()));
	//			if(currentControlFocus != IDC_HVCOUNTER)	UpdateDlgItemHex(hwnd, IDC_HVCOUNTER, 4, device->GetHVCounter());
	if(currentControlFocus != IDC_INTERLACEMODE)	UpdateDlgItemHex(hwnd, IDC_INTERLACEMODE, 1, device->GetInterlaceMode());
	if(currentControlFocus != IDC_ODDFRAMEININTERLACE)	UpdateDlgItemHex(hwnd, IDC_ODDFRAMEININTERLACE, 1, device->GetOddInterlaceFrame());

	//Window settings
	if(currentControlFocus != IDC_WINDOWRIGHT)	UpdateDlgItemHex(hwnd, IDC_WINDOWRIGHT, 1, device->WindowRightAligned());
	if(currentControlFocus != IDC_WINDOWDOWN)	UpdateDlgItemHex(hwnd, IDC_WINDOWDOWN, 1, device->WindowDownAligned());
	if(currentControlFocus != IDC_WINDOWBASEX)	UpdateDlgItemHex(hwnd, IDC_WINDOWBASEX, 2, device->WindowBasePointX());
	if(currentControlFocus != IDC_WINDOWBASEY)	UpdateDlgItemHex(hwnd, IDC_WINDOWBASEY, 2, device->WindowBasePointY());

	//Control port registers
	if(currentControlFocus != IDC_CODE)	UpdateDlgItemHex(hwnd, IDC_CODE, 1, device->code);
	if(currentControlFocus != IDC_ADDRESS)	UpdateDlgItemHex(hwnd, IDC_ADDRESS, 4, device->address);
	if(currentControlFocus != IDC_WRITEPENDING)	UpdateDlgItemHex(hwnd, IDC_WRITEPENDING, 1, device->writePending);
	if(currentControlFocus != IDC_DMAPENDING)	UpdateDlgItemHex(hwnd, IDC_DMAPENDING, 1, device->dmaPending);

	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR VDP::RegistersView::msgWM_COMMAND(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if((HIWORD(wparam) == EN_SETFOCUS) && initializedDialog)
	{
		previousText = GetDlgItemString(hwnd, LOWORD(wparam));
		currentControlFocus = LOWORD(wparam);
	}
	else if((HIWORD(wparam) == EN_KILLFOCUS) && initializedDialog)
	{
		std::wstring newText = GetDlgItemString(hwnd, LOWORD(wparam));
		if(newText != previousText)
		{
			//Raw registers
			if((LOWORD(wparam) >= IDC_REG_0) && (LOWORD(wparam) < (IDC_REG_0 + registerCount)))
			{
				device->reg.WriteCommitted(LOWORD(wparam) - IDC_REG_0, (unsigned char)GetDlgItemHex(hwnd, LOWORD(wparam)));
			}
			else switch(LOWORD(wparam))
			{
			//Data table addresses
			case IDC_SCROLLABASE:
				device->SetMappingAddressA(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_SCROLLBBASE:
				device->SetMappingAddressB(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_WINDOWBASE:
				device->SetMappingAddressWindow(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_SPRITEBASE:
				device->SetSpriteTableAddress(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_HSCROLLBASE:
				device->SetHScrollTableAddress(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;

			//Size and movement
			case IDC_HFIELDSIZE:
				device->SetScrollSizeH(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_VFIELDSIZE:
				device->SetScrollSizeV(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_HSCREENSIZE:
				device->SetVisibleCellWidth(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_VSCREENSIZE:
				device->SetVisibleCellHeight(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_HSCROLLMODE:
				device->SetScrollModeH(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_VSCROLLMODE:
				device->SetScrollModeV(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;

			//DMA registers
			case IDC_DMASOURCE:
				device->SetDMASourceAddress(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_DMALENGTH:
				device->SetDMATransferCount(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_DMAENABLED:
				device->SetDMAEnabled(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_ADDRESSINCREMENT:
				device->SetAddressIncrement(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;

			//Window settings
			case IDC_WINDOWRIGHT:
				device->SetWindowRightAligned(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_WINDOWDOWN:
				device->SetWindowDownAligned(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_WINDOWBASEX:
				device->SetWindowBasePointX(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_WINDOWBASEY:
				device->SetWindowBasePointY(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;

			//VBlank and HBlank
			case IDC_HINTERRUPTENABLED:
				device->SetHInterruptEnabled(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_VINTERRUPTENABLED:
				device->SetVInterruptEnabled(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_HINTERRUPTCOUNTER:
				device->SetHInterruptCounter(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;

			//HV counter functions
			case IDC_HVCOUNTERSTOP:
				device->SetHVCounterStop(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;

			//Interlace functions
			case IDC_INTERLACEMODE:
				device->SetInterlaceMode(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_ODDFRAMEININTERLACE:
				device->SetOddInterlaceFrame(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;

			//Miscellaneous settings
			case IDC_DISPLAYENABLED:
				device->SetDisplayEnabled(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_SHADOWHIGHLIGHTENABLED:
				device->SetShadowHighlightEnabled(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_BACKGROUNDPALETTELINE:
				device->SetBackgroundPaletteLine(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_BACKGROUNDPALETTEENTRY:
				device->SetBackgroundPaletteEntry(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;

			//Control port registers
			case IDC_CODE:
				device->SetControlCode(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_ADDRESS:
				device->SetControlAddress(GetDlgItemHex(hwnd, LOWORD(wparam)));
			break;
			case IDC_WRITEPENDING:
				device->SetWritePending(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			case IDC_DMAPENDING:
				device->SetDMAPending(GetDlgItemHex(hwnd, LOWORD(wparam)) != 0);
			break;
			}
		}
	}

	return TRUE;
}
