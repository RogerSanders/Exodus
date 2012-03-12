#ifndef __IMENUSUBMENU_H__
#define __IMENUSUBMENU_H__
#include "IMenuItem.h"
#include <string>
#include <list>
class IMenuSegment;

class IMenuSubmenu :public IMenuItem
{
public:
	//Menu name functions
	inline std::wstring GetMenuName() const;

	//Item management functions
	virtual bool NoMenuSegmentsExist() const = 0;
	virtual bool NoMenuItemsExist() const = 0;
	inline std::list<IMenuSegment*> GetMenuSegments();

	//Menu segment creation and deletion
	virtual IMenuSegment& CreateMenuSegment() = 0;
	virtual void DeleteMenuSegment(IMenuSegment& menuSegment) = 0;
	virtual void DeleteAllMenuSegments() = 0;

protected:
	//Menu name functions
	virtual wchar_t* GetMenuNameInternal() const = 0;

	//Item management functions
	virtual IMenuSegment** GetMenuSegmentList(unsigned int& itemCount) = 0;
};

#include "IMenuSubmenu.inl"
#endif
