#ifndef __IMENUSEGMENT_H__
#define __IMENUSEGMENT_H__
#include "InteropSupport/InteropSupport.pkg"
#include "IMenuItem.h"
#include <string>
#include <list>
class IMenuSubmenu;
class IMenuSelectableOption;
class IMenuHandler;

class IMenuSegment :public IMenuItem
{
public:
	//Enumerations
	enum SortMode
	{
		SORTMODE_ADDITIONORDER,
		SORTMODE_TITLE
	};

public:
	//Constructors
	virtual ~IMenuSegment() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIMenuSegmentVersion();
	virtual unsigned int GetIMenuSegmentVersion() const = 0;

	//Menu title functions
	inline std::wstring GetMenuSortTitle() const;

	//Sort mode functions
	virtual SortMode GetSortMode() const = 0;

	//Separator functions
	virtual bool GetSurroundWithSeparators() const = 0;

	//Item management functions
	virtual bool NoMenuItemsExist() const = 0;
	inline std::list<IMenuItem*> GetMenuItems() const;
	inline std::list<IMenuItem*> GetSortedMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSegment& AddMenuItemSegment(bool asurroundWithSeparators = true, IMenuSegment::SortMode sortMode = IMenuSegment::SORTMODE_ADDITIONORDER) = 0;
	inline IMenuSubmenu& AddMenuItemSubmenu(const std::wstring& title);
	inline IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& title);
	virtual void DeleteMenuItem(IMenuItem& menuItem) = 0;
	virtual void DeleteAllMenuItems() = 0;

protected:
	//Menu title functions
	virtual void GetMenuSortTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Item management functions
	virtual void GetMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const = 0;
	virtual void GetSortedMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const = 0;

	//Menu item creation and deletion
	virtual IMenuSubmenu& AddMenuItemSubmenuInternal(const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller) = 0;
	virtual IMenuSelectableOption& AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const InteropSupport::ISTLObjectSource<std::wstring>& titleMarshaller) = 0;
};

#include "IMenuSegment.inl"
#endif
