#ifndef __SETTINGSMENUHANDLER_H__
#define __SETTINGSMENUHANDLER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "GenericAccess/GenericAccess.pkg"
#include "GenericAccessMenus.h"

class SettingsMenuHandler :public MenuHandlerBase
{
public:
	//Constructors
	SettingsMenuHandler(GenericAccessMenus& aowner, const IDevice& amodelInstanceKey, IGenericAccess& amodel);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	GenericAccessMenus& owner;
	const IDevice& modelInstanceKey;
	IGenericAccess& model;
};

#endif
