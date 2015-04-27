#ifndef __SPRITELISTDETAILSVIEW_H__
#define __SPRITELISTDETAILSVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "SpriteListDetailsViewPresenter.h"
#include "315-5313/IS315_5313.h"

class SpriteListDetailsView :public ViewBase
{
public:
	//Constructors
	SpriteListDetailsView(IUIManager& auiManager, SpriteListDetailsViewPresenter& apresenter, IS315_5313& amodel, unsigned int aspriteIndex);

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
	SpriteListDetailsViewPresenter& presenter;
	IS315_5313& model;
	bool initializedDialog;
	std::wstring previousText;
	unsigned int currentControlFocus;
	unsigned int spriteIndex;
	IS315_5313::SpriteMappingTableEntry spriteMapping;
};

#endif
