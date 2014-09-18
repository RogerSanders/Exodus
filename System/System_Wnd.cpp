#include "System.h"

//----------------------------------------------------------------------------------------
//View functions
//----------------------------------------------------------------------------------------
void System::BuildFileOpenMenu(IMenuSubmenu& menuSubmenu) const
{
	//Add all file open menu item entries from the set of registered system menu handlers
	IMenuSegment& menuSegmentForSystemMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for(std::set<IExtension*>::const_iterator menuHandlerIterator = systemMenuHandlers.begin(); menuHandlerIterator != systemMenuHandlers.end(); ++menuHandlerIterator)
	{
		(*menuHandlerIterator)->AddSystemMenuItems(IExtension::SYSTEMMENU_FILE, menuSegmentForSystemMenuHandlers);
	}
}

//----------------------------------------------------------------------------------------
void System::BuildSystemMenu(IMenuSubmenu& menuSubmenu) const
{
	//Add all system menu item entries from the set of registered system menu handlers
	IMenuSegment& menuSegmentForSystemMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for(std::set<IExtension*>::const_iterator menuHandlerIterator = systemMenuHandlers.begin(); menuHandlerIterator != systemMenuHandlers.end(); ++menuHandlerIterator)
	{
		(*menuHandlerIterator)->AddSystemMenuItems(IExtension::SYSTEMMENU_SYSTEM, menuSegmentForSystemMenuHandlers);
	}
}

