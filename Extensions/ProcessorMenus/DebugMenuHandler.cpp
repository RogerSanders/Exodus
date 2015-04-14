#include "DebugMenuHandler.h"
#include "ControlViewPresenter.h"
#include "BreakpointViewPresenter.h"
#include "WatchpointViewPresenter.h"
#include "CallStackViewPresenter.h"
#include "TraceViewPresenter.h"
#include "DisassemblyViewPresenter.h"
#include "ActiveDisassemblyViewPresenter.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugMenuHandler::DebugMenuHandler(ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel)
:MenuHandlerBase(L"ProcessorDebugMenu", aowner.GetViewManager()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
void DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	if(model.ActiveDisassemblySupported())
	{
		menuItems.push_back(MenuItemDefinition(MENUITEM_ACTIVEDISASSEMBLY, L"ActiveDisassembly", ActiveDisassemblyViewPresenter::GetUnqualifiedViewTitle(), true));
	}
	menuItems.push_back(MenuItemDefinition(MENUITEM_BREAKPOINTS, L"Breakpoints", BreakpointViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_CALLSTACK, L"CallStack", CallStackViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_DISASSEMBLY, L"Disassembly", DisassemblyViewPresenter::GetUnqualifiedViewTitle(), true, true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_CONTROL, L"ProcessorControl", ControlViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_TRACE, L"Trace", TraceViewPresenter::GetUnqualifiedViewTitle(), true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_WATCHPOINTS, L"Watchpoints", WatchpointViewPresenter::GetUnqualifiedViewTitle(), true));
}

//----------------------------------------------------------------------------------------
IViewPresenter* DebugMenuHandler::CreateViewForItem(int menuItemID, const std::wstring& viewName)
{
	switch(menuItemID)
	{
	case MENUITEM_CONTROL:
		return new ControlViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_BREAKPOINTS:
		return new BreakpointViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_WATCHPOINTS:
		return new WatchpointViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_CALLSTACK:
		return new CallStackViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_TRACE:
		return new TraceViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_DISASSEMBLY:
		return new DisassemblyViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	case MENUITEM_ACTIVEDISASSEMBLY:
		return new ActiveDisassemblyViewPresenter(GetMenuHandlerName(), viewName, menuItemID, owner, modelInstanceKey, model);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void DebugMenuHandler::DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter)
{
	delete viewPresenter;
}
