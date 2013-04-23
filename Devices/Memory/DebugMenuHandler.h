#include "MemoryRead.h"
#ifndef __MEMORYREAD_DEBUGMENUHANDLER_H__
#define __MEMORYREAD_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class MemoryRead::DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_MEMORYEDITOR
	};

	//Constructors
	DebugMenuHandler(MemoryRead* adevice);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	MemoryRead* device;
};

#endif
