#ifndef __INPUTMAPPINGVIEW_H__
#define __INPUTMAPPINGVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExtensionInterface/ExtensionInterface.pkg"
#include "EmbeddedROMViewPresenter.h"

class EmbeddedROMView :public ViewBase
{
public:
	//Constructors
	EmbeddedROMView(IUIManager& uiManager, EmbeddedROMViewPresenter& presenter, ISystemGUIInterface& model);

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
	EmbeddedROMViewPresenter& _presenter;
	ISystemGUIInterface& _model;
	unsigned int _embeddedROMIDLastModifiedToken;
	unsigned int _selectedROMID;
};

#endif
