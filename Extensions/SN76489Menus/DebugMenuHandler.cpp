#include "DebugMenuHandler.h"
#include "RegistersViewPresenter.h"
#include "LoggingViewPresenter.h"
#include "PropertiesViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(SN76489Menus& aowner, const IDevice& amodelInstanceKey, ISN76489& amodel)
:MenuHandlerBase(L"SN76489DebugMenu", aowner.GetViewManager()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", RegistersViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_PROPERTIES, L"Properties", PropertiesViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_WAV_FILE_LOGGING, L"OutputLogging", LoggingViewPresenter::GetUnqualifiedViewTitle(), true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_REGISTERS:
		return new RegistersViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_PROPERTIES:
		return new PropertiesViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_WAV_FILE_LOGGING:
		return new LoggingViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
