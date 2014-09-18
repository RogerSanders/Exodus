#ifndef __IMENUSUBMENU_H__
#define __IMENUSUBMENU_H__
#include "InteropSupport/InteropSupport.pkg"
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
	static inline unsigned int ThisIMenuSubmenuVersion();
	virtual unsigned int GetIMenuSubmenuVersion() const = 0;

	//Menu title functions
	inline std::wstring GetMenuTitle() const;

	//Item management functions
	virtual bool NoMenuItemsExist() const = 0;
	inline std::list<IMenuItem*> GetMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSegment& AddMenuItemSegment(bool asurroundWithSeparators = true, IMenuSegment::SortMode sortMode = IMenuSegment::SORTMODE_ADDITIONORDER) = 0;
	inline IMenuSubmenu& AddMenuItemSubmenu(const std::wstring& title);
	inline IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& title);
	virtual void DeleteMenuItem(IMenuItem& menuItem) = 0;
	virtual void DeleteAllMenuItems() = 0;

protected:
	//Menu title functions
	virtual void GetMenuTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Item management functions
	virtual void GetMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const = 0;

	//Menu item creation and deletion
	virtual IMenuSubmenu& AddMenuItemSubmenuInternal(const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller) = 0;
	virtual IMenuSelectableOption& AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller) = 0;
};

#include "IMenuSubmenu.inl"
#endif
