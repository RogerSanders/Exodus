#include "System.h"
#ifndef __SYSTEM_SYSTEMMENUHANDLER_H__
#define __SYSTEM_SYSTEMMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class System::SystemMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_LOGGER,
		MENUITEM_LOGGERDETAILS,
	};

	//Constructors
	SystemMenuHandler(System* adevice);

	//Window functions
	void OpenLoggerDetailsView(const LogEntryInternal& alogEntry);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	System* device;
};

#endif
