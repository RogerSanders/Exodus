#ifndef __IMENUSUBMENU_H__
#define __IMENUSUBMENU_H__
#include "MarshalSupport/MarshalSupport.pkg"
#include "IMenuItem.h"
#include <string>
#include <list>
class IMenuSegment;
class IMenuSelectableOption;
class IMenuHandler;

class IMenuSubmenu :public IMenuItem
{
public:
	//Interface version functions
	static inline unsigned int ThisIMenuSubmenuVersion() { return 1; }
	virtual unsigned int GetIMenuSubmenuVersion() const = 0;

	//Menu title functions
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetMenuTitle() const = 0;

	//Item management functions
	virtual bool NoMenuItemsExist() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<IMenuItem*>> GetMenuItems() const = 0;

	//Menu item creation and deletion
	virtual IMenuSegment& AddMenuItemSegment(bool asurroundWithSeparators = true, IMenuSegment::SortMode sortMode = IMenuSegment::SORTMODE_ADDITIONORDER) = 0;
	virtual IMenuSubmenu& AddMenuItemSubmenu(const MarshalSupport::Marshal::In<std::wstring>& title) = 0;
	virtual IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const MarshalSupport::Marshal::In<std::wstring>& title) = 0;
	virtual void DeleteMenuItem(IMenuItem& menuItem) = 0;
	virtual void DeleteAllMenuItems() = 0;
};

#endif
