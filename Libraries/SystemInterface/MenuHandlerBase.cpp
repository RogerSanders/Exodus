#include "MenuHandlerBase.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MenuHandlerBase::MenuHandlerBase()
:deleteThreadCount(0), viewModelLauncher(0)
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
		menuItems.insert(MenuItemsEntry(i->menuItemID, MenuItemInternal(i->menuItemID, i->menuItemName, i->menuItemOpensViewModel, i->hiddenMenuItem)));
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
	for(std::list<int>::const_iterator menuItemOrderIterator = menuItemOrder.begin(); menuItemOrderIterator != menuItemOrder.end(); ++menuItemOrderIterator)
	{
		//Attempt to locate the menu item
		MenuItems::iterator i = menuItems.find(*menuItemOrderIterator);
		if(i != menuItems.end())
		{
			MenuItemInternal& menuItem = i->second;

			//If the item isn't hidden from the menu, add it to the menu structure.
			if(!menuItem.hiddenMenuItem)
			{
				menuSegment.AddMenuItemSelectableOption(*this, menuItem.menuItemID, menuItem.menuItemName);
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
	MenuItems::iterator i = menuItems.find(menuItemID);
	if(i != menuItems.end())
	{
		MenuItemInternal& menuItem = i->second;

		//Check if this menu item opens a view model
		if(!menuItem.menuItemOpensViewModel)
		{
			//If this menu item doesn't open a view model, pass the selection event on to
			//the derived class.
			HandleMenuItemSelectNonViewModel(menuItemID, aviewModelLauncher);
		}
		else
		{
			//If this menu item opens a view model, check if the view is currently open.
			IViewModel* viewModel = menuItem.viewModel;
			if(viewModel == 0)
			{
				//If the view isn't currently open, create and open the view.
				viewModel = CreateViewModelForItem(menuItemID);
				if(viewModel != 0)
				{
					menuItem.viewModel = viewModel;
					if(!aviewModelLauncher.OpenViewModel(viewModel, false))
					{
						menuItem.viewModel = 0;
						DeleteViewModelForItem(menuItemID, viewModel);
					}
					else
					{
						DeleteViewModelOnClose(menuItemID);
					}
				}
			}
			else
			{
				//If the view is currently open, activate it.
				aviewModelLauncher.ActivateViewModel(viewModel);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::RestoreMenuViewModelOpen(const std::wstring& menuHandlerName, int menuItemID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& aviewModelLauncher)
{
	//Save a reference to the viewModelLauncher
	if(viewModelLauncher == 0)
	{
		viewModelLauncher = &aviewModelLauncher;
	}

	//Ensure the menu handler name matches
	if(GetMenuHandlerName() != menuHandlerName)
	{
		return;
	}

	//Attempt to locate the menu item
	MenuItems::iterator i = menuItems.find(menuItemID);
	if(i != menuItems.end())
	{
		MenuItemInternal& menuItem = i->second;

		//Check if this menu item opens a view model
		if(menuItem.menuItemOpensViewModel)
		{
			//Check if the view is currently open
			IViewModel* viewModel = menuItem.viewModel;
			if(viewModel == 0)
			{
				//If the view isn't currently open, create and open the view, restoring
				//the view state.
				viewModel = CreateViewModelForItem(menuItemID);
				if(viewModel != 0)
				{
					menuItem.viewModel = viewModel;
					if(!aviewModelLauncher.OpenViewModel(viewModel, false, true))
					{
						menuItem.viewModel = 0;
						DeleteViewModelForItem(menuItemID, viewModel);
					}
					else
					{
						//Note that this is safe, since we're guaranteed that this
						//function is not called in the context of the UI thread.
						aviewModelLauncher.WaitUntilViewModelOpened(viewModel);
						viewModel->LoadViewLayoutState(node);
						viewModel->SetViewPosition(xpos, ypos);
						if((width > 0) && (height > 0))
						{
							viewModel->SetViewSize(width, height);
						}
						aviewModelLauncher.ShowViewModel(viewModel);
						DeleteViewModelOnClose(menuItemID);
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
}

//----------------------------------------------------------------------------------------
void MenuHandlerBase::OpenViewModel(const std::wstring& menuHandlerName, int menuItemID, IViewModelLauncher& aviewModelLauncher)
{
	//Save a reference to the viewModelLauncher
	if(viewModelLauncher == 0)
	{
		viewModelLauncher = &aviewModelLauncher;
	}

	//Ensure the menu handler name matches
	if(GetMenuHandlerName() != menuHandlerName)
	{
		return;
	}

	//Handle the menu item selection as normal
	HandleMenuItemSelect(menuItemID, aviewModelLauncher);
}

//----------------------------------------------------------------------------------------
//Management functions
//----------------------------------------------------------------------------------------
IViewModel* MenuHandlerBase::GetViewModelIfOpen(int menuItemID)
{
	IViewModel* viewModel = 0;

	//Attempt to locate the menu item
	MenuItems::iterator i = menuItems.find(menuItemID);
	if(i != menuItems.end())
	{
		MenuItemInternal& menuItem = i->second;

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
	MenuItems::iterator i = menuItems.find(menuItemID);
	if(i != menuItems.end())
	{
		MenuItemInternal& menuItem = i->second;

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
	MenuItems::iterator i = menuItems.find(menuItemID);
	if(i != menuItems.end())
	{
		MenuItemInternal& menuItem = i->second;

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
