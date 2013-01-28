#include "SystemMenuHandler.h"
#include "LoggerViewModel.h"
#include "LoggerDetailsViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::SystemMenuHandler::SystemMenuHandler(System* adevice)
:device(adevice)
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
		viewModelBase = new LoggerDetailsViewModel(GetMenuHandlerName(), MENUITEM_LOGGERDETAILS, device, alogEntry);
		if(!AddCreatedViewModel(MENUITEM_LOGGERDETAILS, viewModelBase))
		{
			delete viewModelBase;
		}
	}
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
std::wstring System::SystemMenuHandler::GetMenuHandlerName() const
{
	return L"SystemMenu";
}

//----------------------------------------------------------------------------------------
void System::SystemMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGER, L"Event Log", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGERDETAILS, L"Event Log Entry", true, true));
}

//----------------------------------------------------------------------------------------
IViewModel* System::SystemMenuHandler::CreateViewModelForItem(int menuItemID)
{
	switch(menuItemID)
	{
	case MENUITEM_LOGGER:
		return new LoggerViewModel(GetMenuHandlerName(), MENUITEM_LOGGER, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void System::SystemMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
