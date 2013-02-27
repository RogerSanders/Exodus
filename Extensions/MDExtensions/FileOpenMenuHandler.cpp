#include "FileOpenMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MegaDriveROMLoader::FileOpenMenuHandler::FileOpenMenuHandler(MegaDriveROMLoader* adevice)
:device(adevice)
{}

//----------------------------------------------------------------------------------------
//Menu item handler functions
//----------------------------------------------------------------------------------------
void MegaDriveROMLoader::FileOpenMenuHandler::AddMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& aviewModelLauncher)
{
	menuSegment.AddMenuItemSelectableOption(*this, MENUITEM_OPENMEGADRIVEROM, L"Open Mega Drive ROM");
	menuSegment.AddMenuItemSelectableOption(*this, MENUITEM_CLOSEMEGADRIVEROM, L"Close Mega Drive ROM");
}

//----------------------------------------------------------------------------------------
void MegaDriveROMLoader::FileOpenMenuHandler::HandleMenuItemSelect(int menuItemID, IViewModelLauncher& aviewModelLauncher)
{
	switch(menuItemID)
	{
	case MENUITEM_OPENMEGADRIVEROM:
		device->LoadROMFile();
		break;
	case MENUITEM_CLOSEMEGADRIVEROM:
		device->UnloadROMFile();
		break;
	}
}
