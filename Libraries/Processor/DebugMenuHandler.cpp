#include "DebugMenuHandler.h"
#include "ControlViewModel.h"
#include "BreakpointViewModel.h"
#include "WatchpointViewModel.h"
#include "CallStackViewModel.h"
#include "TraceViewModel.h"
#include "DisassemblyOldViewModel.h"
#include "DisassemblyViewModel.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::DebugMenuHandler::DebugMenuHandler(Processor* adevice)
:device(adevice)
{}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
std::wstring Processor::DebugMenuHandler::GetMenuHandlerName() const
{
	return L"ProcessorDebugMenu";
}

//----------------------------------------------------------------------------------------
void Processor::DebugMenuHandler::GetMenuItems(std::list<MenuItemDefinition>& menuItems) const
{
	menuItems.push_back(MenuItemDefinition(MENUITEM_CONTROL, L"Processor Control", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_BREAKPOINTS, L"Breakpoints", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_WATCHPOINTS, L"Watchpoints", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_CALLSTACK, L"Call Stack", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_TRACE, L"Trace", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_DISASSEMBLY, L"Disassembly", true));
	menuItems.push_back(MenuItemDefinition(MENUITEM_DISASSEMBLYOLD, L"Old Disassembly", true));
}

//----------------------------------------------------------------------------------------
IViewModel* Processor::DebugMenuHandler::CreateViewModelForItem(int menuItemID)
{
	switch(menuItemID)
	{
	case MENUITEM_CONTROL:
		return new ControlViewModel(GetMenuHandlerName(), MENUITEM_CONTROL, device);
	case MENUITEM_BREAKPOINTS:
		return new BreakpointViewModel(GetMenuHandlerName(), MENUITEM_BREAKPOINTS, device);
	case MENUITEM_WATCHPOINTS:
		return new WatchpointViewModel(GetMenuHandlerName(), MENUITEM_WATCHPOINTS, device);
	case MENUITEM_CALLSTACK:
		return new CallStackViewModel(GetMenuHandlerName(), MENUITEM_CALLSTACK, device);
	case MENUITEM_TRACE:
		return new TraceViewModel(GetMenuHandlerName(), MENUITEM_TRACE, device);
	case MENUITEM_DISASSEMBLY:
		return new DisassemblyViewModel(GetMenuHandlerName(), MENUITEM_DISASSEMBLY, device);
	case MENUITEM_DISASSEMBLYOLD:
		return new DisassemblyOldViewModel(GetMenuHandlerName(), MENUITEM_DISASSEMBLYOLD, device);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
void Processor::DebugMenuHandler::DeleteViewModelForItem(int menuItemID, IViewModel* viewModel)
{
	delete viewModel;
}
