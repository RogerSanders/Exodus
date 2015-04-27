#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "S315_5313Menus.h"
#include "315-5313/IS315_5313.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_VRAMVIEWER,
		MENUITEM_PALETTEVIEWER,
		MENUITEM_PLANEVIEWER,
		MENUITEM_IMAGE,
		MENUITEM_REGISTERS,
		MENUITEM_LAYERREMOVAL,
		MENUITEM_SETTINGS,
		MENUITEM_SPRITELIST,
		MENUITEM_SPRITELISTDETAILS,
		MENUITEM_PORTMONITOR,
		MENUITEM_PORTMONITORDETAILS
	};

	//Constructors
	DebugMenuHandler(S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel);

	//Window functions
	void OpenSpriteListDetailsView(unsigned int aspriteIndex);
	void OpenPortMonitorDetailsView(const IS315_5313::PortMonitorEntry& aentry);

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
