#include "SystemMenuHandler.h"
#include "LoggerViewModel.h"
#include "LoggerDetailsViewModel.h"
#include "InputMappingViewModel.h"
#include "InputMappingDetailsViewModel.h"

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
void System::SystemMenuHandler::OpenInputMappingDetailsView(IDevice* targetDevice)
{
	IViewModel* viewModelBase = GetViewModelIfOpen(MENUITEM_INPUTMAPPINGDETAILS);
	if(viewModelBase != 0)
	{
		InputMappingDetailsViewModel* viewModel = dynamic_cast<InputMappingDetailsViewModel*>(viewModelBase);
		if(viewModel != 0)
		{
			viewModel->SetTargetDevice(targetDevice);
		}
	}
	else
	{
		viewModelBase = new InputMappingDetailsViewModel(GetMenuHandlerName(), MENUITEM_INPUTMAPPINGDETAILS, device, targetDevice);
		if(!AddCreatedViewModel(MENUITEM_INPUTMAPPINGDETAILS, viewModelBase))
		{
			delete viewModelBase;
		}
	}
}

//----------------------------------------------------------------------------------------
void System::SystemMenuHandler::CloseInputMappingDetailsView()
{
	IViewModel* viewModelBase = GetViewModelIfOpen(MENUITEM_INPUTMAPPINGDETAILS);
	if(viewModelBase != 0)
	{
		viewModelBase->CloseView();
	}
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
std::wstring System::SystemMenuHandler::GetMenuHandlerName() const
{
	return L"SystemSettingsMenu";
}

//----------------------------------------------------------------------------------------
void System::SystemMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGER, L"Logger", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_LOGGERDETAILS, L"Logger Details", true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_INPUTMAPPING, L"Input Mapping", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_INPUTMAPPINGDETAILS, L"Input Mapping Details", true, true));
}

//----------------------------------------------------------------------------------------
IViewModel* System::SystemMenuHandler::CreateViewModelForItem(int menuItemID)
{
	switch(menuItemID)
	{
	case MENUITEM_LOGGER:
		return new LoggerViewModel(GetMenuHandlerName(), MENUITEM_LOGGER, device);
	case MENUITEM_INPUTMAPPING:
		return new InputMappingViewModel(GetMenuHandlerName(), MENUITEM_INPUTMAPPING, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void System::SystemMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
