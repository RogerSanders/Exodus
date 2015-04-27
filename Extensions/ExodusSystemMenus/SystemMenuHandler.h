#ifndef __SYSTEMMENUHANDLER_H__
#define __SYSTEMMENUHANDLER_H__
#include "ExtensionInterface/ExtensionInterface.pkg"
#include "ExodusSystemMenus.h"

class SystemMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_EVENTLOG
	};

	//Constructors
	SystemMenuHandler(ExodusSystemMenus& aowner, ISystemGUIInterface& amodel);

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
