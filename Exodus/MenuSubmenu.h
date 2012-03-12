#ifndef __MENUSUBMENU_H__
#define __MENUSUBMENU_H__
#include "SystemInterface/SystemInterface.pkg"
#include <string>
#include <vector>

class MenuSubmenu :public IMenuSubmenu
{
public:
	//Constructors
	MenuSubmenu(const std::wstring& aname);
	~MenuSubmenu();

	//Type functions
	virtual Type GetType() const;

	//Item management functions
	virtual bool NoMenuSegmentsExist() const;
	virtual bool NoMenuItemsExist() const;

	//Menu segment creation and deletion
	virtual IMenuSegment& CreateMenuSegment();
	virtual void DeleteMenuSegment(IMenuSegment& menuSegment);
	virtual void DeleteAllMenuSegments();

protected:
	//Menu name functions
	virtual wchar_t* GetMenuNameInternal() const;

	//Item management functions
	virtual IMenuSegment** GetMenuSegmentList(unsigned int& itemCount);

private:
	mutable std::wstring name;
	std::vector<IMenuSegment*> menuSegments;
};

#endif
