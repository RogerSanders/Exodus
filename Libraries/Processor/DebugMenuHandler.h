#include "Processor.h"
#ifndef __PROCESSOR_DEBUGMENUHANDLER_H__
#define __PROCESSOR_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class Processor::DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_CONTROL,
		MENUITEM_BREAKPOINTS,
		MENUITEM_WATCHPOINTS,
		MENUITEM_CALLSTACK,
		MENUITEM_TRACE,
		MENUITEM_DISASSEMBLY,
		MENUITEM_ACTIVEDISASSEMBLY
	};

	//Constructors
	DebugMenuHandler(Processor* adevice);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	Processor* device;
};

#endif
