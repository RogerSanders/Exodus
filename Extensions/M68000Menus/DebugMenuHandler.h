#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "M68000Menus.h"
#include "M68000/IM68000.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_EXCEPTIONS
	};

	//Constructors
	DebugMenuHandler(M68000Menus& aowner, const IDevice& amodelInstanceKey, IM68000& amodel);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	M68000Menus& owner;
	const IDevice& modelInstanceKey;
	IM68000& model;
};

#endif
