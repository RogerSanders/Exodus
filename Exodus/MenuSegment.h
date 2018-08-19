#ifndef __MENUSEGMENT_H__
#define __MENUSEGMENT_H__
#include "ExtensionInterface/ExtensionInterface.pkg"
#include <vector>

class MenuSegment :public IMenuSegment
{
public:
	//Constructors
	MenuSegment(bool surroundWithSeparators, SortMode sortMode);
	virtual ~MenuSegment();

	//Interface version functions
	virtual unsigned int GetIMenuItemVersion() const;
	virtual unsigned int GetIMenuSegmentVersion() const;

	//Type functions
	virtual Type GetType() const;

	//Menu title functions
	virtual Marshal::Ret<std::wstring> GetMenuSortTitle() const;

	//Sort mode functions
	virtual SortMode GetSortMode() const;

	//Separator functions
	virtual bool GetSurroundWithSeparators() const;

	//Item management functions
	virtual bool NoMenuItemsExist() const;
	virtual Marshal::Ret<std::list<IMenuItem*>> GetMenuItems() const;
	virtual Marshal::Ret<std::list<IMenuItem*>> GetSortedMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSegment& AddMenuItemSegment(bool surroundWithSeparators = true, IMenuSegment::SortMode sortMode = IMenuSegment::SORTMODE_ADDITIONORDER);
	virtual IMenuSubmenu& AddMenuItemSubmenu(const Marshal::In<std::wstring>& title);
	virtual IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const Marshal::In<std::wstring>& title);
	virtual void DeleteMenuItem(IMenuItem& menuItem);
	virtual void DeleteAllMenuItems();

private:
	bool _surroundWithSeparators;
	SortMode _sortMode;
	std::list<IMenuItem*> _menuItems;
};

#endif
