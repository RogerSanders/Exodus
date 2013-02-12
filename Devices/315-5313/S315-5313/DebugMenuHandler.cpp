#include "DebugMenuHandler.h"
#include "VRAMViewModel.h"
#include "PaletteViewModel.h"
#include "ImageViewModel.h"
#include "RegistersViewModel.h"
#include "LayerRemovalViewModel.h"
#include "DebugSettingsViewModel.h"
#include "SpriteListViewModel.h"
#include "SpriteListDetailsViewModel.h"
#include "PortMonitorViewModel.h"
#include "PortMonitorDetailsViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::DebugMenuHandler::DebugMenuHandler(S315_5313* adevice)
:device(adevice)
{}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void S315_5313::DebugMenuHandler::OpenSpriteListDetailsView(unsigned int aspriteIndex)
{
	IViewModel* viewModelBase = GetViewModelIfOpen(MENUITEM_SPRITELISTDETAILS);
	if(viewModelBase != 0)
	{
		SpriteListDetailsViewModel* viewModel = dynamic_cast<SpriteListDetailsViewModel*>(viewModelBase);
		if(viewModel != 0)
		{
			viewModel->SetSpriteIndex(aspriteIndex);
		}
	}
	else
	{
		viewModelBase = new SpriteListDetailsViewModel(GetMenuHandlerName(), MENUITEM_SPRITELISTDETAILS, device, aspriteIndex);
		if(!AddCreatedViewModel(MENUITEM_SPRITELISTDETAILS, viewModelBase))
		{
			delete viewModelBase;
		}
	}
}

//----------------------------------------------------------------------------------------
void S315_5313::DebugMenuHandler::OpenPortMonitorDetailsView(const PortMonitorEntry& aentry)
{
	IViewModel* viewModelBase = GetViewModelIfOpen(MENUITEM_PORTMONITORDETAILS);
	if(viewModelBase != 0)
	{
		PortMonitorDetailsViewModel* viewModel = dynamic_cast<PortMonitorDetailsViewModel*>(viewModelBase);
		if(viewModel != 0)
		{
			viewModel->SetPortMonitorEntry(aentry);
		}
	}
	else
	{
		viewModelBase = new PortMonitorDetailsViewModel(GetMenuHandlerName(), MENUITEM_PORTMONITORDETAILS, device, aentry);
		if(!AddCreatedViewModel(MENUITEM_PORTMONITORDETAILS, viewModelBase))
		{
			delete viewModelBase;
		}
	}
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
std::wstring S315_5313::DebugMenuHandler::GetMenuHandlerName() const
{
	return L"VDPDebugMenu";
}

//----------------------------------------------------------------------------------------
void S315_5313::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_VRAMVIEWER, L"VRAM Pattern Viewer", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PALETTEVIEWER, L"Palette", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_IMAGE, L"Image", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_LAYERREMOVAL, L"Layer Removal", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_SETTINGS, L"Debug Settings", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_SPRITELIST, L"Sprite List", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_SPRITELISTDETAILS, L"Sprite List Details", true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PORTMONITOR, L"Port Monitor", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PORTMONITORDETAILS, L"Port Monitor Details", true, true));
}

//----------------------------------------------------------------------------------------
IViewModel* S315_5313::DebugMenuHandler::CreateViewModelForItem(int menuItemID)
{
	switch(menuItemID)
	{
	case MENUITEM_VRAMVIEWER:
		return new VRAMViewModel(GetMenuHandlerName(), MENUITEM_VRAMVIEWER, device);
	case MENUITEM_PALETTEVIEWER:
		return new PaletteViewModel(GetMenuHandlerName(), MENUITEM_PALETTEVIEWER, device);
	case MENUITEM_IMAGE:
		return new ImageViewModel(GetMenuHandlerName(), MENUITEM_IMAGE, device);
	case MENUITEM_REGISTERS:
		return new RegistersViewModel(GetMenuHandlerName(), MENUITEM_REGISTERS, device);
	case MENUITEM_LAYERREMOVAL:
		return new LayerRemovalViewModel(GetMenuHandlerName(), MENUITEM_LAYERREMOVAL, device);
	case MENUITEM_SETTINGS:
		return new DebugSettingsViewModel(GetMenuHandlerName(), MENUITEM_SETTINGS, device);
	case MENUITEM_SPRITELIST:
		return new SpriteListViewModel(GetMenuHandlerName(), MENUITEM_SPRITELIST, device);
	case MENUITEM_PORTMONITOR:
		return new PortMonitorViewModel(GetMenuHandlerName(), MENUITEM_PORTMONITOR, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
