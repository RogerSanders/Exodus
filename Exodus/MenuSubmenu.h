#ifndef __MENUSUBMENU_H__
#define __MENUSUBMENU_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include <string>
#include <vector>

class MenuSubmenu :public IMenuSubmenu
{
public:
	//Constructors
	MenuSubmenu(const std::wstring& atitle);
	~MenuSubmenu();

	//Interface version functions
	virtual unsigned int GetIMenuItemVersion() const;
	virtual unsigned int GetIMenuSubmenuVersion() const;

	//Type functions
	virtual Type GetType() const;

	//Menu title functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetMenuTitle() const;

	//Item management functions
	virtual bool NoMenuItemsExist() const;
	MarshalSupport::Marshal::Ret<std::list<IMenuItem*>> GetMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSegment& AddMenuItemSegment(bool asurroundWithSeparators = true, IMenuSegment::SortMode sortMode = IMenuSegment::SORTMODE_ADDITIONORDER);
	virtual IMenuSubmenu& AddMenuItemSubmenu(const MarshalSupport::Marshal::In<std::wstring>& title);
	virtual IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const MarshalSupport::Marshal::In<std::wstring>& title);
	virtual void DeleteMenuItem(IMenuItem& menuItem);
	virtual void DeleteAllMenuItems();

private:
	std::wstring title;
	std::list<IMenuItem*> menuItems;
};

#endif
