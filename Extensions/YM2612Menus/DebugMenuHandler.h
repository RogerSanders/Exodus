#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "YM2612Menus.h"
#include "YM2612/IYM2612.h"

class DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_DEBUGGER,
		MENUITEM_OPERATOR,
		MENUITEM_REGISTERS,
		MENUITEM_WAV_FILE_LOGGING
	};

public:
	//Constructors
	DebugMenuHandler(YM2612Menus& aowner, const IDevice& amodelInstanceKey, IYM2612& amodel);

	//Window functions
	void OpenOperatorView(unsigned int achannelNo, unsigned int aoperatorNo);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	YM2612Menus& owner;
	const IDevice& modelInstanceKey;
	IYM2612& model;
};

#endif
