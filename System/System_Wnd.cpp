#include "System.h"

//----------------------------------------------------------------------------------------
//View functions
//----------------------------------------------------------------------------------------
void System::BuildFileOpenMenu(IMenuSubmenu& menuSubmenu) const
{
	//Add all file open menu item entries from the set of registered system menu handlers
	IMenuSegment& menuSegmentForSystemMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for (std::set<IExtension*>::const_iterator menuHandlerIterator = _systemMenuHandlers.begin(); menuHandlerIterator != _systemMenuHandlers.end(); ++menuHandlerIterator)
	{
		(*menuHandlerIterator)->AddSystemMenuItems(IExtension::SystemMenu::File, menuSegmentForSystemMenuHandlers);
	}
}

//----------------------------------------------------------------------------------------
void System::BuildSystemMenu(IMenuSubmenu& menuSubmenu) const
{
	//Add all system menu item entries from the set of registered system menu handlers
	IMenuSegment& menuSegmentForSystemMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for (std::set<IExtension*>::const_iterator menuHandlerIterator = _systemMenuHandlers.begin(); menuHandlerIterator != _systemMenuHandlers.end(); ++menuHandlerIterator)
	{
		(*menuHandlerIterator)->AddSystemMenuItems(IExtension::SystemMenu::System, menuSegmentForSystemMenuHandlers);
	}
}

