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
		MENUITEM_INPUTMAPPING,
		MENUITEM_INPUTMAPPINGDETAILS
	};

	//Constructors
	SystemMenuHandler(System* adevice);

	//Window functions
	void OpenLoggerDetailsView(const LogEntryInternal& alogEntry);
	void OpenInputMappingDetailsView(IDevice* targetDevice);
	void CloseInputMappingDetailsView();

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
