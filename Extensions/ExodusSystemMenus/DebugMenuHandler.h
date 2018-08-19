#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "ExtensionInterface/ExtensionInterface.pkg"
#include "ExodusSystemMenus.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	// Enumerations
	enum MenuItem
	{
		MENUITEM_DEVICECONTROL
	};

	// Constructors
	DebugMenuHandler(ExodusSystemMenus& owner, ISystemGUIInterface& model);

protected:
	// Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	ExodusSystemMenus& _owner;
	ISystemGUIInterface& _model;
};

#endif
