#include "VDP.h"
#ifndef __VDP_DEBUGMENUHANDLER_H__
#define __VDP_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class VDP::DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_VRAMVIEWER,
		MENUITEM_PALETTEVIEWER,
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
	DebugMenuHandler(VDP* adevice);

	//Window functions
	void OpenSpriteListDetailsView(unsigned int aspriteMappingAddress, unsigned int aspriteIndex);
	void OpenPortMonitorDetailsView(const PortMonitorEntry& aentry);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	VDP* device;
};

#endif
