#include "SettingsMenuHandler.h"
#include "SettingsViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SettingsMenuHandler::SettingsMenuHandler(S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:MenuHandlerBase(L"VDPSettingsMenu", aowner.GetViewManager()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void SettingsMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_SETTINGS, L"Settings", SettingsViewPresenter::GetUnqualifiedViewTitle(), true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* SettingsMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_SETTINGS:
		return new SettingsViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void SettingsMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
