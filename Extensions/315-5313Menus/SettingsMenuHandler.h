#ifndef __SETTINGSMENUHANDLER_H__
#define __SETTINGSMENUHANDLER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "S315_5313Menus.h"
#include "315-5313/IS315_5313.h"

class SettingsMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_SETTINGS
	};

	//Constructors
	SettingsMenuHandler(S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	S315_5313Menus& owner;
	const IDevice& modelInstanceKey;
	IS315_5313& model;
};

#endif
