#include "SettingsMenuHandler.h"
#include "SettingsViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::SettingsMenuHandler::SettingsMenuHandler(S315_5313* adevice)
:MenuHandlerBase(L"VDPSettingsMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void S315_5313::SettingsMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_SETTINGS, L"Settings", L"Settings", true));
}

//----------------------------------------------------------------------------------------
IViewModel* S315_5313::SettingsMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_SETTINGS:
		return new SettingsViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_SETTINGS, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::SettingsMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
