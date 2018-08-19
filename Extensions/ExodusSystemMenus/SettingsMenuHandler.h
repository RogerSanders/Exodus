#ifndef __SETTINGSMENUHANDLER_H__
#define __SETTINGSMENUHANDLER_H__
#include "ExtensionInterface/ExtensionInterface.pkg"
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
	SettingsMenuHandler(ExodusSystemMenus& owner, ISystemGUIInterface& model);

	//Window functions
	void OpenInputMappingDetailsView(IDevice* targetDevice);
	void CloseInputMappingDetailsView();

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	ExodusSystemMenus& _owner;
	ISystemGUIInterface& _model;
};

#endif
