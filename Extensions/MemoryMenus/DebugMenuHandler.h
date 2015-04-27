#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "MemoryMenus.h"
#include "Memory/IMemory.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_MEMORYEDITOR
	};

	//Constructors
	DebugMenuHandler(MemoryMenus& aowner, const IDevice& amodelInstanceKey, IMemory& amodel);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	MemoryMenus& owner;
	const IDevice& modelInstanceKey;
	IMemory& model;
};

#endif
