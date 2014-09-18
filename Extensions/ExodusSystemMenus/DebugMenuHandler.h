#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include "ExodusSystemMenus.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_DEVICECONTROL
	};

	//Constructors
	DebugMenuHandler(ExodusSystemMenus& aowner, ISystemGUIInterface& amodel);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	ExodusSystemMenus& owner;
	ISystemGUIInterface& model;
};

#endif
