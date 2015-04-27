#ifndef __LAYERREMOVALVIEW_H__
#define __LAYERREMOVALVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "LayerRemovalViewPresenter.h"
#include "315-5313/IS315_5313.h"

class LayerRemovalView :public ViewBase
{
public:
	//Constructors
	LayerRemovalView(IUIManager& auiManager, LayerRemovalViewPresenter& apresenter, IS315_5313& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	LayerRemovalViewPresenter& presenter;
	IS315_5313& model;
};

#endif
