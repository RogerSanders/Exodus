#ifndef __MENUSEGMENT_H__
#define __MENUSEGMENT_H__
#include "SystemInterface/SystemInterface.pkg"
#include <vector>

class MenuSegment :public IMenuSegment
{
public:
	//Constructors
	MenuSegment();
	~MenuSegment();

	//Item management functions
	virtual bool NoMenuItemsExist() const;

	//Menu item creation and deletion
	virtual IMenuSeparator& AddMenuItemSeparator();
	virtual IMenuSubmenu& AddMenuItemSubmenu(const wchar_t* name);
	virtual IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const wchar_t* name);
	virtual void DeleteMenuItem(IMenuItem& menuItem);
	virtual void DeleteAllMenuItems();

protected:
	//Item management functions
	virtual IMenuItem** GetMenuItemList(unsigned int& itemCount);

private:
	std::vector<IMenuItem*> menuItems;
};

#endif
