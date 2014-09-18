#ifndef __SETTINGSMENUHANDLER_H__
#define __SETTINGSMENUHANDLER_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include "ExodusSystemMenus.h"

class SettingsMenuHandler :public MenuHandlerBase
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
	SettingsMenuHandler(ExodusSystemMenus& aowner, ISystemGUIInterface& amodel);

	//Window functions
	void OpenInputMappingDetailsView(IDevice* targetDevice);
	void CloseInputMappingDetailsView();

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	ExodusSystemMenus& owner;
	ISystemGUIInterface& model;
};

#endif
