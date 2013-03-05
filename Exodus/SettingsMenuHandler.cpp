#include "SettingsMenuHandler.h"
#include "InputMappingViewModel.h"
#include "InputMappingDetailsViewModel.h"
#include "EmbeddedROMViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::SettingsMenuHandler::SettingsMenuHandler(System* adevice)
:MenuHandlerBase(L"SystemSettingsMenu"), device(adevice)
{}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void System::SettingsMenuHandler::OpenInputMappingDetailsView(IDevice* targetDevice)
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
		viewModelBase = new InputMappingDetailsViewModel(GetMenuHandlerName(), GetMenuItemName(MENUITEM_INPUTMAPPINGDETAILS), MENUITEM_INPUTMAPPINGDETAILS, device, targetDevice);
		if(!AddCreatedViewModel(MENUITEM_INPUTMAPPINGDETAILS, viewModelBase))
		{
			delete viewModelBase;
		}
	}
}

//----------------------------------------------------------------------------------------
void System::SettingsMenuHandler::CloseInputMappingDetailsView()
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
void System::SettingsMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_INPUTMAPPING, L"InputMapping", L"Input Mapping", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_INPUTMAPPINGDETAILS, L"InputMappingDetails", L"Input Mapping Details", true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_EMBEDDEDROM, L"EmbeddedROMControl", L"Embedded ROM Selection", true));
}

//----------------------------------------------------------------------------------------
IViewModel* System::SettingsMenuHandler::CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName)
{
	switch(menuItemID)
	{
	case MENUITEM_INPUTMAPPING:
		return new InputMappingViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_INPUTMAPPING, device);
	case MENUITEM_EMBEDDEDROM:
		return new EmbeddedROMViewModel(GetMenuHandlerName(), viewModelName, MENUITEM_EMBEDDEDROM, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void System::SettingsMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