//----------------------------------------------------------------------------------------
void System::BuildSettingsMenu(IMenuSubmenu& menuSubmenu) const
{
	//Add all system menu item entries from the set of registered system menu handlers
	IMenuSegment& menuSegmentForSystemMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for (std::set<IExtension*>::const_iterator menuHandlerIterator = _systemMenuHandlers.begin(); menuHandlerIterator != _systemMenuHandlers.end(); ++menuHandlerIterator)
	{
		(*menuHandlerIterator)->AddSystemMenuItems(IExtension::SystemMenu::Settings, menuSegmentForSystemMenuHandlers);
	}

	//Add all extension settings menu item entries for each loaded global extension from
	//the list of registered menu handlers for this extension
	IMenuSegment& menuSegmentForGlobalExtensionsMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for (LoadedGlobalExtensionInfoList::const_iterator loadedGlobalExtensionIterator = _globalExtensionInfoList.begin(); loadedGlobalExtensionIterator != _globalExtensionInfoList.end(); ++loadedGlobalExtensionIterator)
	{
		const LoadedGlobalExtensionInfo& loadedGlobalExtensionInfo = loadedGlobalExtensionIterator->second;
		IMenuSubmenu& extensionSubmenu = menuSegmentForGlobalExtensionsMenuHandlers.AddMenuItemSubmenu(loadedGlobalExtensionInfo.extension->GetExtensionInstanceName());
		IMenuSegment& menuSegmentForExtensionSubmenu = extensionSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for (std::set<IExtension*>::const_iterator menuHandlerIterator = loadedGlobalExtensionInfo.menuHandlers.begin(); menuHandlerIterator != loadedGlobalExtensionInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			(*menuHandlerIterator)->AddExtensionMenuItems(IExtension::ExtensionMenu::Settings, menuSegmentForExtensionSubmenu, loadedGlobalExtensionInfo.extension);
		}
		if (menuSegmentForExtensionSubmenu.NoMenuItemsExist())
		{
			menuSegmentForGlobalExtensionsMenuHandlers.DeleteMenuItem(extensionSubmenu);
		}
	}

	//Build our module submenus
	IMenuSegment& menuSegmentForModules = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for (LoadedModuleInfoMap::const_iterator loadedModuleIterator = _loadedModuleInfoMap.begin(); loadedModuleIterator != _loadedModuleInfoMap.end(); ++loadedModuleIterator)
	{
		const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleIterator->second;

		//Create a submenu for this module
		IMenuSubmenu& moduleSubmenu = menuSegmentForModules.AddMenuItemSubmenu(loadedModuleInfo.displayName);

		//Add all module settings menu item entries from the set of registered menu
		//handlers for this module
		IMenuSegment& menuSegmentForModuleMenuHandlers = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for (std::set<IExtension*>::const_iterator menuHandlerIterator = loadedModuleInfo.menuHandlers.begin(); menuHandlerIterator != loadedModuleInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			(*menuHandlerIterator)->AddModuleMenuItems(IExtension::ModuleMenu::Settings, menuSegmentForModuleMenuHandlers, loadedModuleInfo.moduleID);
		}

		//Add all device settings menu item entries for each device in this module from
		//the list of registered menu handlers for this device
		IMenuSegment& menuSegmentForDeviceSettings = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for (LoadedDeviceInfoList::const_iterator deviceInfo = _loadedDeviceInfoList.begin(); deviceInfo != _loadedDeviceInfoList.end(); ++deviceInfo)
		{
			if (deviceInfo->moduleID == loadedModuleInfo.moduleID)
			{
				IMenuSubmenu& deviceSubmenu = menuSegmentForDeviceSettings.AddMenuItemSubmenu(deviceInfo->device->GetDeviceInstanceName());
				IMenuSegment& menuSegmentForDeviceSubmenu = deviceSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
				for (std::set<IExtension*>::const_iterator menuHandlerIterator = deviceInfo->menuHandlers.begin(); menuHandlerIterator != deviceInfo->menuHandlers.end(); ++menuHandlerIterator)
				{
					(*menuHandlerIterator)->AddDeviceMenuItems(IExtension::DeviceMenu::Settings, menuSegmentForDeviceSubmenu, deviceInfo->device);
				}
				if (menuSegmentForDeviceSubmenu.NoMenuItemsExist())
				{
					menuSegmentForDeviceSettings.DeleteMenuItem(deviceSubmenu);
				}
			}
		}

		//Add all extension settings menu item entries for each extension in this module
		//from the list of registered menu handlers for this extension
		IMenuSegment& menuSegmentForExtensionSettings = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for (LoadedExtensionInfoList::const_iterator extensionInfo = _loadedExtensionInfoList.begin(); extensionInfo != _loadedExtensionInfoList.end(); ++extensionInfo)
		{
			if (extensionInfo->moduleID == loadedModuleInfo.moduleID)
			{
				IMenuSubmenu& extensionSubmenu = menuSegmentForExtensionSettings.AddMenuItemSubmenu(extensionInfo->extension->GetExtensionInstanceName());
				IMenuSegment& menuSegmentForExtensionSubmenu = extensionSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
				for (std::set<IExtension*>::const_iterator menuHandlerIterator = extensionInfo->menuHandlers.begin(); menuHandlerIterator != extensionInfo->menuHandlers.end(); ++menuHandlerIterator)
				{
					(*menuHandlerIterator)->AddExtensionMenuItems(IExtension::ExtensionMenu::Settings, menuSegmentForExtensionSubmenu, extensionInfo->extension);
				}
				if (menuSegmentForExtensionSubmenu.NoMenuItemsExist())
				{
					menuSegmentForExtensionSettings.DeleteMenuItem(extensionSubmenu);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
void System::BuildDebugMenu(IMenuSubmenu& menuSubmenu) const
{
	//Add all system menu item entries from the set of registered system menu handlers
	IMenuSegment& menuSegmentForSystemMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for (std::set<IExtension*>::const_iterator menuHandlerIterator = _systemMenuHandlers.begin(); menuHandlerIterator != _systemMenuHandlers.end(); ++menuHandlerIterator)
	{
		(*menuHandlerIterator)->AddSystemMenuItems(IExtension::SystemMenu::Debug, menuSegmentForSystemMenuHandlers);
	}

	//Add all extension settings menu item entries for each loaded global extension from
	//the list of registered menu handlers for this extension
	IMenuSegment& menuSegmentForGlobalExtensionsMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for (LoadedGlobalExtensionInfoList::const_iterator loadedGlobalExtensionIterator = _globalExtensionInfoList.begin(); loadedGlobalExtensionIterator != _globalExtensionInfoList.end(); ++loadedGlobalExtensionIterator)
	{
		const LoadedGlobalExtensionInfo& loadedGlobalExtensionInfo = loadedGlobalExtensionIterator->second;
		IMenuSubmenu& extensionSubmenu = menuSegmentForGlobalExtensionsMenuHandlers.AddMenuItemSubmenu(loadedGlobalExtensionInfo.extension->GetExtensionInstanceName());
		IMenuSegment& menuSegmentForExtensionSubmenu = extensionSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for (std::set<IExtension*>::const_iterator menuHandlerIterator = loadedGlobalExtensionInfo.menuHandlers.begin(); menuHandlerIterator != loadedGlobalExtensionInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			(*menuHandlerIterator)->AddExtensionMenuItems(IExtension::ExtensionMenu::Debug, menuSegmentForExtensionSubmenu, loadedGlobalExtensionInfo.extension);
		}
		if (menuSegmentForExtensionSubmenu.NoMenuItemsExist())
		{
			menuSegmentForGlobalExtensionsMenuHandlers.DeleteMenuItem(extensionSubmenu);
		}
	}

	//Build our module submenus
	IMenuSegment& menuSegmentForModules = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for (LoadedModuleInfoMap::const_iterator loadedModuleIterator = _loadedModuleInfoMap.begin(); loadedModuleIterator != _loadedModuleInfoMap.end(); ++loadedModuleIterator)
	{
		const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleIterator->second;

		//Create a submenu for this module
		IMenuSubmenu& moduleSubmenu = menuSegmentForModules.AddMenuItemSubmenu(loadedModuleInfo.displayName);

		//Add all module debug menu item entries from the set of registered menu handlers
		//for this module
		IMenuSegment& menuSegmentForModuleMenuHandlers = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for (std::set<IExtension*>::const_iterator menuHandlerIterator = loadedModuleInfo.menuHandlers.begin(); menuHandlerIterator != loadedModuleInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			(*menuHandlerIterator)->AddModuleMenuItems(IExtension::ModuleMenu::Debug, menuSegmentForModuleMenuHandlers, loadedModuleInfo.moduleID);
		}

		//Add all device debug menu item entries for each device in this module from the
		//list of registered menu handlers for this device
		IMenuSegment& menuSegmentForDeviceSettings = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for (LoadedDeviceInfoList::const_iterator deviceInfo = _loadedDeviceInfoList.begin(); deviceInfo != _loadedDeviceInfoList.end(); ++deviceInfo)
		{
			if (deviceInfo->moduleID == loadedModuleInfo.moduleID)
			{
				IMenuSubmenu& deviceSubmenu = menuSegmentForDeviceSettings.AddMenuItemSubmenu(deviceInfo->device->GetDeviceInstanceName());
				IMenuSegment& menuSegmentForDeviceSubmenu = deviceSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
				for (std::set<IExtension*>::const_iterator menuHandlerIterator = deviceInfo->menuHandlers.begin(); menuHandlerIterator != deviceInfo->menuHandlers.end(); ++menuHandlerIterator)
				{
					(*menuHandlerIterator)->AddDeviceMenuItems(IExtension::DeviceMenu::Debug, menuSegmentForDeviceSubmenu, deviceInfo->device);
				}
				if (menuSegmentForDeviceSubmenu.NoMenuItemsExist())
				{
					menuSegmentForDeviceSettings.DeleteMenuItem(deviceSubmenu);
				}
			}
		}

		//Add all extension debug menu item entries for each extension in this module from
		//the list of registered menu handlers for this extension
		IMenuSegment& menuSegmentForExtensionSettings = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for (LoadedExtensionInfoList::const_iterator extensionInfo = _loadedExtensionInfoList.begin(); extensionInfo != _loadedExtensionInfoList.end(); ++extensionInfo)
		{
			if (extensionInfo->moduleID == loadedModuleInfo.moduleID)
			{
				IMenuSubmenu& extensionSubmenu = menuSegmentForExtensionSettings.AddMenuItemSubmenu(extensionInfo->extension->GetExtensionInstanceName());
				IMenuSegment& menuSegmentForExtensionSubmenu = extensionSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
				for (std::set<IExtension*>::const_iterator menuHandlerIterator = extensionInfo->menuHandlers.begin(); menuHandlerIterator != extensionInfo->menuHandlers.end(); ++menuHandlerIterator)
				{
					(*menuHandlerIterator)->AddExtensionMenuItems(IExtension::ExtensionMenu::Debug, menuSegmentForExtensionSubmenu, extensionInfo->extension);
				}
				if (menuSegmentForExtensionSubmenu.NoMenuItemsExist())
				{
					menuSegmentForExtensionSettings.DeleteMenuItem(extensionSubmenu);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForSystem(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter) const
{
	std::wstring viewGroupNameCached = viewGroupName.Get();
	std::wstring viewNameCached = viewName.Get();
	bool result = false;
	for (std::set<IExtension*>::const_iterator i = _systemMenuHandlers.begin(); i != _systemMenuHandlers.end(); ++i)
	{
		result |= (*i)->RestoreSystemViewState(viewGroupNameCached, viewNameCached, viewState, restoredViewPresenter);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForModule(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, unsigned int moduleID) const
{
	std::wstring viewGroupNameCached = viewGroupName.Get();
	std::wstring viewNameCached = viewName.Get();
	bool result = false;
	LoadedModuleInfoMap::const_iterator loadedModuleInfoIterator = _loadedModuleInfoMap.find(moduleID);
	if (loadedModuleInfoIterator != _loadedModuleInfoMap.end())
	{
		const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleInfoIterator->second;
		for (std::set<IExtension*>::const_iterator menuHandlerIterator = loadedModuleInfo.menuHandlers.begin(); menuHandlerIterator != loadedModuleInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			result |= (*menuHandlerIterator)->RestoreModuleViewState(viewGroupNameCached, viewNameCached, viewState, restoredViewPresenter, moduleID);
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForDevice(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, unsigned int moduleID, const Marshal::In<std::wstring>& deviceInstanceName) const
{
	std::wstring viewGroupNameCached = viewGroupName.Get();
	std::wstring viewNameCached = viewName.Get();
	std::wstring deviceInstanceNameCached = deviceInstanceName;
	bool result = false;
	bool foundDevice = false;
	LoadedDeviceInfoList::const_iterator loadedDeviceIterator = _loadedDeviceInfoList.begin();
	while (!foundDevice && (loadedDeviceIterator != _loadedDeviceInfoList.end()))
	{
		if ((loadedDeviceIterator->moduleID == moduleID) && (loadedDeviceIterator->name == deviceInstanceNameCached))
		{
			foundDevice = true;
			for (std::set<IExtension*>::const_iterator menuHandlerIterator = loadedDeviceIterator->menuHandlers.begin(); menuHandlerIterator != loadedDeviceIterator->menuHandlers.end(); ++menuHandlerIterator)
			{
				result |= (*menuHandlerIterator)->RestoreDeviceViewState(viewGroupNameCached, viewNameCached, viewState, restoredViewPresenter, loadedDeviceIterator->device);
			}
		}
		++loadedDeviceIterator;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForExtension(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, const Marshal::In<std::wstring>& extensionInstanceName) const
{
	std::wstring viewGroupNameCached = viewGroupName.Get();
	std::wstring viewNameCached = viewName.Get();
	std::wstring extensionInstanceNameCached = extensionInstanceName.Get();
	bool result = false;
	bool foundDevice = false;
	LoadedGlobalExtensionInfoList::const_iterator loadedGlobalExtensionIterator = _globalExtensionInfoList.begin();
	while (!foundDevice && (loadedGlobalExtensionIterator != _globalExtensionInfoList.end()))
	{
		const LoadedGlobalExtensionInfo& loadedGlobalExtensionInfo = loadedGlobalExtensionIterator->second;
		if (loadedGlobalExtensionInfo.name == extensionInstanceNameCached)
		{
			foundDevice = true;
			for (std::set<IExtension*>::const_iterator menuHandlerIterator = loadedGlobalExtensionInfo.menuHandlers.begin(); menuHandlerIterator != loadedGlobalExtensionInfo.menuHandlers.end(); ++menuHandlerIterator)
			{
				result |= (*menuHandlerIterator)->RestoreExtensionViewState(viewGroupNameCached, viewNameCached, viewState, restoredViewPresenter, loadedGlobalExtensionInfo.extension);
			}
		}
		++loadedGlobalExtensionIterator;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForExtension(const Marshal::In<std::wstring>& viewGroupName, const Marshal::In<std::wstring>& viewName, IHierarchicalStorageNode& viewState, IViewPresenter** restoredViewPresenter, unsigned int moduleID, const Marshal::In<std::wstring>& extensionInstanceName) const
{
	std::wstring viewGroupNameCached = viewGroupName.Get();
	std::wstring viewNameCached = viewName.Get();
	std::wstring extensionInstanceNameCached = extensionInstanceName.Get();
	bool result = false;
	bool foundDevice = false;
	LoadedExtensionInfoList::const_iterator loadedExtensionIterator = _loadedExtensionInfoList.begin();
	while (!foundDevice && (loadedExtensionIterator != _loadedExtensionInfoList.end()))
	{
		if ((loadedExtensionIterator->moduleID == moduleID) && (loadedExtensionIterator->name == extensionInstanceNameCached))
		{
			foundDevice = true;
			for (std::set<IExtension*>::const_iterator menuHandlerIterator = loadedExtensionIterator->menuHandlers.begin(); menuHandlerIterator != loadedExtensionIterator->menuHandlers.end(); ++menuHandlerIterator)
			{
				result |= (*menuHandlerIterator)->RestoreExtensionViewState(viewGroupNameCached, viewNameCached, viewState, restoredViewPresenter, loadedExtensionIterator->extension);
			}
		}
		++loadedExtensionIterator;
	}
	return result;
}
