#include "IMenuHandler.h"
#include "IMenuSeparator.h"
#include "IMenuSubmenu.h"
#include "IMenuSelectableOption.h"

//----------------------------------------------------------------------------------------
//Item management functions
//----------------------------------------------------------------------------------------
std::list<IMenuItem*> IMenuSegment::GetMenuItems()
{
	std::list<IMenuItem*> list;
	unsigned int itemCount;
	IMenuItem** itemBuffer = GetMenuItemList(itemCount);
	for(unsigned int i = 0; i < itemCount; ++i)
	{
		list.push_back(*(itemBuffer + i));
	}
	return list;
}

//----------------------------------------------------------------------------------------
//Menu item creation and deletion
//----------------------------------------------------------------------------------------
IMenuSubmenu& IMenuSegment::AddMenuItemSubmenu(const std::wstring& name)
{
	return AddMenuItemSubmenu(name.c_str());
}

//----------------------------------------------------------------------------------------
IMenuSelectableOption& IMenuSegment::AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& name)
{
	return AddMenuItemSelectableOption(menuHandler, menuItemID, name.c_str());
}
