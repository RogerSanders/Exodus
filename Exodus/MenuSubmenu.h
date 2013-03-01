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

	//Interface version functions
	virtual unsigned int GetIMenuItemVersion() const;
	virtual unsigned int GetIMenuSubmenuVersion() const;

	//Type functions
	virtual Type GetType() const;

	//Menu name functions
	std::wstring GetMenuName() const;

	//Item management functions
	virtual bool NoMenuSegmentsExist() const;
	virtual bool NoMenuItemsExist() const;
	std::list<IMenuSegment*> GetMenuSegments() const;

	//Menu segment creation and deletion
	virtual IMenuSegment& CreateMenuSegment();
	virtual void DeleteMenuSegment(IMenuSegment& menuSegment);
	virtual void DeleteAllMenuSegments();

protected:
	//Menu name functions
	virtual const wchar_t* GetMenuNameInternal() const;

	//Item management functions
	virtual void GetMenuSegmentsInternal(IMenuSegment* itemArray[], unsigned int arraySize, unsigned int& requiredSize, bool& itemsRetrieved) const;

private:
	std::wstring name;
	std::vector<IMenuSegment*> menuSegments;
};

#endif
