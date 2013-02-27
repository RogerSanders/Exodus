#include "MegaDriveROMLoader.h"
#ifndef __MEGADRIVEROMLOADER_FILEOPENMENUHANDLER_H__
#define __MEGADRIVEROMLOADER_FILEOPENMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class MegaDriveROMLoader::FileOpenMenuHandler :public IMenuHandler
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_OPENMEGADRIVEROM,
		MENUITEM_CLOSEMEGADRIVEROM
	};

	//Constructors
	FileOpenMenuHandler(MegaDriveROMLoader* adevice);

	//Menu item handler functions
	void AddMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& aviewModelLauncher);
	virtual void HandleMenuItemSelect(int menuItemID, IViewModelLauncher& aviewModelLauncher);

private:
	MegaDriveROMLoader* device;
};

#endif
