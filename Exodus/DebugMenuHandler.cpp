#include "DebugMenuHandler.h"
#include "LoggerViewModel.h"
#include "LoggerDetailsViewModel.h"
#include "DeviceControlViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::DebugMenuHandler::DebugMenuHandler(System* adevice)
:device(adevice)
{}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void System::DebugMenuHandler::OpenLoggerDetailsView(const LogEntryInternal& alogEntry)
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
std::wstring System::DebugMenuHandler::GetMenuHandlerName() const
{
	return L"SystemDebugMenu";
}

//----------------------------------------------------------------------------------------
void System::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGER, L"Logger", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGERDETAILS, L"Logger Details", true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_DEVICECONTROL, L"Device Control", true));
}

//----------------------------------------------------------------------------------------
IViewModel* System::DebugMenuHandler::CreateViewModelForItem(int menuItemID)
{
	switch(menuItemID)
	{
	case MENUITEM_LOGGER:
		return new LoggerViewModel(GetMenuHandlerName(), MENUITEM_LOGGER, device);
	case MENUITEM_DEVICECONTROL:
		return new DeviceControlViewModel(GetMenuHandlerName(), MENUITEM_DEVICECONTROL, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void System::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
