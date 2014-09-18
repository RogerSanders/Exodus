#include "MegaDriveROMLoader.h"
#ifndef __MEGADRIVEROMLOADER_FILEOPENMENUHANDLER_H__
#define __MEGADRIVEROMLOADER_FILEOPENMENUHANDLER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"

class MegaDriveROMLoader::FileOpenMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_OPENMEGADRIVEROM,
		MENUITEM_CLOSEMEGADRIVEROM
	};

public:
	//Constructors
	FileOpenMenuHandler(MegaDriveROMLoader& aextension);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);
	virtual void HandleMenuItemSelectNonView(int menuItemID, IViewManager& aviewManager);

private:
	MegaDriveROMLoader& extension;
};

#endif
