#include "DebugMenuHandler.h"
#include "VRAMViewModel.h"
#include "PaletteViewModel.h"
#include "ImageViewModel.h"
#include "RegistersViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::DebugMenuHandler::DebugMenuHandler(S315_5313* adevice)
:device(adevice)
{}

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
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void S315_5313::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
