#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "GenericAccess/GenericAccess.pkg"
#include "GenericAccessMenus.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	//Constructors
	DebugMenuHandler(GenericAccessMenus& aowner, const IDevice& amodelInstanceKey, IGenericAccess& amodel);

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
