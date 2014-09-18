#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Z80/IZ80.h"
#include "Z80Menus.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_REGISTERS
	};

	//Constructors
	DebugMenuHandler(Z80Menus& aowner, const IDevice& amodelInstanceKey, IZ80& amodel);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	Z80Menus& owner;
	const IDevice& modelInstanceKey;
	IZ80& model;
};

#endif
