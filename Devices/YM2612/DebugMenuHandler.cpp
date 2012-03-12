#include "DebugMenuHandler.h"
#include "DebuggerViewModel.h"
#include "LoggingViewModel.h"
#include "OperatorViewModel.h"
#include "RegistersViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612::DebugMenuHandler::DebugMenuHandler(YM2612* adevice)
:device(adevice)
{}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void YM2612::DebugMenuHandler::OpenOperatorView(unsigned int achannelNo, unsigned int aoperatorNo)
{
	IViewModel* viewModelBase = GetViewModelIfOpen(MENUITEM_OPERATOR);
	if(viewModelBase != 0)
	{
		OperatorViewModel* viewModel = dynamic_cast<OperatorViewModel*>(viewModelBase);
		if(viewModel != 0)
		{
			viewModel->SetTargetOperator(achannelNo, aoperatorNo);
		}
	}
	else
	{
		viewModelBase = new OperatorViewModel(GetMenuHandlerName(), MENUITEM_OPERATOR, device, achannelNo, aoperatorNo);
		if(!AddCreatedViewModel(MENUITEM_OPERATOR, viewModelBase))
		{
			delete viewModelBase;
		}
	}
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
std::wstring YM2612::DebugMenuHandler::GetMenuHandlerName() const
{
	return L"YM2612DebugMenu";
}

//----------------------------------------------------------------------------------------
void YM2612::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_DEBUGGER, L"Debugger", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_OPERATOR, L"Operator", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Raw Registers", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_WAV_FILE_LOGGING, L"WAV File Logging", true));
}

//----------------------------------------------------------------------------------------
IViewModel* YM2612::DebugMenuHandler::CreateViewModelForItem(int menuItemID)
{
	switch(menuItemID)
	{
	case MENUITEM_DEBUGGER:
		return new DebuggerViewModel(GetMenuHandlerName(), MENUITEM_DEBUGGER, device);
	case MENUITEM_OPERATOR:
		return new OperatorViewModel(GetMenuHandlerName(), MENUITEM_OPERATOR, device);
	case MENUITEM_REGISTERS:
		return new RegistersViewModel(GetMenuHandlerName(), MENUITEM_REGISTERS, device);
	case MENUITEM_WAV_FILE_LOGGING:
		return new LoggingViewModel(GetMenuHandlerName(), MENUITEM_WAV_FILE_LOGGING, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void YM2612::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
