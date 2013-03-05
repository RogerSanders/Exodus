#include "DebugMenuHandler.h"
#include "DeviceControlViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::DebugMenuHandler::DebugMenuHandler(System* adevice)
:MenuHandlerBase(L"SystemDebugMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void System::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_DEVICECONTROL, L"DeviceControl", L"Device Control", true));
}

//----------------------------------------------------------------------------------------
IViewModel* System::DebugMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_DEVICECONTROL:
		return new DeviceControlViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_DEVICECONTROL, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void System::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
