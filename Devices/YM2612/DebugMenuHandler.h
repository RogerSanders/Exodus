#include "YM2612.h"
#ifndef __YM2612_DEBUGMENUHANDLER_H__
#define __YM2612_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class YM2612::DebugMenuHandler :public MenuHandlerBase
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

	//Constructors
	DebugMenuHandler(YM2612* adevice);

	//Window functions
	void OpenOperatorView(unsigned int achannelNo, unsigned int aoperatorNo);

protected:
	//Management functions
	virtual std::wstring GetMenuHandlerName() const;
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	YM2612* device;
};

#endif
