#include "M68000.h"
#ifndef __M68000_DEBUGMENUHANDLER_H__
#define __M68000_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"
namespace M68000{

class M68000::DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_REGISTERS,
		MENUITEM_EXCEPTIONS
	};

	//Constructors
	DebugMenuHandler(M68000* adevice);

protected:
	//Management functions
	virtual std::wstring GetMenuHandlerName() const;
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	M68000* device;
};

} //Close namespace M68000
#endif
