#include "DebugMenuHandler.h"
#include "RegistersViewModel.h"
#include "ExceptionsViewModel.h"
namespace M68000{

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
M68000::DebugMenuHandler::DebugMenuHandler(M68000* adevice)
:MenuHandlerBase(L"M68000DebugMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void M68000::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", L"Registers", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_EXCEPTIONS, L"Exceptions", L"Exception Control", true));
}

//----------------------------------------------------------------------------------------
IViewModel* M68000::DebugMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_REGISTERS:
		return new RegistersViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_REGISTERS, device);
	case MENUITEM_EXCEPTIONS:
		return new ExceptionsViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_EXCEPTIONS, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void M68000::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}

} //Close namespace M68000
