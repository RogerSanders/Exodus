#ifndef __MENUHANDLERBASE_H__
#define __MENUHANDLERBASE_H__
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include "IMenuHandler.h"
#include "IMenuSegment.h"
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
	MenuHandlerBase();
	virtual ~MenuHandlerBase();
	void LoadMenuItems();
	void ClearMenuItems();

	//Menu binding functions
	void AddMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& aviewModelLauncher);

	//Menu item handler functions
	virtual void HandleMenuItemSelect(int menuItemID, IViewModelLauncher& aviewModelLauncher);
	void RestoreMenuViewModelOpen(const std::wstring& menuHandlerName, int menuItemID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& aviewModelLauncher);
	void OpenViewModel(const std::wstring& menuHandlerName, int menuItemID, IViewModelLauncher& aviewModelLauncher);

protected:
	//Structures
	struct MenuItemDefinition;

protected:
	//Management functions
	virtual std::wstring GetMenuHandlerName() const = 0;
	IViewModel* GetViewModelIfOpen(int menuItemID);
	bool AddCreatedViewModel(int menuItemID, IViewModel* viewModel);
	virtual void GetMenuItems(std::list<MenuItemDefinition>& menuItems) const = 0;
	virtual IViewModel* CreateViewModelForItem(int menuItemID) = 0;
	virtual void DeleteViewModelForItem(int menuItemID, IViewModel* viewModel) = 0;
	virtual void HandleMenuItemSelectNonViewModel(int menuItemID, IViewModelLauncher& aviewModelLauncher);

private:
	//Structures
	struct MenuItemInternal;

private:
	//View model management
	void DeleteViewModelOnClose(int menuItemID);
	void DeleteViewModelHandler(int menuItemID);

private:
	typedef std::map<int, MenuItemInternal> MenuItems;
	typedef std::pair<int, MenuItemInternal> MenuItemsEntry;
	MenuItems menuItems;
	std::list<int> menuItemOrder;
	IViewModelLauncher* viewModelLauncher;
	mutable boost::mutex deleteThreadMutex;
	boost::condition allDeleteThreadsTerminated;
	unsigned int deleteThreadCount;
};

#include "MenuHandlerBase.inl"
#endif
