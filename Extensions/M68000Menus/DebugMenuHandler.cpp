#include "DebugMenuHandler.h"
#include "RegistersViewPresenter.h"
#include "ExceptionsViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(M68000Menus& aowner, const IDevice& amodelInstanceKey, IM68000& amodel)
:MenuHandlerBase(L"M68000DebugMenu", aowner.GetViewManager()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_EXCEPTIONS, L"Exceptions", ExceptionsViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", RegistersViewPresenter::GetUnqualifiedViewTitle(), true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_REGISTERS:
		return new RegistersViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_EXCEPTIONS:
		return new ExceptionsViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
