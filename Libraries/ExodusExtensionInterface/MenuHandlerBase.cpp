#include "MenuHandlerBase.h"
#include <thread>
#include <functional>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuHandlerBase::MenuHandlerBase(const std::wstring& amenuHandlerName, IViewManager& aviewManager)
:menuHandlerName(amenuHandlerName), viewManager(aviewManager), deleteThreadCount(0)
{}

//----------------------------------------------------------------------------------------
MenuHandlerBase::~MenuHandlerBase()
{
	//Note that we cannot attempt to close any remaining views have, since we can't call
	//the DeleteViewForItem() function at this point during destruction. Technically, at
	//this point we should check to ensure our views have been closed and our menus have
	//been removed, and if they haven't, we should fire an exception or raise an assert.
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::LoadMenuItems()
{
	//Obtain the list of menu items to create
	std::list<MenuItemDefinition> menuItemsToCreate;
	GetMenuItems(menuItemsToCreate);

	//Process each menu item
	for(std::list<MenuItemDefinition>::const_iterator i = menuItemsToCreate.begin(); i != menuItemsToCreate.end(); ++i)
	{
		//Add the menu item to our internal structures
		menuItemOrder.push_back(i->menuItemID);
		menuItems.insert(MenuItemsEntry(i->menuItemID, MenuItemInternal(*i)));
	}
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::ClearMenuItems()
{
	//Ensure that all the views are closed
	for(MenuItems::const_iterator menuItemIterator = menuItems.begin(); menuItemIterator != menuItems.end(); ++menuItemIterator)
	{
		const MenuItemInternal& menuItem = menuItemIterator->second;
		for(std::set<IViewPresenter*>::const_iterator i = menuItem.viewPresenters.begin(); i != menuItem.viewPresenters.end(); ++i)
		{
			viewManager.CloseView(*(*i), true);
		}
	}

	//Wait for all the view close threads to terminate
	std::unique_lock<std::mutex> lock(deleteThreadMutex);
	if(deleteThreadCount > 0)
	{
		allDeleteThreadsTerminated.wait(lock);
	}

	//##TODO## We need to somehow remove the menu segments we've added

	//Clear our menu item structure
	menuItems.clear();
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int MenuHandlerBase::GetIMenuHandlerVersion() const
{
	return ThisIMenuHandlerVersion();
}

//----------------------------------------------------------------------------------------
//Menu binding functions
//----------------------------------------------------------------------------------------
void MenuHandlerBase::AddMenuItems(IMenuSegment& menuSegment)
{
	//Process each menu item
	for(std::list<int>::const_iterator i = menuItemOrder.begin(); i != menuItemOrder.end(); ++i)
	{
		//Attempt to locate the menu item
		MenuItems::iterator menuItemIterator = menuItems.find(*i);
		if(menuItemIterator != menuItems.end())
		{
			//If the item isn't hidden from the menu, add it to the menu structure.
			MenuItemInternal& menuItem = menuItemIterator->second;
			if(!menuItem.itemDefinition.hiddenMenuItem)
			{
				menuSegment.AddMenuItemSelectableOption(*this, menuItem.itemDefinition.menuItemID, menuItem.itemDefinition.menuItemTitle);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//Menu item handler functions
//----------------------------------------------------------------------------------------
void MenuHandlerBase::HandleMenuItemSelect(int menuItemID)
{
	//Attempt to locate the menu item
	MenuItems::iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		//Check if this menu item opens a view
		MenuItemInternal& menuItem = menuItemIterator->second;
		if(!menuItem.itemDefinition.menuItemOpensView)
		{
			//If this menu item doesn't open a view, pass the selection event on to the
			//derived class.
			HandleMenuItemSelectNonView(menuItemID, viewManager);
		}
		else
		{
			//If this menu item opens a view, invoke a background thread to handle the
			//menu item selection.
			std::thread backgroundWorkerThread(std::bind(std::mem_fn(&MenuHandlerBase::HandleViewMenuItemSelect), this, std::ref(menuItem)));
			backgroundWorkerThread.detach();
		}
	}
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::HandleViewMenuItemSelect(MenuItemInternal& menuItem)
{
	//Either activate or open the view as required in response to this selection event
	if(!menuItem.itemDefinition.allowOpenViewMultipleTimes && !menuItem.viewPresenters.empty())
	{
		//If the view is currently open, and it isn't allowed to be opened multiple times,
		//activate it.
		viewManager.ActivateView(*(*menuItem.viewPresenters.begin()));
	}
	else
	{
		//If the view isn't currently open, or multiple instances are allow, create and
		//open the view.
		IViewPresenter* viewPresenter = CreateViewForItem(menuItem.itemDefinition.menuItemID, menuItem.itemDefinition.menuItemName);
		if(viewPresenter != 0)
		{
			menuItem.viewPresenters.insert(viewPresenter);
			if(!viewManager.OpenView(*viewPresenter, false))
			{
				menuItem.viewPresenters.erase(viewPresenter);
				DeleteViewForItem(menuItem.itemDefinition.menuItemID, viewPresenter);
			}
			else
			{
				DeleteViewOnClose(menuItem.itemDefinition.menuItemID, viewPresenter);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
bool MenuHandlerBase::RestoreMenuViewOpen(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter)
{
	//Ensure the menu handler name matches
	if(menuHandlerName != viewGroupName)
	{
		return false;
	}

	//Attempt to retrieve the target menu item
	bool foundMenuItem = false;
	MenuItems::iterator menuItemIterator = menuItems.begin();
	while(!foundMenuItem && (menuItemIterator != menuItems.end()))
	{
		if(menuItemIterator->second.itemDefinition.menuItemName == viewName)
		{
			foundMenuItem = true;
			continue;
		}
		++menuItemIterator;
	}
	if(!foundMenuItem)
	{
		return false;
	}
	MenuItemInternal& menuItem = menuItemIterator->second;

	//Ensure this menu item opens a view
	if(!menuItem.itemDefinition.menuItemOpensView)
	{
		return false;
	}

	//Restore the state for this view, opening the view if it is currently closed.
	bool result = false;
	if(!menuItem.itemDefinition.allowOpenViewMultipleTimes && !menuItem.viewPresenters.empty())
	{
		//If the view is currently open, and only one instance is allowed, restore the
		//view state.
		IViewPresenter* viewPresenter = *menuItem.viewPresenters.begin();
		result = viewPresenter->LoadViewState(viewState);

		//Return the view presenter used for the restore operation to the caller if
		//requested
		if(restoredViewPresenter != 0)
		{
			*restoredViewPresenter = viewPresenter;
		}
	}
	else
	{
		//If the view isn't currently open, or multiple instances are allowed, create and
		//open the view, restoring the view state.
		IViewPresenter* viewPresenter = CreateViewForItem(menuItem.itemDefinition.menuItemID, menuItem.itemDefinition.menuItemName);
		if(viewPresenter != 0)
		{
			menuItem.viewPresenters.insert(viewPresenter);
			if(!viewManager.OpenView(*viewPresenter, viewState, false))
			{
				menuItem.viewPresenters.erase(viewPresenter);
				DeleteViewForItem(menuItem.itemDefinition.menuItemID, viewPresenter);
			}
			else
			{
				DeleteViewOnClose(menuItem.itemDefinition.menuItemID, viewPresenter);
				result = viewManager.WaitUntilViewOpened(*viewPresenter);
			}
		}

		//Return the view presenter used for the restore operation to the caller if
		//requested
		if(result && (restoredViewPresenter != 0))
		{
			*restoredViewPresenter = viewPresenter;
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool MenuHandlerBase::OpenView(const std::wstring& viewGroupName, const std::wstring& viewName)
{
	//Ensure the menu handler name matches
	if(menuHandlerName != viewGroupName)
	{
		return false;
	}

	//Attempt to retrieve the target menu item
	bool foundMenuItem = false;
	MenuItems::const_iterator menuItemIterator = menuItems.begin();
	while(!foundMenuItem && (menuItemIterator != menuItems.end()))
	{
		if(menuItemIterator->second.itemDefinition.menuItemName == viewName)
		{
			foundMenuItem = true;
			continue;
		}
		++menuItemIterator;
	}
	if(!foundMenuItem)
	{
		return false;
	}
	const MenuItemInternal& menuItem = menuItemIterator->second;

	//Handle the menu item selection as normal
	HandleMenuItemSelect(menuItem.itemDefinition.menuItemID);
	return true;
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
std::wstring MenuHandlerBase::GetMenuHandlerName() const
{
	return menuHandlerName;
}

//----------------------------------------------------------------------------------------
std::wstring MenuHandlerBase::GetMenuItemName(int menuItemID) const
{
	std::wstring menuItemName;

	//Attempt to locate the menu item
	MenuItems::const_iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		//Return the name of the menu item
		const MenuItemInternal& menuItem = menuItemIterator->second;
		menuItemName = menuItem.itemDefinition.menuItemName;
	}

	return menuItemName;
}

//----------------------------------------------------------------------------------------
std::set<IViewPresenter*> MenuHandlerBase::GetOpenViewPresenters(int menuItemID) const
{
	//Attempt to locate the menu item
	MenuItems::const_iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		//Check if this menu item opens a view
		const MenuItemInternal& menuItem = menuItemIterator->second;
		if(menuItem.itemDefinition.menuItemOpensView)
		{
			//Return the open views for the menu item
			return menuItem.viewPresenters;
		}
	}
	std::set<IViewPresenter*> emptyViewPresenterSet;
	return emptyViewPresenterSet;
}

//----------------------------------------------------------------------------------------
bool MenuHandlerBase::AddCreatedView(int menuItemID, IViewPresenter* viewPresenter)
{
	//Attempt to locate the menu item
	MenuItems::iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		//Check if this menu item opens a view
		MenuItemInternal& menuItem = menuItemIterator->second;
		if(menuItem.itemDefinition.menuItemOpensView)
		{
			menuItem.viewPresenters.insert(viewPresenter);
			if(!viewManager.OpenView(*viewPresenter, false))
			{
				menuItem.viewPresenters.erase(viewPresenter);
			}
			else
			{
				//Note that this is safe, since we're guaranteed that this
				//function is not called in the context of the UI thread.
				DeleteViewOnClose(menuItemID, viewPresenter);
				return true;
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::HandleMenuItemSelectNonView(int menuItemID, IViewManager& aviewManager)
{}

//----------------------------------------------------------------------------------------
//View management
//----------------------------------------------------------------------------------------
void MenuHandlerBase::DeleteViewOnClose(int menuItemID, IViewPresenter* viewPresenter)
{
	//Increment the delete thread count
	std::unique_lock<std::mutex> lock(deleteThreadMutex);
	++deleteThreadCount;

	//Spawn a worker thread to handle the deletion of the view when it closes
	std::thread workerThread(std::bind(std::mem_fn(&MenuHandlerBase::DeleteViewHandler), this, menuItemID, viewPresenter));
	workerThread.detach();
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::DeleteViewHandler(int menuItemID, IViewPresenter* viewPresenter)
{
	//Attempt to locate the menu item
	MenuItems::iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		//If the view is currently open, wait for it to close, and delete it
		//afterwards.
		MenuItemInternal& menuItem = menuItemIterator->second;
		viewManager.WaitUntilViewClosed(*viewPresenter);
		menuItem.viewPresenters.erase(viewPresenter);
		DeleteViewForItem(menuItem.itemDefinition.menuItemID, viewPresenter);
	}

	//Decrement the delete thread count
	std::unique_lock<std::mutex> lock(deleteThreadMutex);
	--deleteThreadCount;
	if(deleteThreadCount <= 0)
	{
		allDeleteThreadsTerminated.notify_all();
	}
}
