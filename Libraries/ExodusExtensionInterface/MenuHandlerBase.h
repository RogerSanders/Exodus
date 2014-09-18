#ifndef __MENUHANDLERBASE_H__
#define __MENUHANDLERBASE_H__
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "IViewManager.h"
#include "IMenuHandler.h"
#include "IMenuSegment.h"
#include "IMenuSelectableOption.h"
#include "IMenuSubmenu.h"
#include <map>
#include <list>
#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

//##TODO## Add support for views which can be open multiple times
//##TODO## Add support for menu items which are not just selectable items, IE, submenus or
//separators.
class MenuHandlerBase :public IMenuHandler
{
public:
	//Constructors
	MenuHandlerBase(const std::wstring& amenuHandlerName, IViewManager& aviewManager);
	virtual ~MenuHandlerBase();
	void LoadMenuItems();
	void ClearMenuItems();

	//Interface version functions
	virtual unsigned int GetIMenuHandlerVersion() const;

	//Menu binding functions
	void AddMenuItems(IMenuSegment& menuSegment);

	//Menu item handler functions
	virtual void HandleMenuItemSelect(int menuItemID);
	bool RestoreMenuViewOpen(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState);
	bool OpenView(const std::wstring& viewGroupName, const std::wstring& viewName);

protected:
	//Structures
	struct MenuItemDefinition;

protected:
	//Management functions
	std::wstring GetMenuHandlerName() const;
	std::wstring GetMenuItemName(int menuItemID) const;
	IViewPresenter* GetViewIfOpen(int menuItemID);
	bool AddCreatedView(int menuItemID, IViewPresenter* viewPresenter);
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const = 0;
	virtual IViewPresenter* CreateViewForItem(int menuItemID, const std::wstring& viewName) = 0;
	virtual void DeleteViewForItem(int menuItemID, IViewPresenter* viewPresenter) = 0;
	virtual void HandleMenuItemSelectNonView(int menuItemID, IViewManager& aviewManager);

private:
	//Structures
	struct MenuItemInternal;

private:
	//View management
	void DeleteViewOnClose(int menuItemID);
	void DeleteViewHandler(int menuItemID);

	//Menu item handler functions
	void HandleViewMenuItemSelect(MenuItemInternal& menuItem);

private:
	typedef std::map<int, MenuItemInternal> MenuItems;
	typedef std::pair<int, MenuItemInternal> MenuItemsEntry;
	std::wstring menuHandlerName;
	MenuItems menuItems;
	std::list<int> menuItemOrder;
	IViewManager& viewManager;
	mutable boost::mutex deleteThreadMutex;
	boost::condition allDeleteThreadsTerminated;
	unsigned int deleteThreadCount;
};

#include "MenuHandlerBase.inl"
#endif
