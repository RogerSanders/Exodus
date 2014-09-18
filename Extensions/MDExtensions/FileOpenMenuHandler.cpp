#include "FileOpenMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MegaDriveROMLoader::FileOpenMenuHandler::FileOpenMenuHandler(MegaDriveROMLoader& aextension)
:MenuHandlerBase(L"MegaDriveROMLoaderFileOpenMenu", aextension.GetViewManager()), extension(aextension)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void MegaDriveROMLoader::FileOpenMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_OPENMEGADRIVEROM, L"OpenMegaDriveROM", L"Open Mega Drive ROM", false));
	menuItems.push_back(MenuItemDefinition(MENUITEM_CLOSEMEGADRIVEROM, L"CloseMegaDriveROM", L"Close Mega Drive ROM", false));
}

//----------------------------------------------------------------------------------------
IViewPresenter* MegaDriveROMLoader::FileOpenMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	return 0;
}

//----------------------------------------------------------------------------------------
void MegaDriveROMLoader::FileOpenMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}

//----------------------------------------------------------------------------------------
void MegaDriveROMLoader::FileOpenMenuHandler::HandleMenuItemSelectNonView(int menuItemID, IViewManager& aviewManager)
{
	switch(menuItemID)
	{
	case MENUITEM_OPENMEGADRIVEROM:
		extension.LoadROMFile();
		break;
	case MENUITEM_CLOSEMEGADRIVEROM:
		extension.UnloadROMFile();
		break;
	}
}
