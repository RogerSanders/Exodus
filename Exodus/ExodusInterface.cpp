#include "ExodusInterface.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "resource.h"
#include "ZIP/ZIP.pkg"
#include "Stream/Stream.pkg"
#include "HierarchicalStorage/HierarchicalStorage.pkg"
#include "DataConversion/DataConversion.pkg"
#include "MenuSelectableOption.h"
#include <boost/functional/hash.hpp>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExodusInterface::ExodusInterface(ISystemExternal& asystem)
:system(asystem), moduleManagerDialog(NULL)
{
	//Provide a reference to the GUI extension interface for the system
	system.BindToGUIExtensionInterface(this);

	//Set the size of the savestate window cell array
	cell.resize(cellCount);

	//Set the next menu ID number to use when building menus
	nextFreeMenuID = 1;

	//Create our submenu objects
	fileSubmenu = new MenuSubmenu(L"File");
	systemSubmenu = new MenuSubmenu(L"System");
	settingsSubmenu = new MenuSubmenu(L"System");
	debugSubmenu = new MenuSubmenu(L"Debug");

	//Initialize the savestate popup window settings
	selectedSaveCell = 0;
	savestatePopupTimeout = 5000;
	savestateMonitorPosX = 0;
	savestateMonitorPosY = 0;
	savestateMonitorWidth = 0;
	savestateMonitorHeight = 0;

	//Initialize the default child window position
	childWindowXPosDefault = 0;
	childWindowYPosDefault = 0;

	//Initialize the system load state
	systemLoaded = false;
	systemDestructionInProgress = false;
	viewEventProcessingPaused = false;

	//Initialize other system options
	debugConsoleOpen = false;
}

//----------------------------------------------------------------------------------------
ExodusInterface::~ExodusInterface()
{
	//Stop the joystick worker thread
	boost::mutex::scoped_lock lock(joystickWorkerThreadMutex);
	joystickWorkerThreadActive = false;
	joystickWorkerThreadStopped.wait(lock);

	//delete our submenu objects
	delete fileSubmenu;
	delete systemSubmenu;
	delete settingsSubmenu;
	delete debugSubmenu;
}

