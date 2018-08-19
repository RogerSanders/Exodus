#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ProcessorMenus.h"
#include "Processor/Processor.pkg"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	// Enumerations
	enum MenuItem
	{
		MENUITEM_CONTROL,
		MENUITEM_WATCHPOINTS,
		MENUITEM_CALLSTACK,
		MENUITEM_TRACE,
		MENUITEM_DISASSEMBLY,
		MENUITEM_ACTIVEDISASSEMBLY
	};

	// Constructors
	DebugMenuHandler(ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model);

protected:
	// Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	ProcessorMenus& _owner;
	const IDevice& _modelInstanceKey;
	IProcessor& _model;
};

#endif
