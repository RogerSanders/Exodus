#ifndef __SETTINGSMENUHANDLER_H__
#define __SETTINGSMENUHANDLER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "GenericAccess/GenericAccess.pkg"
#include "GenericAccessMenus.h"

class SettingsMenuHandler :public MenuHandlerBase
{
public:
	// Constructors
	SettingsMenuHandler(GenericAccessMenus& owner, const IDevice& modelInstanceKey, IGenericAccess& model);

protected:
	// Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	GenericAccessMenus& _owner;
	const IDevice& _modelInstanceKey;
	IGenericAccess& _model;
};

#endif
