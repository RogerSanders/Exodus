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
void MenuSubmenu::GetMenuNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetMenuName());
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
void MenuSubmenu::GetMenuSegmentsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuSegment*>>& marshaller) const
{
	marshaller.MarshalFrom(GetMenuSegments());
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
