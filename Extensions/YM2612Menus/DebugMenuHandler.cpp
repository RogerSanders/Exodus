#include "DebugMenuHandler.h"
#include "DebuggerViewPresenter.h"
#include "LoggingViewPresenter.h"
#include "OperatorViewPresenter.h"
#include "RegistersViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(YM2612Menus& aowner, const IDevice& amodelInstanceKey, IYM2612& amodel)
:MenuHandlerBase(L"YM2612DebugMenu", aowner.GetViewManager()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_DEBUGGER, L"Debugger", DebuggerViewPresenter::GetUnqualifiedViewTitle(), true));
	//menuItems.push_back(MenuItemDefinition(MENUITEM_OPERATOR, L"Operator", OperatorViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_WAV_FILE_LOGGING, L"OutputLogging", LoggingViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", RegistersViewPresenter::GetUnqualifiedViewTitle(), true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_DEBUGGER:
		return new DebuggerViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_OPERATOR:
		return new OperatorViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_REGISTERS:
		return new RegistersViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
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

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::OpenOperatorView(unsigned int achannelNo, unsigned int aoperatorNo)
{
	std::set<IViewPresenter*> viewPresenters = GetOpenViewPresenters(MENUITEM_OPERATOR);
	if(!viewPresenters.empty())
	{
		IViewPresenter* viewPresenter = *viewPresenters.begin();
		OperatorViewPresenter* operatorViewPresenter = dynamic_cast<OperatorViewPresenter*>(viewPresenter);
		if(operatorViewPresenter != 0)
		{
			operatorViewPresenter->SetTargetOperator(achannelNo, aoperatorNo);
		}
	}
	else
	{
		IViewPresenter* viewPresenter = new OperatorViewPresenter(GetMenuHandlerName(), GetMenuItemName(MENUITEM_OPERATOR), MENUITEM_OPERATOR, owner, modelInstanceKey, model, achannelNo, aoperatorNo);
		if(!AddCreatedView(MENUITEM_OPERATOR, viewPresenter))
		{
			delete viewPresenter;
		}
	}
}
