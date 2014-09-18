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
	std::wstring GetMenuTitle() const;

	//Item management functions
	virtual bool NoMenuItemsExist() const;
	std::list<IMenuItem*> GetMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSegment& AddMenuItemSegment(bool asurroundWithSeparators = true, IMenuSegment::SortMode sortMode = IMenuSegment::SORTMODE_ADDITIONORDER);
	inline IMenuSubmenu& AddMenuItemSubmenu(const std::wstring& title);
	inline IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& title);
	virtual void DeleteMenuItem(IMenuItem& menuItem);
	virtual void DeleteAllMenuItems();

protected:
	//Menu name functions
	virtual void GetMenuTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Item management functions
	virtual void GetMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const;

	//Menu item creation and deletion
	virtual IMenuSubmenu& AddMenuItemSubmenuInternal(const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller);
	virtual IMenuSelectableOption& AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller);

private:
	std::wstring title;
	std::list<IMenuItem*> menuItems;
};

#endif
