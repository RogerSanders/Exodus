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
//Type functions
//----------------------------------------------------------------------------------------
IMenuSubmenu::Type MenuSubmenu::GetType() const
{
	return TYPE_SUBMENU;
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
IMenuSegment** MenuSubmenu::GetMenuSegmentList(unsigned int& itemCount)
{
	itemCount = (unsigned int)menuSegments.size();
	if(itemCount > 0)
	{
		return (IMenuSegment**)&menuSegments[0];
	}
	return (IMenuSegment**)0;
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

//----------------------------------------------------------------------------------------
//Menu name functions
//----------------------------------------------------------------------------------------
wchar_t* MenuSubmenu::GetMenuNameInternal() const
{
	return &name[0];
}
