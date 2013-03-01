#ifndef __MENUSEGMENT_H__
#define __MENUSEGMENT_H__
#include "SystemInterface/SystemInterface.pkg"
#include <vector>

class MenuSegment :public IMenuSegment
{
public:
	//Constructors
	MenuSegment();
	virtual ~MenuSegment();

	//Interface version functions
	virtual unsigned int GetIMenuSegmentVersion() const;

	//Item management functions
	virtual bool NoMenuItemsExist() const;
	std::list<IMenuItem*> GetMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSeparator& AddMenuItemSeparator();
	IMenuSubmenu& AddMenuItemSubmenu(const std::wstring& name);
	IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& name);
	virtual void DeleteMenuItem(IMenuItem& menuItem);
	virtual void DeleteAllMenuItems();

protected:
	//Item management functions
	virtual void GetMenuItemsInternal(IMenuItem* itemArray[], unsigned int arraySize, unsigned int& requiredSize, bool& itemsRetrieved) const;

	//Menu item creation and deletion
	virtual IMenuSubmenu& AddMenuItemSubmenuInternal(const wchar_t* name);
	virtual IMenuSelectableOption& AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const wchar_t* name);

private:
	std::vector<IMenuItem*> menuItems;
};

#endif
