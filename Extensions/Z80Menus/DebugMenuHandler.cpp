#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(Z80Menus& owner, const IDevice& modelInstanceKey, IZ80& model)
:MenuHandlerBase(L"Z80DebugMenu", owner.GetViewManager()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// Management functions
//----------------------------------------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
}

//----------------------------------------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
