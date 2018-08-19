#include "DebugMenuHandler.h"
#include "DebuggerViewPresenter.h"
#include "OperatorViewPresenter.h"
#include "RegistersViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(YM2612Menus& owner, const IDevice& modelInstanceKey, IYM2612& model)
:MenuHandlerBase(L"YM2612DebugMenu", owner.GetViewManager()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_DEBUGGER, L"Debugger", DebuggerViewPresenter::GetUnqualifiedViewTitle(), true, true));
	//menuItems.push_back(MenuItemDefinition(MENUITEM_OPERATOR, L"Operator", OperatorViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_REGISTERS, L"Registers", RegistersViewPresenter::GetUnqualifiedViewTitle(), true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_DEBUGGER:
		return new DebuggerViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_OPERATOR:
		return new OperatorViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
	case MENUITEM_REGISTERS:
		return new RegistersViewPresenter(GetMenuHandlerName(), viewName, menuItemID, _owner, _modelInstanceKey, _model);
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
void DebugMenuHandler::OpenOperatorView(unsigned int channelNo, unsigned int operatorNo)
{
	std::set<IViewPresenter*> viewPresenters = GetOpenViewPresenters(MENUITEM_OPERATOR);
	if(!viewPresenters.empty())
	{
		IViewPresenter* viewPresenter = *viewPresenters.begin();
		OperatorViewPresenter* operatorViewPresenter = dynamic_cast<OperatorViewPresenter*>(viewPresenter);
		if(operatorViewPresenter != 0)
		{
			operatorViewPresenter->SetTargetOperator(channelNo, operatorNo);
		}
	}
	else
	{
		IViewPresenter* viewPresenter = new OperatorViewPresenter(GetMenuHandlerName(), GetMenuItemName(MENUITEM_OPERATOR), MENUITEM_OPERATOR, _owner, _modelInstanceKey, _model, channelNo, operatorNo);
		if(!AddCreatedView(MENUITEM_OPERATOR, viewPresenter))
		{
			delete viewPresenter;
		}
	}
}
