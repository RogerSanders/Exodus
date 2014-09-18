#include "DebugMenuHandler.h"
#include "RegistersViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(Z80Menus& aowner, const IDevice& amodelInstanceKey, IZ80& amodel)
:MenuHandlerBase(L"Z80DebugMenu", aowner.GetViewManager()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", RegistersViewPresenter::GetUnqualifiedViewTitle(), true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_REGISTERS:
		return new RegistersViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
