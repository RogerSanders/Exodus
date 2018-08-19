#include "DebugMenuHandler.h"
#include "VRAMViewPresenter.h"
#include "PaletteViewPresenter.h"
#include "PlaneViewPresenter.h"
#include "ImageViewPresenter.h"
#include "RegistersViewPresenter.h"
#include "SpriteListViewPresenter.h"
#include "PortMonitorViewPresenter.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(S315_5313Menus& owner, const IDevice& modelInstanceKey, IS315_5313& model)
:MenuHandlerBase(L"VDPDebugMenu", owner.GetViewManager()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// Management functions
//----------------------------------------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_IMAGE, L"Image", ImageViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PALETTEVIEWER, L"Palette", PaletteViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PLANEVIEWER, L"PlaneViewer", PlaneViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PORTMONITOR, L"PortMonitor", PortMonitorViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", RegistersViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_SPRITELIST, L"SpriteList", SpriteListViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_VRAMVIEWER, L"VRAMPatterns", VRAMViewPresenter::GetUnqualifiedViewTitle(), true, true));
}

//----------------------------------------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch (menuItemID)
	{
	case MENUITEM_VRAMVIEWER:
		return new VRAMViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_PALETTEVIEWER:
		return new PaletteViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_PLANEVIEWER:
		return new PlaneViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_IMAGE:
		return new ImageViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_REGISTERS:
		return new RegistersViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_SPRITELIST:
		return new SpriteListViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_PORTMONITOR:
		return new PortMonitorViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
