#ifndef __MENUHANDLERBASE_H__
#define __MENUHANDLERBASE_H__
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include "IMenuHandler.h"
#include "IMenuSegment.h"
#include "IMenuSelectableOption.h"
#include "IMenuSubmenu.h"
#include "IMenuSeparator.h"
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
	MenuHandlerBase(const std::wstring& amenuHandlerName);
	virtual ~MenuHandlerBase();
	void LoadMenuItems();
	void ClearMenuItems();

	//Interface version functions
	virtual unsigned int GetIMenuHandlerVersion() const;

	//Menu binding functions
	void AddMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& aviewModelLauncher);

	//Menu item handler functions
	virtual void HandleMenuItemSelect(int menuItemID, IViewModelLauncher& aviewModelLauncher);
	void RestoreMenuViewModelOpen(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& aviewModelLauncher);
	void OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& aviewModelLauncher);

protected:
	//Structures
	struct MenuItemDefinition;

protected:
	//Management functions
	std::wstring GetMenuHandlerName() const;
	std::wstring GetMenuItemName(int menuItemID) const;
	IViewModel* GetViewModelIfOpen(int menuItemID);
	bool AddCreatedViewModel(int menuItemID, IViewModel* viewModel);
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const = 0;
	virtual IViewModel* CreateViewModelForItem(int menuItemID, const std::wstring& viewModelName) = 0;
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel) = 0;
	virtual void HandleMenuItemSelectNonViewModel(int menuItemID, IViewModelLauncher& aviewModelLauncher);

private:
	//Structures
	struct MenuItemInternal;

private:
	//View model management
	void DeleteViewModelOnClose(int menuItemID);
	void DeleteViewModelHandler(int menuItemID);

	//Menu item handler functions
	void HandleViewModelMenuItemSelect(MenuItemInternal& menuItem);

private:
	typedef std::map<int, MenuItemInternal> MenuItems;
	typedef std::pair<int, MenuItemInternal> MenuItemsEntry;
	std::wstring menuHandlerName;
	MenuItems menuItems;
	std::list<int> menuItemOrder;
	IViewModelLauncher* viewModelLauncher;
	mutable boost::mutex deleteThreadMutex;
	boost::condition allDeleteThreadsTerminated;
	unsigned int deleteThreadCount;
};

#include "MenuHandlerBase.inl"
#endif
