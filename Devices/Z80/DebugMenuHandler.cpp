#include "DebugMenuHandler.h"
#include "RegistersViewModel.h"
namespace Z80{

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Z80::DebugMenuHandler::DebugMenuHandler(Z80* adevice)
:MenuHandlerBase(L"Z80DebugMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void Z80::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", L"Registers", true));
}

//----------------------------------------------------------------------------------------
IViewModel* Z80::DebugMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_REGISTERS:
		return new RegistersViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_REGISTERS, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void Z80::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}

} //Close namespace Z80
