#include "System.h"
#ifndef __SYSTEM_DEBUGMENUHANDLER_H__
#define __SYSTEM_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class System::DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_DEVICECONTROL
	};

	//Constructors
	DebugMenuHandler(System* adevice);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	System* device;
};

#endif
