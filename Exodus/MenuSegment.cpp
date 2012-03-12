#include "MenuSegment.h"
#include "MenuSeparator.h"
#include "MenuSelectableOption.h"
#include "MenuSubmenu.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuSegment::MenuSegment()
{}

//----------------------------------------------------------------------------------------
MenuSegment::~MenuSegment()
{
	for(std::vector<IMenuItem*>::const_iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		delete *i;
	}
	menuItems.clear();
}

//----------------------------------------------------------------------------------------
//Item management functions
//----------------------------------------------------------------------------------------
bool MenuSegment::NoMenuItemsExist() const
{
	return menuItems.empty();
}

//----------------------------------------------------------------------------------------
IMenuItem** MenuSegment::GetMenuItemList(unsigned int& itemCount)
{
	itemCount = (unsigned int)menuItems.size();
	if(itemCount > 0)
	{
		return (IMenuItem**)&menuItems[0];
	}
	return (IMenuItem**)0;
}

//----------------------------------------------------------------------------------------
//Menu item creation functions
//----------------------------------------------------------------------------------------
IMenuSeparator& MenuSegment::AddMenuItemSeparator()
{
	IMenuSeparator* newMenuItem = new MenuSeparator();
	menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
IMenuSubmenu& MenuSegment::AddMenuItemSubmenu(const wchar_t* name)
{
	IMenuSubmenu* newMenuItem = new MenuSubmenu(name);
	menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& MenuSegment::AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const wchar_t* name)
{
	IMenuSelectableOption* newMenuItem = new MenuSelectableOption(menuHandler, menuItemID, name);
	menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
void MenuSegment::DeleteMenuItem(IMenuItem& menuItem)
{
	IMenuItem* menuItemPointer = &menuItem;
	bool done = false;
	std::vector<IMenuItem*>::iterator i = menuItems.begin();
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
	for(std::vector<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		delete *i;
	}
	menuItems.clear();
}
