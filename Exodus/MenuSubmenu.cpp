#include "MenuSubmenu.h"
#include "MenuSegment.h"
#include "MenuSelectableOption.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuSubmenu::MenuSubmenu(const std::wstring& title)
:_title(title)
{}

//----------------------------------------------------------------------------------------
MenuSubmenu::~MenuSubmenu()
{
	for (std::list<IMenuItem*>::const_iterator i = _menuItems.begin(); i != _menuItems.end(); ++i)
	{
		delete *i;
	}
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int MenuSubmenu::GetIMenuItemVersion() const
{
	return ThisIMenuItemVersion();
}

//----------------------------------------------------------------------------------------
unsigned int MenuSubmenu::GetIMenuSubmenuVersion() const
{
	return ThisIMenuSubmenuVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
MenuSubmenu::Type MenuSubmenu::GetType() const
{
	return Type::SubMenu;
}

//----------------------------------------------------------------------------------------
//Menu title functions
//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> MenuSubmenu::GetMenuTitle() const
{
	return _title;
}

//----------------------------------------------------------------------------------------
//Item management functions
//----------------------------------------------------------------------------------------
bool MenuSubmenu::NoMenuItemsExist() const
{
	for (std::list<IMenuItem*>::const_iterator i = _menuItems.begin(); i != _menuItems.end(); ++i)
	{
		const IMenuItem* menuItem = *i;
		IMenuItem::Type menuItemType = menuItem->GetType();
		if (menuItemType == IMenuItem::Type::Segment)
		{
			const IMenuSegment* menuItemAsSegment = (IMenuSegment*)menuItem;
			if (!menuItemAsSegment->NoMenuItemsExist())
			{
				return false;
			}
		}
		else if (menuItemType == IMenuItem::Type::SubMenu)
		{
			const IMenuSubmenu* menuItemAsSubmenu = (IMenuSubmenu*)menuItem;
			if (!menuItemAsSubmenu->NoMenuItemsExist())
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
Marshal::Ret<std::list<IMenuItem*>> MenuSubmenu::GetMenuItems() const
{
	return _menuItems;
}

//----------------------------------------------------------------------------------------
//Menu item creation functions
//----------------------------------------------------------------------------------------
IMenuSegment& MenuSubmenu::AddMenuItemSegment(bool surroundWithSeparators, IMenuSegment::SortMode sortMode)
{
	IMenuSegment* newMenuItem = new MenuSegment(surroundWithSeparators, sortMode);
	_menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
IMenuSubmenu& MenuSubmenu::AddMenuItemSubmenu(const Marshal::In<std::wstring>& title)
{
	IMenuSubmenu* newMenuItem = new MenuSubmenu(title);
	_menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& MenuSubmenu::AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const Marshal::In<std::wstring>& title)
{
	IMenuSelectableOption* newMenuItem = new MenuSelectableOption(menuHandler, menuItemID, title);
	_menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
void MenuSubmenu::DeleteMenuItem(IMenuItem& menuItem)
{
	IMenuItem* menuItemPointer = &menuItem;
	bool done = false;
	std::list<IMenuItem*>::iterator i = _menuItems.begin();
	while (!done && (i != _menuItems.end()))
	{
		if (*i == menuItemPointer)
		{
			_menuItems.erase(i);
			delete menuItemPointer;
			done = true;
			continue;
		}
		++i;
	}
}

//----------------------------------------------------------------------------------------
void MenuSubmenu::DeleteAllMenuItems()
{
	for (std::list<IMenuItem*>::iterator i = _menuItems.begin(); i != _menuItems.end(); ++i)
	{
		delete *i;
	}
	_menuItems.clear();
}
