#include "MenuSegment.h"
#include "MenuSubmenu.h"
#include "MenuSelectableOption.h"
#include <map>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuSegment::MenuSegment(bool asurroundWithSeparators, SortMode asortMode)
:surroundWithSeparators(asurroundWithSeparators), sortMode(asortMode)
{}

//----------------------------------------------------------------------------------------
MenuSegment::~MenuSegment()
{
	for(std::list<IMenuItem*>::const_iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		delete *i;
	}
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int MenuSegment::GetIMenuItemVersion() const
{
	return ThisIMenuItemVersion();
}

//----------------------------------------------------------------------------------------
unsigned int MenuSegment::GetIMenuSegmentVersion() const
{
	return ThisIMenuSegmentVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
MenuSegment::Type MenuSegment::GetType() const
{
	return Type::Segment;
}

//----------------------------------------------------------------------------------------
//Menu title functions
//----------------------------------------------------------------------------------------
std::wstring MenuSegment::GetMenuSortTitle() const
{
	//If we have no child menu items, return an empty string.
	if(menuItems.empty())
	{
		return L"";
	}

	//Return the title of the first child menu item to the caller
	IMenuItem* firstChildMenuItem = menuItems.front();
	std::wstring firstChildMenuItemTitle;
	switch(firstChildMenuItem->GetType())
	{
	case Type::Segment:
		firstChildMenuItemTitle = ((IMenuSegment*)firstChildMenuItem)->GetMenuSortTitle();
		break;
	case Type::SelectableOption:
		firstChildMenuItemTitle = ((IMenuSelectableOption*)firstChildMenuItem)->GetMenuTitle();
		break;
	case Type::SubMenu:
		firstChildMenuItemTitle = ((IMenuSubmenu*)firstChildMenuItem)->GetMenuTitle();
		break;
	}
	return firstChildMenuItemTitle;
}

//----------------------------------------------------------------------------------------
void MenuSegment::GetMenuSortTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetMenuSortTitle());
}

//----------------------------------------------------------------------------------------
//Sort mode functions
//----------------------------------------------------------------------------------------
MenuSegment::SortMode MenuSegment::GetSortMode() const
{
	return sortMode;
}

//----------------------------------------------------------------------------------------
//Separator functions
//----------------------------------------------------------------------------------------
bool MenuSegment::GetSurroundWithSeparators() const
{
	return surroundWithSeparators;
}

//----------------------------------------------------------------------------------------
//Item management functions
//----------------------------------------------------------------------------------------
bool MenuSegment::NoMenuItemsExist() const
{
	return menuItems.empty();
}

//----------------------------------------------------------------------------------------
std::list<IMenuItem*> MenuSegment::GetMenuItems() const
{
	return menuItems;
}

//----------------------------------------------------------------------------------------
void MenuSegment::GetMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const
{
	marshaller.MarshalFrom(GetMenuItems());
}

//----------------------------------------------------------------------------------------
std::list<IMenuItem*> MenuSegment::GetSortedMenuItems() const
{
	//If the menu items in this segment are sorted based on the order items were added,
	//return the actual menu item list directly here, and abort any further processing.
	if(sortMode == SORTMODE_ADDITIONORDER)
	{
		return menuItems;
	}

	//If the menu items in this segment are sorted based on their title, build a container
	//to sort each menu item entry based on their titles.
	std::map<std::wstring, IMenuItem*> menuItemSortContainer;
	for(std::list<IMenuItem*>::const_iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		IMenuItem* childMenuItem = *i;
		std::wstring childMenuItemTitle;
		switch(childMenuItem->GetType())
		{
		case Type::Segment:
			childMenuItemTitle = ((IMenuSegment*)childMenuItem)->GetMenuSortTitle();
			break;
		case Type::SelectableOption:
			childMenuItemTitle = ((IMenuSelectableOption*)childMenuItem)->GetMenuTitle();
			break;
		case Type::SubMenu:
			childMenuItemTitle = ((IMenuSubmenu*)childMenuItem)->GetMenuTitle();
			break;
		}
		menuItemSortContainer.insert(std::pair<std::wstring, IMenuItem*>(childMenuItemTitle, childMenuItem));
	}

	//Build and return a list of each item in this menu, sorted based on the specified
	//sort order.
	std::list<IMenuItem*> sortedMenuItems;
	for(std::map<std::wstring, IMenuItem*>::const_iterator i = menuItemSortContainer.begin(); i != menuItemSortContainer.end(); ++i)
	{
		sortedMenuItems.push_back(i->second);
	}
	return sortedMenuItems;
}

//----------------------------------------------------------------------------------------
void MenuSegment::GetSortedMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const
{
	marshaller.MarshalFrom(GetSortedMenuItems());
}

//----------------------------------------------------------------------------------------
//Menu item creation functions
//----------------------------------------------------------------------------------------
IMenuSegment& MenuSegment::AddMenuItemSegment(bool asurroundWithSeparators, IMenuSegment::SortMode sortMode)
{
	IMenuSegment* newMenuItem = new MenuSegment(asurroundWithSeparators, sortMode);
	menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
IMenuSubmenu& MenuSegment::AddMenuItemSubmenu(const std::wstring& title)
{
	IMenuSubmenu* newMenuItem = new MenuSubmenu(title);
	menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& MenuSegment::AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& title)
{
	IMenuSelectableOption* newMenuItem = new MenuSelectableOption(menuHandler, menuItemID, title);
	menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
IMenuSubmenu& MenuSegment::AddMenuItemSubmenuInternal(const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller)
{
	return AddMenuItemSubmenu(titleMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& MenuSegment::AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller)
{
	return AddMenuItemSelectableOption(menuHandler, menuItemID, titleMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void MenuSegment::DeleteMenuItem(IMenuItem& menuItem)
{
	IMenuItem* menuItemPointer = &menuItem;
	bool done = false;
	std::list<IMenuItem*>::iterator i = menuItems.begin();
	while(!done && (i != menuItems.end()))
	{
		if(*i == menuItemPointer)
		{
			menuItems.erase(i);
			delete menuItemPointer;
			done = true;
			continue;
		}
		++i;
	}
}

//----------------------------------------------------------------------------------------
void MenuSegment::DeleteAllMenuItems()
{
	for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		delete *i;
	}
	menuItems.clear();
}
