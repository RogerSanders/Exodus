#include "DebugMenuHandler.h"
#include "VRAMViewPresenter.h"
#include "PaletteViewPresenter.h"
#include "PlaneViewPresenter.h"
#include "ImageViewPresenter.h"
#include "RegistersViewPresenter.h"
#include "SpriteListViewPresenter.h"
#include "SpriteListDetailsViewPresenter.h"
#include "PortMonitorViewPresenter.h"
#include "PortMonitorDetailsViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:MenuHandlerBase(L"VDPDebugMenu", aowner.GetViewManager()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_IMAGE, L"Image", ImageViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PALETTEVIEWER, L"Palette", PaletteViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PLANEVIEWER, L"PlaneViewer", PlaneViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PORTMONITOR, L"PortMonitor", PortMonitorViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PORTMONITORDETAILS, L"PortMonitorDetails", PortMonitorDetailsViewPresenter::GetUnqualifiedViewTitle(), true, false, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", RegistersViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_SPRITELIST, L"SpriteList", SpriteListViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_SPRITELISTDETAILS, L"SpriteListDetails", SpriteListDetailsViewPresenter::GetUnqualifiedViewTitle(), true, false, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_VRAMVIEWER, L"VRAMPatterns", VRAMViewPresenter::GetUnqualifiedViewTitle(), true, true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_VRAMVIEWER:
		return new VRAMViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_PALETTEVIEWER:
		return new PaletteViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_PLANEVIEWER:
		return new PlaneViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_IMAGE:
		return new ImageViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_REGISTERS:
		return new RegistersViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_SPRITELIST:
		return new SpriteListViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_PORTMONITOR:
		return new PortMonitorViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::OpenSpriteListDetailsView(unsigned int aspriteIndex)
{
	std::set<IViewPresenter*> viewPresenters = GetOpenViewPresenters(MENUITEM_SPRITELISTDETAILS);
	if(!viewPresenters.empty())
	{
		SpriteListDetailsViewPresenter* spriteListDetailsViewPresenter = dynamic_cast<SpriteListDetailsViewPresenter*>(*viewPresenters.begin());
		if(spriteListDetailsViewPresenter != 0)
		{
			spriteListDetailsViewPresenter->SetSpriteIndex(aspriteIndex);
		}
	}
	else
	{
		IViewPresenter* viewPresenter = new SpriteListDetailsViewPresenter(GetMenuHandlerName(), GetMenuItemName(MENUITEM_SPRITELISTDETAILS), MENUITEM_SPRITELISTDETAILS, owner, modelInstanceKey, model, aspriteIndex);
		if(!AddCreatedView(MENUITEM_SPRITELISTDETAILS, viewPresenter))
		{
			delete viewPresenter;
		}
	}
}

//----------------------------------------------------------------------------------------
void DebugMenuHandler::OpenPortMonitorDetailsView(const IS315_5313::PortMonitorEntry& aentry)
{
	std::set<IViewPresenter*> viewPresenters = GetOpenViewPresenters(MENUITEM_PORTMONITORDETAILS);
	if(!viewPresenters.empty())
	{
		IViewPresenter* viewPresenter = *viewPresenters.begin();
		PortMonitorDetailsViewPresenter* portMonitorDetailsViewPresenter = dynamic_cast<PortMonitorDetailsViewPresenter*>(viewPresenter);
		if(portMonitorDetailsViewPresenter != 0)
		{
			portMonitorDetailsViewPresenter->SetPortMonitorEntry(aentry);
		}
	}
	else
	{
		IViewPresenter* viewPresenter = new PortMonitorDetailsViewPresenter(GetMenuHandlerName(), GetMenuItemName(MENUITEM_PORTMONITORDETAILS), MENUITEM_PORTMONITORDETAILS, owner, modelInstanceKey, model, aentry);
		if(!AddCreatedView(MENUITEM_PORTMONITORDETAILS, viewPresenter))
		{
			delete viewPresenter;
		}
	}
}
