#include "MenuHandlerBase.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>

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
		menuItems.insert(MenuItemsEntry(i->menuItemID, MenuItemInternal(i->menuItemID, i->menuItemName, i->menuItemTitle, i->menuItemOpensView, i->hiddenMenuItem)));
	}
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::ClearMenuItems()
{
	//Ensure that all the views are closed
	for(MenuItems::const_iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		if(i->second.viewPresenter != 0)
		{
			viewManager.CloseView(*i->second.viewPresenter, true);
		}
	}

	//Wait for all the view close threads to terminate
	boost::mutex::scoped_lock lock(deleteThreadMutex);
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
			MenuItemInternal& menuItem = menuItemIterator->second;

			//If the item isn't hidden from the menu, add it to the menu structure.
			if(!menuItem.hiddenMenuItem)
			{
				menuSegment.AddMenuItemSelectableOption(*this, menuItem.menuItemID, menuItem.menuItemTitle);
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
		MenuItemInternal& menuItem = menuItemIterator->second;

		//Check if this menu item opens a view
		if(!menuItem.menuItemOpensView)
		{
			//If this menu item doesn't open a view, pass the selection event on to the
			//derived class.
			HandleMenuItemSelectNonView(menuItemID, viewManager);
		}
		else
		{
			//If this menu item opens a view, invoke a background thread to handle the
			//menu item selection.
			boost::thread backgroundWorkerThread(boost::bind(boost::mem_fn(&MenuHandlerBase::HandleViewMenuItemSelect), this, boost::ref(menuItem)));
		}
	}
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::HandleViewMenuItemSelect(MenuItemInternal& menuItem)
{
	//Check if the view is currently open, and either activate it or open it depending on
	//its open state.
	IViewPresenter* viewPresenter = menuItem.viewPresenter;
	if(viewPresenter == 0)
	{
		//If the view isn't currently open, create and open the view.
		viewPresenter = CreateViewForItem(menuItem.menuItemID, menuItem.menuItemName);
		if(viewPresenter != 0)
		{
			menuItem.viewPresenter = viewPresenter;
			if(!viewManager.OpenView(*viewPresenter, false))
			{
				menuItem.viewPresenter = 0;
				DeleteViewForItem(menuItem.menuItemID, viewPresenter);
			}
			else
			{
				DeleteViewOnClose(menuItem.menuItemID);
			}
		}
	}
	else
	{
		//If the view is currently open, activate it.
		viewManager.ActivateView(*viewPresenter);
	}
}

//----------------------------------------------------------------------------------------
bool MenuHandlerBase::RestoreMenuViewOpen(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState)
{
	//Ensure the menu handler name matches
	if(menuHandlerName != viewGroupName)
	{
		return false;
	}

	//Attempt to retrieve the target menu item
	MenuItemInternal* menuItem = 0;
	bool foundMenuItem = false;
	MenuItems::iterator menuItemIterator = menuItems.begin();
	while(!foundMenuItem && (menuItemIterator != menuItems.end()))
	{
		if(menuItemIterator->second.menuItemName == viewName)
		{
			menuItem = &menuItemIterator->second;
			foundMenuItem = true;
		}
		++menuItemIterator;
	}
	if(!foundMenuItem)
	{
		return false;
	}

	//Ensure this menu item opens a view
	if(!menuItem->menuItemOpensView)
	{
		return false;
	}

	//Restore the state for this view, opening the view if it is currently closed.
	bool result = false;
	IViewPresenter* viewPresenter = menuItem->viewPresenter;
	if(viewPresenter == 0)
	{
		//If the view isn't currently open, create and open the view, restoring the
		//view state.
		viewPresenter = CreateViewForItem(menuItem->menuItemID, menuItem->menuItemName);
		if(viewPresenter != 0)
		{
			menuItem->viewPresenter = viewPresenter;
			if(!viewManager.OpenView(*viewPresenter, viewState, false))
			{
				menuItem->viewPresenter = 0;
				DeleteViewForItem(menuItem->menuItemID, viewPresenter);
			}
			else
			{
				DeleteViewOnClose(menuItem->menuItemID);
				result = viewManager.WaitUntilViewOpened(*menuItem->viewPresenter);
			}
		}
	}
	else
	{
		//If the view is currently open, restore the view state.
		result = viewPresenter->LoadViewState(viewState);
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
	const MenuItemInternal* menuItem = 0;
	bool foundMenuItem = false;
	MenuItems::const_iterator menuItemIterator = menuItems.begin();
	while(!foundMenuItem && (menuItemIterator != menuItems.end()))
	{
		if(menuItemIterator->second.menuItemName == viewName)
		{
			menuItem = &menuItemIterator->second;
			foundMenuItem = true;
		}
		++menuItemIterator;
	}
	if(!foundMenuItem)
	{
		return false;
	}

	//Handle the menu item selection as normal
	HandleMenuItemSelect(menuItem->menuItemID);
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
		menuItemName = menuItem.menuItemName;
	}

	return menuItemName;
}

//----------------------------------------------------------------------------------------
IViewPresenter* MenuHandlerBase::GetViewIfOpen(int menuItemID)
{
	IViewPresenter* viewPresenter = 0;

	//Attempt to locate the menu item
	MenuItems::const_iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		const MenuItemInternal& menuItem = menuItemIterator->second;

		//Check if this menu item opens a view
		if(menuItem.menuItemOpensView)
		{
			//Return the view for the menu item
			viewPresenter = menuItem.viewPresenter;
		}
	}

	return viewPresenter;
}

