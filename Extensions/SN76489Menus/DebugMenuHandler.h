#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "SN76489Menus.h"
#include "SN76489/ISN76489.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_REGISTERS,
		MENUITEM_PROPERTIES,
		MENUITEM_WAV_FILE_LOGGING
	};

public:
	//Constructors
	DebugMenuHandler(SN76489Menus& aowner, const IDevice& amodelInstanceKey, ISN76489& amodel);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	SN76489Menus& owner;
	const IDevice& modelInstanceKey;
	ISN76489& model;
};

#endif
