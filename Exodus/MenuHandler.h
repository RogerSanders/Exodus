#ifndef __MENUHANDLER_H__
#define __MENUHANDLER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusInterface.h"

class MenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_ABOUT,
		MENUITEM_SETTINGS,
		MENUITEM_MODULEMANAGER,
		MENUITEM_CREATEDASHBOARD
	};

	//Constructors
	MenuHandler(ExodusInterface& owner, ExodusInterface& model);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	ExodusInterface& _owner;
	ExodusInterface& _model;
};

#endif
