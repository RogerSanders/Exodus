#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "Z80/IZ80.h"
#include "Z80Menus.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	//Constructors
	DebugMenuHandler(Z80Menus& owner, const IDevice& modelInstanceKey, IZ80& model);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	Z80Menus& _owner;
	const IDevice& _modelInstanceKey;
	IZ80& _model;
};

#endif
