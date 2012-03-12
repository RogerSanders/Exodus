#include "MemoryRead.h"
#ifndef __MEMORYREAD_DEBUGMENUHANDLER_H__
#define __MEMORYREAD_DEBUGMENUHANDLER_H__
#include "SystemInterface/SystemInterface.pkg"

class MemoryRead::DebugMenuHandler :public MenuHandlerBase
{
public:
	//Enumerations
	enum MenuItem
	{
		MENUITEM_MEMORYEDITOR,
		MENUITEM_MEMORYVIEWEROLD,
		MENUITEM_MEMORYEDITOROLD
	};

	//Constructors
	DebugMenuHandler(MemoryRead* adevice);

	//Window functions
	void OpenOldMemoryEditorView(unsigned int atargetMemoryAddress);

protected:
	//Management functions
	virtual std::wstring GetMenuHandlerName() const;
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const;
	virtual IViewModel* CreateViewModelForItem(int menuItemID);
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel);

private:
	MemoryRead* device;
};

#endif
