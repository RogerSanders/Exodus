#ifndef __IMENUSEGMENT_H__
#define __IMENUSEGMENT_H__
class IMenuItem;
class IMenuSeparator;
class IMenuSubmenu;
class IMenuSelectableOption;
class IMenuHandler;
#include <list>

class IMenuSegment
{
public:
	//Constructors
	virtual ~IMenuSegment() = 0 {}

	//Item management functions
	virtual bool NoMenuItemsExist() const = 0;
	inline std::list<IMenuItem*> GetMenuItems();

	//Menu item creation and deletion
	virtual IMenuSeparator& AddMenuItemSeparator() = 0;
	inline IMenuSubmenu& AddMenuItemSubmenu(const std::wstring& name);
	virtual IMenuSubmenu& AddMenuItemSubmenu(const wchar_t* name) = 0;
	inline IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& name);
	virtual IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const wchar_t* name) = 0;
	virtual void DeleteMenuItem(IMenuItem& menuItem) = 0;
	virtual void DeleteAllMenuItems() = 0;

protected:
	//Item management functions
	virtual IMenuItem** GetMenuItemList(unsigned int& itemCount) = 0;
};

#include "IMenuSegment.inl"
#endif
