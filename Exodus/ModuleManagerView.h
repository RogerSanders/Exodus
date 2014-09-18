#ifndef __MODULEMANAGERVIEW_H__
#define __MODULEMANAGERVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "CallbackSupport/CallbackSupport.pkg"
#include "ModuleManagerViewPresenter.h"
#include "ExodusInterface.h"

class ModuleManagerView :public ViewBase
{
public:
	//Constructors
	ModuleManagerView(IUIManager& auiManager, ModuleManagerViewPresenter& apresenter, ExodusInterface& amodel);

protected:
	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	//Refresh methods
	void RefreshModuleList() const;

	//Event handlers
	INT_PTR msgWM_INITDIALOG(HWND hwnd, WPARAM wParam, LPARAM lParam);
	INT_PTR msgWM_USER(HWND hwnd, WPARAM wparam, LPARAM lparam);
	INT_PTR msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	HWND windowHandle;
	ModuleManagerViewPresenter& presenter;
	ExodusInterface& model;
	ObserverSubscription loadedModulesChangeSubscription;
};

#endif
