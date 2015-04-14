#include "DebugMenuHandler.h"
#include "DeviceControlViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(ExodusSystemMenus& aowner, ISystemGUIInterface& amodel)
:MenuHandlerBase(L"SystemDebugMenu", aowner.GetViewManager()), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_DEVICECONTROL, L"DeviceControl", DeviceControlViewPresenter::GetUnqualifiedViewTitle(), true, true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_DEVICECONTROL:
		return new DeviceControlViewPresenter(GetMenuHandlerName(), viewName, MENUITEM_DEVICECONTROL, owner, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
