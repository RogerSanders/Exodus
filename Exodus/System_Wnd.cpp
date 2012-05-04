#include "System.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//View functions
//----------------------------------------------------------------------------------------
void System::BuildDebugMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher) const
{
	//Add the system debug menu items
	IMenuSegment& menuSegmentForSystem = menuSubmenu.CreateMenuSegment();
	debugMenuHandler->AddMenuItems(menuSegmentForSystem, viewModelLauncher);
	IMenuSeparator& menuSystemItemsSeparator = menuSegmentForSystem.AddMenuItemSeparator();

	//Add global debug menu entries, which appear above our device submenus
	IMenuSegment& menuSegmentForGlobalItems = menuSubmenu.CreateMenuSegment();
	for(DeviceArray::const_iterator i = devices.begin(); i != devices.end(); ++i)
	{
		IDeviceContext* device = *i;
		device->GetTargetDevice()->AddGlobalDebugMenuItems(menuSegmentForGlobalItems, viewModelLauncher);
	}

	//Add a separator after the global debug menu entries if items were added, otherwise
	//remove the global debug menu segment.
	bool menuGlobalItemsSeparatorExists = true;
	IMenuSegment& menuGlobalItemsSeparatorSegment = menuSubmenu.CreateMenuSegment();
	IMenuSeparator& menuGlobalItemsSeparator = menuGlobalItemsSeparatorSegment.AddMenuItemSeparator();
	if(menuSegmentForGlobalItems.NoMenuItemsExist())
	{
		menuSubmenu.DeleteMenuSegment(menuSegmentForGlobalItems);
		menuSubmenu.DeleteMenuSegment(menuGlobalItemsSeparatorSegment);
		menuGlobalItemsSeparatorExists = false;
	}

	//Build our module submenus
	IMenuSegment& menuSegmentForModules = menuSubmenu.CreateMenuSegment();
	for(LoadedModuleInfoList::const_iterator i = loadedModuleInfoList.begin(); i != loadedModuleInfoList.end(); ++i)
	{
		//Create a submenu for this module
		IMenuSubmenu& moduleSubmenu = menuSegmentForModules.AddMenuItemSubmenu(i->displayName);

		//Add the debug submenus for each device in this module
		IMenuSegment& menuSegmentForDevices = moduleSubmenu.CreateMenuSegment();
		for(LoadedDeviceInfoList::const_iterator deviceInfo = loadedDeviceInfoList.begin(); deviceInfo != loadedDeviceInfoList.end(); ++deviceInfo)
		{
			if(deviceInfo->moduleID == i->moduleID)
			{
				IMenuSubmenu& deviceSubmenu = menuSegmentForDevices.AddMenuItemSubmenu(deviceInfo->device->GetDeviceInstanceName());
				IMenuSegment& menuSegmentForDeviceSubmenu = deviceSubmenu.CreateMenuSegment();
				deviceInfo->device->AddDebugMenuItems(menuSegmentForDeviceSubmenu, viewModelLauncher);
				if(menuSegmentForDeviceSubmenu.NoMenuItemsExist())
				{
					menuSegmentForDevices.DeleteMenuItem(deviceSubmenu);
				}
			}
		}

		//If no device submenus were defined for this module, remove this module submenu.
		if(menuSegmentForDevices.NoMenuItemsExist())
		{
			menuSegmentForModules.DeleteMenuItem(moduleSubmenu);
		}
	}

	//If no module submenus were defined, remove the module segment, and ensure no
	//trailing separators are left on the menu.
	if(menuSegmentForModules.NoMenuItemsExist())
	{
		menuSubmenu.DeleteMenuSegment(menuSegmentForModules);
		if(menuGlobalItemsSeparatorExists)
		{
			menuGlobalItemsSeparatorSegment.DeleteMenuItem(menuGlobalItemsSeparator);
		}
		else
		{
			menuSegmentForSystem.DeleteMenuItem(menuSystemItemsSeparator);
		}
	}
}

//----------------------------------------------------------------------------------------
void System::RestoreViewModelState(const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	debugMenuHandler->RestoreMenuViewModelOpen(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewModelStateForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName, const std::wstring& menuHandlerName, int viewModelID, IHeirarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	//Attempt to find the referenced device
	IDevice* device = GetDevice(moduleID, deviceInstanceName);
	if(device == 0)
	{
		return false;
	}

	//Restore the view model state
	device->RestoreViewModelState(menuHandlerName, viewModelID, node, xpos, ypos, width, height, viewModelLauncher);
	return true;
}

//----------------------------------------------------------------------------------------
void System::DeleteSystemViews()
{
	debugMenuHandler->ClearMenuItems();
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void System::OpenLoggerDetailsView(const LogEntryInternal& alogEntry)
{
	debugMenuHandler->OpenLoggerDetailsView(alogEntry);
}
