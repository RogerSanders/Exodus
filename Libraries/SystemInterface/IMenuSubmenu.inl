#include "IMenuSegment.h"

//----------------------------------------------------------------------------------------
//Menu name functions
//----------------------------------------------------------------------------------------
std::wstring IMenuSubmenu::GetMenuName() const
{
	return GetMenuNameInternal();
}

//----------------------------------------------------------------------------------------
//Menu segment functions
//----------------------------------------------------------------------------------------
std::list<IMenuSegment*> IMenuSubmenu::GetMenuSegments()
{
	std::list<IMenuSegment*> list;
	unsigned int itemCount;
	IMenuSegment** itemBuffer = GetMenuSegmentList(itemCount);
	for(unsigned int i = 0; i < itemCount; ++i)
	{
		list.push_back(*(itemBuffer + i));
	}
	return list;
}
