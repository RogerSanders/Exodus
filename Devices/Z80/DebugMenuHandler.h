#include "Z80.h"
#ifndef __Z80_DEBUGMENUHANDLER_H__
#define __Z80_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"
namespace Z80{

class Z80::DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_REGISTERS
	};

	//Constructors
	DebugMenuHandler(Z80* adevice);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	Z80* device;
};

} //Close namespace Z80
#endif
