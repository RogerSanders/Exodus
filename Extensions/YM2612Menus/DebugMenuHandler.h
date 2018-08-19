#ifndef __DEBUGMENUHANDLER_H__
#define __DEBUGMENUHANDLER_H__
#include "DeviceInterface/DeviceInterface.pkg"
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
		MENUITEM_REGISTERS
	};

public:
	//Constructors
	DebugMenuHandler(YM2612Menus& owner, const IDevice& modelInstanceKey, IYM2612& model);

	//Window functions
	void OpenOperatorView(unsigned int channelNo, unsigned int operatorNo);

protected:
	//Management functions
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName);
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter);

private:
	YM2612Menus& _owner;
	const IDevice& _modelInstanceKey;
	IYM2612& _model;
};

#endif
