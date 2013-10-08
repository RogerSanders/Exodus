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
void MenuSegment::GetMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const
{
	marshaller.MarshalFrom(GetMenuItems());
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
IMenuSubmenu& MenuSegment::AddMenuItemSubmenuInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller)
{
	return AddMenuItemSubmenu(nameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& MenuSegment::AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller)
{
	return AddMenuItemSelectableOption(menuHandler, menuItemID, nameMarshaller.MarshalTo());
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
