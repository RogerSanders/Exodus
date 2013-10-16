#include "System.h"
#include "SystemMenuHandler.h"
#include "DebugMenuHandler.h"

//----------------------------------------------------------------------------------------
//View functions
//----------------------------------------------------------------------------------------
void System::BuildFileOpenMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher)
{
	//Create a menu segment for loaded extensions
	IMenuSegment& menuSegmentForExtensions = menuSubmenu.CreateMenuSegment();

	//Add all file open menu item entries from the list of loaded global extensions
	for(LoadedGlobalExtensionInfoList::const_iterator extensionIterator = globalExtensionInfoList.begin(); extensionIterator != globalExtensionInfoList.end(); ++extensionIterator)
	{
		extensionIterator->second.extension->AddFileOpenMenuItems(menuSegmentForExtensions, viewModelLauncher);
	}

	//Add all file open menu item entries from the list of loaded extensions
	for(LoadedExtensionInfoList::const_iterator extensionIterator = loadedExtensionInfoList.begin(); extensionIterator != loadedExtensionInfoList.end(); ++extensionIterator)
	{
		extensionIterator->extension->AddFileOpenMenuItems(menuSegmentForExtensions, viewModelLauncher);
	}

	//If no extension menu items were defined, remove the extension segment.
	if(menuSegmentForExtensions.NoMenuItemsExist())
	{
		menuSubmenu.DeleteMenuSegment(menuSegmentForExtensions);
	}
}