//----------------------------------------------------------------------------------------
void System::BuildSettingsMenu(IMenuSubmenu& menuSubmenu) const
{
	//Add all system menu item entries from the set of registered system menu handlers
	IMenuSegment& menuSegmentForSystemMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for(std::set<IExtension*>::const_iterator menuHandlerIterator = systemMenuHandlers.begin(); menuHandlerIterator != systemMenuHandlers.end(); ++menuHandlerIterator)
	{
		(*menuHandlerIterator)->AddSystemMenuItems(IExtension::SYSTEMMENU_SETTINGS, menuSegmentForSystemMenuHandlers);
	}

	//Add all extension settings menu item entries for each loaded global extension from
	//the list of registered menu handlers for this extension
	IMenuSegment& menuSegmentForGlobalExtensionsMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for(LoadedGlobalExtensionInfoList::const_iterator loadedGlobalExtensionIterator = globalExtensionInfoList.begin(); loadedGlobalExtensionIterator != globalExtensionInfoList.end(); ++loadedGlobalExtensionIterator)
	{
		const LoadedGlobalExtensionInfo& loadedGlobalExtensionInfo = loadedGlobalExtensionIterator->second;
		IMenuSubmenu& extensionSubmenu = menuSegmentForGlobalExtensionsMenuHandlers.AddMenuItemSubmenu(loadedGlobalExtensionInfo.extension->GetExtensionInstanceName());
		IMenuSegment& menuSegmentForExtensionSubmenu = extensionSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedGlobalExtensionInfo.menuHandlers.begin(); menuHandlerIterator != loadedGlobalExtensionInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			(*menuHandlerIterator)->AddExtensionMenuItems(IExtension::EXTENSIONMENU_SETTINGS, menuSegmentForExtensionSubmenu, loadedGlobalExtensionInfo.extension);
		}
		if(menuSegmentForExtensionSubmenu.NoMenuItemsExist())
		{
			menuSegmentForGlobalExtensionsMenuHandlers.DeleteMenuItem(extensionSubmenu);
		}
	}

	//Build our module submenus
	IMenuSegment& menuSegmentForModules = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for(LoadedModuleInfoMap::const_iterator loadedModuleIterator = loadedModuleInfoMap.begin(); loadedModuleIterator != loadedModuleInfoMap.end(); ++loadedModuleIterator)
	{
		const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleIterator->second;

		//Create a submenu for this module
		IMenuSubmenu& moduleSubmenu = menuSegmentForModules.AddMenuItemSubmenu(loadedModuleInfo.displayName);

		//Add all module settings menu item entries from the set of registered menu
		//handlers for this module
		IMenuSegment& menuSegmentForModuleMenuHandlers = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedModuleInfo.menuHandlers.begin(); menuHandlerIterator != loadedModuleInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			(*menuHandlerIterator)->AddModuleMenuItems(IExtension::MODULEMENU_SETTINGS, menuSegmentForModuleMenuHandlers, loadedModuleInfo.moduleID);
		}

		//Add all device settings menu item entries for each device in this module from
		//the list of registered menu handlers for this device
		IMenuSegment& menuSegmentForDeviceSettings = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for(LoadedDeviceInfoList::const_iterator deviceInfo = loadedDeviceInfoList.begin(); deviceInfo != loadedDeviceInfoList.end(); ++deviceInfo)
		{
			if(deviceInfo->moduleID == loadedModuleInfo.moduleID)
			{
				IMenuSubmenu& deviceSubmenu = menuSegmentForDeviceSettings.AddMenuItemSubmenu(deviceInfo->device->GetDeviceInstanceName());
				IMenuSegment& menuSegmentForDeviceSubmenu = deviceSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
				for(std::set<IExtension*>::const_iterator menuHandlerIterator = deviceInfo->menuHandlers.begin(); menuHandlerIterator != deviceInfo->menuHandlers.end(); ++menuHandlerIterator)
				{
					(*menuHandlerIterator)->AddDeviceMenuItems(IExtension::DEVICEMENU_SETTINGS, menuSegmentForDeviceSubmenu, deviceInfo->device);
				}
				if(menuSegmentForDeviceSubmenu.NoMenuItemsExist())
				{
					menuSegmentForDeviceSettings.DeleteMenuItem(deviceSubmenu);
				}
			}
		}

		//Add all extension settings menu item entries for each extension in this module
		//from the list of registered menu handlers for this extension
		IMenuSegment& menuSegmentForExtensionSettings = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for(LoadedExtensionInfoList::const_iterator extensionInfo = loadedExtensionInfoList.begin(); extensionInfo != loadedExtensionInfoList.end(); ++extensionInfo)
		{
			if(extensionInfo->moduleID == loadedModuleInfo.moduleID)
			{
				IMenuSubmenu& extensionSubmenu = menuSegmentForExtensionSettings.AddMenuItemSubmenu(extensionInfo->extension->GetExtensionInstanceName());
				IMenuSegment& menuSegmentForExtensionSubmenu = extensionSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
				for(std::set<IExtension*>::const_iterator menuHandlerIterator = extensionInfo->menuHandlers.begin(); menuHandlerIterator != extensionInfo->menuHandlers.end(); ++menuHandlerIterator)
				{
					(*menuHandlerIterator)->AddExtensionMenuItems(IExtension::EXTENSIONMENU_SETTINGS, menuSegmentForExtensionSubmenu, extensionInfo->extension);
				}
				if(menuSegmentForExtensionSubmenu.NoMenuItemsExist())
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
	for(std::set<IExtension*>::const_iterator menuHandlerIterator = systemMenuHandlers.begin(); menuHandlerIterator != systemMenuHandlers.end(); ++menuHandlerIterator)
	{
		(*menuHandlerIterator)->AddSystemMenuItems(IExtension::SYSTEMMENU_DEBUG, menuSegmentForSystemMenuHandlers);
	}

	//Add all extension settings menu item entries for each loaded global extension from
	//the list of registered menu handlers for this extension
	IMenuSegment& menuSegmentForGlobalExtensionsMenuHandlers = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for(LoadedGlobalExtensionInfoList::const_iterator loadedGlobalExtensionIterator = globalExtensionInfoList.begin(); loadedGlobalExtensionIterator != globalExtensionInfoList.end(); ++loadedGlobalExtensionIterator)
	{
		const LoadedGlobalExtensionInfo& loadedGlobalExtensionInfo = loadedGlobalExtensionIterator->second;
		IMenuSubmenu& extensionSubmenu = menuSegmentForGlobalExtensionsMenuHandlers.AddMenuItemSubmenu(loadedGlobalExtensionInfo.extension->GetExtensionInstanceName());
		IMenuSegment& menuSegmentForExtensionSubmenu = extensionSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedGlobalExtensionInfo.menuHandlers.begin(); menuHandlerIterator != loadedGlobalExtensionInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			(*menuHandlerIterator)->AddExtensionMenuItems(IExtension::EXTENSIONMENU_DEBUG, menuSegmentForExtensionSubmenu, loadedGlobalExtensionInfo.extension);
		}
		if(menuSegmentForExtensionSubmenu.NoMenuItemsExist())
		{
			menuSegmentForGlobalExtensionsMenuHandlers.DeleteMenuItem(extensionSubmenu);
		}
	}

	//Build our module submenus
	IMenuSegment& menuSegmentForModules = menuSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
	for(LoadedModuleInfoMap::const_iterator loadedModuleIterator = loadedModuleInfoMap.begin(); loadedModuleIterator != loadedModuleInfoMap.end(); ++loadedModuleIterator)
	{
		const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleIterator->second;

		//Create a submenu for this module
		IMenuSubmenu& moduleSubmenu = menuSegmentForModules.AddMenuItemSubmenu(loadedModuleInfo.displayName);

		//Add all module debug menu item entries from the set of registered menu handlers
		//for this module
		IMenuSegment& menuSegmentForModuleMenuHandlers = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedModuleInfo.menuHandlers.begin(); menuHandlerIterator != loadedModuleInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			(*menuHandlerIterator)->AddModuleMenuItems(IExtension::MODULEMENU_DEBUG, menuSegmentForModuleMenuHandlers, loadedModuleInfo.moduleID);
		}

		//Add all device debug menu item entries for each device in this module from the
		//list of registered menu handlers for this device
		IMenuSegment& menuSegmentForDeviceSettings = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for(LoadedDeviceInfoList::const_iterator deviceInfo = loadedDeviceInfoList.begin(); deviceInfo != loadedDeviceInfoList.end(); ++deviceInfo)
		{
			if(deviceInfo->moduleID == loadedModuleInfo.moduleID)
			{
				IMenuSubmenu& deviceSubmenu = menuSegmentForDeviceSettings.AddMenuItemSubmenu(deviceInfo->device->GetDeviceInstanceName());
				IMenuSegment& menuSegmentForDeviceSubmenu = deviceSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
				for(std::set<IExtension*>::const_iterator menuHandlerIterator = deviceInfo->menuHandlers.begin(); menuHandlerIterator != deviceInfo->menuHandlers.end(); ++menuHandlerIterator)
				{
					(*menuHandlerIterator)->AddDeviceMenuItems(IExtension::DEVICEMENU_DEBUG, menuSegmentForDeviceSubmenu, deviceInfo->device);
				}
				if(menuSegmentForDeviceSubmenu.NoMenuItemsExist())
				{
					menuSegmentForDeviceSettings.DeleteMenuItem(deviceSubmenu);
				}
			}
		}

		//Add all extension debug menu item entries for each extension in this module from
		//the list of registered menu handlers for this extension
		IMenuSegment& menuSegmentForExtensionSettings = moduleSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
		for(LoadedExtensionInfoList::const_iterator extensionInfo = loadedExtensionInfoList.begin(); extensionInfo != loadedExtensionInfoList.end(); ++extensionInfo)
		{
			if(extensionInfo->moduleID == loadedModuleInfo.moduleID)
			{
				IMenuSubmenu& extensionSubmenu = menuSegmentForExtensionSettings.AddMenuItemSubmenu(extensionInfo->extension->GetExtensionInstanceName());
				IMenuSegment& menuSegmentForExtensionSubmenu = extensionSubmenu.AddMenuItemSegment(true, IMenuSegment::SORTMODE_TITLE);
				for(std::set<IExtension*>::const_iterator menuHandlerIterator = extensionInfo->menuHandlers.begin(); menuHandlerIterator != extensionInfo->menuHandlers.end(); ++menuHandlerIterator)
				{
					(*menuHandlerIterator)->AddExtensionMenuItems(IExtension::EXTENSIONMENU_DEBUG, menuSegmentForExtensionSubmenu, extensionInfo->extension);
				}
				if(menuSegmentForExtensionSubmenu.NoMenuItemsExist())
				{
					menuSegmentForExtensionSettings.DeleteMenuItem(extensionSubmenu);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForSystem(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState) const
{
	bool result = false;
	for(std::set<IExtension*>::const_iterator i = systemMenuHandlers.begin(); i != systemMenuHandlers.end(); ++i)
	{
		result |= (*i)->RestoreSystemViewState(viewGroupName, viewName, viewState);
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForSystemInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState) const
{
	return RestoreViewStateForSystem(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), viewState);
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForModule(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID) const
{
	bool result = false;
	LoadedModuleInfoMap::const_iterator loadedModuleInfoIterator = loadedModuleInfoMap.find(moduleID);
	if(loadedModuleInfoIterator != loadedModuleInfoMap.end())
	{
		const LoadedModuleInfoInternal& loadedModuleInfo = loadedModuleInfoIterator->second;
		for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedModuleInfo.menuHandlers.begin(); menuHandlerIterator != loadedModuleInfo.menuHandlers.end(); ++menuHandlerIterator)
		{
			result |= (*menuHandlerIterator)->RestoreModuleViewState(viewGroupName, viewName, viewState, moduleID);
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForModuleInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, unsigned int moduleID) const
{
	return RestoreViewStateForModule(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), viewState, moduleID);
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForDevice(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const std::wstring& deviceInstanceName) const
{
	bool result = false;
	bool foundDevice = false;
	LoadedDeviceInfoList::const_iterator loadedDeviceIterator = loadedDeviceInfoList.begin();
	while(!foundDevice && (loadedDeviceIterator != loadedDeviceInfoList.end()))
	{
		if((loadedDeviceIterator->moduleID == moduleID) && (loadedDeviceIterator->name == deviceInstanceName))
		{
			foundDevice = true;
			for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedDeviceIterator->menuHandlers.begin(); menuHandlerIterator != loadedDeviceIterator->menuHandlers.end(); ++menuHandlerIterator)
			{
				result |= (*menuHandlerIterator)->RestoreDeviceViewState(viewGroupName, viewName, viewState, loadedDeviceIterator->device);
			}
		}
		++loadedDeviceIterator;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForDeviceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, unsigned int moduleID, const InteropSupport::ISTLObjectSource<std::wstring>& deviceInstanceNameMarshaller) const
{
	return RestoreViewStateForDevice(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), viewState, moduleID, deviceInstanceNameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForExtension(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, const std::wstring& extensionInstanceName) const
{
	bool result = false;
	bool foundDevice = false;
	LoadedGlobalExtensionInfoList::const_iterator loadedGlobalExtensionIterator = globalExtensionInfoList.begin();
	while(!foundDevice && (loadedGlobalExtensionIterator != globalExtensionInfoList.end()))
	{
		const LoadedGlobalExtensionInfo& loadedGlobalExtensionInfo = loadedGlobalExtensionIterator->second;
		if(loadedGlobalExtensionInfo.name == extensionInstanceName)
		{
			foundDevice = true;
			for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedGlobalExtensionInfo.menuHandlers.begin(); menuHandlerIterator != loadedGlobalExtensionInfo.menuHandlers.end(); ++menuHandlerIterator)
			{
				result |= (*menuHandlerIterator)->RestoreExtensionViewState(viewGroupName, viewName, viewState, loadedGlobalExtensionInfo.extension);
			}
		}
		++loadedGlobalExtensionIterator;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForExtensionInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, const InteropSupport::ISTLObjectSource<std::wstring>& extensionInstanceNameMarshaller) const
{
	return RestoreViewStateForExtension(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), viewState, extensionInstanceNameMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForExtension(const std::wstring& viewGroupName, const std::wstring& viewName, IHierarchicalStorageNode& viewState, unsigned int moduleID, const std::wstring& extensionInstanceName) const
{
	bool result = false;
	bool foundDevice = false;
	LoadedExtensionInfoList::const_iterator loadedExtensionIterator = loadedExtensionInfoList.begin();
	while(!foundDevice && (loadedExtensionIterator != loadedExtensionInfoList.end()))
	{
		if((loadedExtensionIterator->moduleID == moduleID) && (loadedExtensionIterator->name == extensionInstanceName))
		{
			foundDevice = true;
			for(std::set<IExtension*>::const_iterator menuHandlerIterator = loadedExtensionIterator->menuHandlers.begin(); menuHandlerIterator != loadedExtensionIterator->menuHandlers.end(); ++menuHandlerIterator)
			{
				result |= (*menuHandlerIterator)->RestoreExtensionViewState(viewGroupName, viewName, viewState, loadedExtensionIterator->extension);
			}
		}
		++loadedExtensionIterator;
	}
	return result;
}

//----------------------------------------------------------------------------------------
bool System::RestoreViewStateForExtensionInternal(const InteropSupport::ISTLObjectSource<std::wstring>& viewGroupNameMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& viewNameMarshaller, IHierarchicalStorageNode& viewState, unsigned int moduleID, const InteropSupport::ISTLObjectSource<std::wstring>& extensionInstanceNameMarshaller) const
{
	return RestoreViewStateForExtension(viewGroupNameMarshaller.MarshalTo(), viewNameMarshaller.MarshalTo(), viewState, moduleID, extensionInstanceNameMarshaller.MarshalTo());
}
