#include "S315_5313.h"
#ifndef __S315_5313_DEBUGMENUHANDLER_H__
#define __S315_5313_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class S315_5313::DebugMenuHandler :public MenuHandlerBase
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
	DebugMenuHandler(S315_5313* adevice);

	//Window functions
	void OpenSpriteListDetailsView(unsigned int aspriteIndex);
	void OpenPortMonitorDetailsView(const PortMonitorEntry& aentry);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	S315_5313* device;
};

#endif
