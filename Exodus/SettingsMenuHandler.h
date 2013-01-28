#include "System.h"
#ifndef __SYSTEM_SETTINGSMENUHANDLER_H__
#define __SYSTEM_SETTINGSMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class System::SettingsMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_INPUTMAPPING,
		MENUITEM_INPUTMAPPINGDETAILS,
		MENUITEM_EMBEDDEDROM
	};

	//Constructors
	SettingsMenuHandler(System* adevice);

	//Window functions
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
