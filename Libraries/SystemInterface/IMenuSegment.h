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

	//Interface version functions
	static inline unsigned int ThisIMenuSegmentVersion();
	virtual unsigned int GetIMenuSegmentVersion() const = 0;

	//Item management functions
	virtual bool NoMenuItemsExist() const = 0;
	inline std::list<IMenuItem*> GetMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSeparator& AddMenuItemSeparator() = 0;
	inline IMenuSubmenu& AddMenuItemSubmenu(const std::wstring& name);
	inline IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& name);
	virtual void DeleteMenuItem(IMenuItem& menuItem) = 0;
	virtual void DeleteAllMenuItems() = 0;

protected:
	//Item management functions
	virtual void GetMenuItemsInternal(IMenuItem* itemArray[], unsigned int arraySize, unsigned int& requiredSize, bool& itemsRetrieved) const = 0;

	//Menu item creation and deletion
	virtual IMenuSubmenu& AddMenuItemSubmenuInternal(const wchar_t* name) = 0;
	virtual IMenuSelectableOption& AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const wchar_t* name) = 0;
};

#include "IMenuSegment.inl"
#endif
