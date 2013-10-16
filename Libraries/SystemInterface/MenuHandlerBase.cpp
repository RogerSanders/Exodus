#include "MenuHandlerBase.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuHandlerBase::MenuHandlerBase(const std::wstring& amenuHandlerName)
:menuHandlerName(amenuHandlerName), deleteThreadCount(0), viewModelLauncher(0)
{}

//----------------------------------------------------------------------------------------
MenuHandlerBase::~MenuHandlerBase()
{
	//Note that we cannot attempt to close any remaining views have, since we can't call
	//the DeleteViewModelForItem() function at this point during destruction. Technically,
	//at this point we should check to ensure our views have been closed and our menus
	//have been removed, and if they haven't, we should fire an exception or raise an
	//assert.
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
		menuItems.insert(MenuItemsEntry(i->menuItemID, MenuItemInternal(i->menuItemID, i->menuItemName, i->menuItemTitle, i->menuItemOpensViewModel, i->hiddenMenuItem)));
	}
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::ClearMenuItems()
{
	//Ensure that all the views are closed
	for(MenuItems::const_iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		if(i->second.viewModel != 0)
		{
			viewModelLauncher->CloseViewModel(i->second.viewModel, true);
		}
	}

	//Wait for all the view model close threads to terminate
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
void MenuHandlerBase::AddMenuItems(IMenuSegment& menuSegment, IViewModelLauncher& aviewModelLauncher)
{
	//Save a reference to the viewModelLauncher
	if(viewModelLauncher == 0)
	{
		viewModelLauncher = &aviewModelLauncher;
	}

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
void MenuHandlerBase::HandleMenuItemSelect(int menuItemID, IViewModelLauncher& aviewModelLauncher)
{
	//Save a reference to the viewModelLauncher
	if(viewModelLauncher == 0)
	{
		viewModelLauncher = &aviewModelLauncher;
	}

	//Attempt to locate the menu item
	MenuItems::iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		MenuItemInternal& menuItem = menuItemIterator->second;

		//Check if this menu item opens a view model
		if(!menuItem.menuItemOpensViewModel)
		{
			//If this menu item doesn't open a view model, pass the selection event on to
			//the derived class.
			HandleMenuItemSelectNonViewModel(menuItemID, aviewModelLauncher);
		}
		else
		{
			//If this menu item opens a view model, invoke a background thread to handle
			//the menu item selection.
			boost::thread backgroundWorkerThread(boost::bind(boost::mem_fn(&MenuHandlerBase::HandleViewModelMenuItemSelect), this, boost::ref(menuItem)));
		}
	}
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::HandleViewModelMenuItemSelect(MenuItemInternal& menuItem)
{
	//Check if the view is currently open, and either activate it or open it depending on
	//its open state.
	IViewModel* viewModel = menuItem.viewModel;
	if(viewModel == 0)
	{
		//If the view isn't currently open, create and open the view.
		viewModel = CreateViewModelForItem(menuItem.menuItemID, menuItem.menuItemName);
		if(viewModel != 0)
		{
			menuItem.viewModel = viewModel;
			if(!viewModelLauncher->OpenViewModel(viewModel, false))
			{
				menuItem.viewModel = 0;
				DeleteViewModelForItem(menuItem.menuItemID, viewModel);
			}
			else
			{
				DeleteViewModelOnClose(menuItem.menuItemID);
			}
		}
	}
	else
	{
		//If the view is currently open, activate it.
		viewModelLauncher->ActivateViewModel(viewModel);
	}
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::RestoreMenuViewModelOpen(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& aviewModelLauncher)
{
	//Save a reference to the viewModelLauncher
	if(viewModelLauncher == 0)
	{
		viewModelLauncher = &aviewModelLauncher;
	}

	//Ensure the menu handler name matches
	if(menuHandlerName != viewModelGroupName)
	{
		return;
	}

	//Attempt to retrieve the target menu item
	MenuItemInternal* menuItem = 0;
	bool foundMenuItem = false;
	MenuItems::iterator menuItemIterator = menuItems.begin();
	while(!foundMenuItem && (menuItemIterator != menuItems.end()))
	{
		if(menuItemIterator->second.menuItemName == viewModelName)
		{
			menuItem = &menuItemIterator->second;
			foundMenuItem = true;
		}
		++menuItemIterator;
	}
	if(!foundMenuItem)
	{
		return;
	}

	//Check if this menu item opens a view model
	if(menuItem->menuItemOpensViewModel)
	{
		//Check if the view is currently open
		IViewModel* viewModel = menuItem->viewModel;
		if(viewModel == 0)
		{
			//If the view isn't currently open, create and open the view, restoring the
			//view state.
			viewModel = CreateViewModelForItem(menuItem->menuItemID, menuItem->menuItemName);
			if(viewModel != 0)
			{
				menuItem->viewModel = viewModel;
				if(!aviewModelLauncher.OpenViewModel(viewModel, false, true))
				{
					menuItem->viewModel = 0;
					DeleteViewModelForItem(menuItem->menuItemID, viewModel);
				}
				else
				{
					//Note that this is safe, since we're guaranteed that this function is
					//not called in the context of the UI thread.
					aviewModelLauncher.WaitUntilViewModelOpened(viewModel);
					viewModel->LoadViewLayoutState(node);
					viewModel->SetViewPosition(xpos, ypos);
					if((width > 0) && (height > 0))
					{
						viewModel->SetViewSize(width, height);
					}
					aviewModelLauncher.ShowViewModel(viewModel);
					DeleteViewModelOnClose(menuItem->menuItemID);
				}
			}
		}
		else
		{
			//If the view is currently open, restore the view state, and activate it.
			viewModel->LoadViewLayoutState(node);
			viewModel->SetViewPosition(xpos, ypos);
			if((width > 0) && (height > 0))
			{
				viewModel->SetViewSize(width, height);
			}
			aviewModelLauncher.ActivateViewModel(viewModel);
		}
	}
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::OpenViewModel(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IViewModelLauncher& aviewModelLauncher)
{
	//Save a reference to the viewModelLauncher
	if(viewModelLauncher == 0)
	{
		viewModelLauncher = &aviewModelLauncher;
	}

	//Ensure the menu handler name matches
	if(menuHandlerName != viewModelGroupName)
	{
		return;
	}

	//Attempt to retrieve the target menu item
	const MenuItemInternal* menuItem = 0;
	bool foundMenuItem = false;
	MenuItems::const_iterator menuItemIterator = menuItems.begin();
	while(!foundMenuItem && (menuItemIterator != menuItems.end()))
	{
		if(menuItemIterator->second.menuItemName == viewModelName)
		{
			menuItem = &menuItemIterator->second;
			foundMenuItem = true;
		}
		++menuItemIterator;
	}
	if(!foundMenuItem)
	{
		return;
	}

	//Handle the menu item selection as normal
	HandleMenuItemSelect(menuItem->menuItemID, aviewModelLauncher);
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
IViewModel* MenuHandlerBase::GetViewModelIfOpen(int menuItemID)
{
	IViewModel* viewModel = 0;

	//Attempt to locate the menu item
	MenuItems::const_iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		const MenuItemInternal& menuItem = menuItemIterator->second;

		//Check if this menu item opens a view model
		if(menuItem.menuItemOpensViewModel)
		{
			//Return the view model for the menu item
			viewModel = menuItem.viewModel;
		}
	}

	return viewModel;
}

//----------------------------------------------------------------------------------------
bool MenuHandlerBase::AddCreatedViewModel(int menuItemID, IViewModel* viewModel)
{
	bool result = false;

	//Attempt to locate the menu item
	MenuItems::iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		MenuItemInternal& menuItem = menuItemIterator->second;

		//Check if this menu item opens a view model
		if(menuItem.menuItemOpensViewModel)
		{
			//Ensure the view isn't currently open
			if(menuItem.viewModel == 0)
			{
				menuItem.viewModel = viewModel;
				if(!viewModelLauncher->OpenViewModel(viewModel, false, false))
				{
					menuItem.viewModel = 0;
				}
				else
				{
					//Note that this is safe, since we're guaranteed that this
					//function is not called in the context of the UI thread.
					DeleteViewModelOnClose(menuItemID);
					result = true;
				}
			}
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::HandleMenuItemSelectNonViewModel(int menuItemID, IViewModelLauncher& aviewModelLauncher)
{}

//----------------------------------------------------------------------------------------
//View model management
//----------------------------------------------------------------------------------------
void MenuHandlerBase::DeleteViewModelOnClose(int menuItemID)
{
	//Increment the delete thread count
	boost::mutex::scoped_lock lock(deleteThreadMutex);
	++deleteThreadCount;

	//Spawn a worker thread to handle the deletion of the view model when it closes
	boost::thread workerThread(boost::bind(boost::mem_fn(&MenuHandlerBase::DeleteViewModelHandler), this, menuItemID));
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::DeleteViewModelHandler(int menuItemID)
{
	//Attempt to locate the menu item
	MenuItems::iterator menuItemIterator = menuItems.find(menuItemID);
	if(menuItemIterator != menuItems.end())
	{
		MenuItemInternal& menuItem = menuItemIterator->second;

		//Check if this menu item opens a view model
		if(menuItem.menuItemOpensViewModel)
		{
			//Check if the view is currently open
			IViewModel* viewModel = menuItem.viewModel;
			if(viewModel != 0)
			{
				//If the view is currently open, wait for it to close, and delete it
				//afterwards.
				viewModelLauncher->WaitUntilViewModelClosed(viewModel);
				menuItem.viewModel = 0;
				DeleteViewModelForItem(menuItem.menuItemID, viewModel);
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
