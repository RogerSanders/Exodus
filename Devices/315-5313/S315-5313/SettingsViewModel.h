#include "S315_5313.h"
#ifndef __S315_5313_SETTINGSVIEWMODEL_H__
#define __S315_5313_SETTINGSVIEWMODEL_H__
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::SettingsViewModel :public ViewModelBase
{
public:
	//Constructors
	SettingsViewModel(const std::wstring& amenuHandlerName, int aviewModelID, S315_5313* adevice);

	//View creation and deletion
	virtual IView* CreateView();
	virtual void DeleteView(IView* aview);

private:
	S315_5313* device;
};

#endif
