#ifndef __MENUSEGMENT_H__
#define __MENUSEGMENT_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include <vector>

class MenuSegment :public IMenuSegment
{
public:
	//Constructors
	MenuSegment(bool asurroundWithSeparators, SortMode asortMode);
	virtual ~MenuSegment();

	//Interface version functions
	virtual unsigned int GetIMenuItemVersion() const;
	virtual unsigned int GetIMenuSegmentVersion() const;

	//Type functions
	virtual Type GetType() const;

	//Menu title functions
	std::wstring GetMenuSortTitle() const;

	//Sort mode functions
	virtual SortMode GetSortMode() const;

	//Separator functions
	virtual bool GetSurroundWithSeparators() const;

	//Item management functions
	virtual bool NoMenuItemsExist() const;
	std::list<IMenuItem*> GetMenuItems() const;
	std::list<IMenuItem*> GetSortedMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSegment& AddMenuItemSegment(bool asurroundWithSeparators = true, IMenuSegment::SortMode sortMode = IMenuSegment::SORTMODE_ADDITIONORDER);
	IMenuSubmenu& AddMenuItemSubmenu(const std::wstring& title);
	IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& title);
	virtual void DeleteMenuItem(IMenuItem& menuItem);
	virtual void DeleteAllMenuItems();

protected:
	//Menu title functions
	virtual void GetMenuSortTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Item management functions
	virtual void GetMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const;
	virtual void GetSortedMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const;

	//Menu item creation and deletion
	virtual IMenuSubmenu& AddMenuItemSubmenuInternal(const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller);
	virtual IMenuSelectableOption& AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller);

private:
	bool surroundWithSeparators;
	SortMode sortMode;
	std::list<IMenuItem*> menuItems;
};

#endif
