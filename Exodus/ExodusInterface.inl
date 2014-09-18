//----------------------------------------------------------------------------------------
//Structures
//----------------------------------------------------------------------------------------
struct ExodusInterface::NewMenuItem
{
	NewMenuItem(HMENU aparentMenu, unsigned int amenuID, IMenuSelectableOption& amenuItem)
	:parentMenu(aparentMenu), menuID(amenuID), menuItem(amenuItem)
	{}

	HMENU parentMenu;
	unsigned int menuID;
	IMenuSelectableOption& menuItem;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::SavestateCellWindowState
{
	SavestateCellWindowState()
	:hbitmap(NULL), hwnd(NULL), savestatePresent(false), savestateSlotSelected(false), debugStatePresent(false), screenshotPresent(false), initializedBitmap(false)
	{}

	HWND hwnd;
	unsigned int slotNo;
	std::wstring slotName;
	std::wstring date;
	std::wstring time;
	std::wstring timestamp;
	bool savestatePresent;
	bool savestateSlotSelected;
	bool debugStatePresent;

	Image originalImage;
	Image resizedImage;
	bool screenshotPresent;
	bool initializedBitmap;
	HBITMAP hbitmap;
	unsigned int bitmapWidth;
	unsigned int bitmapHeight;
	unsigned int bitmapXPos;
	unsigned int bitmapYPos;
	unsigned int infoRectangleHeight;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::MapConnectorDialogParams
{
	MapConnectorDialogParams()
	:selectionMade(false), system(0)
	{}

	bool selectionMade;
	std::list<ConnectorInfo> connectorList;
	ConnectorInfo selectedConnector;
	ISystemGUIInterface* system;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::SelectCompressedFileDialogParams
{
	SelectCompressedFileDialogParams()
	:selectionMade(false)
	{}

	bool selectionMade;
	unsigned int selectedEntryID;
	std::list<SelectCompressedFileDialogParamsFileEntry> fileList;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::SelectCompressedFileDialogParamsFileEntry
{
	std::wstring fileName;
	unsigned int entryID;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::RegisteredDeviceInfo
{
	std::wstring assemblyPath;
	DeviceInfo info;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::RegisteredExtensionInfo
{
	std::wstring assemblyPath;
	ExtensionInfo info;
};

//----------------------------------------------------------------------------------------
struct ExodusInterface::PluginInfo
{
	AssemblyHandle assemblyHandle;
	unsigned int interfaceVersion;
	bool (*GetDeviceEntry)(unsigned int entryNo, IDeviceInfo& entry);
	bool (*GetExtensionEntry)(unsigned int entryNo, IExtensionInfo& entry);
	bool (*GetSystemEntry)(unsigned int entryNo, ISystemInfo& entry);
};

//----------------------------------------------------------------------------------------
//Global preference functions
//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetPreferenceDirectoryPath() const
{
	return preferenceDirectoryPath;
}

//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetGlobalPreferencePathModules() const
{
	return prefs.pathModules;
}

//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetGlobalPreferencePathSavestates() const
{
	return prefs.pathSavestates;
}

//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetGlobalPreferencePathPersistentState() const
{
	return prefs.pathPersistentState;
}

//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetGlobalPreferencePathWorkspaces() const
{
	return prefs.pathWorkspaces;
}

//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetGlobalPreferencePathCaptures() const
{
	return prefs.pathCaptures;
}

//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetGlobalPreferencePathAssemblies() const
{
	return prefs.pathAssemblies;
}

//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetGlobalPreferenceInitialSystem() const
{
	return prefs.loadSystem;
}

//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetGlobalPreferenceInitialWorkspace() const
{
	return prefs.loadWorkspace;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferencePathModules(const std::wstring& state)
{
	//Ensure the specified path is an absolute path
	std::wstring absolutePath = state;
	if(PathIsRelativePath(state))
	{
		absolutePath = PathCombinePaths(preferenceDirectoryPath, state);
	}

	//Ensure the target directory exists
	CreateDirectory(absolutePath, true);

	//Apply the new preference setting
	prefs.pathModules = absolutePath;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferencePathSavestates(const std::wstring& state)
{
	//Ensure the specified path is an absolute path
	std::wstring absolutePath = state;
	if(PathIsRelativePath(state))
	{
		absolutePath = PathCombinePaths(preferenceDirectoryPath, state);
	}

	//Ensure the target directory exists
	CreateDirectory(absolutePath, true);

	//Apply the new preference setting
	prefs.pathSavestates = absolutePath;

	//Update our savestate cell slots
	UpdateSaveSlots();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferencePathPersistentState(const std::wstring& state)
{
	//Ensure the specified path is an absolute path
	std::wstring absolutePath = state;
	if(PathIsRelativePath(state))
	{
		absolutePath = PathCombinePaths(preferenceDirectoryPath, state);
	}

	//Ensure the target directory exists
	CreateDirectory(absolutePath, true);

	//Apply the new preference setting
	prefs.pathPersistentState = absolutePath;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferencePathWorkspaces(const std::wstring& state)
{
	//Ensure the specified path is an absolute path
	std::wstring absolutePath = state;
	if(PathIsRelativePath(state))
	{
		absolutePath = PathCombinePaths(preferenceDirectoryPath, state);
	}

	//Ensure the target directory exists
	CreateDirectory(absolutePath, true);

	//Apply the new preference setting
	prefs.pathWorkspaces = absolutePath;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferencePathCaptures(const std::wstring& state)
{
	//Ensure the specified path is an absolute path
	std::wstring absolutePath = state;
	if(PathIsRelativePath(state))
	{
		absolutePath = PathCombinePaths(preferenceDirectoryPath, state);
	}

	//Ensure the target directory exists
	CreateDirectory(absolutePath, true);

	//Apply the new preference setting
	prefs.pathCaptures = absolutePath;
	system->SetCapturePath(prefs.pathCaptures);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferencePathAssemblies(const std::wstring& state)
{
	//Ensure the specified path is an absolute path
	std::wstring absolutePath = state;
	if(PathIsRelativePath(state))
	{
		absolutePath = PathCombinePaths(preferenceDirectoryPath, state);
	}

	//Ensure the target directory exists
	CreateDirectory(absolutePath, true);

	//Apply the new preference setting
	prefs.pathAssemblies = absolutePath;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferenceInitialSystem(const std::wstring& state)
{
	//Ensure the specified path is an absolute path
	std::wstring absolutePath = state;
	if(!absolutePath.empty() && PathIsRelativePath(state))
	{
		absolutePath = PathCombinePaths(prefs.pathModules, state);
	}

	//Apply the new preference setting
	prefs.loadSystem = absolutePath;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferenceInitialWorkspace(const std::wstring& state)
{
	//Ensure the specified path is an absolute path
	std::wstring absolutePath = state;
	if(!absolutePath.empty() && PathIsRelativePath(state))
	{
		absolutePath = PathCombinePaths(prefs.pathWorkspaces, state);
	}

	//Apply the new preference setting
	prefs.loadWorkspace = absolutePath;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferenceEnableThrottling(bool state)
{
	//Apply the new preference setting
	prefs.enableThrottling = state;
	system->SetThrottlingState(prefs.enableThrottling);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferenceRunWhenProgramModuleLoaded(bool state)
{
	//Apply the new preference setting
	prefs.runWhenProgramModuleLoaded = state;
	system->SetRunWhenProgramModuleLoadedState(prefs.runWhenProgramModuleLoaded);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferenceEnablePersistentState(bool state)
{
	//Apply the new preference setting
	prefs.enablePersistentState = state;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferenceLoadWorkspaceWithDebugState(bool state)
{
	//Apply the new preference setting
	prefs.loadWorkspaceWithDebugState = state;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SetGlobalPreferenceShowDebugConsole(bool state)
{
	//Apply the new preference setting
	prefs.showDebugConsole = state;
	if(debugConsoleOpen != prefs.showDebugConsole)
	{
		if(!debugConsoleOpen)
		{
			//Create a debug command console
			AllocConsole();
			BindStdHandlesToConsole();
		}
		else
		{
			//Close the current debug command console
			FreeConsole();
		}
		debugConsoleOpen = prefs.showDebugConsole;
	}
}
