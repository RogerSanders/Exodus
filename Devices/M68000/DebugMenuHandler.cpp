#include "DebugMenuHandler.h"
#include "RegistersViewModel.h"
#include "ExceptionsViewModel.h"
namespace M68000{

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
M68000::DebugMenuHandler::DebugMenuHandler(M68000* adevice)
:device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
std::wstring M68000::DebugMenuHandler::GetMenuHandlerName() const
{
	return L"M68000DebugMenu";
}

//----------------------------------------------------------------------------------------
void M68000::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_EXCEPTIONS, L"Exception Control", true));
}

//----------------------------------------------------------------------------------------
IViewModel* M68000::DebugMenuHandler::CreateViewModelForItem(int menuItemID)
{
	switch(menuItemID)
	{
	case MENUITEM_REGISTERS:
		return new RegistersViewModel(GetMenuHandlerName(), MENUITEM_REGISTERS, device);
	case MENUITEM_EXCEPTIONS:
		return new ExceptionsViewModel(GetMenuHandlerName(), MENUITEM_EXCEPTIONS, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void M68000::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}

} //Close namespace M68000
