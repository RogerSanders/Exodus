#include "SettingsMenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SettingsMenuHandler::SettingsMenuHandler(S315_5313Menus& owner, const IDevice& modelInstanceKey, IS315_5313& model)
:MenuHandlerBase(L"VDPSettingsMenu", owner.GetViewManager()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void SettingsMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{}

//----------------------------------------------------------------------------------------
IViewPresenter* SettingsMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	return 0;
}

//----------------------------------------------------------------------------------------
void SettingsMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
