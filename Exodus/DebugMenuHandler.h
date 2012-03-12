#include "System.h"
#ifndef __SYSTEM_DEBUGMENUHANDLER_H__
#define __SYSTEM_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class System::DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_LOGGER,
		MENUITEM_LOGGERDETAILS,
		MENUITEM_DEVICECONTROL
	};

	//Constructors
	DebugMenuHandler(System* adevice);

	//Window functions
	void OpenLoggerDetailsView(const LogEntryInternal& alogEntry);

protected:
	//Management functions
	virtual std::wstring GetMenuHandlerName() const;
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	System* device;
};

#endif