//----------------------------------------------------------------------------------------
bool MenuHandlerBase::AddCreatedView(int menuItemID, IViewPresenter* viewPresenter)
{
	bool result = false;

	//Attempt to locate the menu item
	MenuItems::iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		MenuItemInternal& menuItem = menuItemIterator->second;

		//Check if this menu item opens a view
		if(menuItem.menuItemOpensView)
		{
			//Ensure the view isn't currently open
			if(menuItem.viewPresenter == 0)
			{
				menuItem.viewPresenter = viewPresenter;
				if(!viewManager.OpenView(*viewPresenter, false))
				{
					menuItem.viewPresenter = 0;
				}
				else
				{
					//Note that this is safe, since we're guaranteed that this
					//function is not called in the context of the UI thread.
					DeleteViewOnClose(menuItemID);
					result = true;
				}
			}
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::HandleMenuItemSelectNonView(int menuItemID, IViewManager& aviewManager)
{}

//----------------------------------------------------------------------------------------
//View management
//----------------------------------------------------------------------------------------
void MenuHandlerBase::DeleteViewOnClose(int menuItemID)
{
	//Increment the delete thread count
	boost::mutex::scoped_lock lock(deleteThreadMutex);
	++deleteThreadCount;

	//Spawn a worker thread to handle the deletion of the view when it closes
	boost::thread workerThread(boost::bind(boost::mem_fn(&MenuHandlerBase::DeleteViewHandler), this, menuItemID));
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::DeleteViewHandler(int menuItemID)
{
	//Attempt to locate the menu item
	MenuItems::iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		MenuItemInternal& menuItem = menuItemIterator->second;

		//Check if this menu item opens a view
		if(menuItem.menuItemOpensView)
		{
			//Check if the view is currently open
			IViewPresenter* viewPresenter = menuItem.viewPresenter;
			if(viewPresenter != 0)
			{
				//If the view is currently open, wait for it to close, and delete it
				//afterwards.
				viewManager.WaitUntilViewClosed(*viewPresenter);
				menuItem.viewPresenter = 0;
				DeleteViewForItem(menuItem.menuItemID, viewPresenter);
			}
		}
	}

	//Decrement the delete thread count
	boost::mutex::scoped_lock lock(deleteThreadMutex);
	--deleteThreadCount;
	if(deleteThreadCount <= 0)
	{
		allDeleteThreadsTerminated.notify_all();
	}
}
