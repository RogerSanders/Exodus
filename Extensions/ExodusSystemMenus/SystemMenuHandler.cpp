#include "SystemMenuHandler.h"
#include "EventLogViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SystemMenuHandler::SystemMenuHandler(ExodusSystemMenus& aowner, ISystemGUIInterface& amodel)
:MenuHandlerBase(L"SystemMenu", aowner.GetViewManager()), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void SystemMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_EVENTLOG, L"EventLog", EventLogViewPresenter::GetUnqualifiedViewTitle(), true, true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* SystemMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_EVENTLOG:
		return new EventLogViewPresenter(GetMenuHandlerName(), viewName, MENUITEM_EVENTLOG, owner, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void SystemMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
