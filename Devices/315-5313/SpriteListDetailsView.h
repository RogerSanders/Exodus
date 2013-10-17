#include "S315_5313.h"
#ifndef __S315_5313_SPRITELISTDETAILSVIEW_H__
#define __S315_5313_SPRITELISTDETAILSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::SpriteListDetailsView :public ViewBase
{
public:
	//Constructors
	SpriteListDetailsView(S315_5313* adevice, unsigned int aspriteIndex);

	//Sprite index functions
	void SetSpriteIndex(unsigned int aspriteIndex);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	S315_5313* device;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	unsigned int spriteIndex;
	SpriteMappingTableEntry spriteMapping;
};

#endif
