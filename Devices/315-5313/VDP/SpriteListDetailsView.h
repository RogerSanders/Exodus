#include "VDP.h"
#ifndef __VDP_SPRITELISTDETAILSVIEW_H__
#define __VDP_SPRITELISTDETAILSVIEW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include "SystemInterface/SystemInterface.pkg"

class VDP::SpriteListDetailsView :public ViewBase
{
public:
	//Constructors
	SpriteListDetailsView(VDP* adevice, unsigned int aspriteMappingAddress, unsigned int aspriteIndex);

	//Sprite address functions
	void SetSpriteAddress(unsigned int aspriteMappingAddress, unsigned int aspriteIndex);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_CLOSE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	VDP* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	unsigned int spriteMappingAddress;
	unsigned int spriteIndex;
	SpriteMapping spriteMapping;
};

#endif
