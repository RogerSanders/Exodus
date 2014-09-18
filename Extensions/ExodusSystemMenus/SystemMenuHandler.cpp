#include "SystemMenuHandler.h"
#include "LoggerViewPresenter.h"
#include "LoggerDetailsViewPresenter.h"

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
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGER, L"EventLog", LoggerViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGERDETAILS, L"EventLogEntry", LoggerDetailsViewPresenter::GetUnqualifiedViewTitle(), true, true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* SystemMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_LOGGER:
		return new LoggerViewPresenter(GetMenuHandlerName(), viewName, MENUITEM_LOGGER, owner, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void SystemMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void SystemMenuHandler::OpenLoggerDetailsView(const ISystemGUIInterface::SystemLogEntry& alogEntry)
{
	IViewPresenter* viewPresenter = GetViewIfOpen(MENUITEM_LOGGERDETAILS);
	if(viewPresenter != 0)
	{
		LoggerDetailsViewPresenter* loggerDetailsViewPresenter = dynamic_cast<LoggerDetailsViewPresenter*>(viewPresenter);
		if(loggerDetailsViewPresenter != 0)
		{
			loggerDetailsViewPresenter->SetLogEntry(alogEntry);
		}
	}
	else
	{
		viewPresenter = new LoggerDetailsViewPresenter(GetMenuHandlerName(), GetMenuItemName(MENUITEM_LOGGERDETAILS), MENUITEM_LOGGERDETAILS, owner, model, alogEntry);
		if(!AddCreatedView(MENUITEM_LOGGERDETAILS, viewPresenter))
		{
			delete viewPresenter;
		}
	}
}
