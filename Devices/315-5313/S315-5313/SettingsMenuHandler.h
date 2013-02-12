#include "S315_5313.h"
#ifndef __S315_5313_SETTINGSMENUHANDLER_H__
#define __S315_5313_SETTINGSMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::SettingsMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_SETTINGS
	};

	//Constructors
	SettingsMenuHandler(S315_5313* adevice);

protected:
	//Management functions
	virtual std::wstring GetMenuHandlerName() const;
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	S315_5313* device;
};

#endif
