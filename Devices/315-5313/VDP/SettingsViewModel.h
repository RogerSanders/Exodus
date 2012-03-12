#include "VDP.h"
#ifndef __VDP_SETTINGSVIEWMODEL_H__
#define __VDP_SETTINGSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::SettingsViewModel :public ViewModelBase
{
public:
	//Constructors
	SettingsViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	VDP* device;
};

#endif
