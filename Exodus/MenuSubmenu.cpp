#include "MenuSubmenu.h"
#include "MenuSegment.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuSubmenu::MenuSubmenu(const std::wstring& aname)
:name(aname)
{}

//----------------------------------------------------------------------------------------
MenuSubmenu::~MenuSubmenu()
{
	for(std::vector<IMenuSegment*>::const_iterator i = menuSegments.begin(); i != menuSegments.end(); ++i)
	{
		delete *i;
	}
	menuSegments.clear();
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
IMenuSubmenu::Type MenuSubmenu::GetType() const
{
	return TYPE_SUBMENU;
}

//----------------------------------------------------------------------------------------
//Menu name functions
//----------------------------------------------------------------------------------------
std::wstring MenuSubmenu::GetMenuName() const
{
	return name;
}

//----------------------------------------------------------------------------------------
const wchar_t* MenuSubmenu::GetMenuNameInternal() const
{
	return name.c_str();
}

//----------------------------------------------------------------------------------------
//Item management functions
//----------------------------------------------------------------------------------------
bool MenuSubmenu::NoMenuSegmentsExist() const
{
	return menuSegments.empty();
}

//----------------------------------------------------------------------------------------
bool MenuSubmenu::NoMenuItemsExist() const
{
	bool noMenuItems = true;
	for(std::vector<IMenuSegment*>::const_iterator i = menuSegments.begin(); i != menuSegments.end(); ++i)
	{
		noMenuItems &= (*i)->NoMenuItemsExist();
	}
	return noMenuItems;
}

//----------------------------------------------------------------------------------------
std::list<IMenuSegment*> MenuSubmenu::GetMenuSegments() const
{
	std::list<IMenuSegment*> menuSegmentList;
	for(unsigned int i = 0; i < (unsigned int)menuSegments.size(); ++i)
	{
		menuSegmentList.push_back(menuSegments[i]);
	}
	return menuSegmentList;
}

//----------------------------------------------------------------------------------------
void MenuSubmenu::GetMenuSegmentsInternal(IMenuSegment* itemArray[], unsigned int arraySize, unsigned int& requiredSize, bool& itemsRetrieved) const
{
	//Ensure that the supplied array is big enough to hold all the items
	requiredSize = (unsigned int)menuSegments.size();
	if(requiredSize > arraySize)
	{
		itemsRetrieved = false;
		return;
	}

	//Write all the items to the array
	unsigned int arrayIndex = 0;
	for(unsigned int i = 0; i < (unsigned int)menuSegments.size(); ++i)
	{
		itemArray[arrayIndex++] = menuSegments[i];
	}
	itemsRetrieved = true;
}

//----------------------------------------------------------------------------------------
//Menu segment creation and deletion
//----------------------------------------------------------------------------------------
IMenuSegment& MenuSubmenu::CreateMenuSegment()
{
	IMenuSegment* newMenuSegment = new MenuSegment();
	menuSegments.push_back(newMenuSegment);
	return *newMenuSegment;
}

//----------------------------------------------------------------------------------------
void MenuSubmenu::DeleteMenuSegment(IMenuSegment& menuSegment)
{
	IMenuSegment* menuSegmentPointer = &menuSegment;
	bool done = false;
	std::vector<IMenuSegment*>::iterator i = menuSegments.begin();
	while(!done && (i != menuSegments.end()))
	{
		if(*i == menuSegmentPointer)
		{
			menuSegments.erase(i);
			delete menuSegmentPointer;
			done = true;
			continue;
		}
		++i;
	}
}

//----------------------------------------------------------------------------------------
void MenuSubmenu::DeleteAllMenuSegments()
{
	for(std::vector<IMenuSegment*>::iterator i = menuSegments.begin(); i != menuSegments.end(); ++i)
	{
		delete *i;
	}
	menuSegments.clear();
}
