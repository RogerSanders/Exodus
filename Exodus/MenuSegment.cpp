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
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int MenuSegment::GetIMenuSegmentVersion() const
{
	return ThisIMenuSegmentVersion();
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
	std::list<IMenuItem*> menuItemList;
	for(unsigned int i = 0; i < (unsigned int)menuItems.size(); ++i)
	{
		menuItemList.push_back(menuItems[i]);
	}
	return menuItemList;
}

//----------------------------------------------------------------------------------------
void MenuSegment::GetMenuItemsInternal(IMenuItem* itemArray[], unsigned int arraySize, unsigned int& requiredSize, bool& itemsRetrieved) const
{
	//Ensure that the supplied array is big enough to hold all the items
	requiredSize = (unsigned int)menuItems.size();
	if(requiredSize > arraySize)
	{
		itemsRetrieved = false;
		return;
	}

	//Write all the items to the array
	unsigned int arrayIndex = 0;
	for(unsigned int i = 0; i < (unsigned int)menuItems.size(); ++i)
	{
		itemArray[arrayIndex++] = menuItems[i];
	}
	itemsRetrieved = true;
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
IMenuSubmenu& MenuSegment::AddMenuItemSubmenu(const std::wstring& name)
{
	IMenuSubmenu* newMenuItem = new MenuSubmenu(name);
	menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& MenuSegment::AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& name)
{
	IMenuSelectableOption* newMenuItem = new MenuSelectableOption(menuHandler, menuItemID, name);
	menuItems.push_back(newMenuItem);
	return *newMenuItem;
}

//----------------------------------------------------------------------------------------
IMenuSubmenu& MenuSegment::AddMenuItemSubmenuInternal(const wchar_t* name)
{
	return AddMenuItemSubmenu(name);
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& MenuSegment::AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const wchar_t* name)
{
	return AddMenuItemSelectableOption(menuHandler, menuItemID, name);
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
