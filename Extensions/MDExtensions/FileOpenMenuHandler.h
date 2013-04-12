#include "MegaDriveROMLoader.h"
#ifndef __MEGADRIVEROMLOADER_FILEOPENMENUHANDLER_H__
#define __MEGADRIVEROMLOADER_FILEOPENMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

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
	FileOpenMenuHandler(MegaDriveROMLoader* adevice);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);
	virtual void HandleMenuItemSelectNonViewModel(int menuItemID, IViewModelLauncher& aviewModelLauncher);

private:
	MegaDriveROMLoader* device;
};

#endif
