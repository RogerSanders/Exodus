#ifndef __MENUSUBMENU_H__
#define __MENUSUBMENU_H__
#include "ExtensionInterface/ExtensionInterface.pkg"
#include <string>
#include <vector>

class MenuSubmenu :public IMenuSubmenu
{
public:
	// Constructors
	MenuSubmenu(const std::wstring& title);
	~MenuSubmenu();

	// Interface version functions
	virtual unsigned int GetIMenuItemVersion() const;
	virtual unsigned int GetIMenuSubmenuVersion() const;

	// Type functions
	virtual Type GetType() const;

	// Menu title functions
	virtual Marshal::Ret<std::wstring> GetMenuTitle() const;

	// Item management functions
	virtual bool NoMenuItemsExist() const;
	Marshal::Ret<std::list<IMenuItem*>> GetMenuItems() const;

	// Menu item creation and deletion
	virtual IMenuSegment& AddMenuItemSegment(bool surroundWithSeparators = true, IMenuSegment::SortMode sortMode = IMenuSegment::SORTMODE_ADDITIONORDER);
	virtual IMenuSubmenu& AddMenuItemSubmenu(const Marshal::In<std::wstring>& title);
	virtual IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const Marshal::In<std::wstring>& title);
	virtual void DeleteMenuItem(IMenuItem& menuItem);
	virtual void DeleteAllMenuItems();

private:
	std::wstring _title;
	std::list<IMenuItem*> _menuItems;
};

#endif
