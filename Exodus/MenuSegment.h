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
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetMenuSortTitle() const;

	//Sort mode functions
	virtual SortMode GetSortMode() const;

	//Separator functions
	virtual bool GetSurroundWithSeparators() const;

	//Item management functions
	virtual bool NoMenuItemsExist() const;
	virtual MarshalSupport::Marshal::Ret<std::list<IMenuItem*>> GetMenuItems() const;
	virtual MarshalSupport::Marshal::Ret<std::list<IMenuItem*>> GetSortedMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSegment& AddMenuItemSegment(bool asurroundWithSeparators = true, IMenuSegment::SortMode sortMode = IMenuSegment::SORTMODE_ADDITIONORDER);
	virtual IMenuSubmenu& AddMenuItemSubmenu(const MarshalSupport::Marshal::In<std::wstring>& title);
	virtual IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const MarshalSupport::Marshal::In<std::wstring>& title);
	virtual void DeleteMenuItem(IMenuItem& menuItem);
	virtual void DeleteAllMenuItems();

private:
	bool surroundWithSeparators;
	SortMode sortMode;
	std::list<IMenuItem*> menuItems;
};

#endif