//----------------------------------------------------------------------------------------
void System::BuildSystemMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher)
{
	//Add the system menu items
	IMenuSegment& menuSegmentForSystem = menuSubmenu.CreateMenuSegment();
	systemMenuHandler->AddMenuItems(menuSegmentForSystem, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
void System::BuildSettingsMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher)
{
	//Add the settings menu items
	IMenuSegment& menuSegmentForSystem = menuSubmenu.CreateMenuSegment();
	settingsMenuHandler->AddMenuItems(menuSegmentForSystem, viewModelLauncher);
	IMenuSeparator& menuSystemItemsSeparator = menuSegmentForSystem.AddMenuItemSeparator();

	//Build our module submenus
	IMenuSegment& menuSegmentForModules = menuSubmenu.CreateMenuSegment();
	for(ModuleSystemSettingMap::iterator moduleSettingsIterator = moduleSettings.begin(); moduleSettingsIterator != moduleSettings.end(); ++moduleSettingsIterator)
	{
		unsigned int targetModuleID = moduleSettingsIterator->first;
		SystemSettingsIDList& moduleSettings = moduleSettingsIterator->second;

		//Retrieve the display name for the target module
		std::wstring moduleDisplayName;
		if(!GetModuleDisplayName(targetModuleID, moduleDisplayName))
		{
			continue;
		}

		//Create a submenu for this module
		IMenuSubmenu& moduleSubmenu = menuSegmentForModules.AddMenuItemSubmenu(moduleDisplayName);

		//Add menu items for each system option defined in this module
		IMenuSegment& menuSegmentForSettings = moduleSubmenu.CreateMenuSegment();
		for(SystemSettingsIDList::const_iterator settingsIDIterator = moduleSettings.begin(); settingsIDIterator != moduleSettings.end(); ++settingsIDIterator)
		{
			//Retrieve the info for the target system setting
			unsigned int systemSettingID = *settingsIDIterator;
			SystemSettingsMap::iterator settingsIterator = systemSettings.find(systemSettingID);
			if(settingsIterator == systemSettings.end())
			{
				continue;
			}
			SystemSettingInfo& systemSettingInfo = settingsIterator->second;

			//Build the menu for this system setting
			if(systemSettingInfo.toggleSetting)
			{
				IMenuSelectableOption& settingMenuItem = menuSegmentForSettings.AddMenuItemSelectableOption(*systemOptionMenuHandler, systemSettingInfo.menuItemID, systemSettingInfo.displayName);
				systemSettingInfo.menuItemEntry = &settingMenuItem;
				settingMenuItem.SetCheckedState(systemSettingInfo.selectedOption == systemSettingInfo.onOption);
			}
			else
			{
				IMenuSubmenu& settingSubmenu = menuSegmentForSettings.AddMenuItemSubmenu(systemSettingInfo.displayName);
				IMenuSegment& menuSegmentForSettingSubmenu = settingSubmenu.CreateMenuSegment();
				for(unsigned int i = 0; i < systemSettingInfo.options.size(); ++i)
				{
					IMenuSelectableOption& optionMenuItem = menuSegmentForSettingSubmenu.AddMenuItemSelectableOption(*systemOptionMenuHandler, systemSettingInfo.options[i].menuItemID, systemSettingInfo.options[i].displayName);
					systemSettingInfo.options[i].menuItemEntry = &optionMenuItem;
					if(systemSettingInfo.selectedOption == i)
					{
						optionMenuItem.SetCheckedState(true);
					}
				}
			}
		}

		//Create a separator between the system settings and the device settings
		IMenuSeparator& menuSettingsSeparator = menuSegmentForSettings.AddMenuItemSeparator();

		//Add the settings submenus for each device in this module
		IMenuSegment& menuSegmentForDevices = moduleSubmenu.CreateMenuSegment();
		for(LoadedDeviceInfoList::const_iterator deviceInfo = loadedDeviceInfoList.begin(); deviceInfo != loadedDeviceInfoList.end(); ++deviceInfo)
		{
			if(deviceInfo->moduleID == targetModuleID)
			{
				IMenuSubmenu& deviceSubmenu = menuSegmentForDevices.AddMenuItemSubmenu(deviceInfo->device->GetDeviceInstanceName());
				IMenuSegment& menuSegmentForDeviceSubmenu = deviceSubmenu.CreateMenuSegment();
				deviceInfo->device->AddSettingsMenuItems(menuSegmentForDeviceSubmenu, viewModelLauncher);
				if(menuSegmentForDeviceSubmenu.NoMenuItemsExist())
				{
					menuSegmentForDevices.DeleteMenuItem(deviceSubmenu);
				}
			}
		}

		//If no device submenus were defined, remove the device segment, and ensure no
		//trailing separators are left on the menu.
		if(menuSegmentForDevices.NoMenuItemsExist())
		{
			moduleSubmenu.DeleteMenuSegment(menuSegmentForDevices);
			menuSegmentForSettings.DeleteMenuItem(menuSettingsSeparator);
		}
	}

	//If no module submenus were defined, remove the module segment, and ensure no
	//trailing separators are left on the menu.
	if(menuSegmentForModules.NoMenuItemsExist())
	{
		menuSubmenu.DeleteMenuSegment(menuSegmentForModules);
		menuSegmentForSystem.DeleteMenuItem(menuSystemItemsSeparator);
	}
}

//----------------------------------------------------------------------------------------
void System::BuildDebugMenu(IMenuSubmenu& menuSubmenu, IViewModelLauncher& viewModelLauncher)
{
	//Add the system debug menu items
	IMenuSegment& menuSegmentForSystem = menuSubmenu.CreateMenuSegment();
	debugMenuHandler->AddMenuItems(menuSegmentForSystem, viewModelLauncher);
	IMenuSeparator& menuSystemItemsSeparator = menuSegmentForSystem.AddMenuItemSeparator();

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
		menuSegmentForSystem.DeleteMenuItem(menuSystemItemsSeparator);
	}
}

//----------------------------------------------------------------------------------------
void System::RestoreViewModelState(const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	debugMenuHandler->RestoreMenuViewModelOpen(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewModelStateForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName, const std::wstring& viewModelGroupName, const std::wstring& viewModelName, IHierarchicalStorageNode& node, int xpos, int ypos, int width, int height, IViewModelLauncher& viewModelLauncher)
{
	//Attempt to find the referenced device
	IDevice* device = GetDevice(moduleID, deviceInstanceName);
	if(device == 0)
	{
		return false;
	}

	//Restore the view model state
	device->RestoreViewModelState(viewModelGroupName, viewModelName, node, xpos, ypos, width, height, viewModelLauncher);
	return true;
}

//----------------------------------------------------------------------------------------
void System::DeleteSystemViews()
{
	systemMenuHandler->ClearMenuItems();
	debugMenuHandler->ClearMenuItems();
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void System::OpenLoggerDetailsView(const LogEntryInternal& alogEntry)
{
	systemMenuHandler->OpenLoggerDetailsView(alogEntry);
}

//----------------------------------------------------------------------------------------
void System::OpenInputMappingDetailsView(IDevice* targetDevice)
{
	settingsMenuHandler->OpenInputMappingDetailsView(targetDevice);
}

//----------------------------------------------------------------------------------------
void System::CloseInputMappingDetailsView()
{
	settingsMenuHandler->CloseInputMappingDetailsView();
}
