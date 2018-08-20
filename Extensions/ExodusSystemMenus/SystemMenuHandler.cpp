#include "SystemMenuHandler.h"
#include "EventLogViewPresenter.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
SystemMenuHandler::SystemMenuHandler(ExodusSystemMenus& owner, ISystemGUIInterface& model)
:MenuHandlerBase(L"SystemMenu", owner.GetViewManager()), _owner(owner), _model(model)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Management functions
//----------------------------------------------------------------------------------------------------------------------
void SystemMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_EVENTLOG, L"EventLog", EventLogViewPresenter::GetUnqualifiedViewTitle(), true, true));
}

//----------------------------------------------------------------------------------------------------------------------
IViewPresenter* SystemMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch (menuItemID)
	{
	case MENUITEM_EVENTLOG:
		return new EventLogViewPresenter(GetMenuHandlerName(), viewName, MENUITEM_EVENTLOG, _owner, _model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SystemMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
