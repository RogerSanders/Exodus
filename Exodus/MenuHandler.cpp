#include "MenuHandler.h"
#include "AboutViewPresenter.h"
#include "SettingsViewPresenter.h"
#include "ModuleManagerViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuHandler::MenuHandler(ExodusInterface& aowner, ExodusInterface& amodel)
:MenuHandlerBase(L"GUIMenus", aowner.GetViewManager()), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void MenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_ABOUT, L"About", AboutViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_SETTINGS, L"Settings", SettingsViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_MODULEMANAGER, L"ModuleManager", ModuleManagerViewPresenter::GetUnqualifiedViewTitle(), true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* MenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_ABOUT:
		return new AboutViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, model);
	case MENUITEM_SETTINGS:
		return new SettingsViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, model);
	case MENUITEM_MODULEMANAGER:
		return new ModuleManagerViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void MenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
