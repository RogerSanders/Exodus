#include "SN76489.h"
#ifndef __SN76489_DEBUGMENUHANDLER_H__
#define __SN76489_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class SN76489::DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_REGISTERS,
		MENUITEM_PROPERTIES,
		MENUITEM_WAV_FILE_LOGGING
	};

	//Constructors
	DebugMenuHandler(SN76489* adevice);

protected:
	//Management functions
	virtual std::wstring GetMenuHandlerName() const;
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	SN76489* device;
};

#endif
