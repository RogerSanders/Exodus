#include "DebugMenuHandler.h"
#include "RegistersViewModel.h"
namespace Z80{

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Z80::DebugMenuHandler::DebugMenuHandler(Z80* adevice)
:device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
std::wstring Z80::DebugMenuHandler::GetMenuHandlerName() const
{
	return L"Z80DebugMenu";
}

//----------------------------------------------------------------------------------------
void Z80::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", true));
}

//----------------------------------------------------------------------------------------
IViewModel* Z80::DebugMenuHandler::CreateViewModelForItem(int menuItemID)
{
	switch(menuItemID)
	{
	case MENUITEM_REGISTERS:
		return new RegistersViewModel(GetMenuHandlerName(), MENUITEM_REGISTERS, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void Z80::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}

} //Close namespace Z80
