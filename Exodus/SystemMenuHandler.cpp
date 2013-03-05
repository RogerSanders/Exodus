#include "SystemMenuHandler.h"
#include "LoggerViewModel.h"
#include "LoggerDetailsViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::SystemMenuHandler::SystemMenuHandler(System* adevice)
:MenuHandlerBase(L"SystemMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void System::SystemMenuHandler::OpenLoggerDetailsView(const LogEntryInternal& alogEntry)
{
	IViewModel* viewModelBase = GetViewModelIfOpen(MENUITEM_LOGGERDETAILS);
	if(viewModelBase != 0)
	{
		LoggerDetailsViewModel* viewModel = dynamic_cast<LoggerDetailsViewModel*>(viewModelBase);
		if(viewModel != 0)
		{
			viewModel->SetLogEntry(alogEntry);
		}
	}
	else
	{
		viewModelBase = new LoggerDetailsViewModel(GetMenuHandlerName(), GetMenuItemName(MENUITEM_LOGGERDETAILS), MENUITEM_LOGGERDETAILS, device, alogEntry);
		if(!AddCreatedViewModel(MENUITEM_LOGGERDETAILS, viewModelBase))
		{
			delete viewModelBase;
		}
	}
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void System::SystemMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGER, L"EventLog", L"Event Log", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGERDETAILS, L"EventLogEntry", L"Event Log Entry", true, true));
}

//----------------------------------------------------------------------------------------
IViewModel* System::SystemMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_LOGGER:
		return new LoggerViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_LOGGER, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void System::SystemMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
