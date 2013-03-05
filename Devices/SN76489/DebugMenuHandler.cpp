#include "DebugMenuHandler.h"
#include "RegistersViewModel.h"
#include "LoggingViewModel.h"
#include "PropertiesViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489::DebugMenuHandler::DebugMenuHandler(SN76489* adevice)
:MenuHandlerBase(L"SN76489DebugMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void SN76489::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", L"Registers", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PROPERTIES, L"Properties", L"Properties", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_WAV_FILE_LOGGING, L"OutputLogging", L"WAV File Logging", true));
}

//----------------------------------------------------------------------------------------
IViewModel* SN76489::DebugMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_REGISTERS:
		return new RegistersViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_REGISTERS, device);
	case MENUITEM_PROPERTIES:
		return new PropertiesViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_PROPERTIES, device);
	case MENUITEM_WAV_FILE_LOGGING:
		return new LoggingViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_WAV_FILE_LOGGING, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void SN76489::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
