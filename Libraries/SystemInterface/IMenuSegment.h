#ifndef __IMENUSEGMENT_H__
#define __IMENUSEGMENT_H__
#include "InteropSupport/InteropSupport.pkg"
#include <list>
class IMenuItem;
class IMenuSeparator;
class IMenuSubmenu;
class IMenuSelectableOption;
class IMenuHandler;

class IMenuSegment
{
public:
	//Constructors
	virtual ~IMenuSegment() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIMenuSegmentVersion();
	virtual unsigned int GetIMenuSegmentVersion() const = 0;

	//Item management functions
	virtual bool NoMenuItemsExist() const = 0;
	inline std::list<IMenuItem*> GetMenuItems() const;

	//Menu item creation and deletion
	virtual IMenuSeparator& AddMenuItemSeparator() = 0;
	inline IMenuSubmenu& AddMenuItemSubmenu(const std::wstring& name);
	inline IMenuSelectableOption& AddMenuItemSelectableOption(IMenuHandler& menuHandler, int menuItemID, const std::wstring& name);
	virtual void DeleteMenuItem(IMenuItem& menuItem) = 0;
	virtual void DeleteAllMenuItems() = 0;

protected:
	//Item management functions
	virtual void GetMenuItemsInternal(const InteropSupport::ISTLObjectTarget<std::list<IMenuItem*>>& marshaller) const = 0;

	//Menu item creation and deletion
	virtual IMenuSubmenu& AddMenuItemSubmenuInternal(const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) = 0;
	virtual IMenuSelectableOption& AddMenuItemSelectableOptionInternal(IMenuHandler& menuHandler, int menuItemID, const InteropSupport::ISTLObjectSource<std::wstring>& nameMarshaller) = 0;
};

#include "IMenuSegment.inl"
#endif