//----------------------------------------------------------------------------------------
HWND ExodusInterface::CreateMainInterface(HINSTANCE hinstance)
{
	//Calculate the dimensions of the main window. We calculate a window size that
	//achieves a client region height of 0.
	unsigned int width = DPIScaleWidth(800);
	unsigned int height = 0;
	RECT clientRect;
	clientRect.left = 0;
	clientRect.top = 0;
	clientRect.right = width;
	clientRect.bottom = height;
	BOOL adjustWindowRectExReturn;
	adjustWindowRectExReturn = AdjustWindowRectEx(&clientRect, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, TRUE, WS_EX_CONTROLPARENT);
	if(adjustWindowRectExReturn == 0)
	{
		return NULL;
	}
	width = clientRect.right - clientRect.left;
	height = clientRect.bottom - clientRect.top;

	//Register the main window class
	WNDCLASSEX wc;
	std::wstring windowClassName = L"ExodusWC";
	std::wstring windowName = L"Exodus";
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = ExodusInterface::WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hinstance;
	wc.hIcon         = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_PROGRAM));
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
	wc.lpszClassName = windowClassName.c_str();
	wc.hIconSm       = NULL;
	if((RegisterClassEx(&wc) == 0) && (GetLastError() != ERROR_CLASS_ALREADY_EXISTS))
	{
		return NULL;
	}

	//Create the main window
	mainWindowHandle = CreateWindowEx(WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE, windowClassName.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hinstance, (LPVOID)this);
	if(mainWindowHandle == NULL)
	{
		return NULL;
	}

	//Retrieve the handle for the menu
	HMENU mainMenu = GetMenu(mainWindowHandle);
	if(mainMenu == NULL)
	{
		return NULL;
	}

	//Obtain the location of the dynamic portion of the file menu
	MENUITEMINFO fileMenuItemInfo;
	fileMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
	fileMenuItemInfo.fMask = MIIM_SUBMENU;
	BOOL getFileMenuItemInfoReturn = GetMenuItemInfo(mainMenu, 0, TRUE, &fileMenuItemInfo);
	if(getFileMenuItemInfoReturn == 0)
	{
		return NULL;
	}
	if(fileMenuItemInfo.hSubMenu == NULL)
	{
		return NULL;
	}
	fileMenu = fileMenuItemInfo.hSubMenu;
	fileMenuNonDynamicMenuItemCount = GetMenuItemCount(fileMenu)-1;

	//Build the dynamic portion of the file menu
	if(!BuildFileMenu())
	{
		return NULL;
	}

	//Obtain the location of the dynamic portion of the system menu
	MENUITEMINFO systemMenuItemInfo;
	systemMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
	systemMenuItemInfo.fMask = MIIM_SUBMENU;
	BOOL getSystemMenuItemInfoReturn = GetMenuItemInfo(mainMenu, 1, TRUE, &systemMenuItemInfo);
	if(getSystemMenuItemInfoReturn == 0)
	{
		return NULL;
	}
	if(systemMenuItemInfo.hSubMenu == NULL)
	{
		return NULL;
	}
	systemMenu = systemMenuItemInfo.hSubMenu;
	systemMenuFirstItemIndex = GetMenuItemCount(systemMenu)-1;

	//Build the dynamic portion of the system menu
	if(!BuildSystemMenu())
	{
		return NULL;
	}

	//Obtain the location of the dynamic portion of the settings menu
	MENUITEMINFO settingsMenuItemInfo;
	settingsMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
	settingsMenuItemInfo.fMask = MIIM_SUBMENU;
	BOOL getSettingsMenuItemInfoReturn = GetMenuItemInfo(mainMenu, 2, TRUE, &settingsMenuItemInfo);
	if(getSettingsMenuItemInfoReturn == 0)
	{
		return NULL;
	}
	if(settingsMenuItemInfo.hSubMenu == NULL)
	{
		return NULL;
	}
	settingsMenu = settingsMenuItemInfo.hSubMenu;
	settingsMenuFirstItemIndex = GetMenuItemCount(settingsMenu)-1;

	//Build the dynamic portion of the settings menu
	if(!BuildSettingsMenu())
	{
		return NULL;
	}

	//Obtain the location of the dynamic portion of the debug menu
	MENUITEMINFO debugMenuItemInfo;
	debugMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
	debugMenuItemInfo.fMask = MIIM_SUBMENU;
	BOOL getDebugMenuItemInfoReturn = GetMenuItemInfo(mainMenu, 3, TRUE, &debugMenuItemInfo);
	if(getDebugMenuItemInfoReturn == 0)
	{
		return NULL;
	}
	if(debugMenuItemInfo.hSubMenu == NULL)
	{
		return NULL;
	}
	debugMenu = debugMenuItemInfo.hSubMenu;
	debugMenuFirstItemIndex = GetMenuItemCount(debugMenu)-1;

	//Build the dynamic portion of the debug menu
	if(!BuildDebugMenu())
	{
		return NULL;
	}

	//Register the window class for our savestate popup window
	WNDCLASSEX wcSave;
	std::wstring saveClassName = L"ExodusWCSave";
	std::wstring saveWindowName = L"ExodusSave";
	wcSave.cbSize        = sizeof(WNDCLASSEX);
	wcSave.style         = 0;
	wcSave.lpfnWndProc   = ExodusInterface::WndSavestateProc;
	wcSave.cbClsExtra    = 0;
	wcSave.cbWndExtra    = 0;
	wcSave.hInstance     = GetModuleHandle(NULL);
	wcSave.hIcon         = NULL;
	wcSave.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcSave.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcSave.lpszMenuName  = NULL;
	wcSave.lpszClassName = saveClassName.c_str();
	wcSave.hIconSm       = NULL;
	if((RegisterClassEx(&wcSave) == 0) && (GetLastError() != ERROR_CLASS_ALREADY_EXISTS))
	{
		DestroyWindow(mainWindowHandle);
		return NULL;
	}

	//Create the savestate popup window
	savestatePopup = CreateWindowEx(0, saveClassName.c_str(), saveWindowName.c_str(), WS_POPUP, 1, 1, 1, 1, mainWindowHandle, NULL, GetModuleHandle(NULL), (LPVOID)this);
	if(savestatePopup == NULL)
	{
		DestroyWindow(mainWindowHandle);
		return NULL;
	}

	//Register the window class for our ctrl+tab window selection popup
	WNDCLASSEX wcSelect;
	std::wstring selectClassName = L"ExodusWCWindowSelect";
	std::wstring selectWindowName = L"ExodusWindowSelect";
	wcSelect.cbSize        = sizeof(WNDCLASSEX);
	wcSelect.style         = 0;
	wcSelect.lpfnWndProc   = ExodusInterface::WndWindowSelectProc;
	wcSelect.cbClsExtra    = 0;
	wcSelect.cbWndExtra    = 0;
	wcSelect.hInstance     = GetModuleHandle(NULL);
	wcSelect.hIcon         = NULL;
	wcSelect.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcSelect.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcSelect.lpszMenuName  = NULL;
	wcSelect.lpszClassName = selectClassName.c_str();
	wcSelect.hIconSm       = NULL;
	if((RegisterClassEx(&wcSelect) == 0) && (GetLastError() != ERROR_CLASS_ALREADY_EXISTS))
	{
		DestroyWindow(mainWindowHandle);
		DestroyWindow(savestatePopup);
		return NULL;
	}

	//Create the ctrl+tab window selection popup
	windowSelectHandle = CreateWindowEx(0, selectClassName.c_str(), selectWindowName.c_str(), WS_POPUP, 1, 1, 1, 1, mainWindowHandle, NULL, GetModuleHandle(NULL), (LPVOID)this);
	if(windowSelectHandle == NULL)
	{
		DestroyWindow(mainWindowHandle);
		DestroyWindow(savestatePopup);
		return NULL;
	}

	//Show the main window
	ShowWindow(mainWindowHandle, SW_SHOWMAXIMIZED);
	UpdateWindow(mainWindowHandle);

	//Save the initial position of the main window, so we can reposition child
	//windows relative to it when the main window is moved.
	RECT windowRect;
	GetWindowRect(mainWindowHandle, &windowRect);
	mainWindowPosX = windowRect.left;
	mainWindowPosY = windowRect.top;

	//Return the handle of the main window
	return mainWindowHandle;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::InitializeSystem()
{
	//Save the initial working directory of the process, so that we can restore it later
	//when needed.
	originalWorkingDir = PathGetCurrentWorkingDirectory();

	//Initialize the system prefs to their default values
	prefs.pathModulesRaw = L"Modules";
	prefs.pathSavestatesRaw = L"Savestates";
	prefs.pathPersistentStateRaw = L"PersistentState";
	prefs.pathWorkspacesRaw = L"Workspaces";
	prefs.pathCapturesRaw = L"Captures";
	prefs.pathAssembliesRaw = L"Plugins";
	prefs.enableThrottling = true;
	prefs.runWhenProgramModuleLoaded = true;
	prefs.enablePersistentState = true;
	prefs.loadWorkspaceWithDebugState = true;
	prefs.showDebugConsole = false;

	//Load preferences from the settings.xml file if present
	LoadPrefs(L"settings.xml");

	//Now that the preferences have been initialized, resolve the preference settings, and
	//propagate the settings to other areas.
	ResolvePrefs();
	ApplyPrefs();

	//Load addon modules
	LoadAssembliesFromFolder(prefs.pathAssemblies);

	//Load the default system
	if(!prefs.loadSystem.empty())
	{
		if(LoadModuleFromFile(prefs.loadSystem))
		{
			//Load the default workspace
			if(!prefs.loadWorkspace.empty())
			{
				//Spawn a worker thread to handle loading of the workspace. We do this in
				//a separate thread so that viewmodel requests are still processed during
				//the operation, which is important in order to make sure views can
				//actually be loaded. Note that we specifically take a copy of the path
				//string, rather than pass it by reference.
				boost::thread workerThread(boost::bind(boost::mem_fn(&ExodusInterface::LoadWorkspaceFromFile), this, prefs.loadWorkspace));
			}
		}
	}

	//Start the joystick worker thread
	boost::mutex::scoped_lock lock(joystickWorkerThreadMutex);
	joystickWorkerThreadActive = true;
	boost::thread workerThread(boost::bind(boost::mem_fn(&ExodusInterface::JoystickInputWorkerThread), this));

	return true;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ExodusInterface::GetIViewModelLauncherVersion() const
{
	return ThisIViewModelLauncherVersion();
}

//----------------------------------------------------------------------------------------
unsigned int ExodusInterface::GetIGUIExtensionInterfaceVersion() const
{
	return ThisIGUIExtensionInterfaceVersion();
}

//----------------------------------------------------------------------------------------
//Main window functions
//----------------------------------------------------------------------------------------
void* ExodusInterface::GetMainWindowHandle() const
{
	return mainWindowHandle;
}

//----------------------------------------------------------------------------------------
//Savestate functions
//----------------------------------------------------------------------------------------
std::wstring ExodusInterface::GetSavestateAutoFileNamePrefix() const
{
	//Obtain lists of full identifier names of all loaded module instances, and the
	//instance names of all loaded program modules.
	std::list<std::wstring> moduleIdentifierStrings;
	std::list<std::wstring> loadedProgramModuleInstanceNames;
	std::list<unsigned int> loadedModuleIDs = system.GetLoadedModuleIDs();
	for(std::list<unsigned int>::const_iterator i = loadedModuleIDs.begin(); i != loadedModuleIDs.end(); ++i)
	{
		LoadedModuleInfo moduleInfo;
		if(system.GetLoadedModuleInfo(*i, moduleInfo))
		{
			std::wstring moduleIdentifierString = moduleInfo.GetSystemClassName() + L"." + moduleInfo.GetClassName() + L"." + moduleInfo.GetInstanceName();
			moduleIdentifierStrings.push_back(moduleIdentifierString);
			if(moduleInfo.GetIsProgramModule())
			{
				loadedProgramModuleInstanceNames.push_back(moduleInfo.GetInstanceName());
			}
		}
	}

	//Obtain a list of all current connector mapping identifier strings
	std::list<std::wstring> connectorMappingStrings;
	std::list<unsigned int> connectorIDs = system.GetConnectorIDs();
	for(std::list<unsigned int>::const_iterator i = connectorIDs.begin(); i != connectorIDs.end(); ++i)
	{
		ConnectorInfo connectorInfo;
		if(system.GetConnectorInfo(*i, connectorInfo))
		{
			//If this connector is used, add information about the connector mapping to
			//the list of connector mappings.
			if(connectorInfo.GetIsConnectorUsed())
			{
				//Load information on the module which exports this connector
				LoadedModuleInfo exportingModuleInfo;
				if(!system.GetLoadedModuleInfo(connectorInfo.GetExportingModuleID(), exportingModuleInfo))
				{
					continue;
				}

				//Load information on the module which imports this connector
				LoadedModuleInfo importingModuleInfo;
				if(!system.GetLoadedModuleInfo(connectorInfo.GetImportingModuleID(), importingModuleInfo))
				{
					continue;
				}

				//Build a string describing the connector mapping
				std::wstring connectorMappingString = L"(" + importingModuleInfo.GetSystemClassName() + L"." + importingModuleInfo.GetClassName() + L"." + importingModuleInfo.GetInstanceName() + L"." + connectorInfo.GetImportingModuleConnectorInstanceName() + L"@" + exportingModuleInfo.GetSystemClassName() + L"." + exportingModuleInfo.GetClassName() + L"." + exportingModuleInfo.GetInstanceName() + L"." + connectorInfo.GetExportingModuleConnectorInstanceName() + L")";

				//Add the connector mapping string to the list of connector mappings
				connectorMappingStrings.push_back(connectorMappingString);
			}
		}
	}

	//Sort our lists. This ensures that all elements will be processed in the same order,
	//regardless of the order they were loaded in.
	moduleIdentifierStrings.sort();
	loadedProgramModuleInstanceNames.sort();
	connectorMappingStrings.sort();

	//Build a system identifier string using the module identifier and connector mapping
	//identifier strings
	std::wstring systemIdentifierString;
	for(std::list<std::wstring>::const_iterator i = moduleIdentifierStrings.begin(); i != moduleIdentifierStrings.end(); ++i)
	{
		if(!systemIdentifierString.empty())
		{
			systemIdentifierString += L" + ";
		}
		systemIdentifierString += *i;
	}
	for(std::list<std::wstring>::const_iterator i = connectorMappingStrings.begin(); i != connectorMappingStrings.end(); ++i)
	{
		if(!systemIdentifierString.empty())
		{
			systemIdentifierString += L" + ";
		}
		systemIdentifierString += *i;
	}

	//Build a hash of the system identifier string. We encode this into the filename to
	//try and ensure that only savestates compatible with the current system configuration
	//are offered to the user.
	boost::hash<std::wstring> stringHasher;
	size_t systemIdentifierHashRaw = stringHasher(systemIdentifierString);
	std::wstring systemIdentifierHash;
	IntToStringBase16((unsigned int)systemIdentifierHashRaw, systemIdentifierHash, 8, false);

	//Build the filename for the savestate
	std::wstring fileName;
	for(std::list<std::wstring>::const_iterator i = loadedProgramModuleInstanceNames.begin(); i != loadedProgramModuleInstanceNames.end(); ++i)
	{
		if(!fileName.empty())
		{
			fileName += L" + ";
		}
		fileName += *i;
	}
	fileName += L" {" + systemIdentifierHash + L"}";

	//Escape out any invalid characters in the filename
	std::wstring invalidFilenameChars = L"\\/:?\"<>|";
	wchar_t invalidCharReplacement = L'_';
	for(size_t i = 0; i < fileName.size(); ++i)
	{
		if(invalidFilenameChars.find(fileName[i]) != invalidFilenameChars.npos)
		{
			fileName[i] = invalidCharReplacement;
		}
	}

	//Return the constructed filename
	return fileName;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::LoadState(const std::wstring& folder, bool debuggerState)
{
	bool running = system.SystemRunning();
	system.StopSystem();

	//Load a state file
	std::wstring selectedFilePath;
	if(SelectExistingFile(L"Compressed savestate files|exs;Uncompressed savestate files|xml", L"exs", L"", folder, true, selectedFilePath))
	{
		//Determine the type of state file being loaded
		std::wstring fileExtension = PathGetFileExtension(selectedFilePath);
		ISystemExternal::FileType fileType = ISystemExternal::FILETYPE_ZIP;
		if(fileExtension == L"xml")
		{
			fileType = ISystemExternal::FILETYPE_XML;
		}

		//Perform the state load operation
		LoadStateFromFile(selectedFilePath, fileType, debuggerState);
	}

	if(running)
	{
		system.RunSystem();
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::LoadStateFromFile(const std::wstring& filePath, ISystemExternal::FileType fileType, bool debuggerState)
{
	system.LoadState(filePath, fileType, debuggerState);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SaveState(const std::wstring& folder, bool debuggerState)
{
	bool running = system.SystemRunning();
	system.StopSystem();

	//Save a state file
	std::wstring selectedFilePath;
	if(SelectNewFile(L"Compressed savestate files|exs;Uncompressed savestate files|xml", L"exs", L"", folder, selectedFilePath))
	{
		//Determine the type of state file being saved
		std::wstring fileExtension = PathGetFileExtension(selectedFilePath);
		ISystemExternal::FileType fileType = ISystemExternal::FILETYPE_ZIP;
		if(fileExtension == L"xml")
		{
			fileType = ISystemExternal::FILETYPE_XML;
		}

		//Perform the state save operation
		SaveStateToFile(selectedFilePath, fileType, debuggerState);
	}

	if(running)
	{
		system.RunSystem();
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SaveStateToFile(const std::wstring& filePath, ISystemExternal::FileType fileType, bool debuggerState)
{
	system.SaveState(filePath, fileType, debuggerState);
}

//----------------------------------------------------------------------------------------
//Savestate quick-select popup functions
//----------------------------------------------------------------------------------------
void ExodusInterface::QuickLoadState(bool debuggerState)
{
	std::wstring filePostfix = cell[selectedSaveCell].slotName;
	std::wstring fileName;
	if(debuggerState)
	{
		fileName = GetSavestateAutoFileNamePrefix() + L" - " + filePostfix + L" - DebugState" + L".exs";
	}
	else
	{
		fileName = GetSavestateAutoFileNamePrefix() + L" - " + filePostfix + L".exs";
	}
	std::wstring filePath = PathCombinePaths(prefs.pathSavestates, fileName);
	LoadStateFromFile(filePath, ISystemExternal::FILETYPE_ZIP, debuggerState);
	if(debuggerState && prefs.loadWorkspaceWithDebugState)
	{
		std::wstring workspaceFileName = GetSavestateAutoFileNamePrefix() + L" - " + filePostfix + L" - DebugWorkspace" + L".xml";
		std::wstring workspaceFilePath = PathCombinePaths(prefs.pathSavestates, workspaceFileName);

		//Spawn a worker thread to handle loading of the workspace. We do this in a
		//separate thread so that viewmodel requests are still processed during the
		//operation, which is important in order to make sure views can actually be
		//loaded. Note that we specifically take a copy of the path string, rather than
		//pass it by reference.
		boost::thread workerThread(boost::bind(boost::mem_fn(&ExodusInterface::LoadWorkspaceFromFile), this, workspaceFilePath));
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::QuickSaveState(bool debuggerState)
{
	std::wstring filePostfix = cell[selectedSaveCell].slotName;
	std::wstring fileName;
	if(debuggerState)
	{
		fileName = GetSavestateAutoFileNamePrefix() + L" - " + filePostfix + L" - DebugState" + L".exs";
	}
	else
	{
		fileName = GetSavestateAutoFileNamePrefix() + L" - " + filePostfix + L".exs";
	}
	std::wstring filePath = PathCombinePaths(prefs.pathSavestates, fileName);
	SaveStateToFile(filePath, ISystemExternal::FILETYPE_ZIP, debuggerState);
	if(debuggerState)
	{
		std::wstring workspaceFileName = GetSavestateAutoFileNamePrefix() + L" - " + filePostfix + L" - DebugWorkspace" + L".xml";
		std::wstring workspaceFilePath = PathCombinePaths(prefs.pathSavestates, workspaceFileName);
		SaveWorkspaceToFile(workspaceFilePath);
	}
	PostMessage(cell[selectedSaveCell].hwnd, WM_USER, 0, (LPARAM)this);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::IncrementSaveSlot()
{
	unsigned int newSaveSlot = (selectedSaveCell == (cellCount - 1))? 0: selectedSaveCell + 1;
	SelectSaveSlot(newSaveSlot);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::DecrementSaveSlot()
{
	unsigned int newSaveSlot = (selectedSaveCell == 0)? (cellCount - 1): selectedSaveCell - 1;
	SelectSaveSlot(newSaveSlot);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SelectSaveSlot(unsigned int slotNo)
{
	//Switch the savestate cell focus to the new target cell, and force both the newly
	//selected cell and previously selected cell to redraw.
	cell[selectedSaveCell].savestateSlotSelected = false;
	InvalidateRect(cell[selectedSaveCell].hwnd, NULL, FALSE);
	selectedSaveCell = slotNo;
	cell[selectedSaveCell].savestateSlotSelected = true;
	InvalidateRect(cell[selectedSaveCell].hwnd, NULL, FALSE);

	//Move the savestate selection popup to the top of the Z order, and make it visible
	//if it is currently hidden. We also initialize the timer to hide the popup window
	//after the timeout period has expired.
	SetWindowPos(savestatePopup, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	ShowWindow(savestatePopup, SW_SHOWNOACTIVATE);
	KillTimer(savestatePopup, 1);
	SetTimer(savestatePopup, 1, savestatePopupTimeout, NULL);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::UpdateSaveSlots()
{
	for(unsigned int i = 0; i < cellCount; ++i)
	{
		PostMessage(cell[i].hwnd, WM_USER, 0, (LPARAM)this);
	}
}

//----------------------------------------------------------------------------------------
//Module functions
//----------------------------------------------------------------------------------------
void ExodusInterface::UpdateModuleDisplayInfo() const
{
	if(moduleManagerDialog != NULL)
	{
		PostMessage(moduleManagerDialog, WM_USER, 0, (LPARAM)this);
	}
}

//----------------------------------------------------------------------------------------
//Workspace functions
//----------------------------------------------------------------------------------------
void ExodusInterface::LoadWorkspace(const std::wstring& folder)
{
	//Load a state file
	std::wstring selectedFilePath;
	if(SelectExistingFile(L"Workspace Files|xml", L"xml", L"", folder, true, selectedFilePath))
	{
		//Spawn a worker thread to handle loading of the workspace. We do this in a
		//separate thread so that viewmodel requests are still processed during the
		//operation, which is important in order to make sure views can actually be
		//loaded. Note that we specifically take a copy of the path string, rather than
		//pass it by reference, since it's going to go out of scope when we leave this
		//function.
		boost::thread workerThread(boost::bind(boost::mem_fn(&ExodusInterface::LoadWorkspaceFromFile), this, selectedFilePath));
	}
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadWorkspaceFromFile(const std::wstring& filePath)
{
	//Open the target file
	FileStreamReference fileStreamReference(*this);
	if(!fileStreamReference.OpenExistingFileForRead(filePath))
	{
		return false;
	}
	Stream::IStream& file = *fileStreamReference;

	//Attempt to load an XML structure from the file
	file.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
	file.ProcessByteOrderMark();
	HierarchicalStorageTree tree;
	if(!tree.LoadTree(file))
	{
		return false;
	}
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();
	if(rootNode.GetName() == L"Workspace")
	{
		//Save details about all views which are currently open
		std::list<WorkspaceViewEntryDetails> existingViewInfo;
		for(ViewModels::const_iterator i = viewModels.begin(); i != viewModels.end(); ++i)
		{
			IViewModel* viewModel = i->first;
			if(viewModel->IsViewOwnerSystem())
			{
				existingViewInfo.push_back(WorkspaceViewEntryDetails(viewModel->GetViewModelGroupName(), viewModel->GetViewModelName(), true, L"", viewModel));
			}
			else
			{
				existingViewInfo.push_back(WorkspaceViewEntryDetails(viewModel->GetViewModelGroupName(), viewModel->GetViewModelName(), false, viewModel->GetViewOwnerDeviceInstanceName(), viewModel));
			}
		}

		//Iterate through each valid window entry in the file, and open matching windows
		//with the specified properties. Z-Order is based on the order the windows are
		//listed in the file.
		ISystemExternal::ModuleRelationshipMap relationshipMap;
		std::list<WorkspaceViewEntryDetails> loadedWorkspaceViewInfo;
		std::list<IHierarchicalStorageNode*> childList = rootNode.GetChildList();
		for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
		{
			if((*i)->GetName() == L"Window")
			{
				IHierarchicalStorageAttribute* ownerAttribute = (*i)->GetAttribute(L"Owner");
				IHierarchicalStorageAttribute* moduleIDAttribute = (*i)->GetAttribute(L"ModuleID");
				IHierarchicalStorageAttribute* deviceInstanceNameAttribute = (*i)->GetAttribute(L"DeviceInstanceName");
				IHierarchicalStorageAttribute* viewModelGroupNameAttribute = (*i)->GetAttribute(L"ViewModelGroupName");
				IHierarchicalStorageAttribute* viewModelNameAttribute = (*i)->GetAttribute(L"ViewModelName");
				IHierarchicalStorageAttribute* xposAttribute = (*i)->GetAttribute(L"XPos");
				IHierarchicalStorageAttribute* yposAttribute = (*i)->GetAttribute(L"YPos");
				IHierarchicalStorageAttribute* widthAttribute = (*i)->GetAttribute(L"Width");
				IHierarchicalStorageAttribute* heightAttribute = (*i)->GetAttribute(L"Height");
				if((ownerAttribute != 0) && (viewModelGroupNameAttribute != 0) && (viewModelNameAttribute != 0) && (xposAttribute != 0) && (yposAttribute != 0))
				{
					//Extract the basic attributes which are defined for all entries
					std::wstring owner = ownerAttribute->GetValue();
					std::wstring viewModelGroupName = viewModelGroupNameAttribute->GetValue();
					std::wstring viewModelName = viewModelNameAttribute->GetValue();
					int xpos = xposAttribute->ExtractValue<int>();
					int ypos = yposAttribute->ExtractValue<int>();

					//Adjust the view position to take the current DPI settings into
					//account
					xpos = DPIScaleWidth(xpos);
					ypos = DPIScaleHeight(ypos);

					//Convert the view position from main window client coordinates into
					//screen coordinates
					POINT windowPos;
					windowPos.x = (LONG)xpos;
					windowPos.y = (LONG)ypos;
					ClientToScreen(mainWindowHandle, &windowPos);
					xpos = (int)windowPos.x;
					ypos = (int)windowPos.y;

					//Extract the width and height attributes
					int width = 0;
					int height = 0;
					if((widthAttribute != 0) && (heightAttribute != 0))
					{
						//Extract the view size parameters
						width = widthAttribute->ExtractValue<int>();
						height = heightAttribute->ExtractValue<int>();

						//Adjust the view size to take the current DPI settings into
						//account
						width = DPIScaleWidth(width);
						height = DPIScaleHeight(height);
					}

					//Restore the view
					if(owner == L"System")
					{
						loadedWorkspaceViewInfo.push_back(WorkspaceViewEntryDetails(viewModelGroupName, viewModelName, true, L""));
						system.RestoreViewModelState(viewModelGroupName, viewModelName, *(*i), xpos, ypos, width, height, *this);
					}
					else if(owner == L"Device")
					{
						if((deviceInstanceNameAttribute != 0) && (moduleIDAttribute != 0))
						{
							std::wstring deviceInstanceName = deviceInstanceNameAttribute->GetValue();
							unsigned int savedModuleID = moduleIDAttribute->ExtractValue<unsigned int>();

							//Attempt to restore the view model state
							ISystemExternal::ModuleRelationshipMap::const_iterator relationshipMapIterator = relationshipMap.find(savedModuleID);
							if(relationshipMapIterator != relationshipMap.end())
							{
								const ISystemExternal::ModuleRelationship& moduleRelationship = relationshipMapIterator->second;
								if(moduleRelationship.foundMatch)
								{
									//If we found a matching loaded module for the saved
									//data, attempt to restore the view model state.
									if(system.RestoreViewModelStateForDevice(moduleRelationship.loadedModuleID, deviceInstanceName, viewModelGroupName, viewModelName, *(*i), xpos, ypos, width, height, *this))
									{
										//If the view model state was restored, save info
										//about this view.
										loadedWorkspaceViewInfo.push_back(WorkspaceViewEntryDetails(viewModelGroupName, viewModelName, false, deviceInstanceName));
									}
								}
							}
						}
					}
				}
			}
			//Load the ModuleRelationships node
			else if((*i)->GetName() == L"ModuleRelationships")
			{
				if(!system.LoadModuleRelationshipsNode(*(*i), relationshipMap))
				{
					return false;
				}
			}
		}

		//Close any views which were not referenced in the workspace file
		for(std::list<WorkspaceViewEntryDetails>::const_iterator existingViewDetails = existingViewInfo.begin(); existingViewDetails != existingViewInfo.end(); ++existingViewDetails)
		{
			bool viewReferenced = false;
			for(std::list<WorkspaceViewEntryDetails>::const_iterator loadedViewDetails = loadedWorkspaceViewInfo.begin(); loadedViewDetails != loadedWorkspaceViewInfo.end(); ++loadedViewDetails)
			{
				if((existingViewDetails->viewModelGroupName == loadedViewDetails->viewModelGroupName) &&
				   (existingViewDetails->viewModelName == loadedViewDetails->viewModelName) &&
				   (existingViewDetails->ownerIsSystem == loadedViewDetails->ownerIsSystem) &&
				   (existingViewDetails->deviceInstanceName == loadedViewDetails->deviceInstanceName))
				{
					viewReferenced = true;
				}
			}
			if(!viewReferenced)
			{
				CloseViewModel(existingViewDetails->viewModel, false);
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SaveWorkspace(const std::wstring& folder)
{
	std::wstring selectedFilePath;
	if(SelectNewFile(L"Workspace Files|xml", L"xml", L"", folder, selectedFilePath))
	{
		SaveWorkspaceToFile(selectedFilePath);
	}
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::SaveWorkspaceToFile(const std::wstring& filePath)
{
	//Create a new XML tree to store our saved workspace info
	HierarchicalStorageTree tree;
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();
	rootNode.SetName(L"Workspace");

	//Save the ModuleRelationships node
	IHierarchicalStorageNode& moduleRelationshipsNode = tree.GetRootNode().CreateChild(L"ModuleRelationships");
	system.SaveModuleRelationshipsNode(moduleRelationshipsNode);

	//Scan all open windows, and build a list of all child windows belonging to our
	//program which are currently open. Note that the windows are listed based on
	//Z-Order, from top to bottom. We scan the list from back to front, so the topmost
	//window appears last in the XML tree.
	std::list<HWND> windowList;
	EnumThreadWindows(GetCurrentThreadId(), &ExodusInterface::EnumWindowsProc, (LPARAM)&windowList);
	for(std::list<HWND>::const_reverse_iterator windowIterator = windowList.rbegin(); windowIterator != windowList.rend(); ++windowIterator)
	{
		HWND window = *windowIterator;
		for(ViewModels::const_iterator i = viewModels.begin(); i != viewModels.end(); ++i)
		{
			//If we've found a window belonging to our program, save its details to the
			//XML tree.
			IViewModel* viewModel = i->second->viewModel;
			if(viewModel->DoesWindowHandleMatchView(window))
			{
				//Obtain the current position of the view
				int windowPosX;
				int windowPosY;
				viewModel->GetViewPosition(windowPosX, windowPosY);

				//Convert the view position into coordinates relative to the main window
				POINT windowPos;
				windowPos.x = windowPosX;
				windowPos.y = windowPosY;
				ScreenToClient(mainWindowHandle, &windowPos);
				windowPosX = (int)windowPos.x;
				windowPosY = (int)windowPos.y;

				//Adjust the view position to convert it back into a DPI-independent
				//location
				windowPosX = DPIReverseScaleWidth(windowPosX);
				windowPosY = DPIReverseScaleHeight(windowPosY);

				//Save the basic attributes of the window
				IHierarchicalStorageNode& node = rootNode.CreateChild(L"Window");
				if(viewModel->IsViewOwnerSystem())
				{
					node.CreateAttribute(L"Owner", "System");
				}
				else if(viewModel->IsViewOwnerDevice())
				{
					node.CreateAttribute(L"Owner", "Device");
					node.CreateAttribute(L"DeviceInstanceName", viewModel->GetViewOwnerDeviceInstanceName());
					node.CreateAttribute(L"ModuleID", viewModel->GetViewOwnerModuleID());
				}
				else
				{
					node.CreateAttribute(L"Owner", "Unknown");
				}
				node.CreateAttribute(L"ViewModelGroupName", viewModel->GetViewModelGroupName());
				node.CreateAttribute(L"ViewModelName", viewModel->GetViewModelName());
				node.CreateAttribute(L"XPos", windowPosX);
				node.CreateAttribute(L"YPos", windowPosY);

				//If the view is resizable, save the current window size.
				if(viewModel->CanResizeView())
				{
					//Obtain the current window size
					int windowWidth;
					int windowHeight;
					viewModel->GetViewSize(windowWidth, windowHeight);

					//Adjust the view size to convert it back into a DPI-independent size
					windowWidth = DPIReverseScaleWidth(windowWidth);
					windowHeight = DPIReverseScaleHeight(windowHeight);

					//Save the window dimensions
					node.CreateAttribute(L"Width", windowWidth);
					node.CreateAttribute(L"Height", windowHeight);
				}
			}
		}
	}

	//Save the workspace XML tree to the target workspace file
	Stream::File file(Stream::IStream::TEXTENCODING_UTF8);
	if(!file.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		return false;
	}
	file.InsertByteOrderMark();
	tree.SaveTree(file);
	return true;
}

//----------------------------------------------------------------------------------------
//Module functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::CanModuleBeLoaded(const std::wstring& filePath) const
{
	//Read the connector info for the module
	ISystemExternal::ConnectorImportList connectorsImported;
	ISystemExternal::ConnectorExportList connectorsExported;
	std::wstring systemClassName;
	if(!system.ReadModuleConnectorInfo(filePath, systemClassName, connectorsImported, connectorsExported))
	{
		return false;
	}

	//Ensure that all connectors required by this module are available
	ISystemExternal::ConnectorMappingList connectorMappings;
	std::list<unsigned int> loadedConnectorIDList = system.GetConnectorIDs();
	for(ISystemExternal::ConnectorImportList::const_iterator i = connectorsImported.begin(); i != connectorsImported.end(); ++i)
	{
		//Build a list of all available, loaded connectors which match the desired type.
		std::list<ConnectorInfo> availableConnectors;
		std::list<unsigned int>::const_iterator loadedConnectorID = loadedConnectorIDList.begin();
		while(loadedConnectorID != loadedConnectorIDList.end())
		{
			ConnectorInfo connectorInfo;
			if(system.GetConnectorInfo(*loadedConnectorID, connectorInfo))
			{
				//If this connector matches the connector type the module wants to import,
				//and the connector is free, add it to the available connector list.
				if(!connectorInfo.GetIsConnectorUsed() && (connectorInfo.GetSystemClassName() == systemClassName) && (i->className == connectorInfo.GetConnectorClassName()))
				{
					availableConnectors.push_back(connectorInfo);
				}
			}
			++loadedConnectorID;
		}

		//Ensure that at least one compatible connector was found
		if(availableConnectors.empty())
		{
			return false;
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::CanModuleBeLoadedInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller) const
{
	return CanModuleBeLoaded(filePathMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadModule(const std::wstring& folder)
{
	//Select a module definition
	std::wstring selectedFilePath;
	if(!SelectExistingFile(L"Module Definitions|xml", L"xml", L"", folder, true, selectedFilePath))
	{
		return false;
	}

	//Return the result of the module load process
	return LoadModuleFromFile(selectedFilePath);
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadModuleFromFileInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller)
{
	return LoadModuleFromFile(filePathMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadModuleFromFile(const std::wstring& filePath)
{
	//Pause view event processing while the system load is in progress. We do this to
	//ensure any views which are flagged to be displayed in the system definition don't
	//appear until after the load is complete.
	viewEventProcessingPaused = true;

	//Read the connector info for the module
	ISystemExternal::ConnectorImportList connectorsImported;
	ISystemExternal::ConnectorExportList connectorsExported;
	std::wstring systemClassName;
	if(!system.ReadModuleConnectorInfo(filePath, systemClassName, connectorsImported, connectorsExported))
	{
		std::wstring text = L"Could not read connector info for module.";
		std::wstring title = L"Error loading module!";
		SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);

		viewEventProcessingPaused = false;
		FlagProcessPendingEvents();
		return false;
	}

	//Map all imported connectors to available connectors in the system
	//##TODO## Cache this information in our module database when the modules are scanned,
	//so that we only have to access the module definition file once for a module load.
	ISystemExternal::ConnectorMappingList connectorMappings;
	std::list<unsigned int> loadedConnectorIDList = system.GetConnectorIDs();
	for(ISystemExternal::ConnectorImportList::const_iterator i = connectorsImported.begin(); i != connectorsImported.end(); ++i)
	{
		//Build a list of all available, loaded connectors which match the desired type.
		std::list<ConnectorInfo> availableConnectors;
		std::list<unsigned int>::const_iterator loadedConnectorID = loadedConnectorIDList.begin();
		while(loadedConnectorID != loadedConnectorIDList.end())
		{
			ConnectorInfo connectorInfo;
			if(system.GetConnectorInfo(*loadedConnectorID, connectorInfo))
			{
				//If this connector matches the connector type the module wants to import,
				//and the connector is free, add it to the available connector list.
				if(!connectorInfo.GetIsConnectorUsed() && (connectorInfo.GetSystemClassName() == systemClassName) && (i->className == connectorInfo.GetConnectorClassName()))
				{
					availableConnectors.push_back(connectorInfo);
				}
			}
			++loadedConnectorID;
		}

		//Ensure that at least one compatible connector was found
		if(availableConnectors.empty())
		{
			std::wstring text = L"No available connector of type " + systemClassName + L"." + i->className + L" could be found!";
			std::wstring title = L"Error loading module!";
			SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);

			viewEventProcessingPaused = false;
			FlagProcessPendingEvents();
			return false;
		}

		//Map the connector
		if(availableConnectors.size() == 1)
		{
			//If only one connector was found that matches the required type, map it.
			ConnectorInfo connector = *availableConnectors.begin();
			ISystemExternal::ConnectorMapping connectorMapping;
			connectorMapping.connectorID = connector.GetConnectorID();
			connectorMapping.importingModuleConnectorInstanceName = i->instanceName;
			connectorMappings.push_back(connectorMapping);
		}
		else
		{
			//If multiple compatible connectors are available, open a dialog allowing the
			//user to select a connector from the list of available connectors.
			MapConnectorDialogParams mapConnectorDialogParams;
			mapConnectorDialogParams.system = &system;
			mapConnectorDialogParams.connectorList = availableConnectors;
			INT_PTR mapConnectorDialogBoxParamResult;
			mapConnectorDialogBoxParamResult = SafeDialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAPCONNECTOR), mainWindowHandle, MapConnectorProc, (LPARAM)&mapConnectorDialogParams);
			bool connectorMapped = ((mapConnectorDialogBoxParamResult == 1) && (mapConnectorDialogParams.selectionMade));

			//Ensure that a connector mapping has been made
			if(!connectorMapped)
			{
				std::wstring text = L"No connector mapping specified for imported connector " + systemClassName + L"." + i->className + L"!";
				std::wstring title = L"Error loading module!";
				SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);

				viewEventProcessingPaused = false;
				FlagProcessPendingEvents();
				return false;
			}

			//Map the selected connector
			ISystemExternal::ConnectorMapping connectorMapping;
			connectorMapping.connectorID = mapConnectorDialogParams.selectedConnector.GetConnectorID();
			connectorMapping.importingModuleConnectorInstanceName = i->instanceName;
			connectorMappings.push_back(connectorMapping);
		}
	}

	//Begin the module load process
	system.LoadModuleSynchronous(filePath, connectorMappings, *this);

	//Spawn a modal dialog to display the module load progress
	bool result = false;
	INT_PTR dialogBoxParamResult;
	dialogBoxParamResult = SafeDialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOADMODULE), mainWindowHandle, LoadModuleProc, (LPARAM)this);
	bool moduleLoadSucceeded = (dialogBoxParamResult == 1);
	if(moduleLoadSucceeded)
	{
		//Since the system loaded successfully, construct our dynamic menus.
		result = BuildFileMenu();
		result &= BuildSystemMenu();
		result &= BuildSettingsMenu();
		result &= BuildDebugMenu();
		if(!result)
		{
			std::wstring text = L"An error occurred while building the system, settings, or debug windows for the system.";
			std::wstring title = L"Error loading module!";
			SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);
		}
		else
		{
			systemLoaded = true;
		}
	}
	if(dialogBoxParamResult == 0)
	{
		//If there was an error loading the system, report it to the user. Note that a
		//return code of -1 indicates the user aborted the load, in which case, we don't
		//display this message.
		std::wstring text = L"An error occurred while trying to load the module file.\nCheck the log window for more information.";
		std::wstring title = L"Error loading module!";
		SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);
	}

	//Now that the operation is complete, resume view event processing, and fire off a
	//notification to check the queue immediately.
	viewEventProcessingPaused = false;
	FlagProcessPendingEvents();

	//Since the loaded system configuration has now changed, update any display info which
	//is dependent on the loaded modules.
	UpdateSaveSlots();
	UpdateModuleDisplayInfo();

	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::SaveSystem(const std::wstring& folder)
{
	std::wstring selectedFilePath;
	if(!SelectNewFile(L"Module Definitions|xml", L"xml", L"", folder, selectedFilePath))
	{
		return false;
	}

	return SaveSystemToFile(selectedFilePath);
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::SaveSystemToFile(const std::wstring& filePath)
{
	return system.SaveSystem(filePath);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::UnloadModule(unsigned int moduleID)
{
	//Spawn a worker thread to perform the module unload
	moduleCommandComplete = false;
	boost::thread workerThread(boost::bind(boost::mem_fn(&ExodusInterface::UnloadModuleThread), this, moduleID));

	//Open the unload system progress dialog
	INT_PTR dialogBoxParamResult;
	dialogBoxParamResult = SafeDialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UNLOADMODULE), mainWindowHandle, UnloadModuleProc, (LPARAM)this);

	//Initialize the menu
	BuildFileMenu();
	BuildSystemMenu();
	BuildSettingsMenu();
	BuildDebugMenu();

	//Since the loaded system configuration has now changed, update any display info which
	//is dependent on the loaded modules.
	UpdateSaveSlots();
	UpdateModuleDisplayInfo();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::UnloadAllModules()
{
	if(systemLoaded)
	{
		//Spawn a worker thread to perform the system unload
		moduleCommandComplete = false;
		boost::thread workerThread(boost::bind(boost::mem_fn(&ExodusInterface::UnloadSystemThread), this));

		//Open the unload system progress dialog
		INT_PTR dialogBoxParamResult;
		dialogBoxParamResult = SafeDialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_UNLOADMODULE), mainWindowHandle, UnloadModuleProc, (LPARAM)this);

		//Initialize the menu
		BuildFileMenu();
		BuildSystemMenu();
		BuildSettingsMenu();
		BuildDebugMenu();

		//Since the loaded system configuration has now changed, update any display info
		//which is dependent on the loaded modules.
		UpdateSaveSlots();
		UpdateModuleDisplayInfo();

		systemLoaded = false;
		moduleCommandComplete = true;
	}
}

//----------------------------------------------------------------------------------------
//Global preference functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadPrefs(const std::wstring& filePath)
{
	//Attempt to open the target preference file
	Stream::File file;
	if(!file.Open(filePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
	{
		return false;
	}

	//Attempt to decode the XML contents of the file
	file.SetTextEncoding(Stream::IStream::TEXTENCODING_UTF8);
	file.ProcessByteOrderMark();
	HierarchicalStorageTree tree;
	if(!tree.LoadTree(file))
	{
		return false;
	}

	//Validate the root node of the XML tree
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();
	if(rootNode.GetName() != L"Settings")
	{
		return false;
	}

	//Extract each preference from the loaded data
	std::list<IHierarchicalStorageNode*> childList = rootNode.GetChildList();
	for(std::list<IHierarchicalStorageNode*>::const_iterator i = childList.begin(); i != childList.end(); ++i)
	{
		if((*i)->GetName() == L"ModulesPath")
		{
			prefs.pathModulesRaw = (*i)->GetData();
		}
		else if((*i)->GetName() == L"SavestatesPath")
		{
			prefs.pathSavestatesRaw = (*i)->GetData();
		}
		else if((*i)->GetName() == L"PersistentStatePath")
		{
			prefs.pathPersistentStateRaw = (*i)->GetData();
		}
		else if((*i)->GetName() == L"WorkspacesPath")
		{
			prefs.pathWorkspacesRaw = (*i)->GetData();
		}
		else if((*i)->GetName() == L"CapturesPath")
		{
			prefs.pathCapturesRaw = (*i)->GetData();
		}
		else if((*i)->GetName() == L"AssembliesPath")
		{
			prefs.pathAssembliesRaw = (*i)->GetData();
		}
		else if((*i)->GetName() == L"DefaultSystem")
		{
			prefs.loadSystemRaw = (*i)->GetData();
		}
		else if((*i)->GetName() == L"DefaultWorkspace")
		{
			prefs.loadWorkspaceRaw = (*i)->GetData();
		}
		else if((*i)->GetName() == L"EnableThrottling")
		{
			prefs.enableThrottling = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"RunWhenProgramModuleLoaded")
		{
			prefs.runWhenProgramModuleLoaded = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"EnablePersistentState")
		{
			prefs.enablePersistentState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"LoadWorkspaceWithDebugState")
		{
			prefs.loadWorkspaceWithDebugState = (*i)->ExtractData<bool>();
		}
		else if((*i)->GetName() == L"ShowDebugConsole")
		{
			prefs.showDebugConsole = (*i)->ExtractData<bool>();
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SavePrefs(const std::wstring& filePath)
{
	HierarchicalStorageTree tree;
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();
	rootNode.SetName(L"Settings");
	rootNode.CreateChild(L"ModulesPath").SetData(prefs.pathModulesRaw);
	rootNode.CreateChild(L"SavestatesPath").SetData(prefs.pathSavestatesRaw);
	rootNode.CreateChild(L"PersistentStatePath").SetData(prefs.pathPersistentStateRaw);
	rootNode.CreateChild(L"WorkspacesPath").SetData(prefs.pathWorkspacesRaw);
	rootNode.CreateChild(L"CapturesPath").SetData(prefs.pathCapturesRaw);
	rootNode.CreateChild(L"AssembliesPath").SetData(prefs.pathAssembliesRaw);
	rootNode.CreateChild(L"DefaultSystem").SetData(prefs.loadSystemRaw);
	rootNode.CreateChild(L"DefaultWorkspace").SetData(prefs.loadWorkspaceRaw);
	rootNode.CreateChild(L"EnableThrottling").SetData(prefs.enableThrottling);
	rootNode.CreateChild(L"RunWhenProgramModuleLoaded").SetData(prefs.runWhenProgramModuleLoaded);
	rootNode.CreateChild(L"EnablePersistentState").SetData(prefs.enablePersistentState);
	rootNode.CreateChild(L"LoadWorkspaceWithDebugState").SetData(prefs.loadWorkspaceWithDebugState);
	rootNode.CreateChild(L"ShowDebugConsole").SetData(prefs.showDebugConsole);

	Stream::File file(Stream::IStream::TEXTENCODING_UTF8);
	if(file.Open(filePath, Stream::File::OPENMODE_READANDWRITE, Stream::File::CREATEMODE_CREATE))
	{
		file.InsertByteOrderMark();
		tree.SaveTree(file);
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ResolvePrefs()
{
	//Resolve all raw paths in our loaded preferences down to absolute paths
	prefs.pathModules = prefs.pathModulesRaw;
	if(PathIsRelativePath(prefs.pathModules))
	{
		prefs.pathModules = PathCombinePaths(originalWorkingDir, prefs.pathModules);
	}
	prefs.pathSavestates = prefs.pathSavestatesRaw;
	if(PathIsRelativePath(prefs.pathSavestates))
	{
		prefs.pathSavestates = PathCombinePaths(originalWorkingDir, prefs.pathSavestates);
	}
	prefs.pathPersistentState = prefs.pathPersistentStateRaw;
	if(PathIsRelativePath(prefs.pathPersistentState))
	{
		prefs.pathPersistentState = PathCombinePaths(originalWorkingDir, prefs.pathPersistentState);
	}
	prefs.pathWorkspaces = prefs.pathWorkspacesRaw;
	if(PathIsRelativePath(prefs.pathWorkspaces))
	{
		prefs.pathWorkspaces = PathCombinePaths(originalWorkingDir, prefs.pathWorkspaces);
	}
	prefs.pathCaptures = prefs.pathCapturesRaw;
	if(PathIsRelativePath(prefs.pathCaptures))
	{
		prefs.pathCaptures = PathCombinePaths(originalWorkingDir, prefs.pathCaptures);
	}
	prefs.pathAssemblies = prefs.pathAssembliesRaw;
	if(PathIsRelativePath(prefs.pathAssemblies))
	{
		prefs.pathAssemblies = PathCombinePaths(originalWorkingDir, prefs.pathAssemblies);
	}
	prefs.loadSystem = prefs.loadSystemRaw;
	if(!prefs.loadSystem.empty())
	{
		if(PathIsRelativePath(prefs.loadSystem))
		{
			prefs.loadSystem = PathCombinePaths(prefs.pathModules, prefs.loadSystem);
		}
	}
	prefs.loadWorkspace = prefs.loadWorkspaceRaw;
	if(!prefs.loadWorkspace.empty())
	{
		if(PathIsRelativePath(prefs.loadWorkspace))
		{
			prefs.loadWorkspace = PathCombinePaths(prefs.pathWorkspaces, prefs.loadWorkspace);
		}
	}

	//Ensure all the directories referenced by our preferences exist
	CreateDirectory(prefs.pathModules, true);
	CreateDirectory(prefs.pathSavestates, true);
	CreateDirectory(prefs.pathPersistentState, true);
	CreateDirectory(prefs.pathWorkspaces, true);
	CreateDirectory(prefs.pathCaptures, true);
	CreateDirectory(prefs.pathAssemblies, true);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ApplyPrefs()
{
	//Propagate preferences to the system object
	system.SetCapturePath(prefs.pathCaptures);
	system.SetRunWhenProgramModuleLoadedState(prefs.runWhenProgramModuleLoaded);
	system.SetThrottlingState(prefs.enableThrottling);

	//Apply the new debug console setting
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

	//Update our savestate cell slots
	UpdateSaveSlots();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::GetGlobalPreferencePathModulesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetGlobalPreferencePathModules());
}

//----------------------------------------------------------------------------------------
void ExodusInterface::GetGlobalPreferencePathSavestatesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetGlobalPreferencePathSavestates());
}

//----------------------------------------------------------------------------------------
void ExodusInterface::GetGlobalPreferencePathPersistentStateInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetGlobalPreferencePathPersistentState());
}

//----------------------------------------------------------------------------------------
void ExodusInterface::GetGlobalPreferencePathWorkspacesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetGlobalPreferencePathWorkspaces());
}

//----------------------------------------------------------------------------------------
void ExodusInterface::GetGlobalPreferencePathCapturesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetGlobalPreferencePathCaptures());
}

//----------------------------------------------------------------------------------------
void ExodusInterface::GetGlobalPreferencePathAssembliesInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetGlobalPreferencePathAssemblies());
}

//----------------------------------------------------------------------------------------
void ExodusInterface::GetGlobalPreferenceInitialSystemInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetGlobalPreferenceInitialSystem());
}

//----------------------------------------------------------------------------------------
void ExodusInterface::GetGlobalPreferenceInitialWorkspaceInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetGlobalPreferenceInitialWorkspace());
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::GetGlobalPreferenceEnableThrottling() const
{
	return prefs.enableThrottling;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::GetGlobalPreferenceRunWhenProgramModuleLoaded() const
{
	return prefs.runWhenProgramModuleLoaded;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::GetGlobalPreferenceEnablePersistentState() const
{
	return prefs.enablePersistentState;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::GetGlobalPreferenceLoadWorkspaceWithDebugState() const
{
	return prefs.loadWorkspaceWithDebugState;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::GetGlobalPreferenceShowDebugConsole() const
{
	return prefs.showDebugConsole;
}

//----------------------------------------------------------------------------------------
//Assembly functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadAssembliesFromFolder(const std::wstring& folderPath)
{
	//Begin the plugin load process
	loadPluginsComplete = false;
	loadPluginsProgress = 0;
	loadPluginsAborted = false;
	boost::thread workerThread(boost::bind(boost::mem_fn(&ExodusInterface::LoadAssembliesFromFolderSynchronous), this, boost::ref(folderPath)));

	//Spawn a modal dialog to display the plugin load progress
	bool result = true;
	INT_PTR dialogBoxParamResult;
	dialogBoxParamResult = SafeDialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOADPLUGIN), mainWindowHandle, LoadPluginProc, (LPARAM)this);
	if(dialogBoxParamResult == 0)
	{
		//If there was an error loading the system, report it to the user. Note that a
		//return code of -1 indicates the user aborted the load, in which case, we don't
		//display this message.
		std::wstring text = L"An error occurred while trying to load one or more plugins.\nCheck the log window for more information.";
		std::wstring title = L"Error loading plugin!";
		SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);
		result = false;
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadAssembliesFromFolderSynchronous(const std::wstring& folderPath)
{
	//Begin the folder search
	std::wstring fileSearchString = PathCombinePaths(folderPath, L"*.dll");
	WIN32_FIND_DATA findData;
	HANDLE findFileHandle;
	findFileHandle = FindFirstFile(fileSearchString.c_str(), &findData);
	if(findFileHandle == INVALID_HANDLE_VALUE)
	{
		loadPluginsResult = false;
		loadPluginsComplete = true;
		return false;
	}

	//Build a list of all possible plugins in the target folder
	std::list<std::wstring> pluginPaths;
	bool foundFile = true;
	while(foundFile)
	{
		//Build the name and path of this entry
		std::wstring entryName = findData.cFileName;
		std::wstring entryPath = PathCombinePaths(folderPath, entryName);

		//Ensure this isn't a virtual navigation folder
		if(entryName.find_first_not_of(L'.') != std::wstring::npos)
		{
			//Ensure we're actually looking at a file
			if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				pluginPaths.push_back(entryPath);
			}
		}

		//Advance to the next matching entry in the folder
		foundFile = FindNextFile(findFileHandle, &findData) != 0;
	}

	//End the folder search
	FindClose(findFileHandle);

	//Attempt to load all possible plugins found in the target path
	unsigned int entriesProcessed = 0;
	unsigned int entryCount = (unsigned int)pluginPaths.size();
	for(std::list<std::wstring>::const_iterator i = pluginPaths.begin(); !loadPluginsAborted && (i != pluginPaths.end()); ++i)
	{
		//Update the progress and current plugin name
		boost::mutex::scoped_lock lock(loadPluginsMutex);
		loadPluginsCurrentPluginName = *i;
		loadPluginsProgress = ((float)++entriesProcessed / (float)entryCount);
		lock.unlock();

		//Load this plugin
		LoadAssembly(*i);
	}

	//Set the result of the plugin load operation, and flag the load operation as
	//complete.
	loadPluginsResult = true;
	loadPluginsComplete = true;

	return true;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadAssemblyInternal(const InteropSupport::ISTLObjectSource<std::wstring>& filePathMarshaller)
{
	return LoadAssembly(filePathMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadAssembly(const std::wstring& filePath)
{
	//Attach the assembly to the process
	HMODULE dllHandle = LoadLibrary(filePath.c_str());
	if(dllHandle == NULL)
	{
		//##DEBUG##
		std::wcout << "Error loading assembly " << filePath << "!\n"
		           << "LoadLibrary failed with error code " << GetLastError() << '\n';
		return false;
	}

	//##DEBUG##
	std::wcout << L"Loading assembly:\t" << filePath << '\n';

	//Ensure the assembly exports the core GetInterfaceVersion function, and obtain a
	//pointer to it.
	unsigned int (*GetInterfaceVersion)();
	GetInterfaceVersion = (unsigned int (*)())GetProcAddress(dllHandle, "GetInterfaceVersion");
	if(GetInterfaceVersion == 0)
	{
		//##DEBUG##
		std::wcout << "Error loading assembly " << filePath << "!\n"
		           << "This assembly doesn't appear to be a valid plugin!\n";
		FreeLibrary(dllHandle);
		return false;
	}

	//Validate the interface version of the assembly
	if(GetInterfaceVersion() < EXODUS_INTERFACEVERSION)
	{
		//##DEBUG##
		std::wcout << "Error loading assembly " << filePath << "!\n"
		           << "This assembly has an interface version number of " << GetInterfaceVersion() << ". A minimum interface\n"
		           << "version of " << EXODUS_INTERFACEVERSION << " is required.\n";
		FreeLibrary(dllHandle);
		return false;
	}

	//Obtain pointers to all the interface functions for the assembly
	bool (*GetDeviceEntry)(unsigned int entryNo, IDeviceInfo& entry);
	bool (*GetExtensionEntry)(unsigned int entryNo, IExtensionInfo& entry);
	GetDeviceEntry = (bool (*)(unsigned int entryNo, IDeviceInfo& entry))GetProcAddress(dllHandle, "GetDeviceEntry");
	GetExtensionEntry = (bool (*)(unsigned int entryNo, IExtensionInfo& entry))GetProcAddress(dllHandle, "GetExtensionEntry");
	if((GetDeviceEntry == 0) && (GetExtensionEntry == 0))
	{
		//##DEBUG##
		std::wcout << "Error loading assembly " << filePath << "!\n"
		           << "The assembly appears to be a plugin, but is missing required exports!\n";
		FreeLibrary(dllHandle);
		return false;
	}

	//Register each device in the assembly
	bool result = true;
	if(GetDeviceEntry != 0)
	{
		unsigned int entryNo = 0;
		DeviceInfo entry;
		while(GetDeviceEntry(entryNo, entry))
		{
			//##DEBUG##
			std::wcout << L"Registering device \"" << entry.GetDeviceImplementationName() << L"\"...";

			if(!system.RegisterDevice(entry, (IDevice::AssemblyHandle)dllHandle))
			{
				//##DEBUG##
				std::wcout << L"Failed! Check event log.\n";

				result = false;
				++entryNo;
				continue;
			}

			//##DEBUG##
			std::wcout << L"Done.\n";

			RegisteredDeviceInfo registeredDeviceInfo;
			registeredDeviceInfo.assemblyPath = filePath;
			registeredDeviceInfo.info = entry;
			registeredDevices.push_back(registeredDeviceInfo);

			++entryNo;
		}
	}

	//Register each extension in the assembly
	if(GetExtensionEntry != 0)
	{
		unsigned int entryNo = 0;
		ExtensionInfo entry;
		while(GetExtensionEntry(entryNo, entry))
		{
			//##DEBUG##
			std::wcout << L"Registering extension \"" << entry.GetExtensionImplementationName() << L"\"...";

			if(!system.RegisterExtension(entry, (IExtension::AssemblyHandle)dllHandle))
			{
				//##DEBUG##
				std::wcout << L"Failed! Check event log.\n";

				result = false;
				++entryNo;
				continue;
			}

			//##DEBUG##
			std::wcout << L"Done.\n";

			RegisteredExtensionInfo registeredExtensionInfo;
			registeredExtensionInfo.assemblyPath = filePath;
			registeredExtensionInfo.info = entry;
			registeredExtensions.push_back(registeredExtensionInfo);

			++entryNo;
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
//File selection functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::SelectExistingFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, bool scanIntoArchives, std::wstring& selectedFilePath) const
{
	//Break the supplied selection type string down into individual type entries
	std::list<FileSelectionType> selectionTypes = ParseSelectionTypeString(selectionTypeString);

	//Build a modified selection type list, adding all supported archive formats to each
	//type entry if scanning into archives has been requested.
	std::list<FileSelectionType> fullSelectionTypes = selectionTypes;
	if(scanIntoArchives)
	{
		for(std::list<FileSelectionType>::iterator i = fullSelectionTypes.begin(); i != fullSelectionTypes.end(); ++i)
		{
			i->extensionFilters.push_back(L"zip");
		}
	}

	//Extract the first component from our combined path
	std::vector<std::wstring> initialPathElements = PathSplitElements(initialFilePath);
	std::wstring initialPathFirstElement = initialPathElements[0];

	//Attempt to select the target file until one is selected, or the selection process is
	//aborted.
	bool fileSelected = false;
	bool fileSelectionAborted = false;
	while(!fileSelected && !fileSelectionAborted)
	{
		//Select a target file, using the first element in the supplied path as the
		//initial target.
		if(!::SelectExistingFile(mainWindowHandle, fullSelectionTypes, defaultExtension, initialPathFirstElement, initialDirectory, selectedFilePath))
		{
			fileSelectionAborted = true;
			continue;
		}

		//If the user has requested that we scan into archives for the target file, and a
		//supported archive format has been selected, scan into that archive for a
		//compatible target file.
		if(scanIntoArchives)
		{
			//##TODO## Add support for more compression archive formats
			//##TODO## Make this comparison case insensitive
			std::wstring selectedFileExtension = PathGetFileExtension(selectedFilePath);
			if(selectedFileExtension == L"zip")
			{
				//Attempt to find a compatible file in this archive. Note that we pass the
				//original selection types here, not the modified selection type list,
				//since we only want to match types that were originally specified.
				std::wstring relativePathWithinArchive;
				if(!SelectExistingFileScanIntoArchive(selectionTypes, selectedFilePath, relativePathWithinArchive))
				{
					continue;
				}

				//Build the full path to the selected file within the archive
				selectedFilePath = selectedFilePath + L'|' + relativePathWithinArchive;
			}
		}

		//Flag that a file has been selected
		fileSelected = true;
	}

	return fileSelected;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::SelectExistingFileScanIntoArchive(const std::list<FileSelectionType>& selectionTypes, const std::wstring archivePath, std::wstring& selectedFilePath) const
{
	//Open the target archive file
	Stream::File archiveFile;
	if(!archiveFile.Open(archivePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
	{
		std::wstring title = L"Error opening file!";
		std::wstring text = L"Could not open the target compressed archive.";
		SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);
		return false;
	}

	//Load the archive from the file
	ZIPArchive archive;
	if(!archive.LoadFromStream(archiveFile))
	{
		std::wstring title = L"Error opening file!";
		std::wstring text = L"Failed to read the structure of the compressed archive.";
		SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);
		return false;
	}

	//Build list of all files in the archive that match the target type
	std::map<unsigned int, std::wstring> fileNamesInArchive;
	unsigned int archiveEntryCount = archive.GetFileEntryCount();
	for(unsigned int i = 0; i < archiveEntryCount; ++i)
	{
		//Retrieve this file entry
		ZIPFileEntry* fileEntry = archive.GetFileEntry(i);
		if(fileEntry == 0)
		{
			std::wstring title = L"Error opening file!";
			std::wstring text = L"Failed to read the structure of the compressed archive.";
			SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);
			return false;
		}

		//Obtain the name and extension of this file entry
		std::wstring fileName = fileEntry->GetFileName();
		std::wstring fileExtension = PathGetFileExtension(fileName);

		//Ensure that this file extension matches one of the target file extensions, and
		//skip it if it does not.
		bool foundMatchingExtension = false;
		std::list<FileSelectionType>::const_iterator selectionTypeIterator = selectionTypes.begin();
		while(!foundMatchingExtension && (selectionTypeIterator != selectionTypes.end()))
		{
			std::list<std::wstring>::const_iterator extensionFilterIterator = selectionTypeIterator->extensionFilters.begin();
			while(!foundMatchingExtension && (extensionFilterIterator != selectionTypeIterator->extensionFilters.end()))
			{
				//##TODO## Add support for wildcards and unknown characters in extension
				//filter expressions.
				if(*extensionFilterIterator == fileExtension)
				{
					foundMatchingExtension = true;
				}
				++extensionFilterIterator;
			}
			++selectionTypeIterator;
		}
		if(!foundMatchingExtension)
		{
			continue;
		}

		//Insert this file entry into the list of possible target files
		fileNamesInArchive.insert(std::pair<unsigned int, std::wstring>(i, fileEntry->GetFileName()));
	}

	//If no possible target files were found, return an error.
	if(fileNamesInArchive.empty())
	{
		std::wstring title = L"Error opening file!";
		std::wstring text = L"No matching file types were found within the compressed archive.";
		SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);
		return false;
	}

	//If there was only one matching file in the target archive, select that file,
	//otherwise ask the user to select the file to use.
	if(fileNamesInArchive.size() == 1)
	{
		selectedFilePath = fileNamesInArchive.begin()->second;
	}
	else
	{
		//Build the list of files to choose from for the selection dialog
		std::list<SelectCompressedFileDialogParamsFileEntry> fileListForSelection;
		for(std::map<unsigned int, std::wstring>::const_iterator i = fileNamesInArchive.begin(); i != fileNamesInArchive.end(); ++i)
		{
			SelectCompressedFileDialogParamsFileEntry entry;
			entry.entryID = i->first;
			entry.fileName = i->second;
			fileListForSelection.push_back(entry);
		}

		//Open a dialog allowing the user to select a file from the list of matching files
		SelectCompressedFileDialogParams selectCompressedFileDialogParams;
		selectCompressedFileDialogParams.fileList = fileListForSelection;
		INT_PTR selectCompressedFileDialogBoxParamResult;
		selectCompressedFileDialogBoxParamResult = SafeDialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECTCOMPRESSEDFILE), mainWindowHandle, SelectCompressedFileProc, (LPARAM)&selectCompressedFileDialogParams);
		if(selectCompressedFileDialogBoxParamResult <= 0)
		{
			std::wstring title = L"Error opening file!";
			std::wstring text = L"An error occurred while attempting to display the archive content selection dialog.";
			SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);
			return false;
		}

		//If no selection was made in the dialog because the user canceled the selection,
		//we return false, but we don't report an error to the user, because this state
		//only occurs when the user specifically cancels the file selection.
		if(!selectCompressedFileDialogParams.selectionMade)
		{
			return false;
		}

		//Load the path to the selected file into the selected file path
		selectedFilePath = fileNamesInArchive[selectCompressedFileDialogParams.selectedEntryID];
	}

	return true;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::SelectNewFile(const std::wstring& selectionTypeString, const std::wstring& defaultExtension, const std::wstring& initialFilePath, const std::wstring& initialDirectory, std::wstring& selectedFilePath) const
{
	//Extract the first component from our combined path
	std::vector<std::wstring> initialPathElements = PathSplitElements(initialFilePath);
	std::wstring initialPathFirstElement = initialPathElements[0];

	//Select a target file, using the first element in the supplied path as the initial
	//target.
	if(!::SelectNewFile(mainWindowHandle, selectionTypeString, defaultExtension, initialPathFirstElement, initialDirectory, selectedFilePath))
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------
std::vector<std::wstring> ExodusInterface::PathSplitElements(const std::wstring& path) const
{
	//##TODO## Format and comment this
	const std::wstring elementSeparators = L"|";
	std::vector<std::wstring> pathElements;
	std::wstring::size_type currentPos = 0;
	while(currentPos != std::wstring::npos)
	{
		std::wstring::size_type separatorPos = path.find_first_of(elementSeparators, currentPos);
		std::wstring::size_type pathElementEndPos = (separatorPos != std::wstring::npos)? separatorPos - currentPos: std::wstring::npos;
		std::wstring pathElement = path.substr(currentPos, pathElementEndPos);
		pathElements.push_back(pathElement);
		currentPos = (separatorPos != std::wstring::npos)? (separatorPos + 1): std::wstring::npos;
	}
	return pathElements;
}

//----------------------------------------------------------------------------------------
Stream::IStream* ExodusInterface::OpenExistingFileForRead(const std::wstring& path) const
{
	//##TODO## Provide more comments
	std::vector<std::wstring> pathElements = PathSplitElements(path);
	Stream::IStream* tempStream = 0;
	for(unsigned int i = 0; i < pathElements.size(); ++i)
	{
		if(tempStream == 0)
		{
			//Open the target file
			Stream::File* file = new Stream::File();
			tempStream = file;
			if(!file->Open(pathElements[i], Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
			{
				delete tempStream;
				return 0;
			}
		}
		else
		{
			//Load the ZIP header structure
			ZIPArchive archive;
			bool archiveLoadResult = false;
			archiveLoadResult = archive.LoadFromStream(*tempStream);
			if(!archiveLoadResult)
			{
				delete tempStream;
				return 0;
			}

			//Retrieve the target file entry from the archive
			ZIPFileEntry* entry = archive.GetFileEntry(pathElements[i]);
			if(entry == 0)
			{
				delete tempStream;
				return 0;
			}

			//Decompress the target file
			Stream::Buffer* buffer = new Stream::Buffer(0);
			if(!entry->Decompress(*buffer))
			{
				delete buffer;
				delete tempStream;
				return 0;
			}
			buffer->SetStreamPos(0);

			//Replace the current stream with the decompressed target file stream
			delete tempStream;
			tempStream = buffer;
		}
	}

	return tempStream;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::SelectExistingFileInternal(const InteropSupport::ISTLObjectSource<std::wstring>& selectionTypeStringMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& defaultExtensionMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& initialFilePathMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& initialDirectoryMarshaller, bool scanIntoArchives, const InteropSupport::ISTLObjectTarget<std::wstring>& selectedFilePathMarshaller) const
{
	std::wstring selectedFilePath;
	bool result = SelectExistingFile(selectionTypeStringMarshaller.MarshalTo(), defaultExtensionMarshaller.MarshalTo(), initialFilePathMarshaller.MarshalTo(), initialDirectoryMarshaller.MarshalTo(), scanIntoArchives, selectedFilePath);
	selectedFilePathMarshaller.MarshalFrom(selectedFilePath);
	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::SelectNewFileInternal(const InteropSupport::ISTLObjectSource<std::wstring>& selectionTypeStringMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& defaultExtensionMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& initialFilePathMarshaller, const InteropSupport::ISTLObjectSource<std::wstring>& initialDirectoryMarshaller, const InteropSupport::ISTLObjectTarget<std::wstring>& selectedFilePathMarshaller) const
{
	std::wstring selectedFilePath;
	bool result = SelectNewFile(selectionTypeStringMarshaller.MarshalTo(), defaultExtensionMarshaller.MarshalTo(), initialFilePathMarshaller.MarshalTo(), initialDirectoryMarshaller.MarshalTo(), selectedFilePath);
	selectedFilePathMarshaller.MarshalFrom(selectedFilePath);
	return result;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::PathSplitElementsInternal(const InteropSupport::ISTLObjectTarget<std::vector<std::wstring>>& marshaller, const InteropSupport::ISTLObjectSource<std::wstring>& pathMarshaller) const
{
	marshaller.MarshalFrom(PathSplitElements(pathMarshaller.MarshalTo()));
}

//----------------------------------------------------------------------------------------
Stream::IStream* ExodusInterface::OpenExistingFileForReadInternal(const InteropSupport::ISTLObjectSource<std::wstring>& pathMarshaller) const
{
	return OpenExistingFileForRead(pathMarshaller.MarshalTo());
}

//----------------------------------------------------------------------------------------
void ExodusInterface::DeleteFileStream(Stream::IStream* stream) const
{
	delete stream;
}

//----------------------------------------------------------------------------------------
//View management functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::OpenViewModel(IViewModel* aviewModel, bool waitToClose, bool openHidden)
{
	boost::mutex::scoped_lock lock(viewMutex);

	//Ensure this view isn't already open or in the queue to be opened
	if(viewInfoSet.find(aviewModel) != viewInfoSet.end())
	{
		return false;
	}

	//Create the ViewInfo object and add it to the list of views
	ViewInfo* viewInfo = new ViewInfo;
	viewInfo->viewModel = aviewModel;
	viewInfo->openHidden = openHidden;
	viewInfo->viewCurrentlyOpen = false;
	viewInfoSet.insert(ViewInfoSetEntry(viewInfo->viewModel, viewInfo));

	//Add an operation to the queue to open this view
	viewOperationQueue.push_back(ViewOperation(VIEWOPERATIONTYPE_OPEN, aviewModel));
	viewOperationPending = true;
	FlagProcessPendingEvents();

	//Wait for the view to close
	if(waitToClose)
	{
		++viewInfo->waitCount;
		viewInfo->viewClosed.wait(lock);
		--viewInfo->waitCount;

		//Inform the worker thread that the view info is safe to delete
		if(viewInfo->waitCount <= 0)
		{
			viewInfo->viewInfoSafeToDelete.notify_all();
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::CloseViewModel(IViewModel* aviewModel, bool waitToClose)
{
	boost::mutex::scoped_lock lock(viewMutex);

	//Retrieve the viewInfo structure for this view
	ViewInfo* viewInfo = 0;
	ViewInfoSet::iterator i = viewInfoSet.find(aviewModel);
	if(i == viewInfoSet.end())
	{
		return;
	}
	viewInfo = i->second;

	//Add an operation to the queue to close this view
	viewOperationQueue.push_back(ViewOperation(VIEWOPERATIONTYPE_CLOSE, aviewModel));
	viewOperationPending = true;
	FlagProcessPendingEvents();

	//Wait for the view to close
	if(waitToClose)
	{
		++viewInfo->waitCount;
		viewInfo->viewClosed.wait(lock);
		--viewInfo->waitCount;

		//Inform the worker thread that the view info is safe to delete
		if(viewInfo->waitCount <= 0)
		{
			viewInfo->viewInfoSafeToDelete.notify_all();
		}
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ShowViewModel(IViewModel* aviewModel)
{
	//Add an operation to the queue to show this view
	boost::mutex::scoped_lock lock(viewMutex);
	viewOperationQueue.push_back(ViewOperation(VIEWOPERATIONTYPE_SHOW, aviewModel));
	viewOperationPending = true;
	FlagProcessPendingEvents();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::HideViewModel(IViewModel* aviewModel)
{
	//Add an operation to the queue to hide this view
	boost::mutex::scoped_lock lock(viewMutex);
	viewOperationQueue.push_back(ViewOperation(VIEWOPERATIONTYPE_HIDE, aviewModel));
	viewOperationPending = true;
	FlagProcessPendingEvents();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ActivateViewModel(IViewModel* aviewModel)
{
	//Add an operation to the queue to activate this view
	boost::mutex::scoped_lock lock(viewMutex);
	viewOperationQueue.push_back(ViewOperation(VIEWOPERATIONTYPE_ACTIVATE, aviewModel));
	viewOperationPending = true;
	FlagProcessPendingEvents();
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::WaitUntilViewModelOpened(IViewModel* aviewModel)
{
	boost::mutex::scoped_lock lock(viewMutex);

	//Retrieve the viewInfo structure for this view
	ViewInfo* viewInfo = 0;
	ViewInfoSet::iterator i = viewInfoSet.find(aviewModel);
	if(i == viewInfoSet.end())
	{
		return false;
	}
	viewInfo = i->second;

	//Wait for the view to open
	//##TODO## Check if our logic here is correct. Shouldn't we increment waitCount here,
	//and flag viewInfoSafeToDelete if the final waitCount is 0?
	if(!viewInfo->viewCurrentlyOpen)
	{
		viewInfo->viewOpened.wait(lock);
	}

	return true;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::WaitUntilViewModelClosed(IViewModel* aviewModel)
{
	boost::mutex::scoped_lock lock(viewMutex);

	//Retrieve the viewInfo structure for this view
	ViewInfo* viewInfo = 0;
	ViewInfoSet::iterator i = viewInfoSet.find(aviewModel);
	if(i == viewInfoSet.end())
	{
		return;
	}
	viewInfo = i->second;

	//Wait for the view to close
	++viewInfo->waitCount;
	viewInfo->viewClosed.wait(lock);
	--viewInfo->waitCount;
	//Inform the worker thread that the view info is safe to delete
	if(viewInfo->waitCount <= 0)
	{
		viewInfo->viewInfoSafeToDelete.notify_all();
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::NotifyModelViewClosed(IViewModel* aviewModel)
{
	boost::mutex::scoped_lock lock(viewMutex);
	viewOperationQueue.push_back(ViewOperation(VIEWOPERATIONTYPE_DELETE, aviewModel));
	viewOperationPending = true;
	FlagProcessPendingEvents();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::FlagProcessPendingEvents()
{
	//Post a message to the UI thread to make it check for pending events
	PostMessage(mainWindowHandle, WM_USER+1, 0, 0);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ProcessPendingEvents()
{
	if(viewOperationPending)
	{
		boost::mutex::scoped_lock lock(viewMutex);
		while(!viewOperationQueue.empty())
		{
			//Grab the details of the next view operation, and pop it from the queue.
			ViewOperationQueue::iterator nextViewOperationIterator = viewOperationQueue.begin();
			ViewOperation viewOperation = *nextViewOperationIterator;
			viewOperationQueue.erase(nextViewOperationIterator);

			//Attempt to locate the view info data for this view
			ViewModels::iterator viewInfoSetIterator = viewInfoSet.find(viewOperation.viewModel);
			if(viewInfoSetIterator != viewInfoSet.end())
			{
				//Get the view info data associated with this view
				ViewInfo* viewInfo = viewInfoSetIterator->second;

				//Process the operation. Note that we explicitly release our lock here.
				//This allows the view model to call back into routines which affect views.
				lock.unlock();
				switch(viewOperation.type)
				{
				case VIEWOPERATIONTYPE_OPEN:
					ProcessOpenView(viewOperation.viewModel, viewInfo);
					break;
				case VIEWOPERATIONTYPE_CLOSE:
					ProcessCloseView(viewOperation.viewModel, viewInfo);
					break;
				case VIEWOPERATIONTYPE_DELETE:
					ProcessDeleteView(viewOperation.viewModel, viewInfo);
					break;
				case VIEWOPERATIONTYPE_ACTIVATE:
					ProcessActivateView(viewOperation.viewModel, viewInfo);
					break;
				case VIEWOPERATIONTYPE_SHOW:
					ProcessShowView(viewOperation.viewModel, viewInfo);
					break;
				case VIEWOPERATIONTYPE_HIDE:
					ProcessHideView(viewOperation.viewModel, viewInfo);
					break;
				}
				lock.lock();
			}
		}

		//Now that we have processed all pending view events, flag that there are no view
		//operations pending.
		viewOperationPending = false;
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ProcessOpenView(IViewModel* viewModel, ViewInfo* viewInfo)
{
	//Create an entry for this dialog in the list of open dialog view models
	boost::mutex::scoped_lock lock(viewMutex);
	viewModels.insert(ViewModelsEntry(viewInfo->viewModel, viewInfo));
	lock.unlock();

	//Create a notifier for the view model
	viewInfo->notifier = new ViewModelNotifier(*this, viewInfo->viewModel);

	//Request the view to open
	if(!viewInfo->viewModel->OpenView(viewInfo->notifier, (void*)mainWindowHandle, 0, 0))
	{
		viewInfo->notifier->NotifyViewClosed();
	}
	else
	{
		//Obtain the width and height of the view
		int sizex;
		int sizey;
		viewInfo->viewModel->GetViewSize(sizex, sizey);

		//Flag that we want to ignore this view for layout purposes. Since the
		//view currently has a position, but this is the view we want to
		//re-position, we want to ignore it when calculating free regions.
		viewInfo->ignoreForLayoutPurposes = true;

		//Calculate the screen position to display the view at
		int xpos = 0;
		int ypos = 0;
		GetNewWindowPosition(sizex, sizey, xpos, ypos);

		//Now that we've positioned the window, make it active for layout purposes
		//again.
		viewInfo->ignoreForLayoutPurposes = false;

		//Position the view
		viewInfo->viewModel->SetViewPosition(xpos, ypos);

		//Show the view
		if(!viewInfo->openHidden)
		{
			viewInfo->viewModel->ShowView();
		}
	}

	//Notify any waiting threads that the view has now been opened
	viewInfo->viewCurrentlyOpen = true;
	viewInfo->viewOpened.notify_all();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ProcessCloseView(IViewModel* viewModel, ViewInfo* viewInfo)
{
	//Request the view to close
	viewModel->CloseView();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ProcessDeleteView(IViewModel* viewModel, ViewInfo* viewInfo)
{
	//Erase all references to this view from our lists of views
	boost::mutex::scoped_lock lock(viewMutex);
	ViewOperationQueue::iterator viewOperationQueueIterator = viewOperationQueue.begin();
	while(viewOperationQueueIterator != viewOperationQueue.end())
	{
		if(viewOperationQueueIterator->viewModel == viewModel)
		{
			ViewOperationQueue::iterator viewOperationQueueIteratorToErase = viewOperationQueueIterator;
			++viewOperationQueueIterator;
			viewOperationQueue.erase(viewOperationQueueIteratorToErase);
		}
		else
		{
			++viewOperationQueueIterator;
		}
	}
	viewModels.erase(viewModel);
	viewInfoSet.erase(viewModel);

	//Wait for all threads to inform us that we can now safely delete the view info data
	while(viewInfo->waitCount > 0)
	{
		viewInfo->viewClosed.notify_all();
		viewInfo->viewInfoSafeToDelete.wait(lock);
	}

	//Clean up the view info
	delete viewInfo->notifier;
	viewInfo->notifier = 0;
	delete viewInfo;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ProcessActivateView(IViewModel* viewModel, ViewInfo* viewInfo)
{
	//Request the view to activate
	viewModel->ActivateView();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ProcessShowView(IViewModel* viewModel, ViewInfo* viewInfo)
{
	//Request the view to show
	viewModel->ShowView();
}

//----------------------------------------------------------------------------------------
void ExodusInterface::ProcessHideView(IViewModel* viewModel, ViewInfo* viewInfo)
{
	//Request the view to hide
	viewModel->HideView();
}

//----------------------------------------------------------------------------------------
//View closing helper functions
//----------------------------------------------------------------------------------------
void ExodusInterface::CloseViewsForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName)
{
	//Build a set of all view models to close
	std::set<IViewModel*> viewModelsToClose;
	for(ViewModels::iterator i = viewModels.begin(); i != viewModels.end(); ++i)
	{
		ViewInfo& viewInfo = *i->second;
		if(viewInfo.viewCurrentlyOpen && viewInfo.viewModel->IsViewOwnerDevice() && (viewInfo.viewModel->GetViewOwnerModuleID() == moduleID) && (viewInfo.viewModel->GetViewOwnerDeviceInstanceName() == deviceInstanceName))
		{
			viewModelsToClose.insert(viewInfo.viewModel);
		}
	}

	//Close all selected view models
	for(std::set<IViewModel*>::const_iterator i = viewModelsToClose.begin(); i != viewModelsToClose.end(); ++i)
	{
		CloseViewModel(*i, true);
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::CloseViewsForModule(unsigned int moduleID, bool closeDependentModuleViews)
{
	//Build a set of all view models to close
	std::set<IViewModel*> viewModelsToClose;
	for(ViewModels::iterator i = viewModels.begin(); i != viewModels.end(); ++i)
	{
		ViewInfo& viewInfo = *i->second;
		if(viewInfo.viewCurrentlyOpen && viewInfo.viewModel->IsViewOwnerDevice() && (viewInfo.viewModel->GetViewOwnerModuleID() == moduleID))
		{
			viewModelsToClose.insert(viewInfo.viewModel);
		}
	}

	//Close all selected view models
	for(std::set<IViewModel*>::const_iterator i = viewModelsToClose.begin(); i != viewModelsToClose.end(); ++i)
	{
		CloseViewModel(*i, true);
	}

	//##TODO## Recursively call this function for any modules which are dependent on this
	//module, if the closeDependentModuleViews parameter is true.
	if(closeDependentModuleViews)
	{
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::CloseViewsForSystem()
{
	//Build a set of all view models to close
	std::set<IViewModel*> viewModelsToClose;
	for(ViewModels::iterator i = viewModels.begin(); i != viewModels.end(); ++i)
	{
		ViewInfo& viewInfo = *i->second;
		if(viewInfo.viewCurrentlyOpen && viewInfo.viewModel->IsViewOwnerSystem())
		{
			viewModelsToClose.insert(viewInfo.viewModel);
		}
	}

	//Close all selected view models
	for(std::set<IViewModel*>::const_iterator i = viewModelsToClose.begin(); i != viewModelsToClose.end(); ++i)
	{
		CloseViewModel(*i, true);
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::CloseAllViews()
{
	//Build a set of all view models to close
	//##FIX## This needs a lock, along with the above functions.
	std::set<IViewModel*> viewModelsToClose;
	for(ViewModels::iterator i = viewModels.begin(); i != viewModels.end(); ++i)
	{
		ViewInfo& viewInfo = *i->second;
		if(viewInfo.viewCurrentlyOpen)
		{
			viewModelsToClose.insert(viewInfo.viewModel);
		}
	}

	//Close all selected view models
	for(std::set<IViewModel*>::const_iterator i = viewModelsToClose.begin(); i != viewModelsToClose.end(); ++i)
	{
		CloseViewModel(*i, true);
	}
}

//----------------------------------------------------------------------------------------
//Child window selection functions
//----------------------------------------------------------------------------------------
void ExodusInterface::BuildActiveWindowList()
{
	//Scan all open windows, and build a list of all child windows belonging to our
	//program which are currently open. Note that the windows are listed based on
	//Z-Order, from top to bottom. We scan the list from back to front, so the topmost
	//window appears last in the XML tree.
	//##FIX## The comment above talks about the XML tree. This only applied when this was
	//done as part of our workspace saving process.
	//##TODO## Make this function more generic. We should return the window list from this
	//function, rather than writing directly to a specific list in this object.
	activeWindowList.clear();
	std::list<HWND> windowList;
	EnumThreadWindows(GetCurrentThreadId(), &ExodusInterface::EnumWindowsProc, (LPARAM)&windowList);
	for(std::list<HWND>::const_iterator windowIterator = windowList.begin(); windowIterator != windowList.end(); ++windowIterator)
	{
		HWND window = *windowIterator;
		for(ViewModels::const_iterator i = viewModels.begin(); i != viewModels.end(); ++i)
		{
			//If we've found a debug window belonging to our program, add it to the active
			//window list.
			IViewModel* viewModel = i->second->viewModel;
			if(i->second->viewCurrentlyOpen && viewModel->DoesWindowHandleMatchView(window))
			{
				activeWindowList.push_back(i->second->viewModel);
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//Menu functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::BuildMenuRecursive(HWND parentWindow, HMENU parentMenu, IMenuItem& amenuItem, unsigned int& nextMenuID, int& insertPos)
{
	bool result = true;

	if(amenuItem.GetType() == IMenuItem::TYPE_SEPARATOR)
	{
		IMenuSeparator& menuItem = *((IMenuSeparator*)(&amenuItem));
		MENUITEMINFO menuItemInfo;
		menuItemInfo.cbSize = sizeof(MENUITEMINFO);
		menuItemInfo.fMask = MIIM_FTYPE;
		menuItemInfo.fType = MF_SEPARATOR;
		BOOL insertMenuItemReturn;
		insertMenuItemReturn = InsertMenuItem(parentMenu, insertPos, TRUE, &menuItemInfo);
		if(insertMenuItemReturn == 0)
		{
			result = false;
		}
		else
		{
			if(insertPos >= 0)
			{
				++insertPos;
			}
		}
	}
	else if(amenuItem.GetType() == IMenuItem::TYPE_SUBMENU)
	{
		IMenuSubmenu& menuItem = *((IMenuSubmenu*)(&amenuItem));
		if(!menuItem.NoMenuItemsExist())
		{
			HMENU deviceMenu = CreatePopupMenu();
			if(deviceMenu == NULL)
			{
				result = false;
			}
			else
			{
				std::wstring menuItemName = menuItem.GetMenuName();
				MENUITEMINFO menuItemInfo;
				menuItemInfo.cbSize = sizeof(MENUITEMINFO);
				menuItemInfo.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU;
				menuItemInfo.fType = MF_STRING;
				menuItemInfo.hSubMenu = deviceMenu;
				menuItemInfo.dwTypeData = &menuItemName[0];
				BOOL insertMenuItemReturn;
				insertMenuItemReturn = InsertMenuItem(parentMenu, insertPos, TRUE, &menuItemInfo);
				if(insertMenuItemReturn == 0)
				{
					result = false;
				}
				else
				{
					if(insertPos >= 0)
					{
						++insertPos;
					}
					int childMenuItemInsertPos = -1;
					std::list<IMenuSegment*> menuSegments = menuItem.GetMenuSegments();
					for(std::list<IMenuSegment*>::iterator menuSegment = menuSegments.begin(); menuSegment != menuSegments.end(); ++menuSegment)
					{
						std::list<IMenuItem*> menuItems = (*menuSegment)->GetMenuItems();
						for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
						{
							result &= BuildMenuRecursive(parentWindow, deviceMenu, *(*i), nextMenuID, childMenuItemInsertPos);
						}
					}
				}
			}
		}
	}
	else if(amenuItem.GetType() == IMenuItem::TYPE_SELECTABLEOPTION)
	{
		MenuSelectableOption& menuItem = *((MenuSelectableOption*)(&amenuItem));
		std::wstring menuItemName = menuItem.GetName();
		MENUITEMINFO menuItemInfo;
		menuItemInfo.cbSize = sizeof(MENUITEMINFO);
		menuItemInfo.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING;
		menuItemInfo.fType = MF_STRING;
		menuItemInfo.fState = (menuItem.GetCheckedState()? MFS_CHECKED: 0);
		menuItemInfo.wID = nextMenuID;
		menuItemInfo.dwTypeData = &menuItemName[0];
		BOOL insertMenuItemReturn;
		insertMenuItemReturn = InsertMenuItem(parentMenu, insertPos, TRUE, &menuItemInfo);
		if(insertMenuItemReturn == 0)
		{
			result = false;
		}
		else
		{
			if(insertPos >= 0)
			{
				++insertPos;
			}

			//Associate nextMenuID with this menu item
			menuItem.SetPhysicalMenuHandle(parentMenu);
			menuItem.SetPhysicalMenuItemID(nextMenuID);
			NewMenuItem item(parentMenu, nextMenuID, menuItem);
			newMenuList.insert(NewMenuListEntry(nextMenuID, item));
			++nextMenuID;
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::BuildFileMenu()
{
	bool result = true;

	//Clear all existing dynamic items from the physical file menu
	int menuItemCount = GetMenuItemCount(fileMenu);
	int currentDyanmicMenuItemCount = menuItemCount - fileMenuNonDynamicMenuItemCount;
	for(int i = 0; i < currentDyanmicMenuItemCount; ++i)
	{
		DeleteMenu(fileMenu, 0, MF_BYPOSITION);
	}

	//Clear all items from our file submenu structure
	fileSubmenu->DeleteAllMenuSegments();

	//Add menu items to the submenu
	system.BuildFileOpenMenu(*fileSubmenu, *this);

	//If at least one menu item is defined, add a separator to the end of the menu item
	//list, to divide our dynamic menu items from the predefined menu items.
	if(!fileSubmenu->NoMenuItemsExist())
	{
		fileSubmenu->CreateMenuSegment().AddMenuItemSeparator();
	}

	//Build the actual menu using the menu structure
	int insertPos = 0;
	std::list<IMenuSegment*> menuSegments = fileSubmenu->GetMenuSegments();
	for(std::list<IMenuSegment*>::iterator menuSegment = menuSegments.begin(); menuSegment != menuSegments.end(); ++menuSegment)
	{
		std::list<IMenuItem*> menuItems = (*menuSegment)->GetMenuItems();
		for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
		{
			result &= BuildMenuRecursive(mainWindowHandle, fileMenu, *(*i), nextFreeMenuID, insertPos);
		}
	}

	DrawMenuBar(mainWindowHandle);
	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::BuildSystemMenu()
{
	bool result = true;

	//Clear all existing dynamic items from the physical system menu
	int menuItemCount = GetMenuItemCount(systemMenu);
	for(int i = systemMenuFirstItemIndex; i < menuItemCount; ++i)
	{
		int menuItemIndex = (menuItemCount - 1) - (i - systemMenuFirstItemIndex);
		DeleteMenu(systemMenu, menuItemIndex, MF_BYPOSITION);
	}

	//Clear all items from our submenu structure
	systemSubmenu->DeleteAllMenuSegments();

	//Add menu items to the submenu
	system.BuildSystemMenu(*systemSubmenu, *this);

	//Build the actual menu using the menu structure
	int insertPos = -1;
	std::list<IMenuSegment*> menuSegments = systemSubmenu->GetMenuSegments();
	for(std::list<IMenuSegment*>::iterator menuSegment = menuSegments.begin(); menuSegment != menuSegments.end(); ++menuSegment)
	{
		std::list<IMenuItem*> menuItems = (*menuSegment)->GetMenuItems();
		for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
		{
			result &= BuildMenuRecursive(mainWindowHandle, systemMenu, *(*i), nextFreeMenuID, insertPos);
		}
	}

	DrawMenuBar(mainWindowHandle);
	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::BuildSettingsMenu()
{
	bool result = true;

	//Clear all existing dynamic items from the physical settings menu
	int menuItemCount = GetMenuItemCount(settingsMenu);
	for(int i = settingsMenuFirstItemIndex; i < menuItemCount; ++i)
	{
		int menuItemIndex = (menuItemCount - 1) - (i - settingsMenuFirstItemIndex);
		DeleteMenu(settingsMenu, menuItemIndex, MF_BYPOSITION);
	}

	//Clear all items from our submenu structure
	settingsSubmenu->DeleteAllMenuSegments();

	//Add menu items to the submenu
	system.BuildSettingsMenu(*settingsSubmenu, *this);

	//Build the actual menu using the menu structure
	int insertPos = -1;
	std::list<IMenuSegment*> menuSegments = settingsSubmenu->GetMenuSegments();
	for(std::list<IMenuSegment*>::iterator menuSegment = menuSegments.begin(); menuSegment != menuSegments.end(); ++menuSegment)
	{
		std::list<IMenuItem*> menuItems = (*menuSegment)->GetMenuItems();
		for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
		{
			result &= BuildMenuRecursive(mainWindowHandle, settingsMenu, *(*i), nextFreeMenuID, insertPos);
		}
	}

	DrawMenuBar(mainWindowHandle);
	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::BuildDebugMenu()
{
	bool result = true;

	//Clear all existing dynamic items from the physical debug menu
	int menuItemCount = GetMenuItemCount(debugMenu);
	for(int i = debugMenuFirstItemIndex; i < menuItemCount; ++i)
	{
		int menuItemIndex = (menuItemCount - 1) - (i - debugMenuFirstItemIndex);
		DeleteMenu(debugMenu, menuItemIndex, MF_BYPOSITION);
	}

	//Clear all items from our submenu structure
	debugSubmenu->DeleteAllMenuSegments();

	//Add menu items to the submenu
	system.BuildDebugMenu(*debugSubmenu, *this);

	//Build the actual menu using the menu structure
	int insertPos = -1;
	std::list<IMenuSegment*> menuSegments = debugSubmenu->GetMenuSegments();
	for(std::list<IMenuSegment*>::iterator menuSegment = menuSegments.begin(); menuSegment != menuSegments.end(); ++menuSegment)
	{
		std::list<IMenuItem*> menuItems = (*menuSegment)->GetMenuItems();
		for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
		{
			result &= BuildMenuRecursive(mainWindowHandle, debugMenu, *(*i), nextFreeMenuID, insertPos);
		}
	}

	DrawMenuBar(mainWindowHandle);
	return result;
}

//----------------------------------------------------------------------------------------
//Thread handlers
//----------------------------------------------------------------------------------------
void ExodusInterface::UnloadModuleThread(unsigned int moduleID)
{
	//Unload the specified module
	system.UnloadModule(moduleID);

	moduleCommandComplete = true;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::UnloadSystemThread()
{
	if(systemLoaded)
	{
		//Clear all previously loaded system info
		system.UnloadAllModules();
	}

	systemLoaded = false;
	moduleCommandComplete = true;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::DestroySystemInterfaceThread()
{
	//Stop the system if it is currently running
	system.StopSystem();

	//Explicitly close all currently open views
	CloseAllViews();

	//Unload the system, to ensure all device menus are cleaned up.
	UnloadAllModules();

	//Clean up the system menus
	system.DeleteSystemViews();

	//Save settings.xml
	SetCurrentDirectory(&originalWorkingDir[0]);
	SavePrefs(L"settings.xml");

	//Send a message to the main window, requesting it to close.
	SendMessage(mainWindowHandle, WM_USER+2, 0, 0);
}

//----------------------------------------------------------------------------------------
//Window auto-position functions
//----------------------------------------------------------------------------------------
void ExodusInterface::GetNewWindowPosition(int newWindowWidth, int newWindowHeight, int& newWindowPosX, int& newWindowPosY)
{
	//Get the position of the client area of our main window in screen coordinates
	Region2D screenRegion;
	screenRegion.posx = 0;
	screenRegion.posy = 0;
	POINT clientPos;
	clientPos.x = 0;
	clientPos.y = 0;
	if(ClientToScreen(mainWindowHandle, &clientPos) != 0)
	{
		screenRegion.posx = clientPos.x;
		screenRegion.posy = clientPos.y;
	}

	//Calculate the region within which we will allow child windows to be auto-positioned
	screenRegion.sizex = 800;
	screenRegion.sizey = 600;
	RECT rect;
	if(GetClientRect(mainWindowHandle, &rect) != 0)
	{
		//Get the width and height of the client area of our main window
		screenRegion.sizex = rect.right;
		screenRegion.sizey = rect.bottom;

		//In the case where the height of the client area is very small, we consider the
		//client region of the window to be collapsed. In this case, make a region defined
		//by the screen height instead, taking into account the window position and width.
		if(screenRegion.sizey < 50)
		{
			//Obtain the position of the client region of our window in screen coordinates
			POINT mainWindowClientPosInScreenCoordinates;
			mainWindowClientPosInScreenCoordinates.x = 0;
			mainWindowClientPosInScreenCoordinates.y = 0;
			if(ClientToScreen(mainWindowHandle, &mainWindowClientPosInScreenCoordinates) != 0)
			{
				//Obtain information on the dimensions of the screen
				HMONITOR monitor = MonitorFromWindow(mainWindowHandle, MONITOR_DEFAULTTONEAREST);
				MONITORINFO monitorInfo;
				monitorInfo.cbSize = sizeof(monitorInfo);
				if(GetMonitorInfo(monitor, &monitorInfo) != 0)
				{
					//Extend the screen region down to the bottom of the display
					int monitorEndPosY = monitorInfo.rcWork.bottom;
					if(monitorEndPosY > mainWindowClientPosInScreenCoordinates.y)
					{
						screenRegion.sizey = monitorEndPosY - mainWindowClientPosInScreenCoordinates.y;
					}
				}
			}
		}
	}

	//Combine the existing child window positions
	std::list<Region2D> regionList;
	regionList.push_back(screenRegion);
	for(ViewModels::const_iterator i = viewModels.begin(); i != viewModels.end(); ++i)
	{
		//If this view is being ignored during the layout process, skip it.
		ViewInfo* viewInfo = i->second;
		if(viewInfo->ignoreForLayoutPurposes)
		{
			continue;
		}

		//Calculate the region of this view
		int xpos;
		int ypos;
		int width;
		int height;
		viewInfo->viewModel->GetViewPosition(xpos, ypos);
		viewInfo->viewModel->GetViewSize(width, height);
		Region2D region;
		region.posx = xpos;
		region.posy = ypos;
		region.sizex = width;
		region.sizey = height;

		//Intersect the window region with the free space region list
		std::list<Region2D>::iterator targetRegion = regionList.begin();
		while(targetRegion != regionList.end())
		{
			std::list<Region2D> newRegions;
			if(IntersectRegion(*targetRegion, region, newRegions))
			{
				regionList.splice(regionList.end(), newRegions);
				std::list<Region2D>::iterator targetRegionCopy = targetRegion;
				++targetRegion;
				regionList.erase(targetRegionCopy);
			}
			else
			{
				++targetRegion;
			}
		}
	}

	//Evaluate the best auto-generated region, if one exists.
	bool foundRegion = false;
	Region2D bestRegion;
	long bestRegionDistance;
	for(std::list<Region2D>::const_iterator i = regionList.begin(); i != regionList.end(); ++i)
	{
		if((i->sizex >= newWindowWidth) && (i->sizey >= newWindowHeight))
		{
			long regionDistance = (i->posx - screenRegion.posx) + (i->posy - screenRegion.posy);
			if(!foundRegion || (regionDistance < bestRegionDistance))
			{
				foundRegion = true;
				bestRegion = *i;
				bestRegionDistance = regionDistance;
			}
		}
	}

	//In the case that no free region can be found which is large enough, do a second pass
	//to find the next best option based on useful area.
	if(!foundRegion)
	{
		long bestUsefulRegionArea;
		for(std::list<Region2D>::const_iterator i = regionList.begin(); i != regionList.end(); ++i)
		{
			Region2D usefulRegion = *i;
			usefulRegion.sizex = (usefulRegion.sizex > newWindowWidth)? newWindowWidth: usefulRegion.sizex;
			usefulRegion.sizey = (usefulRegion.sizey > newWindowHeight)? newWindowHeight: usefulRegion.sizey;
			long usefulRegionArea = usefulRegion.sizex * usefulRegion.sizey;
			if(!foundRegion || (usefulRegionArea > bestUsefulRegionArea))
			{
				bestUsefulRegionArea = usefulRegionArea;
				bestRegion = usefulRegion;
				foundRegion = true;
			}
		}
	}

	//Determine the screen position to place the new window
	if(foundRegion)
	{
		//Output the best auto-generated location
		newWindowPosX = (int)bestRegion.posx;
		newWindowPosY = (int)bestRegion.posy;
	}
	else
	{
		//Fall back to placing the new window in an arbitrary position
		newWindowPosX = (int)screenRegion.posx + (int)childWindowXPosDefault;
		newWindowPosY = (int)screenRegion.posy + (int)childWindowYPosDefault;
		childWindowXPosDefault = (childWindowXPosDefault + childWindowXPosIncrement) % (childWindowXPosIncrement * 10);
		childWindowYPosDefault = (childWindowYPosDefault + childWindowYPosIncrement) % (childWindowYPosIncrement * 10);
	}
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::IntersectRegion(const Region2D& existingRegion, const Region2D& regionToIntersect, std::list<Region2D>& newRegionsToCreate) const
{
	bool result = false;

	//Check if the two regions overlap
	bool overlapsHorizontally = 
		(regionToIntersect.sizex > 0) && (existingRegion.sizex > 0)
	 && (((regionToIntersect.posx >= existingRegion.posx) && (regionToIntersect.posx < (existingRegion.posx + existingRegion.sizex))) //New region starts within old region
	  || (((regionToIntersect.posx + regionToIntersect.sizex) > existingRegion.posx) && ((regionToIntersect.posx + regionToIntersect.sizex) < (existingRegion.posx + existingRegion.sizex))) //New region ends within old region
	  || ((regionToIntersect.posx <= existingRegion.posx) && ((regionToIntersect.posx + regionToIntersect.sizex) >= (existingRegion.posx + existingRegion.sizex)))); //New region completely swallows old region
	bool overlapsVertically = 
		(regionToIntersect.sizey > 0) && (existingRegion.sizey > 0)
	 && (((regionToIntersect.posy >= existingRegion.posy) && (regionToIntersect.posy < (existingRegion.posy + existingRegion.sizey))) //New region starts within old region
	  || (((regionToIntersect.posy + regionToIntersect.sizey) > existingRegion.posy) && ((regionToIntersect.posy + regionToIntersect.sizey) < (existingRegion.posy + existingRegion.sizey))) //New region ends within old region
	  || ((regionToIntersect.posy <= existingRegion.posy) && ((regionToIntersect.posy + regionToIntersect.sizey) >= (existingRegion.posy + existingRegion.sizey)))); //New region completely swallows old region

	//If the two regions overlap, calculate the resultant regions
	if(overlapsVertically && overlapsHorizontally)
	{
		result = true;

		//We start by checking which corners of our existing region are contained within
		//the intersecting region. This will give us a basis for determining how to divide
		//the existing region.
		bool topLeftCornerOverlapped = PointWithinRegion(existingRegion.posx, existingRegion.posy, regionToIntersect);
		bool topRightCornerOverlapped = PointWithinRegion(existingRegion.posx + existingRegion.sizex, existingRegion.posy, regionToIntersect);
		bool bottomLeftCornerOverlapped = PointWithinRegion(existingRegion.posx, existingRegion.posy + existingRegion.sizey, regionToIntersect);
		bool bottomRightCornerOverlapped = PointWithinRegion(existingRegion.posx + existingRegion.sizex, existingRegion.posy + existingRegion.sizey, regionToIntersect);
		int cornerIntersectionCount = (topLeftCornerOverlapped? 1: 0) + (topRightCornerOverlapped? 1: 0) + (bottomLeftCornerOverlapped? 1: 0) + (bottomRightCornerOverlapped? 1: 0);;

		//Check if none of the corners of our existing region intersect
		if(cornerIntersectionCount == 0)
		{
			//If none of the corners intersect, we have three possibilities. Since we know
			//the two regions do overlap, since we did an initial test for that above, the
			//intersecting region must either be entirely contained within our region, it
			//must be cutting into one side of our region, or it must be slicing our
			//region straight across into two pieces. To determine which is happening,
			//we're going to look for intersections against the four edges. The number of
			//edge intersections will tell us which case we've encountered.
			bool leftEdgeIntersected = RegionIntersectsVerticalLine(existingRegion.posx, existingRegion.posy, existingRegion.sizey, regionToIntersect);
			bool rightEdgeIntersected = RegionIntersectsVerticalLine(existingRegion.posx + existingRegion.sizex, existingRegion.posy, existingRegion.sizey, regionToIntersect);
			bool topEdgeIntersected = RegionIntersectsHorizontalLine(existingRegion.posx, existingRegion.posy, existingRegion.sizex, regionToIntersect);
			bool bottomEdgeIntersected = RegionIntersectsHorizontalLine(existingRegion.posx, existingRegion.posy + existingRegion.sizey, existingRegion.sizex, regionToIntersect);
			int edgeIntersectionCount = (leftEdgeIntersected? 1: 0) + (rightEdgeIntersected? 1: 0) + (topEdgeIntersected? 1: 0) + (bottomEdgeIntersected? 1: 0);;
			if(edgeIntersectionCount == 0)
			{
				//The intersected region punches a hole in our region. 4 regions created.
				//==========  1. ===        2. ========== 3.            4.        ===
				//=        =     = =           ==========                         = =
				//=  ++++  =     = =                                              = =
				//=  +  +  =     = =                                              = =
				//=  ++++  =     = =                                              = =
				//=        =     = =                         ==========           = =
				//==========     ===                         ==========           ===
					Region2D region1(existingRegion.posx,
									 existingRegion.posy,
									 regionToIntersect.posx - existingRegion.posx,
									 existingRegion.sizey);
					Region2D region2(existingRegion.posx,
									 existingRegion.posy,
									 existingRegion.sizex,
									 regionToIntersect.posy - existingRegion.posy);
					Region2D region3(existingRegion.posx,
									 regionToIntersect.posy + regionToIntersect.sizey,
									 existingRegion.sizex,
									 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
					Region2D region4(regionToIntersect.posx + regionToIntersect.sizex,
									 existingRegion.posy,
									 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
									 existingRegion.sizey);
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
					newRegionsToCreate.push_back(region3);
					newRegionsToCreate.push_back(region4);
			}
			else if(edgeIntersectionCount == 1)
			{
				//The intersected region hits a side. 3 regions created.
				//==========  1. =======    2. ========== 3.           
				//=        =     =     =       ==========              
				//=      +++++   =     =                               
				//=      + = +   =     =                               
				//=      +++++   =     =                               
				//=        =     =     =                     ==========
				//==========     =======                     ==========
				if(leftEdgeIntersected)
				{
					Region2D region1(regionToIntersect.posx + regionToIntersect.sizex,
									 existingRegion.posy,
									 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
									 existingRegion.sizey);
					Region2D region2(existingRegion.posx,
									 existingRegion.posy,
									 existingRegion.sizex,
									 regionToIntersect.posy - existingRegion.posy);
					Region2D region3(existingRegion.posx,
									 regionToIntersect.posy + regionToIntersect.sizey,
									 existingRegion.sizex,
									 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
					newRegionsToCreate.push_back(region3);
				}
				else if(rightEdgeIntersected)
				{
					Region2D region1(existingRegion.posx,
									 existingRegion.posy,
									 regionToIntersect.posx - existingRegion.posx,
									 existingRegion.sizey);
					Region2D region2(existingRegion.posx,
									 existingRegion.posy,
									 existingRegion.sizex,
									 regionToIntersect.posy - existingRegion.posy);
					Region2D region3(existingRegion.posx,
									 regionToIntersect.posy + regionToIntersect.sizey,
									 existingRegion.sizex,
									 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
					newRegionsToCreate.push_back(region3);
				}
				else if(topEdgeIntersected)
				{
					Region2D region1(existingRegion.posx,
									 existingRegion.posy,
									 regionToIntersect.posx - existingRegion.posx,
									 existingRegion.sizey);
					Region2D region2(existingRegion.posx,
									 regionToIntersect.posy + regionToIntersect.sizey,
									 existingRegion.sizex,
									 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
					Region2D region3(regionToIntersect.posx + regionToIntersect.sizex,
									 existingRegion.posy,
									 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
									 existingRegion.sizey);
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
					newRegionsToCreate.push_back(region3);
				}
				else //"bottomEdgeIntersected" should be the only other case
				{
					Region2D region1(existingRegion.posx,
									 existingRegion.posy,
									 regionToIntersect.posx - existingRegion.posx,
									 existingRegion.sizey);
					Region2D region2(existingRegion.posx,
									 existingRegion.posy,
									 existingRegion.sizex,
									 regionToIntersect.posy - existingRegion.posy);
					Region2D region3(regionToIntersect.posx + regionToIntersect.sizex,
									 existingRegion.posy,
									 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
									 existingRegion.sizey);
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
					newRegionsToCreate.push_back(region3);
				}
			}
			else //"edgeIntersectionCount == 2" should be the only other case we encounter
			{
				//The intersected region slices our region. 2 regions created.
				//  ==========  1. ========== 2.           
				//  =        =     ==========              
				//++++++++++++++                      
				//+ =        = +   
				//++++++++++++++                                  
				//  =        =                   ==========
				//  ==========                   ==========
				if(leftEdgeIntersected && rightEdgeIntersected)
				{
					Region2D region1(existingRegion.posx,
									 existingRegion.posy,
									 existingRegion.sizex,
									 regionToIntersect.posy - existingRegion.posy);
					Region2D region2(existingRegion.posx,
									 regionToIntersect.posy + regionToIntersect.sizey,
									 existingRegion.sizex,
									 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
				}
				else //"topEdgeIntersected && bottomEdgeIntersected" should be the only other case
				{
					Region2D region1(existingRegion.posx,
									 existingRegion.posy,
									 regionToIntersect.posx - existingRegion.posx,
									 existingRegion.sizey);
					Region2D region2(regionToIntersect.posx + regionToIntersect.sizex,
									 existingRegion.posy,
									 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
									 existingRegion.sizey);
					newRegionsToCreate.push_back(region1);
					newRegionsToCreate.push_back(region2);
				}
			}
		}
		else if(cornerIntersectionCount == 1)
		{
			//The intersected region hits a corner. 2 regions created.
			//==========   1. ======     2. ==========
			//=        =      =    =        =        =
			//=        =      =    =        =        =
			//=        =      =    =        ==========
			//=     ++++++    =    =
			//=     +  = +    =    =
			//======+=== +    ======
			//      ++++++
			if(topLeftCornerOverlapped)
			{
				Region2D region1(regionToIntersect.posx + regionToIntersect.sizex,
				                 existingRegion.posy,
								 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
								 existingRegion.sizey);
				Region2D region2(existingRegion.posx,
				                 regionToIntersect.posy + regionToIntersect.sizey,
								 existingRegion.sizex,
								 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
				newRegionsToCreate.push_back(region1);
				newRegionsToCreate.push_back(region2);
			}
			else if(topRightCornerOverlapped)
			{
				Region2D region1(existingRegion.posx,
				                 existingRegion.posy,
								 regionToIntersect.posx - existingRegion.posx,
								 existingRegion.sizey);
				Region2D region2(existingRegion.posx,
				                 regionToIntersect.posy + regionToIntersect.sizey,
								 existingRegion.sizex,
								 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
				newRegionsToCreate.push_back(region1);
				newRegionsToCreate.push_back(region2);
			}
			else if(bottomLeftCornerOverlapped)
			{
				Region2D region1(regionToIntersect.posx + regionToIntersect.sizex,
				                 existingRegion.posy,
								 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
								 existingRegion.sizey);
				Region2D region2(existingRegion.posx,
				                 existingRegion.posy,
								 existingRegion.sizex,
								 (regionToIntersect.posy - existingRegion.posy));
				newRegionsToCreate.push_back(region1);
				newRegionsToCreate.push_back(region2);
			}
			else //"bottomRightCornerOverlapped" should be the only other case
			{
				Region2D region1(existingRegion.posx,
				                 existingRegion.posy,
								 regionToIntersect.posx - existingRegion.posx,
								 existingRegion.sizey);
				Region2D region2(existingRegion.posx,
				                 existingRegion.posy,
								 existingRegion.sizex,
								 regionToIntersect.posy - existingRegion.posy);
				newRegionsToCreate.push_back(region1);
				newRegionsToCreate.push_back(region2);
			}
		}
		else if(cornerIntersectionCount == 2)
		{
			//The intersected region restricts our region. 1 region created.
			//++++++++++++++ 1.
			//+            +
			//+ ========== +
			//+ =        = +
			//++++++++++++++                      
			//  =        =      ==========
			//  =        =      =        =
			//  =        =      =        =
			//  ==========      ==========
			if(topLeftCornerOverlapped && topRightCornerOverlapped)
			{
				Region2D region1(existingRegion.posx,
				                 regionToIntersect.posy + regionToIntersect.sizey,
								 existingRegion.sizex,
								 (existingRegion.posy + existingRegion.sizey) - (regionToIntersect.posy + regionToIntersect.sizey));
				newRegionsToCreate.push_back(region1);
			}
			else if(bottomLeftCornerOverlapped && bottomRightCornerOverlapped)
			{
				Region2D region1(existingRegion.posx,
				                 existingRegion.posy,
								 existingRegion.sizex,
								 regionToIntersect.posy - existingRegion.posy);
				newRegionsToCreate.push_back(region1);
			}
			else if(topLeftCornerOverlapped && bottomLeftCornerOverlapped)
			{
				Region2D region1(regionToIntersect.posx + regionToIntersect.sizex,
				                 existingRegion.posy,
								 (existingRegion.posx + existingRegion.sizex) - (regionToIntersect.posx + regionToIntersect.sizex),
								 existingRegion.sizey);
				newRegionsToCreate.push_back(region1);
			}
			else //"topRightCornerOverlapped && bottomRightCornerOverlapped" should be the only other case
			{
				Region2D region1(existingRegion.posx,
				                 existingRegion.posy,
								 regionToIntersect.posx - existingRegion.posx,
								 existingRegion.sizey);
				newRegionsToCreate.push_back(region1);
			}
		}
		else //"cornerIntersectionCount == 4" should be the only other case we encounter
		{
			//The intersected region completely swallows our region. No regions created.
			//++++++++++
			//+        +
			//+  ====  +
			//+  =  =  +
			//+  ====  +
			//+        +
			//++++++++++
			//Since no regions are created, we have nothing to do in this case.
		}
	}

	return result;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::PointWithinRegion(int posx, int posy, const Region2D& region) const
{
	return (posx >= region.posx) && (posx < (region.posx + region.sizex)) //Point is within region horizontally
	    && (posy >= region.posy) && (posy < (region.posy + region.sizey)); //Point is within region vertically
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::RegionIntersectsHorizontalLine(int posx, int posy, int width, const Region2D& region) const
{
	return (posy >= region.posy) && (posy < (region.posy + region.sizey)) //The line is overlapping with or adjacent to the region vertically
	   && ((posx <= region.posx) && ((posx + width) > region.posx) //The line begins at or before the region, and intersects it as it moves right
	    || (posx >= region.posx) && (posx < (region.posx + region.sizex))); //The line begins within the region
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::RegionIntersectsVerticalLine(int posx, int posy, int height, const Region2D& region) const
{
	return (posx >= region.posx) && (posx < (region.posx + region.sizex)) //The line is overlapping with or adjacent to the region horizontally
	   && ((posy <= region.posy) && ((posy + height) > region.posy) //The line begins at or before the region, and intersects it as it moves down
	    || (posy >= region.posy) && (posy < (region.posy + region.sizey))); //The line begins within the region
}

//----------------------------------------------------------------------------------------
//Window callbacks
//----------------------------------------------------------------------------------------
BOOL CALLBACK ExodusInterface::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	std::list<HWND>& windowList = *((std::list<HWND>*)lParam);
	windowList.push_back(hwnd);
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ExodusInterface::MapConnectorProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	MapConnectorDialogParams* state = (MapConnectorDialogParams*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(Message)
	{
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	case WM_COMMAND:{
		bool validateSelection = false;
		if(HIWORD(wParam) == LBN_DBLCLK)
		{
			switch(LOWORD(wParam))
			{
			case IDC_MAPCONNECTOR_LIST:
				validateSelection = true;
				break;
			}
		}
		else if(HIWORD(wParam) == BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDOK:{
				validateSelection = true;
				break;}
			case IDCANCEL:
				EndDialog(hwnd, 0);
				break;
			}
		}
		if(validateSelection)
		{
			int currentItemListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_MAPCONNECTOR_LIST), LB_GETCURSEL, 0, 0);
			ConnectorInfo* targetItemConnectorInfo = (ConnectorInfo*)SendMessage(GetDlgItem(hwnd, IDC_MAPCONNECTOR_LIST), LB_GETITEMDATA, currentItemListIndex, NULL);
			if(targetItemConnectorInfo != 0)
			{
				state->selectedConnector = *targetItemConnectorInfo;
				state->selectionMade = true;
				EndDialog(hwnd, 1);
			}
		}
		break;}
	case WM_INITDIALOG:{
		//Set the object pointer
		state = (MapConnectorDialogParams*)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		for(std::list<ConnectorInfo>::iterator i = state->connectorList.begin(); i != state->connectorList.end(); ++i)
		{
			LoadedModuleInfo moduleInfo;
			if(state->system->GetLoadedModuleInfo(i->GetExportingModuleID(), moduleInfo))
			{
				std::wstringstream text;
				text << moduleInfo.GetInstanceName() << L"." << i->GetExportingModuleConnectorInstanceName();
				LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_MAPCONNECTOR_LIST), LB_ADDSTRING, 0, (LPARAM)text.str().c_str());
				SendMessage(GetDlgItem(hwnd, IDC_MAPCONNECTOR_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)&(*i));
			}
		}

		break;}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ExodusInterface::LoadPluginProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ExodusInterface* state = (ExodusInterface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(Message)
	{
	case WM_TIMER:{
		if(state->loadPluginsComplete)
		{
			INT_PTR result = 0;
			if(state->loadPluginsResult)
			{
				result = 1;
			}
			else if(state->loadPluginsAborted)
			{
				result = -1;
			}
			KillTimer(hwnd, 1);
			EndDialog(hwnd, result);
		}
		else
		{
			boost::mutex::scoped_lock lock(state->loadPluginsMutex);
			std::wstring currentPluginName = state->loadPluginsCurrentPluginName;
			lock.unlock();
			if(!currentPluginName.empty())
			{
				UpdateDlgItemString(hwnd, IDC_LOADPLUGIN_PLUGINTEXT, PathGetFileName(currentPluginName));
			}
			float progress = state->loadPluginsProgress;
			int progressInt = (int)((progress * 100) + 0.5);
			SendMessage(GetDlgItem(hwnd, IDC_LOADPLUGIN_PROGRESS), PBM_SETPOS, (WPARAM)progressInt, 0);
		}
		break;}
	case WM_CLOSE:
		state->loadPluginsAborted = true;
		break;
	case WM_COMMAND:
		if(LOWORD(wParam) == IDC_LOADPLUGIN_CANCEL)
		{
			state->loadPluginsAborted = true;
		}
		break;
	case WM_INITDIALOG:{
		//Set the object pointer
		state = (ExodusInterface*)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		SetTimer(hwnd, 1, 100, NULL);
		break;}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ExodusInterface::LoadModuleProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ExodusInterface* state = (ExodusInterface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(Message)
	{
	case WM_TIMER:{
		if(state->system.LoadModuleSynchronousComplete())
		{
			INT_PTR result = 0;
			if(state->system.LoadModuleSynchronousResult())
			{
				result = 1;
			}
			else if(state->system.LoadModuleSynchronousAborted())
			{
				result = -1;
			}
			KillTimer(hwnd, 1);
			EndDialog(hwnd, result);
		}
		else
		{
			std::wstring currentModuleName = state->system.LoadModuleSynchronousCurrentModuleName();
			if(!currentModuleName.empty())
			{
				UpdateDlgItemString(hwnd, IDC_LOADMODULE_MODULETEXT, currentModuleName);
			}
			float progress = state->system.LoadModuleSynchronousProgress();
			int progressInt = (int)((progress * 100) + 0.5);
			SendMessage(GetDlgItem(hwnd, IDC_LOADMODULE_PROGRESS), PBM_SETPOS, (WPARAM)progressInt, 0);
		}
		break;}
	case WM_CLOSE:
		state->system.LoadModuleSynchronousAbort();
		break;
	case WM_COMMAND:
		if(LOWORD(wParam) == IDC_LOADMODULE_CANCEL)
		{
			state->system.LoadModuleSynchronousAbort();
		}
		break;
	case WM_INITDIALOG:{
		//Set the object pointer
		state = (ExodusInterface*)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		SetTimer(hwnd, 1, 100, NULL);
		break;}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ExodusInterface::UnloadModuleProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ExodusInterface* state = (ExodusInterface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(Message)
	{
	case WM_TIMER:{
		if(state->moduleCommandComplete)
		{
			KillTimer(hwnd, 1);
			EndDialog(hwnd, 0);
		}
		else
		{
			std::wstring currentModuleName = state->system.UnloadModuleSynchronousCurrentModuleName();
			if(!currentModuleName.empty())
			{
				UpdateDlgItemString(hwnd, IDC_UNLOADMODULE_MODULETEXT, currentModuleName);
			}
		}
		break;}
	case WM_CLOSE:
		break;
	case WM_INITDIALOG:{
		//Set the object pointer
		state = (ExodusInterface*)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		SetTimer(hwnd, 1, 100, NULL);

		HWND progressWindowHandle = GetDlgItem(hwnd, IDC_UNLOADMODULE_PROGRESS);
		if(progressWindowHandle != NULL)
		{
			LONG_PTR windowStyles = GetWindowLongPtr(progressWindowHandle, GWL_STYLE);
			windowStyles |= PBS_MARQUEE;
			SetWindowLongPtr(progressWindowHandle, GWL_STYLE, windowStyles);
			SendMessage(progressWindowHandle, PBM_SETMARQUEE, (WPARAM)TRUE, (LPARAM)50);
		}
		break;}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ExodusInterface::ModuleManagerProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ExodusInterface* state = (ExodusInterface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(Message)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		state->moduleManagerDialog = NULL;
		break;
	case WM_COMMAND:{
		if(HIWORD(wParam) == BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_MODULEMANAGER_LOAD:{
				state->LoadModule(state->prefs.pathModules);
				break;}
			case IDC_MODULEMANAGER_UNLOADSELECTED:{
				//Retrieve the selected list item
				LRESULT selectedListItem = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_GETCURSEL, 0, 0);
				if(selectedListItem != LB_ERR)
				{
					//Retrieve the module ID number of the selected module
					LRESULT listItemData = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_GETITEMDATA, selectedListItem, NULL);
					if(listItemData != LB_ERR)
					{
						//Unload the selected module
						unsigned int targetModuleID = (unsigned int)listItemData;
						state->UnloadModule(targetModuleID);

						//Refresh the module list
						SendMessage(hwnd, WM_USER, 0, 0);
					}
				}
				break;}
			case IDC_MODULEMANAGER_UNLOADALL:
				//Clear all loaded modules
				state->UnloadAllModules();

				//Refresh the module list
				SendMessage(hwnd, WM_USER, 0, 0);
				break;
			}
		}
		break;}
	case WM_INITDIALOG:
		//Set the object pointer
		state = (ExodusInterface*)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Refresh the module list
		SendMessage(hwnd, WM_USER, 0, 0);
		break;
	case WM_USER:{
		SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), WM_SETREDRAW, FALSE, 0);

		LRESULT top = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_GETTOPINDEX, 0, 0);
		LRESULT selected = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_GETCURSEL, 0, 0);
		SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_RESETCONTENT, 0, NULL);
		std::list<unsigned int> loadedModuleIDs = state->system.GetLoadedModuleIDs();
		for(std::list<unsigned int>::const_iterator i = loadedModuleIDs.begin(); i != loadedModuleIDs.end(); ++i)
		{
			LoadedModuleInfo moduleInfo;
			if(state->system.GetLoadedModuleInfo(*i, moduleInfo))
			{
				std::wstringstream text;
				text << moduleInfo.GetDisplayName() << " (" << moduleInfo.GetClassName() << ")";
				LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_ADDSTRING, 0, (LPARAM)text.str().c_str());
				SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)moduleInfo.GetModuleID());
			}
		}
		SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_SETCURSEL, selected, 0);
		SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), LB_SETTOPINDEX, top, 0);

		SendMessage(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), WM_SETREDRAW, TRUE, 0);
		InvalidateRect(GetDlgItem(hwnd, IDC_MODULEMANAGER_MODULES_LIST), NULL, FALSE);
		break;}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ExodusInterface::AboutProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ExodusInterface* state = (ExodusInterface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	const unsigned int deviceListControlID = 50000;
	const unsigned int extensionListControlID = 50001;
	switch(Message)
	{
	case WM_PARENTNOTIFY:
		switch(LOWORD(wParam))
		{
		case WM_LBUTTONDOWN:{
			//If the user has clicked on a child window within our window region, ensure
			//that the child window gets focus.
			POINT mousePos;
			mousePos.x = LOWORD(lParam);
			mousePos.y = HIWORD(lParam);
			HWND targetWindow = ChildWindowFromPoint(hwnd, mousePos);
			if(targetWindow != NULL)
			{
				SetFocus(targetWindow);
			}
			break;}
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		//Delete the default font object
		SendMessage(hwnd, WM_SETFONT, (WPARAM)NULL, (LPARAM)FALSE);
		DeleteObject(state->aboutDialogHFont);
		return FALSE;
	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK)
		{
			DestroyWindow(hwnd);
		}
		break;
	case WM_INITDIALOG:{
		//Set the object pointer
		state = (ExodusInterface*)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Retrieve the required information from the version info table for our program
		std::wstring mainExecutablePath = GetModuleFilePath(GetModuleHandle(NULL));
		std::wstring versionText;
		std::wstring copyrightText;
		GetModuleVersionInfoString(mainExecutablePath, VERSIONINFOPROPERTY_PRODUCTVERSION, versionText);
		GetModuleVersionInfoString(mainExecutablePath, VERSIONINFOPROPERTY_LEGALCOPYRIGHT, copyrightText);

		//Load the program version info into the about dialog
		SetDlgItemText(hwnd, IDC_ABOUT_VERSION, versionText.c_str());
		SetDlgItemText(hwnd, IDC_ABOUT_COPYRIGHT, copyrightText.c_str());

		//Create the default font for this window
		int fontPointSize = 8;
		HDC hdc = GetDC(hwnd);
		int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		ReleaseDC(hwnd, hdc);
		std::wstring fontTypefaceName = L"MS Shell Dlg";
		state->aboutDialogHFont = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &fontTypefaceName[0]);

		//Calculate the marked target position for the device list inside the dialog
		RECT deviceListMarkerRect;
		GetWindowRect(GetDlgItem(hwnd, IDC_ABOUT_DEVICELIST_PLACEHOLDER), &deviceListMarkerRect);
		POINT deviceListMarkerPos;
		deviceListMarkerPos.x = deviceListMarkerRect.left;
		deviceListMarkerPos.y = deviceListMarkerRect.top;
		ScreenToClient(hwnd, &deviceListMarkerPos);
		int deviceListPosX = deviceListMarkerPos.x;
		int deviceListPosY = deviceListMarkerPos.y;
		int deviceListWidth = (int)(deviceListMarkerRect.right - deviceListMarkerRect.left);
		int deviceListHeight = (int)(deviceListMarkerRect.bottom - deviceListMarkerRect.top);

		//Calculate the marked target position for the extension list inside the dialog
		RECT extensionListMarkerRect;
		GetWindowRect(GetDlgItem(hwnd, IDC_ABOUT_EXTENSIONLIST_PLACEHOLDER), &extensionListMarkerRect);
		POINT extensionListMarkerPos;
		extensionListMarkerPos.x = extensionListMarkerRect.left;
		extensionListMarkerPos.y = extensionListMarkerRect.top;
		ScreenToClient(hwnd, &extensionListMarkerPos);
		int extensionListPosX = extensionListMarkerPos.x;
		int extensionListPosY = extensionListMarkerPos.y;
		int extensionListWidth = (int)(extensionListMarkerRect.right - extensionListMarkerRect.left);
		int extensionListHeight = (int)(extensionListMarkerRect.bottom - extensionListMarkerRect.top);

		//Register the GridList window class
		WNDCLASSEX wc;
		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = 0;
		wc.lpfnWndProc   = WC_GridList::WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = (HINSTANCE)GetModuleHandle(NULL);
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = L"EX_GridList";
		wc.hIconSm       = NULL;
		RegisterClassEx(&wc);

		//Create the GridList child controls
		HWND hwndDeviceList = CreateWindowEx(WS_EX_CLIENTEDGE, L"EX_GridList", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, deviceListPosX, deviceListPosY, deviceListWidth, deviceListHeight, hwnd, (HMENU)((UINT_PTR)deviceListControlID), (HINSTANCE)GetModuleHandle(NULL), NULL);
		HWND hwndExtensionList = CreateWindowEx(WS_EX_CLIENTEDGE, L"EX_GridList", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, extensionListPosX, extensionListPosY, extensionListWidth, extensionListHeight, hwnd, (HMENU)((UINT_PTR)extensionListControlID), (HINSTANCE)GetModuleHandle(NULL), NULL);

		//Set the default font for the child controls
		SendMessage(hwndDeviceList, WM_SETFONT, (WPARAM)state->aboutDialogHFont, (LPARAM)TRUE);
		SendMessage(hwndExtensionList, WM_SETFONT, (WPARAM)state->aboutDialogHFont, (LPARAM)TRUE);
		SendMessage(hwndDeviceList, WC_GridList::GRID_SETDATAAREAFONT, (WPARAM)state->aboutDialogHFont, (LPARAM)TRUE);
		SendMessage(hwndExtensionList, WC_GridList::GRID_SETDATAAREAFONT, (WPARAM)state->aboutDialogHFont, (LPARAM)TRUE);

		//Read all the device data to be shown in the grid control
		unsigned int registeredDeviceCount = (unsigned int)state->registeredDevices.size();
		std::vector<std::wstring> deviceColumnDataName(registeredDeviceCount);
		std::vector<std::wstring> deviceColumnDataAssembly(registeredDeviceCount);
		std::vector<std::wstring> deviceColumnDataCopyright(registeredDeviceCount);
		std::vector<std::wstring> deviceColumnDataClassName(registeredDeviceCount);
		std::vector<std::wstring> deviceColumnDataVersion(registeredDeviceCount);
		std::vector<std::wstring> deviceColumnDataComments(registeredDeviceCount);
		unsigned int deviceColumnIndexNo = 0;
		for(std::list<RegisteredDeviceInfo>::const_iterator i = state->registeredDevices.begin(); i != state->registeredDevices.end(); ++i)
		{
			deviceColumnDataName[deviceColumnIndexNo] = i->info.GetDeviceImplementationName();
			deviceColumnDataAssembly[deviceColumnIndexNo] = PathGetFileName(i->assemblyPath);
			deviceColumnDataCopyright[deviceColumnIndexNo] = i->info.GetDeviceCopyright();
			deviceColumnDataClassName[deviceColumnIndexNo] = i->info.GetDeviceClassName();
			std::wstring versionString;
			IntToString(i->info.GetDeviceVersionNo(), versionString);
			deviceColumnDataVersion[deviceColumnIndexNo] = versionString;
			deviceColumnDataComments[deviceColumnIndexNo] = i->info.GetDeviceComments();
			++deviceColumnIndexNo;
		}

		//Insert our columns into the device GridList control
		SendMessage(hwndDeviceList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Name", 1, 80));
		SendMessage(hwndDeviceList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Assembly", 2, 80));
		SendMessage(hwndDeviceList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Copyright", 3, 160));
		SendMessage(hwndDeviceList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Class Name", 4, 120));
		SendMessage(hwndDeviceList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Version", 5, 40));
		SendMessage(hwndDeviceList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Comments", 6, 300));
		SendMessage(hwndDeviceList, WC_GridList::GRID_UPDATECOLUMNTEXT, 1, (LPARAM)&deviceColumnDataName);
		SendMessage(hwndDeviceList, WC_GridList::GRID_UPDATECOLUMNTEXT, 2, (LPARAM)&deviceColumnDataAssembly);
		SendMessage(hwndDeviceList, WC_GridList::GRID_UPDATECOLUMNTEXT, 3, (LPARAM)&deviceColumnDataCopyright);
		SendMessage(hwndDeviceList, WC_GridList::GRID_UPDATECOLUMNTEXT, 4, (LPARAM)&deviceColumnDataClassName);
		SendMessage(hwndDeviceList, WC_GridList::GRID_UPDATECOLUMNTEXT, 5, (LPARAM)&deviceColumnDataVersion);
		SendMessage(hwndDeviceList, WC_GridList::GRID_UPDATECOLUMNTEXT, 6, (LPARAM)&deviceColumnDataComments);

		//Read all the extension data to be shown in the grid control
		unsigned int registeredExtensionCount = (unsigned int)state->registeredExtensions.size();
		std::vector<std::wstring> extensionColumnDataName(registeredExtensionCount);
		std::vector<std::wstring> extensionColumnDataAssembly(registeredExtensionCount);
		std::vector<std::wstring> extensionColumnDataCopyright(registeredExtensionCount);
		std::vector<std::wstring> extensionColumnDataClassName(registeredExtensionCount);
		std::vector<std::wstring> extensionColumnDataVersion(registeredExtensionCount);
		std::vector<std::wstring> extensionColumnDataComments(registeredExtensionCount);
		unsigned int extensionColumnIndexNo = 0;
		for(std::list<RegisteredExtensionInfo>::const_iterator i = state->registeredExtensions.begin(); i != state->registeredExtensions.end(); ++i)
		{
			extensionColumnDataName[extensionColumnIndexNo] = i->info.GetExtensionImplementationName();
			extensionColumnDataAssembly[extensionColumnIndexNo] = PathGetFileName(i->assemblyPath);
			extensionColumnDataCopyright[extensionColumnIndexNo] = i->info.GetExtensionCopyright();
			extensionColumnDataClassName[extensionColumnIndexNo] = i->info.GetExtensionClassName();
			std::wstring versionString;
			IntToString(i->info.GetExtensionVersionNo(), versionString);
			extensionColumnDataVersion[extensionColumnIndexNo] = versionString;
			extensionColumnDataComments[extensionColumnIndexNo] = i->info.GetExtensionComments();
			++extensionColumnIndexNo;
		}

		//Insert our columns into the device GridList control
		SendMessage(hwndExtensionList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Name", 1, 80));
		SendMessage(hwndExtensionList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Assembly", 2, 80));
		SendMessage(hwndExtensionList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Copyright", 3, 160));
		SendMessage(hwndExtensionList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Class Name", 4, 120));
		SendMessage(hwndExtensionList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Version", 5, 40));
		SendMessage(hwndExtensionList, WC_GridList::GRID_INSERTCOLUMN, 0, (LPARAM)&WC_GridList::Grid_InsertColumn(L"Comments", 6, 300));
		SendMessage(hwndExtensionList, WC_GridList::GRID_UPDATECOLUMNTEXT, 1, (LPARAM)&extensionColumnDataName);
		SendMessage(hwndExtensionList, WC_GridList::GRID_UPDATECOLUMNTEXT, 2, (LPARAM)&extensionColumnDataAssembly);
		SendMessage(hwndExtensionList, WC_GridList::GRID_UPDATECOLUMNTEXT, 3, (LPARAM)&extensionColumnDataCopyright);
		SendMessage(hwndExtensionList, WC_GridList::GRID_UPDATECOLUMNTEXT, 4, (LPARAM)&extensionColumnDataClassName);
		SendMessage(hwndExtensionList, WC_GridList::GRID_UPDATECOLUMNTEXT, 5, (LPARAM)&extensionColumnDataVersion);
		SendMessage(hwndExtensionList, WC_GridList::GRID_UPDATECOLUMNTEXT, 6, (LPARAM)&extensionColumnDataComments);

		//Build the list of third party libraries
		std::list<std::wstring> thirdPartyLibraryList;
		thirdPartyLibraryList.push_back(L"boost");
		thirdPartyLibraryList.push_back(L"expat");
		thirdPartyLibraryList.push_back(L"zlib");
		thirdPartyLibraryList.push_back(L"libpng");
		thirdPartyLibraryList.push_back(L"libjpeg");
		thirdPartyLibraryList.push_back(L"libtiff");
		thirdPartyLibraryList.push_back(L"glew");
		for(std::list<std::wstring>::const_iterator i = thirdPartyLibraryList.begin(); i != thirdPartyLibraryList.end(); ++i)
		{
			SendMessage(GetDlgItem(hwnd, IDC_ABOUT_THIRDLIBRARIES_LIST), LB_ADDSTRING, 0, (LPARAM)i->c_str());
		}

		//Select an appropriate size for the program icon image based on the current
		//screen DPI settings
		int iconWidth = 0;
		int iconHeight = 0;
		int iconRegionWidth = DPIScaleWidth(48);
		int iconRegionHeight = DPIScaleHeight(48);
		if     ((iconRegionWidth >= 128) && (iconRegionHeight >= 128)) { iconWidth = 128; iconHeight = 128; }
		else if((iconRegionWidth >=  84) && (iconRegionHeight >=  84)) { iconWidth =  84; iconHeight =  84; }
		else if((iconRegionWidth >=  72) && (iconRegionHeight >=  72)) { iconWidth =  72; iconHeight =  72; }
		else if((iconRegionWidth >=  60) && (iconRegionHeight >=  60)) { iconWidth =  60; iconHeight =  60; }
		else if((iconRegionWidth >=  56) && (iconRegionHeight >=  56)) { iconWidth =  56; iconHeight =  56; }
		else if((iconRegionWidth >=  48) && (iconRegionHeight >=  48)) { iconWidth =  48; iconHeight =  48; }
		else if((iconRegionWidth >=  40) && (iconRegionHeight >=  40)) { iconWidth =  40; iconHeight =  40; }
		else if((iconRegionWidth >=  32) && (iconRegionHeight >=  32)) { iconWidth =  32; iconHeight =  32; }
		else if((iconRegionWidth >=  28) && (iconRegionHeight >=  28)) { iconWidth =  28; iconHeight =  28; }
		else if((iconRegionWidth >=  24) && (iconRegionHeight >=  24)) { iconWidth =  24; iconHeight =  24; }
		else if((iconRegionWidth >=  20) && (iconRegionHeight >=  20)) { iconWidth =  20; iconHeight =  20; }
		else if((iconRegionWidth >=  16) && (iconRegionHeight >=  16)) { iconWidth =  16; iconHeight =  16; }

		//Set the program icon image
		if((iconWidth > 0) && (iconHeight > 0))
		{
			HWND logoControl = GetDlgItem(hwnd, IDC_ABOUT_LOGO);
			HANDLE programIconHandle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAM), IMAGE_ICON, iconWidth, iconHeight, LR_SHARED);
			if(programIconHandle != NULL)
			{
				SendMessage(logoControl, STM_SETIMAGE, IMAGE_ICON, (LPARAM)programIconHandle);
			}
		}
		break;}
	case WM_NOTIFY:{
		LPNMHDR message = (LPNMHDR)lParam;
		switch(message->code)
		{
		case NM_CLICK:
		case NM_RETURN:{
			//If the user has clicked a hyperlink, open the target in the default browser.
			HWND linkControl = GetDlgItem(hwnd, IDC_ABOUT_URL);
			if(message->hwndFrom == linkControl)
			{
				PNMLINK linkInfo = (PNMLINK)message;
				ShellExecute(hwnd, L"open", linkInfo->item.szUrl, NULL, NULL, SW_SHOW);
			}
			break;}
		}
		break;}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ExodusInterface::SettingsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ExodusInterface* state = (ExodusInterface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(Message)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			CoUninitialize();
		}
		return FALSE;
	case WM_COMMAND:
		if(state == 0)
		{
			break;
		}
		if(HIWORD(wParam) == EN_CHANGE)
		{
			EnableWindow(GetDlgItem(hwnd, IDC_SETTINGS_APPLY), TRUE);
		}
		else if(HIWORD(wParam) == BN_CLICKED)
		{
			unsigned int controlID = LOWORD(wParam);
			switch(controlID)
			{
			case IDC_SETTINGS_ENABLETHROTTLE:
			case IDC_SETTINGS_RUNWHENPROGRAMLOADED:
			case IDC_SETTINGS_ENABLEPERSISTENTSTATE:
			case IDC_SETTINGS_LOADWORKSPACEWITHDEBUGSTATE:
			case IDC_SETTINGS_SHOWDEBUGCONSOLE:
				EnableWindow(GetDlgItem(hwnd, IDC_SETTINGS_APPLY), TRUE);
				break;
			case IDC_SETTINGS_OK:
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_SETTINGS_APPLY, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_SETTINGS_APPLY));
				DestroyWindow(hwnd);
				break;
			case IDC_SETTINGS_CANCEL:
				DestroyWindow(hwnd);
				break;
			case IDC_SETTINGS_APPLY:
				state->prefs.pathModulesRaw = GetDlgItemString(hwnd, IDC_SETTINGS_PATHMODULES);
				state->prefs.pathSavestatesRaw = GetDlgItemString(hwnd, IDC_SETTINGS_PATHSAVESTATES);
				state->prefs.pathPersistentStateRaw = GetDlgItemString(hwnd, IDC_SETTINGS_PATHPERSISTENTSTATE);
				state->prefs.pathWorkspacesRaw = GetDlgItemString(hwnd, IDC_SETTINGS_PATHWORKSPACES);
				state->prefs.pathCapturesRaw = GetDlgItemString(hwnd, IDC_SETTINGS_PATHCAPTURES);
				state->prefs.pathAssembliesRaw = GetDlgItemString(hwnd, IDC_SETTINGS_PATHASSEMBLIES);
				state->prefs.loadSystemRaw = GetDlgItemString(hwnd, IDC_SETTINGS_LOADSYSTEM);
				state->prefs.loadWorkspaceRaw = GetDlgItemString(hwnd, IDC_SETTINGS_LOADWORKSPACE);
				state->prefs.enableThrottling = (IsDlgButtonChecked(hwnd, IDC_SETTINGS_ENABLETHROTTLE) == BST_CHECKED);
				state->prefs.runWhenProgramModuleLoaded = (IsDlgButtonChecked(hwnd, IDC_SETTINGS_RUNWHENPROGRAMLOADED) == BST_CHECKED);
				state->prefs.enablePersistentState = (IsDlgButtonChecked(hwnd, IDC_SETTINGS_ENABLEPERSISTENTSTATE) == BST_CHECKED);
				state->prefs.loadWorkspaceWithDebugState = (IsDlgButtonChecked(hwnd, IDC_SETTINGS_LOADWORKSPACEWITHDEBUGSTATE) == BST_CHECKED);
				state->prefs.showDebugConsole = (IsDlgButtonChecked(hwnd, IDC_SETTINGS_SHOWDEBUGCONSOLE) == BST_CHECKED);
				state->ResolvePrefs();
				state->ApplyPrefs();
				EnableWindow(GetDlgItem(hwnd, IDC_SETTINGS_APPLY), FALSE);
				break;
			case IDC_SETTINGS_LOADSYSTEMCHANGE:{
				//Get the fully resolved path to the current target file
				std::wstring fileNameCurrent = GetDlgItemString(hwnd, IDC_SETTINGS_LOADSYSTEM);
				if(!fileNameCurrent.empty() && PathIsRelativePath(fileNameCurrent))
				{
					fileNameCurrent = PathCombinePaths(state->prefs.pathModules, fileNameCurrent);
				}

				//Select a new target file
				std::wstring selectedFilePath;
				if(state->SelectExistingFile(L"System Definitions|xml", L"xml", fileNameCurrent, state->prefs.pathModules, true, selectedFilePath))
				{
					if(PathStartsWithBasePath(state->prefs.pathModules, selectedFilePath))
					{
						std::wstring relativePath;
						if(PathBuildRelativePathToTarget(state->prefs.pathModules, selectedFilePath, true, relativePath))
						{
							selectedFilePath = relativePath;
						}
					}
					SetDlgItemText(hwnd, IDC_SETTINGS_LOADSYSTEM, &selectedFilePath[0]);
				}
				break;}
			case IDC_SETTINGS_LOADWORKSPACECHANGE:{
				//Get the fully resolved path to the current target file
				std::wstring fileNameCurrent = GetDlgItemString(hwnd, IDC_SETTINGS_LOADWORKSPACE);
				if(!fileNameCurrent.empty() && PathIsRelativePath(fileNameCurrent))
				{
					fileNameCurrent = PathCombinePaths(state->prefs.pathWorkspaces, fileNameCurrent);
				}

				//Select a new target file
				std::wstring selectedFilePath;
				if(state->SelectExistingFile(L"Workspace Files|xml", L"xml", fileNameCurrent, state->prefs.pathWorkspaces, true, selectedFilePath))
				{
					if(PathStartsWithBasePath(state->prefs.pathWorkspaces, selectedFilePath))
					{
						std::wstring relativePath;
						if(PathBuildRelativePathToTarget(state->prefs.pathWorkspaces, selectedFilePath, true, relativePath))
						{
							selectedFilePath = relativePath;
						}
					}
					SetDlgItemText(hwnd, IDC_SETTINGS_LOADWORKSPACE, &selectedFilePath[0]);
				}
				break;}
			case IDC_SETTINGS_PATHMODULESCHANGE:
			case IDC_SETTINGS_PATHSAVESTATESCHANGE:
			case IDC_SETTINGS_PATHPERSISTENTSTATECHANGE:
			case IDC_SETTINGS_PATHDEBUGSESSIONSCHANGE:
			case IDC_SETTINGS_PATHWORKSPACESCHANGE:
			case IDC_SETTINGS_PATHCAPTURESCHANGE:
			case IDC_SETTINGS_PATHASSEMBLIESCHANGE:{
				unsigned int textboxControlID = 0;
				switch(LOWORD(wParam))
				{
				case IDC_SETTINGS_PATHMODULESCHANGE:
					textboxControlID = IDC_SETTINGS_PATHMODULES;
					break;
				case IDC_SETTINGS_PATHPERSISTENTSTATECHANGE:
					textboxControlID = IDC_SETTINGS_PATHPERSISTENTSTATE;
					break;
				case IDC_SETTINGS_PATHSAVESTATESCHANGE:
					textboxControlID = IDC_SETTINGS_PATHSAVESTATES;
					break;
				case IDC_SETTINGS_PATHDEBUGSESSIONSCHANGE:
					textboxControlID = IDC_SETTINGS_PATHDEBUGSESSIONS;
					break;
				case IDC_SETTINGS_PATHWORKSPACESCHANGE:
					textboxControlID = IDC_SETTINGS_PATHWORKSPACES;
					break;
				case IDC_SETTINGS_PATHCAPTURESCHANGE:
					textboxControlID = IDC_SETTINGS_PATHCAPTURES;
					break;
				case IDC_SETTINGS_PATHASSEMBLIESCHANGE:
					textboxControlID = IDC_SETTINGS_PATHASSEMBLIES;
					break;
				}

				//Retrieve the current target directory
				std::wstring currentSelectedDir = GetDlgItemString(hwnd, textboxControlID);
				if(PathIsRelativePath(currentSelectedDir))
				{
					currentSelectedDir = PathCombinePaths(state->originalWorkingDir, currentSelectedDir);
				}

				//Select a new target directory
				std::wstring newSelectedDir;
				if(SelectExistingDirectory(state->mainWindowHandle, currentSelectedDir, newSelectedDir))
				{
					if(PathStartsWithBasePath(state->originalWorkingDir, newSelectedDir))
					{
						std::wstring relativePath;
						if(PathBuildRelativePathToTarget(state->originalWorkingDir, newSelectedDir, false, relativePath))
						{
							newSelectedDir = relativePath;
						}
					}
					SetDlgItemText(hwnd, textboxControlID, newSelectedDir.c_str());
				}
				break;}
			}
		}
		break;
	case WM_INITDIALOG:{
		//Set the object pointer
		state = (ExodusInterface*)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		CoInitializeEx(0, COINIT_APARTMENTTHREADED);

		HANDLE folderIconHandle = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, 0, 0, LR_SHARED);
		SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHMODULESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHSAVESTATESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHPERSISTENTSTATECHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHDEBUGSESSIONSCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHWORKSPACESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHCAPTURESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_PATHASSEMBLIESCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_LOADSYSTEMCHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		SendMessage(GetDlgItem(hwnd, IDC_SETTINGS_LOADWORKSPACECHANGE), BM_SETIMAGE, IMAGE_ICON, (LPARAM)folderIconHandle);
		SetDlgItemText(hwnd, IDC_SETTINGS_PATHMODULES, &state->prefs.pathModulesRaw[0]);
		SetDlgItemText(hwnd, IDC_SETTINGS_PATHSAVESTATES, &state->prefs.pathSavestatesRaw[0]);
		SetDlgItemText(hwnd, IDC_SETTINGS_PATHPERSISTENTSTATE, &state->prefs.pathPersistentStateRaw[0]);
		SetDlgItemText(hwnd, IDC_SETTINGS_PATHWORKSPACES, &state->prefs.pathWorkspacesRaw[0]);
		SetDlgItemText(hwnd, IDC_SETTINGS_PATHCAPTURES, &state->prefs.pathCapturesRaw[0]);
		SetDlgItemText(hwnd, IDC_SETTINGS_PATHASSEMBLIES, &state->prefs.pathAssembliesRaw[0]);
		SetDlgItemText(hwnd, IDC_SETTINGS_LOADSYSTEM, &state->prefs.loadSystemRaw[0]);
		SetDlgItemText(hwnd, IDC_SETTINGS_LOADWORKSPACE, &state->prefs.loadWorkspaceRaw[0]);
		CheckDlgButton(hwnd, IDC_SETTINGS_ENABLETHROTTLE, state->prefs.enableThrottling? BST_CHECKED: BST_UNCHECKED);
		CheckDlgButton(hwnd, IDC_SETTINGS_RUNWHENPROGRAMLOADED, state->prefs.runWhenProgramModuleLoaded? BST_CHECKED: BST_UNCHECKED);
		CheckDlgButton(hwnd, IDC_SETTINGS_ENABLEPERSISTENTSTATE, state->prefs.enablePersistentState? BST_CHECKED: BST_UNCHECKED);
		CheckDlgButton(hwnd, IDC_SETTINGS_LOADWORKSPACEWITHDEBUGSTATE, state->prefs.loadWorkspaceWithDebugState? BST_CHECKED: BST_UNCHECKED);
		CheckDlgButton(hwnd, IDC_SETTINGS_SHOWDEBUGCONSOLE, state->prefs.showDebugConsole? BST_CHECKED: BST_UNCHECKED);

		EnableWindow(GetDlgItem(hwnd, IDC_SETTINGS_APPLY), FALSE);

		break;}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ExodusInterface::SelectCompressedFileProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	SelectCompressedFileDialogParams* state = (SelectCompressedFileDialogParams*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(Message)
	{
	case WM_CLOSE:
		EndDialog(hwnd, 1);
		break;
	case WM_COMMAND:{
		bool validateSelection = false;
		if(HIWORD(wParam) == LBN_DBLCLK)
		{
			switch(LOWORD(wParam))
			{
			case IDC_SELECTCOMPRESSEDFILE_LIST:
				validateSelection = true;
				break;
			}
		}
		else if(HIWORD(wParam) == BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDOK:{
				validateSelection = true;
				break;}
			case IDCANCEL:
				EndDialog(hwnd, 1);
				break;
			}
		}
		if(validateSelection)
		{
			int currentItemListIndex = (int)SendMessage(GetDlgItem(hwnd, IDC_SELECTCOMPRESSEDFILE_LIST), LB_GETCURSEL, 0, 0);
			LRESULT getListItemDataReturn = SendMessage(GetDlgItem(hwnd, IDC_SELECTCOMPRESSEDFILE_LIST), LB_GETITEMDATA, currentItemListIndex, NULL);
			if(getListItemDataReturn != LB_ERR)
			{
				state->selectedEntryID = (unsigned int)getListItemDataReturn;
				state->selectionMade = true;
				EndDialog(hwnd, 2);
			}
		}
		break;}
	case WM_INITDIALOG:{
		//Set the object pointer
		state = (SelectCompressedFileDialogParams*)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		for(std::list<SelectCompressedFileDialogParamsFileEntry>::iterator i = state->fileList.begin(); i != state->fileList.end(); ++i)
		{
			LRESULT newItemIndex = SendMessage(GetDlgItem(hwnd, IDC_SELECTCOMPRESSEDFILE_LIST), LB_ADDSTRING, 0, (LPARAM)i->fileName.c_str());
			SendMessage(GetDlgItem(hwnd, IDC_SELECTCOMPRESSEDFILE_LIST), LB_SETITEMDATA, newItemIndex, (LPARAM)i->entryID);
		}

		break;}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
LRESULT CALLBACK ExodusInterface::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ExodusInterface* state = (ExodusInterface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(msg)
	{
	case WM_USER+1: //View operation queue update
		if(state == 0)
		{
			break;
		}

		if(!state->viewEventProcessingPaused)
		{
			state->ProcessPendingEvents();
		}
		break;
	case WM_USER+2: //Closing main interface
		if(state == 0)
		{
			break;
		}

		if(state->systemDestructionInProgress)
		{
			//Destroy the main window
			DestroyWindow(hwnd);
		}
		break;
	case WM_USER+3: //Initialize platform
		state->InitializeSystem();
		break;
	case WM_COMMAND:{
		if(state == 0)
		{
			break;
		}
		if(state->systemDestructionInProgress)
		{
			break;
		}

		//Menu commands
		int menuID = LOWORD(wParam);
		switch(menuID)
		{
		case ID_FILE_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case ID_FILE_PAUSESYSTEM:
			state->system.StopSystem();
			break;
		case ID_FILE_RUNSYSTEM:
			state->system.RunSystem();
			break;
		case ID_FILE_HARDRESET:
			state->system.Initialize();
			break;
		case ID_SYSTEM_TOGGLETHROTTLE:
			state->system.SetThrottlingState(!state->system.GetThrottlingState());
			break;
		case ID_FILE_LOADMODULE:
			state->LoadModule(state->prefs.pathModules);
			break;
		case ID_FILE_SAVESYSTEM:
			state->SaveSystem(state->prefs.pathModules);
			break;
		case ID_FILE_MANAGEMODULES:{
			if(state->moduleManagerDialog != NULL)
			{
				//Activate the window
				ShowWindow(state->moduleManagerDialog, SW_RESTORE);
				SetActiveWindow(state->moduleManagerDialog);
				SetForegroundWindow(state->moduleManagerDialog);
			}
			else
			{
				//Open the window
				state->moduleManagerDialog = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MODULEMANAGER), hwnd, ExodusInterface::ModuleManagerProc, (LPARAM)state);
				ShowWindow(state->moduleManagerDialog, SW_SHOWNORMAL);
				UpdateWindow(state->moduleManagerDialog);
			}
			break;}
		case ID_FILE_UNLOADALLMODULES:
			state->UnloadAllModules();
			break;
		case ID_FILE_SAVESTATE:
			state->SaveState(state->prefs.pathSavestates, false);
			break;
		case ID_FILE_LOADSTATE:
			state->LoadState(state->prefs.pathSavestates, false);
			break;
		case ID_FILE_QUICKSAVESTATE:
			state->QuickSaveState(false);
			break;
		case ID_FILE_QUICKLOADSTATE:
			state->QuickLoadState(false);
			break;
		case ID_FILE_SAVEDEBUGSTATE:
			state->SaveState(state->prefs.pathSavestates, true);
			break;
		case ID_FILE_LOADDEBUGSTATE:
			state->LoadState(state->prefs.pathSavestates, true);
			break;
		case ID_FILE_QUICKSAVEDEBUGSTATE:
			state->QuickSaveState(true);
			break;
		case ID_FILE_QUICKLOADDEBUGSTATE:
			state->QuickLoadState(true);
			break;
		case ID_FILE_INCREMENTSAVESLOT:
			state->IncrementSaveSlot();
			break;
		case ID_FILE_DECREMENTSAVESLOT:
			state->DecrementSaveSlot();
			break;
		case ID_SELECTSTATESLOT_1:
		case ID_SELECTSTATESLOT_2:
		case ID_SELECTSTATESLOT_3:
		case ID_SELECTSTATESLOT_4:
		case ID_SELECTSTATESLOT_5:
		case ID_SELECTSTATESLOT_6:
		case ID_SELECTSTATESLOT_7:
		case ID_SELECTSTATESLOT_8:
		case ID_SELECTSTATESLOT_9:
		case ID_SELECTSTATESLOT_0:{
			unsigned int slotID = (unsigned int)menuID - ID_SELECTSTATESLOT_1;
			state->SelectSaveSlot(slotID);
			break;}
		case ID_FILE_LOADWORKSPACE:
			state->LoadWorkspace(state->prefs.pathWorkspaces);
			break;
		case ID_FILE_SAVEWORKSPACE:
			state->SaveWorkspace(state->prefs.pathWorkspaces);
			break;
		case ID_SETTINGS_PLATFORMSETTINGS:{
			HWND settings = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS), hwnd, ExodusInterface::SettingsProc, (LPARAM)state);
			ShowWindow(settings, SW_SHOWNORMAL);
			UpdateWindow(settings);
			break;}
		case ID_HELP_ABOUT:{
			HWND about = CreateDialogParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, ExodusInterface::AboutProc, (LPARAM)state);
			ShowWindow(about, SW_SHOWNORMAL);
			UpdateWindow(about);
			break;}
		case ID_SELECTWINDOW_REVERSE:{
			if(IsWindowVisible(state->windowSelectHandle) == TRUE)
			{
				state->selectedWindow = (state->selectedWindow == 0)? (unsigned int)(state->activeWindowList.size() - 1): state->selectedWindow - 1;
				InvalidateRect(state->windowSelectHandle, NULL, FALSE);
			}
			else
			{
				state->BuildActiveWindowList();

				//Only show the window selection screen if more than one active child
				//window exists
				if(state->activeWindowList.size() > 1)
				{
					state->selectedWindow = (unsigned int)state->activeWindowList.size() - 1;
					ShowWindow(state->windowSelectHandle, SW_SHOW);
					UpdateWindow(state->windowSelectHandle);
				}
			}
			break;}
		case ID_SELECTWINDOW:{
			if(IsWindowVisible(state->windowSelectHandle) == TRUE)
			{
				state->selectedWindow = (state->selectedWindow + 1) % (unsigned int)state->activeWindowList.size();
				InvalidateRect(state->windowSelectHandle, NULL, FALSE);
			}
			else
			{
				state->BuildActiveWindowList();

				//Only show the window selection screen if more than one active child
				//window exists
				if(state->activeWindowList.size() > 1)
				{
					state->selectedWindow = 1;
					ShowWindow(state->windowSelectHandle, SW_SHOW);
					UpdateWindow(state->windowSelectHandle);
				}
			}
			break;}
		default:{
			//##TODO## Comment this and clean it up
			ExodusInterface::NewMenuList::iterator newMenuItem = state->newMenuList.find(menuID);
			if(newMenuItem != state->newMenuList.end())
			{
				newMenuItem->second.menuItem.GetMenuHandler().HandleMenuItemSelect(newMenuItem->second.menuItem.GetMenuItemID(), *state);
			}
			break;}
		}
		break;}

	//Windows messages
	case WM_MOVE:{
		if(state == 0)
		{
			break;
		}
		if(state->systemDestructionInProgress)
		{
			break;
		}

		RECT windowRect;
		GetWindowRect(hwnd, &windowRect);
		int newWindowPosX = (int)(windowRect.left);
		int newWindowPosY = (int)(windowRect.top);
		int windowMoveX = newWindowPosX - state->mainWindowPosX;
		int windowMoveY = newWindowPosY - state->mainWindowPosY;
		state->mainWindowPosX = newWindowPosX;
		state->mainWindowPosY = newWindowPosY;
		for(ExodusInterface::ViewModels::iterator i = state->viewModels.begin(); i != state->viewModels.end(); ++i)
		{
			int xpos;
			int ypos;
			i->second->viewModel->GetViewPosition(xpos, ypos);
			i->second->viewModel->SetViewPosition(xpos + windowMoveX, ypos + windowMoveY);
		}

		break;}
	case WM_CREATE:{
		//Set the object pointer
		state = (ExodusInterface*)((CREATESTRUCT*)lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Post a message to the back of the message queue, to perform further
		//initialization of the main window.
		PostMessage(hwnd, WM_USER+3, 0, 0);
		break;}
	case WM_CLOSE:
		if(state != 0)
		{
			if(!state->systemDestructionInProgress)
			{
				state->systemDestructionInProgress = true;
				boost::thread workerThread(boost::bind(boost::mem_fn(&ExodusInterface::DestroySystemInterfaceThread), state));
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT CALLBACK ExodusInterface::WndSavestateCellProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	SavestateCellWindowState* state = (SavestateCellWindowState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(msg)
	{
	case WM_USER:{
		if(state == 0)
		{
			break;
		}
		ExodusInterface* exodusInterface = (ExodusInterface*)lParam;

		std::wstring savePostfix = state->slotName;
		std::wstring saveFileName = exodusInterface->GetSavestateAutoFileNamePrefix() + L" - " + savePostfix + L".exs";
		std::wstring debuggerSaveFileName = exodusInterface->GetSavestateAutoFileNamePrefix() + L" - " + savePostfix + L" - DebugState" + L".exs";
		std::wstring saveFilePath = PathCombinePaths(exodusInterface->prefs.pathSavestates, saveFileName);
		std::wstring debuggerSaveFilePath = PathCombinePaths(exodusInterface->prefs.pathSavestates, debuggerSaveFileName);
		ISystemExternal::StateInfo stateInfo = exodusInterface->system.GetStateInfo(saveFilePath, ISystemExternal::FILETYPE_ZIP);
		ISystemExternal::StateInfo debuggerStateInfo = exodusInterface->system.GetStateInfo(debuggerSaveFilePath, ISystemExternal::FILETYPE_ZIP);
		if((state->savestatePresent == stateInfo.valid)
		&& (state->debugStatePresent == debuggerStateInfo.valid)
		&& (state->date == (state->savestatePresent? stateInfo.creationDate: debuggerStateInfo.creationDate))
		&& (state->time == (state->savestatePresent? stateInfo.creationTime: debuggerStateInfo.creationTime)))
		{
			break;
		}

		ShowWindow(hwnd, SW_HIDE);
		state->savestatePresent = stateInfo.valid;
		state->debugStatePresent = debuggerStateInfo.valid;
		state->date = stateInfo.valid? stateInfo.creationDate: debuggerStateInfo.creationDate;
		state->time = stateInfo.valid? stateInfo.creationTime: debuggerStateInfo.creationTime;
		state->timestamp = state->date + L" " + state->time;

		if(state->initializedBitmap)
		{
			DeleteObject(state->hbitmap);
			state->hbitmap = NULL;
			state->initializedBitmap = false;
		}

		//Load the screenshot
		state->screenshotPresent = stateInfo.valid && stateInfo.screenshotPresent;
		if(stateInfo.screenshotPresent)
		{
			//Open the target file
			std::wstring filePath = PathCombinePaths(exodusInterface->prefs.pathSavestates, saveFileName);
			Stream::File source;
			if(source.Open(filePath, Stream::File::OPENMODE_READONLY, Stream::File::CREATEMODE_OPEN))
			{
				//Load the ZIP header structure
				ZIPArchive archive;
				if(archive.LoadFromStream(source))
				{
					//Decompress the screenshot file to memory
					ZIPFileEntry* entry = archive.GetFileEntry(stateInfo.screenshotFilename);
					if(entry != 0)
					{
						Stream::Buffer buffer(0);
						if(entry->Decompress(buffer))
						{
							//Decode the image file from the memory buffer. Note that we
							//use the generic image load function, so the image can be
							//stored in any recognized format.
							buffer.SetStreamPos(0);
							if(state->originalImage.LoadImageFile(buffer))
							{
								state->screenshotPresent = true;
								state->bitmapWidth = state->originalImage.GetImageWidth();
								state->bitmapHeight = state->originalImage.GetImageHeight();
							}
						}
					}
				}
			}
		}

		ShowWindow(hwnd, SW_SHOWNOACTIVATE);
		break;}
	case WM_CREATE:{
		//Set the object pointer
		state = (SavestateCellWindowState*)(((CREATESTRUCT*)lParam)->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));
		break;}
	case WM_PAINT:{
		if(state == 0)
		{
			break;
		}

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		//Calculate the dimensions of the drawable client region of the window
		RECT rect;
		GetClientRect(hwnd, &rect);
		unsigned int controlWidth = rect.right - rect.left;
		unsigned int controlHeight = rect.bottom - rect.top;

		//Fixed window size properties
		unsigned int borderSize = 3;
		unsigned int infoRectangleHeight = state->infoRectangleHeight;

		//Select the fill and border colours for the information rectangle
		COLORREF borderColorSelected = RGB(0, 255, 0);
		COLORREF borderColorInactive = RGB(0, 0, 0);
		COLORREF fillColorPresent = RGB(255, 0, 0);
		COLORREF fillColorDebugPresent = RGB(0, 0, 255);
		COLORREF fillColorEmpty = RGB(0, 0, 0);
		COLORREF borderColor = (state->savestateSlotSelected)? borderColorSelected: borderColorInactive;
		COLORREF fillColor = (state->savestatePresent)? fillColorPresent: (state->debugStatePresent? fillColorDebugPresent: fillColorEmpty);
		COLORREF debugFillColor = (state->debugStatePresent)? fillColorDebugPresent: (state->savestatePresent? fillColorPresent: fillColorEmpty);

		//Draw the outer rectangle
		unsigned int outerRectangleWidth = controlWidth;
		unsigned int outerRectangleHeight = infoRectangleHeight;
		unsigned int outerRectanglePosX = 0;
		unsigned int outerRectanglePosY = controlHeight - outerRectangleHeight;
		HBRUSH outerRectangleBrush = CreateSolidBrush(borderColor);
		HBRUSH outerRectangleBrushOld = (HBRUSH)SelectObject(hdc, outerRectangleBrush);
		PatBlt(hdc, outerRectanglePosX, outerRectanglePosY, outerRectangleWidth, outerRectangleHeight, PATCOPY);
		SelectObject(hdc, outerRectangleBrushOld);
		DeleteObject(outerRectangleBrush);

		//Draw the inner rectangle
		unsigned int innerRectangleWidth = outerRectangleWidth - (borderSize * 2);
		unsigned int innerRectangleHeight = outerRectangleHeight - (borderSize * 2);
		unsigned int innerRectanglePosX = outerRectanglePosX + borderSize;
		unsigned int innerRectanglePosY = outerRectanglePosY + borderSize;
		HBRUSH innerRectangleBrush = CreateSolidBrush(fillColor);
		HBRUSH innerRectangleBrushOld = (HBRUSH)SelectObject(hdc, innerRectangleBrush);
		PatBlt(hdc, innerRectanglePosX, innerRectanglePosY, innerRectangleWidth, innerRectangleHeight, PATCOPY);
		SelectObject(hdc, innerRectangleBrushOld);
		DeleteObject(innerRectangleBrush);

		//Draw the debug present rectangle
		unsigned int debugRectangleWidth = innerRectangleWidth;
		unsigned int debugRectangleHeight = innerRectangleHeight / 2;
		unsigned int debugRectanglePosX = innerRectanglePosX;
		unsigned int debugRectanglePosY = innerRectanglePosY + (innerRectangleHeight - debugRectangleHeight);
		HBRUSH debugRectangleBrush = CreateSolidBrush(debugFillColor);
		HBRUSH debugRectangleBrushOld = (HBRUSH)SelectObject(hdc, debugRectangleBrush);
		PatBlt(hdc, debugRectanglePosX, debugRectanglePosY, debugRectangleWidth, debugRectangleHeight, PATCOPY);
		SelectObject(hdc, debugRectangleBrushOld);
		DeleteObject(debugRectangleBrush);

		//Calculate the maximum font width and height which will allow two lines of text
		//to be displayed in the information rectangle, with 20 characters per line.
		unsigned int minimumCharacterWidth = 20;
		unsigned int minimumCharacterHeight = 2;
		unsigned int fontWidthPixels = innerRectangleWidth / minimumCharacterWidth;
		unsigned int fontHeightPixels = innerRectangleHeight / minimumCharacterHeight;
		POINT fontPoint;
		fontPoint.x = fontWidthPixels;
		fontPoint.y = fontHeightPixels;
		DPtoLP(hdc, &fontPoint, 1);
		unsigned int fontWidthLogical = fontPoint.x;
		unsigned int fontHeightLogical = fontPoint.y;

		//Create a logical font to display our text
		HFONT hfont = CreateFont(fontHeightLogical, fontWidthLogical, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, NULL);
		HFONT hfontOld = (HFONT)SelectObject(hdc, (HGDIOBJ)hfont);

		//Calculate the position of the info rectangle in logical coordinates
		POINT infoPos[2];
		infoPos[0].x = innerRectanglePosX;
		infoPos[0].y = innerRectanglePosY;
		infoPos[1].x = innerRectanglePosX + innerRectangleWidth;
		infoPos[1].y = innerRectanglePosY + innerRectangleHeight;
		DPtoLP(hdc, &infoPos[0], 2);
		RECT infoRect;
		infoRect.left = infoPos[0].x;
		infoRect.top = infoPos[0].y;
		infoRect.right = infoPos[1].x;
		infoRect.bottom = infoPos[1].y;

		//Set the text colour
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);

		//Write the savestate creation timestamp
		DrawText(hdc, &state->timestamp[0], (int)state->timestamp.length(), &infoRect, DT_SINGLELINE | DT_CENTER | DT_TOP);
//		DrawText(hdc, &state->date[0], (int)state->date.length(), &infoRect, DT_SINGLELINE | DT_LEFT);
//		DrawText(hdc, &state->time[0], (int)state->time.length(), &infoRect, DT_SINGLELINE | DT_RIGHT);

		//Write the slot number
		DrawText(hdc, &state->slotName[0], (int)state->slotName.length(), &infoRect, DT_SINGLELINE | DT_CENTER | DT_BOTTOM);

		//Switch back to the previously selected font, and delete our logical font object.
		SelectObject(hdc, (HGDIOBJ)hfontOld);
		DeleteObject(hfont);

		if(state->screenshotPresent)
		{
			//Calculate the dimensions of the picture box
			unsigned int pictureBoxWidth = controlWidth - (borderSize * 2);
			unsigned int pictureBoxHeight = controlHeight - outerRectangleHeight;
			unsigned int pictureBoxXPos = borderSize;
			unsigned int pictureBoxYPos = 0;

			//If we haven't already created a GDI bitmap object containing our image,
			//build the object.
			if(!state->initializedBitmap)
			{
				state->initializedBitmap = true;

				unsigned int imageWidth = state->originalImage.GetImageWidth();
				unsigned int imageHeight = state->originalImage.GetImageHeight();
				float imageAspect = (float)imageWidth / (float)imageHeight;
				float displayAspect = (float)pictureBoxWidth / (float)pictureBoxHeight;
				if(imageAspect > displayAspect)
				{
					//Our image has a wider aspect ratio than the screen region.
					//Resize the image to maximize the horizontal resolution.
					state->bitmapWidth = pictureBoxWidth;
					state->bitmapHeight = (unsigned int)(((float)pictureBoxWidth * ((float)imageHeight / (float)imageWidth)) + 0.5f);
					state->bitmapXPos = 0;
					state->bitmapYPos = (pictureBoxHeight - state->bitmapHeight) / 2;
				}
				else
				{
					//Our image has a taller aspect ratio than the screen region.
					//Resize the image to maximize the vertical resolution.
					state->bitmapWidth = (unsigned int)(((float)pictureBoxHeight * ((float)imageWidth / (float)imageHeight)) + 0.5f);
					state->bitmapHeight = pictureBoxHeight;
					state->bitmapXPos = (pictureBoxWidth - state->bitmapWidth) / 2;
					state->bitmapYPos = 0;
				}
				state->resizedImage.ResampleBilinear(state->originalImage, state->bitmapWidth, state->bitmapHeight);

				Stream::Buffer ddbData(0);
				BITMAPINFO bitmapInfo;
				if(state->resizedImage.SaveDIBImage(ddbData, bitmapInfo))
				{
					state->hbitmap = CreateCompatibleBitmap(hdc, state->bitmapWidth, state->bitmapHeight);
					SetDIBits(hdc, state->hbitmap, 0, state->bitmapHeight, ddbData.GetRawBuffer(), &bitmapInfo, 0);
				}
			}

			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, state->hbitmap);

			//Pre-fill the picture box with black
			BitBlt(hdc, pictureBoxXPos, pictureBoxYPos, pictureBoxWidth, pictureBoxHeight, hdcMem, 0, 0, BLACKNESS);

			//Draw our image centered in the picture box
			BitBlt(hdc, pictureBoxXPos + state->bitmapXPos, pictureBoxYPos + state->bitmapYPos, state->bitmapWidth, state->bitmapHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
		}

		EndPaint(hwnd, &ps);
		break;}
	case WM_DESTROY:
		if(state != 0)
		{
			DeleteObject(state->hbitmap);
		}
		return DefWindowProc(hwnd, WM_DESTROY, wParam, lParam);
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT CALLBACK ExodusInterface::WndSavestateProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ExodusInterface* state = (ExodusInterface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(msg)
	{
	case WM_TIMER:{
		ShowWindow(hwnd, SW_HIDE);
		KillTimer(hwnd, 1);
	break;}
	case WM_CREATE:{
		//Set the object pointer
		state = (ExodusInterface*)(((CREATESTRUCT*)lParam)->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Register the window class for a cell within our savestate window
		WNDCLASSEX wc;
		std::wstring windowClassName = L"ExodusWCSaveCell";
		std::wstring windowName = L"ExodusSaveCell";
		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = 0;
		wc.lpfnWndProc   = WndSavestateCellProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = GetModuleHandle(NULL);
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = windowClassName.c_str();
		wc.hIconSm       = NULL;
		if((RegisterClassEx(&wc) == 0) && (GetLastError() != ERROR_CLASS_ALREADY_EXISTS))
		{
			return NULL;
		}

		//Initialize the info for each child cell, and create the child cell windows.
		for(unsigned int i = 0; i < state->cellCount; ++i)
		{
			unsigned int slotNo = ((i + 1) % 10);
			std::wstringstream slotName;
			slotName << slotNo;
			state->cell[i].slotNo = slotNo;
			state->cell[i].slotName = slotName.str();
			state->cell[i].screenshotPresent = false;
			state->cell[i].savestateSlotSelected = (i == state->selectedSaveCell);

			state->cell[i].hwnd = CreateWindowEx(0, windowClassName.c_str(), windowName.c_str(), WS_CHILD, 1, 1, 1, 1, hwnd, NULL, GetModuleHandle(NULL), &(state->cell[i]));
			ShowWindow(state->cell[i].hwnd, SW_SHOWNOACTIVATE);
		}

		break;}
	case WM_SHOWWINDOW:{
		if(state == 0)
		{
			break;
		}

		if(wParam == TRUE)
		{
			//Obtain information on the dimensions of the screen
			HMONITOR monitor = MonitorFromWindow(state->mainWindowHandle, MONITOR_DEFAULTTONEAREST);
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(monitorInfo);
			GetMonitorInfo(monitor, &monitorInfo);
			int monitorPosX = monitorInfo.rcWork.left;
			int monitorPosY = monitorInfo.rcWork.top;
			int monitorWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
			int monitorHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

			//If the savestate popup window has changed monitors since the last time it
			//was displayed, calculate the new position and dimensions for the window.
			if((state->savestateMonitorPosX != monitorPosX) || (state->mainWindowPosY != monitorPosY) || (state->savestateMonitorWidth != monitorWidth) || (state->savestateMonitorHeight != monitorHeight))
			{
				//Save the new monitor info
				state->savestateMonitorPosX = monitorPosX;
				state->savestateMonitorPosY = monitorPosY;
				state->savestateMonitorWidth = monitorWidth;
				state->savestateMonitorHeight = monitorHeight;

				//Fixed window size properties
				float imageAspect = 3.0f / 4.0f;
				unsigned int cellsPerLine = 10;
				unsigned int lineCount = 1;
				int imageBoxHeight = (int)((((float)monitorWidth * imageAspect) / (float)cellsPerLine) * (float)lineCount);
				int infoRectangleHeight = (int)((float)(monitorWidth / cellsPerLine) * (1.2f / 4.0f));

				//Calculate the new position and dimensions for the main savestate popup
				//window, and apply the new settings.
				int savestateWindowWidth = monitorWidth;
				int savestateWindowHeight = infoRectangleHeight + imageBoxHeight;
				int savestateWindowPosX = monitorPosX;
				int savestateWindowPosY = (monitorPosY + monitorHeight) - savestateWindowHeight;
				int widthPerCell = savestateWindowWidth / (int)cellsPerLine;
				int heightPerCell = savestateWindowHeight / (int)lineCount;
				MoveWindow(hwnd, savestateWindowPosX, savestateWindowPosY, savestateWindowWidth, savestateWindowHeight, TRUE);

				//Update settings for each child cell window
				for(unsigned int i = 0; i < state->cellCount; ++i)
				{
					//Free the bitmap object for the child cell, and force it to be
					//recalculated the next time the window is drawn.
					state->cell[i].initializedBitmap = false;
					DeleteObject(state->cell[i].hbitmap);

					//Calculate the new position and size of the child cell, and apply
					//the new settings.
					state->cell[i].infoRectangleHeight = (unsigned int)infoRectangleHeight;
					int cellPosX = (i % cellsPerLine) * widthPerCell;
					int cellPosY = (i / cellsPerLine) * heightPerCell;
					MoveWindow(state->cell[i].hwnd, cellPosX, cellPosY, widthPerCell, heightPerCell, TRUE);
				}
			}
		}
	break;}
	case WM_KEYDOWN:{
		if(state == 0)
		{
			break;
		}

		if((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			char pressedChar = (char)wParam;
			if((pressedChar >= '0') && (pressedChar <= '9'))
			{
				unsigned int newSaveSlot = (pressedChar == '0')? 9: (pressedChar - '1');
				state->SelectSaveSlot(newSaveSlot);
			}
		}

		break;}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT CALLBACK ExodusInterface::WndWindowSelectProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Obtain the object pointer
	ExodusInterface* state = (ExodusInterface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch(msg)
	{
	case WM_CREATE:{
		//Set the object pointer
		state = (ExodusInterface*)(((CREATESTRUCT*)lParam)->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));
	break;}
	case WM_SYSKEYUP:
	case WM_KEYUP:{
		if(state == 0)
		{
			break;
		}

		if(wParam == VK_CONTROL)
		{
			state->activeWindowList[state->selectedWindow]->ActivateView();
		}
	break;}
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:{
		if(state == 0)
		{
			break;
		}

		bool selectedWindowChanged = false;
		int selectedCellAdjustment = 0;
		int selectedColumnAdjustment = 0;
		switch(wParam)
		{
		case VK_LEFT:
			selectedColumnAdjustment = -1;
			selectedWindowChanged = true;
			break;
		case VK_RIGHT:
			selectedColumnAdjustment = 1;
			selectedWindowChanged = true;
			break;
		case VK_UP:
			selectedCellAdjustment = -1;
			selectedWindowChanged = true;
			break;
		case VK_DOWN:
			selectedCellAdjustment = 1;
			selectedWindowChanged = true;
			break;
		}

		if(selectedWindowChanged)
		{
			int lastCell = (int)state->windowSelectEntriesPerColumn - 1;
			int lastColumn = (int)state->windowSelectColumns - 1;
			int lastCellLastColumn = (int)(state->activeWindowList.size() - 1) % (int)state->windowSelectEntriesPerColumn;
			int currentCell = (int)(state->selectedWindow % state->windowSelectEntriesPerColumn);
			int currentColumn = (int)(state->selectedWindow / state->windowSelectEntriesPerColumn);

			currentCell += selectedCellAdjustment;
			if((currentCell > lastCell) || ((currentColumn == lastColumn) && (currentCell > lastCellLastColumn)))
			{
				++currentColumn;
				currentCell = 0;
			}
			else if(currentCell < 0)
			{
				--currentColumn;
				currentCell = lastCell;
			}

			currentColumn += selectedColumnAdjustment;
			if(currentColumn > lastColumn)
			{
				currentColumn = 0;
			}
			else if(currentColumn < 0)
			{
				currentColumn = lastColumn;
			}
			if((currentColumn == lastColumn) && (currentCell > lastCellLastColumn))
			{
				currentCell = lastCellLastColumn;
			}

			unsigned int newSelectedWindow = ((unsigned int)currentColumn * state->windowSelectEntriesPerColumn) + (unsigned int)currentCell;
			state->selectedWindow = newSelectedWindow;
			InvalidateRect(state->windowSelectHandle, NULL, FALSE);
		}
	break;}
	case WM_ACTIVATE:{
		if(state == 0)
		{
			break;
		}

		if(wParam == WA_INACTIVE)
		{
			ShowWindow(hwnd, SW_HIDE);
		}
	break;}
	case WM_SHOWWINDOW:{
		if(state == 0)
		{
			break;
		}

		if(wParam == TRUE)
		{
			//Fixed window size properties
			unsigned int windowEntries = (unsigned int)state->activeWindowList.size();
			unsigned int cellOffsetX = 5;
			unsigned int cellOffsetY = 5;
			unsigned int cellWidth = 250;
			unsigned int cellHeight = 20;
			unsigned int columnBreakWidth = 4;

			//Obtain information on the dimensions of the screen
			HMONITOR monitor = MonitorFromWindow(state->mainWindowHandle, MONITOR_DEFAULTTONEAREST);
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(monitorInfo);
			GetMonitorInfo(monitor, &monitorInfo);
			int monitorPosX = monitorInfo.rcWork.left;
			int monitorPosY = monitorInfo.rcWork.top;
			int monitorWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
			int monitorHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
			unsigned int maxColumns = (unsigned int)monitorWidth / cellWidth;
			unsigned int maxEntrierPerColumn = (unsigned int)monitorHeight / cellHeight;
			float aspectRatioScreen = (float)monitorWidth / (float)monitorHeight;

			//Calculate the number of columns to show, and the number of entries per
			//column, which gives us a window select screen whose aspect ratio is closest
			//to that of the screen, with a minimal number of blank entries in additional
			//columns.
			state->windowSelectColumns = 1;
			state->windowSelectEntriesPerColumn = windowEntries;
			bool done = false;
			while(!done)
			{
				unsigned int columnCountWider = state->windowSelectColumns + 1;
				unsigned int columnEntriesWider = (windowEntries + (columnCountWider - 1)) / columnCountWider;

				float aspectRatioCurrent = (float)(cellWidth * state->windowSelectColumns) / (float)(state->windowSelectEntriesPerColumn * cellHeight);
				float aspectRatioWider = (float)(cellWidth * columnCountWider) / (float)(columnEntriesWider * cellHeight);

				float aspectDisplacementCurrent = (aspectRatioCurrent > aspectRatioScreen)? aspectRatioCurrent - aspectRatioScreen: aspectRatioScreen - aspectRatioCurrent;
				float aspectDisplacementWider = (aspectRatioWider > aspectRatioScreen)? aspectRatioWider - aspectRatioScreen: aspectRatioScreen - aspectRatioWider;

				if(aspectDisplacementCurrent > aspectDisplacementWider)
				{
					state->windowSelectEntriesPerColumn = columnEntriesWider;
					state->windowSelectColumns = columnCountWider;
				}
				else
				{
					done = true;
				}
			}

			//Resize the window to the required height to fit the specified number of
			//columns and entries per column, and position it in the center of the
			//screen.
			int windowWidth = (cellOffsetX * 2) + (state->windowSelectColumns * cellWidth) + ((state->windowSelectColumns - 1) * columnBreakWidth);
			int windowHeight = (cellOffsetY * 2) + (state->windowSelectEntriesPerColumn * cellHeight);
			int windowPosX = monitorPosX + ((monitorWidth - windowWidth) / 2);
			int windowPosY = monitorPosY + ((monitorHeight - windowHeight) / 2);
			MoveWindow(hwnd, windowPosX, windowPosY, windowWidth, windowHeight, TRUE);
		}
	break;}
	case WM_PAINT:{
		PAINTSTRUCT paintInfo;
		BeginPaint(hwnd, &paintInfo);
		HDC hdc = paintInfo.hdc;

		//Calculate the width and height of this window
		RECT windowRect;
		GetClientRect(hwnd, &windowRect);
		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;

		//Create a bitmap we can render the control onto
		HDC hdcControl = hdc;
		HDC hdcBitmap = CreateCompatibleDC(hdcControl);
		HBITMAP hbitmap = CreateCompatibleBitmap(hdcControl, windowWidth, windowHeight);
		HBITMAP hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);

		//Send a WM_PRINTCLIENT message to get the window to render into the bitmap
		SendMessage(hwnd, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);

		//Transfer the final bitmap for the control into the screen buffer
		BitBlt(hdcControl, 0, 0, windowWidth, windowHeight, hdcBitmap, 0, 0, SRCCOPY);

		//Validate the entire client area of the control. It's REALLY important that we
		//call this here, otherwise Windows is going to send WM_PAINT messages over and
		//over again waiting for the region to be validated.
		ValidateRect(hwnd, NULL);

		//Clean up the allocated handles
		SelectObject(hdcBitmap, hbitmapOriginal);
		DeleteObject(hbitmap);
		DeleteDC(hdcBitmap);

		EndPaint(hwnd, &paintInfo);

	break;}
	case WM_PRINTCLIENT:{
		if(state == 0)
		{
			break;
		}

		HDC hdc = (HDC)wParam;

		//Fill the background of the window
		RECT backgroundRect;
		GetClientRect(hwnd, &backgroundRect);
		HBRUSH backgroundBrush = GetSysColorBrush(COLOR_3DFACE);
		FillRect(hdc, &backgroundRect, backgroundBrush);
		HBRUSH backgroundBorderBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		FrameRect(hdc, &backgroundRect, backgroundBorderBrush);

		//Calculate a font height which will allow the largest font to be used to display
		//the window names in each cell, without overlapping the cell border.
		//Fixed window size properties
		unsigned int windowEntries = (unsigned int)state->activeWindowList.size();
		unsigned int cellOffsetX = 5;
		unsigned int cellOffsetY = 5;
		unsigned int cellWidth = 250;
		unsigned int cellHeight = 20;
		unsigned int columnBreakWidth = 4;

		unsigned int fontOffset = 2;
		POINT point;
		point.x = 0;
		point.y = cellHeight - (fontOffset * 2);
		DPtoLP(hdc, &point, 1);
		unsigned int fontHeightLogical = point.y;

		//Create a logical font to display our text
		HFONT hfont = CreateFont(fontHeightLogical, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, NULL);
		HFONT hfontOld = (HFONT)SelectObject(hdc, (HGDIOBJ)hfont);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(0, 0, 0));

		//Draw each item in the window
		for(unsigned int i = 0; i < windowEntries; ++i)
		{
			//Calculate the position of this cell in the window
			unsigned int cellPosX = cellOffsetX + ((i / state->windowSelectEntriesPerColumn) * (cellWidth + columnBreakWidth));
			unsigned int cellPosY = cellOffsetY + ((i % state->windowSelectEntriesPerColumn) * cellHeight);

			//If this is the currently selected item, draw a selection rectangle around
			//the cell.
			if(i == state->selectedWindow)
			{
				HBRUSH rectangleBrush = CreateSolidBrush(RGB(128, 128, 192));
				HBRUSH rectangleBrushOld = (HBRUSH)SelectObject(hdc, rectangleBrush);
				HPEN rectanglePen = CreatePen(PS_INSIDEFRAME, 1, RGB(0, 0, 0));
				HPEN rectanglePenOld = (HPEN)SelectObject(hdc, rectanglePen);
				Rectangle(hdc, cellPosX, cellPosY, cellPosX + cellWidth, cellPosY + cellHeight);
				SelectObject(hdc, rectanglePenOld);
				DeleteObject(rectanglePen);
				SelectObject(hdc, rectangleBrushOld);
				DeleteObject(rectangleBrush);
			}

			//Write the name of the window this cell corresponds to
			RECT textRect;
			textRect.left = cellPosX + fontOffset;
			textRect.right = cellPosX + cellWidth;
			textRect.top = cellPosY;
			textRect.bottom = cellPosY + cellHeight;
			std::wstring windowTitle = state->activeWindowList[i]->GetViewTitle();
			DrawText(hdc, windowTitle.c_str(), (int)windowTitle.length(), &textRect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
		}

		//Switch back to the previously selected font, and delete our logical font object.
		SelectObject(hdc, (HGDIOBJ)hfontOld);
		DeleteObject(hfont);
	break;}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------------------------------------
//Joystick functions
//----------------------------------------------------------------------------------------
void ExodusInterface::JoystickInputWorkerThread()
{
	//Calculate the maximum number of joysticks, and the maximum number of buttons and
	//axis per joystick.
	const unsigned int maxButtonCount = 32;
	const unsigned int maxAxisCount = 6;
	UINT maxJoystickCount = joyGetNumDevs();

	//Initialize the button and axis state for each joystick
	std::vector<std::vector<bool>> buttonState;
	std::vector<std::vector<float>> axisState;
	buttonState.resize(maxJoystickCount);
	axisState.resize(maxJoystickCount);
	for(unsigned int i = 0; i < maxJoystickCount; ++i)
	{
		buttonState[i].resize(maxButtonCount);
		axisState[i].resize(maxAxisCount);
	}

	//Process input state changes from joysticks until we're requested to stop
	while(joystickWorkerThreadActive)
	{
		//Latch new values from each joystick
		for(unsigned int i = 0; i < maxJoystickCount; ++i)
		{
			//Obtain info on the capabilities of this joystick
			JOYCAPS joystickCapabilities;
			MMRESULT joyGetDevCapsReturn = joyGetDevCaps(i, &joystickCapabilities, sizeof(joystickCapabilities));
			if(joyGetDevCapsReturn != JOYERR_NOERROR)
			{
				continue;
			}

			//Obtain info on the current button state for this joystick
			JOYINFOEX joystickInfo;
			joystickInfo.dwSize = sizeof(joystickInfo);
			joystickInfo.dwFlags = JOY_RETURNALL;
			MMRESULT joyGetPosExResult = joyGetPosEx(i, &joystickInfo);
			if(joyGetPosExResult != JOYERR_NOERROR)
			{
				continue;
			}

			//Latch the new state for each button in this joystick
			unsigned int activeButtonCount = (joystickCapabilities.wNumButtons > maxButtonCount)? maxButtonCount: joystickCapabilities.wNumButtons;
			Data buttonData(activeButtonCount, joystickInfo.dwButtons);
			for(unsigned int buttonNo = 0; buttonNo < activeButtonCount; ++buttonNo)
			{
				//If the current button state matches the previous button state, advance
				//to the next button.
				bool buttonStateNew = buttonData.GetBit(buttonNo);
				if(buttonState[i][buttonNo] == buttonStateNew)
				{
					continue;
				}

				//Latch the new button state
				buttonState[i][buttonNo] = buttonStateNew;

				//Notify the system of the button state change
				IDeviceContext::KeyCode keyCode;
				if(system.TranslateJoystickButton(i, buttonNo, keyCode))
				{
					if(buttonStateNew)
					{
						system.HandleInputKeyDown(keyCode);
					}
					else
					{
						system.HandleInputKeyUp(keyCode);
					}
				}
			}

			//Latch the new state for each axis in this joystick
			unsigned int reportedAxesCount = joystickCapabilities.wNumAxes;
			bool hasAxisZ = (joystickCapabilities.wCaps & JOYCAPS_HASZ) != 0;
			bool hasAxisR = (joystickCapabilities.wCaps & JOYCAPS_HASR) != 0;
			bool hasAxisU = (joystickCapabilities.wCaps & JOYCAPS_HASU) != 0;
			bool hasAxisV = (joystickCapabilities.wCaps & JOYCAPS_HASV) != 0;
			unsigned int knownAxesCount = (hasAxisZ? 1: 0) + (hasAxisR? 1: 0) + (hasAxisU? 1: 0) + (hasAxisV? 1: 0);
			bool hasAxisX = ((knownAxesCount + 2) <= reportedAxesCount);
			bool hasAxisY = ((knownAxesCount + 2) <= reportedAxesCount);
			std::vector<float> newAxisState;
			if(hasAxisX)
			{
				float newAxisStateX = ((float)(joystickInfo.dwXpos - joystickCapabilities.wXmin) / ((float)(joystickCapabilities.wXmax - joystickCapabilities.wXmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateX);
			}
			if(hasAxisY)
			{
				float newAxisStateY = ((float)(joystickInfo.dwYpos - joystickCapabilities.wYmin) / ((float)(joystickCapabilities.wYmax - joystickCapabilities.wYmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateY);
			}
			if(hasAxisZ)
			{
				float newAxisStateZ = ((float)(joystickInfo.dwZpos - joystickCapabilities.wZmin) / ((float)(joystickCapabilities.wZmax - joystickCapabilities.wZmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateZ);
			}
			if(hasAxisR)
			{
				float newAxisStateR = ((float)(joystickInfo.dwRpos - joystickCapabilities.wRmin) / ((float)(joystickCapabilities.wRmax - joystickCapabilities.wRmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateR);
			}
			if(hasAxisU)
			{
				float newAxisStateU = ((float)(joystickInfo.dwUpos - joystickCapabilities.wUmin) / ((float)(joystickCapabilities.wUmax - joystickCapabilities.wUmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateU);
			}
			if(hasAxisV)
			{
				float newAxisStateV = ((float)(joystickInfo.dwVpos - joystickCapabilities.wVmin) / ((float)(joystickCapabilities.wVmax - joystickCapabilities.wVmin) / 2.0f)) - 1.0f;
				newAxisState.push_back(newAxisStateV);
			}
			for(unsigned int axisNo = 0; axisNo < (unsigned int)newAxisState.size(); ++axisNo)
			{
				//If the current axis state matches the previous axis state, advance to
				//the next axis.
				if(axisState[i][axisNo] == newAxisState[axisNo])
				{
					continue;
				}

				//Latch the new axis state
				float axisStateNew = newAxisState[axisNo];
				float axisStateOld = axisState[i][axisNo];
				axisState[i][axisNo] = axisStateNew;

				//Notify the system of axis state changes
				IDeviceContext::AxisCode axisCode;
				if(system.TranslateJoystickAxis(i, axisNo, axisCode))
				{
					system.HandleInputAxisUpdate(axisCode, axisStateNew);
				}

				//Notify the system of button state changes linked to this axis
				if((axisStateOld >= 0.25f) && (axisStateNew < 0.25f))
				{
					IDeviceContext::KeyCode keyCode;
					if(system.TranslateJoystickAxisAsButton(i, axisNo, true, keyCode))
					{
						system.HandleInputKeyUp(keyCode);
					}
				}
				if((axisStateOld <= -0.25f) && (axisStateNew > -0.25f))
				{
					IDeviceContext::KeyCode keyCode;
					if(system.TranslateJoystickAxisAsButton(i, axisNo, false, keyCode))
					{
						system.HandleInputKeyUp(keyCode);
					}
				}
				if((axisStateOld < 0.25f) && (axisStateNew >= 0.25f))
				{
					IDeviceContext::KeyCode keyCode;
					if(system.TranslateJoystickAxisAsButton(i, axisNo, true, keyCode))
					{
						system.HandleInputKeyDown(keyCode);
					}
				}
				if((axisStateOld > -0.25f) && (axisStateNew <= -0.25f))
				{
					IDeviceContext::KeyCode keyCode;
					if(system.TranslateJoystickAxisAsButton(i, axisNo, false, keyCode))
					{
						system.HandleInputKeyDown(keyCode);
					}
				}
			}
		}

		//Delay until it's time to latch the joystick input state again
		Sleep(10);
	}

	//Since this thread is terminating, notify any waiting threads.
	boost::mutex::scoped_lock lock(joystickWorkerThreadMutex);
	joystickWorkerThreadStopped.notify_all();
}
