#include "FileOpenMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MegaDriveROMLoader::FileOpenMenuHandler::FileOpenMenuHandler(MegaDriveROMLoader* adevice)
:MenuHandlerBase(L"MegaDriveROMLoaderFileOpenMenu"), device(adevice)
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
IViewModel* MegaDriveROMLoader::FileOpenMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	return 0;
}

//----------------------------------------------------------------------------------------
void MegaDriveROMLoader::FileOpenMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}

//----------------------------------------------------------------------------------------
void MegaDriveROMLoader::FileOpenMenuHandler::HandleMenuItemSelectNonViewModel(int menuItemID, IViewModelLauncher& aviewModelLauncher)
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
