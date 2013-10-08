#ifndef __IMENUSUBMENU_H__
#define __IMENUSUBMENU_H__
#include "InteropSupport/InteropSupport.pkg"
#include "IMenuItem.h"
#include <string>
#include <list>
class IMenuSegment;

class IMenuSubmenu :public IMenuItem
{
public:
	//Interface version functions
	static inline unsigned int ThisIMenuSubmenuVersion();
	virtual unsigned int GetIMenuSubmenuVersion() const = 0;

	//Menu name functions
	inline std::wstring GetMenuName() const;

	//Item management functions
	virtual bool NoMenuSegmentsExist() const = 0;
	virtual bool NoMenuItemsExist() const = 0;
	inline std::list<IMenuSegment*> GetMenuSegments() const;

	//Menu segment creation and deletion
	virtual IMenuSegment& CreateMenuSegment() = 0;
	virtual void DeleteMenuSegment(IMenuSegment& menuSegment) = 0;
	virtual void DeleteAllMenuSegments() = 0;

protected:
	//Menu name functions
	virtual void GetMenuNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Item management functions
	virtual void GetMenuSegmentsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuSegment*>>& marshaller) const = 0;
};

#include "IMenuSubmenu.inl"
#endif
