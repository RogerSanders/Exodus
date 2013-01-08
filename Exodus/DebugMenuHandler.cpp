#include "DebugMenuHandler.h"
#include "DeviceControlViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::DebugMenuHandler::DebugMenuHandler(System* adevice)
:device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
std::wstring System::DebugMenuHandler::GetMenuHandlerName() const
{
	return L"SystemDebugMenu";
}

//----------------------------------------------------------------------------------------
void System::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_DEVICECONTROL, L"Device Control", true));
}

//----------------------------------------------------------------------------------------
IViewModel* System::DebugMenuHandler::CreateViewModelForItem(int menuItemID)
{
	switch(menuItemID)
	{
	case MENUITEM_DEVICECONTROL:
		return new DeviceControlViewModel(GetMenuHandlerName(), MENUITEM_DEVICECONTROL, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void System::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
