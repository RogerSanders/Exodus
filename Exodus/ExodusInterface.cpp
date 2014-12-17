#include "ExodusInterface.h"
#include "WindowsSupport/WindowsSupport.pkg"
#include "WindowsControls/WindowsControls.pkg"
#include "resource.h"
#include "ZIP/ZIP.pkg"
#include "Stream/Stream.pkg"
#include "HierarchicalStorage/HierarchicalStorage.pkg"
#include "DataConversion/DataConversion.pkg"
#include "MenuSelectableOption.h"
#include <thread>
#include "MenuHandler.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExodusInterface::ExodusInterface()
:system(0), viewManager(0), menuHandler(0)
{
	//Set the size of the savestate window cell array
	cell.resize(cellCount);

	//Set the next menu ID number to use when building menus
	nextFreeMenuID = 1;

	//Create our submenu objects
	fileSubmenu = new MenuSubmenu(L"File");
	systemSubmenu = new MenuSubmenu(L"System");
	settingsSubmenu = new MenuSubmenu(L"System");
	debugSubmenu = new MenuSubmenu(L"Debug");

	//Initialize the main window state
	mainWindowHandle = NULL;
	mainDockingWindowHandle = NULL;
	hfont = NULL;
	mainWindowPosCaptured = false;

	//Initialize the savestate popup window settings
	selectedSaveCell = 0;
	savestatePopupTimeout = 5000;
	savestateMonitorPosX = 0;
	savestateMonitorPosY = 0;
	savestateMonitorWidth = 0;
	savestateMonitorHeight = 0;

	//Initialize the system load state
	systemLoaded = false;
	systemDestructionInProgress = false;

	//Initialize the joystick worker thread state
	joystickWorkerThreadActive = false;

	//Initialize other system options
	debugConsoleOpen = false;
}

//----------------------------------------------------------------------------------------
ExodusInterface::~ExodusInterface()
{
	//Stop the joystick worker thread
	std::unique_lock<std::mutex> lock(joystickWorkerThreadMutex);
	if(joystickWorkerThreadActive)
	{
		joystickWorkerThreadActive = false;
		joystickWorkerThreadStopped.wait(lock);
	}

	//Delete the view manager
	delete viewManager;

	//Delete our submenu objects
	delete fileSubmenu;
	delete systemSubmenu;
	delete settingsSubmenu;
	delete debugSubmenu;

	//Delete our menu handler
	delete menuHandler;
}

//----------------------------------------------------------------------------------------
//System interface functions
//----------------------------------------------------------------------------------------
void ExodusInterface::BindToSystem(ISystemGUIInterface* asystem)
{
	system = asystem;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::UnbindFromSystem()
{
	system = 0;
}

//----------------------------------------------------------------------------------------
ISystemGUIInterface* ExodusInterface::GetSystemInterface() const
{
	return system;
}

//----------------------------------------------------------------------------------------
//Initialization functions
//----------------------------------------------------------------------------------------
HWND ExodusInterface::CreateMainInterface(HINSTANCE hinstance)
{
	//Calculate the default dimensions of the main window.
	unsigned int width = DPIScaleWidth(1024);
	unsigned int height = DPIScaleWidth(768);
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

	//Add the main docking window to the main window
	DockingWindow::RegisterWindowClass((HINSTANCE)GetModuleHandle(NULL));
	mainDockingWindowHandle = CreateWindowEx(0, DockingWindow::windowClassName, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, mainWindowHandle, NULL, (HINSTANCE)GetModuleHandle(NULL), NULL);
	ShowWindow(mainDockingWindowHandle, SW_SHOWNORMAL);
	UpdateWindow(mainDockingWindowHandle);

	//Create the font for our main window
	int fontPointSize = 8;
	HDC hdc = GetDC(mainWindowHandle);
	int fontnHeight = -MulDiv(fontPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(mainWindowHandle, hdc);
	std::wstring fontTypefaceName = L"MS Shell Dlg";
	hfont = CreateFont(fontnHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, &fontTypefaceName[0]);

	//Set the font for our main window and main docking window
	SendMessage(mainWindowHandle, WM_SETFONT, (WPARAM)hfont, (LPARAM)TRUE);
	SendMessage(mainDockingWindowHandle, WM_SETFONT, (WPARAM)hfont, (LPARAM)TRUE);

	//Create the view manager
	viewManager = new ViewManager(hinstance, mainWindowHandle, mainDockingWindowHandle, *system);

	//Create our menu handler
	menuHandler = new MenuHandler(*this, *this);
	menuHandler->LoadMenuItems();

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
	//Save the initial working directory of the process as the preference directory
	preferenceDirectoryPath = PathGetCurrentWorkingDirectory();

	//Initialize the system prefs to their default values
	SetGlobalPreferencePathModules(L"Modules");
	SetGlobalPreferencePathSavestates(L"Savestates");
	SetGlobalPreferencePathPersistentState(L"PersistentState");
	SetGlobalPreferencePathWorkspaces(L"Workspaces");
	SetGlobalPreferencePathCaptures(L"Captures");
	SetGlobalPreferencePathAssemblies(L"Plugins");
	SetGlobalPreferenceEnableThrottling(true);
	SetGlobalPreferenceRunWhenProgramModuleLoaded(true);
	SetGlobalPreferenceEnablePersistentState(true);
	SetGlobalPreferenceLoadWorkspaceWithDebugState(true);
	SetGlobalPreferenceShowDebugConsole(false);

	//Load preferences from the settings.xml file if present
	std::wstring preferenceFilePath = PathCombinePaths(preferenceDirectoryPath, L"settings.xml");
	LoadPrefs(preferenceFilePath);

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
				//a separate thread so that view requests are still processed during the
				//operation, which is important in order to make sure views can actually
				//be loaded. Note that we specifically take a copy of the path string,
				//rather than pass it by reference.
				std::thread workerThread(std::bind(std::mem_fn(&ExodusInterface::LoadWorkspaceFromFile), this, prefs.loadWorkspace));
				workerThread.detach();
			}
		}
	}

	//Retrieve info on each connected joystick
	//##TODO## Either detect when joystick information has changed, or refresh the
	//joystick info at some point, probably each time the system is started. We've split
	//the retrieval of joystick info out of the joystick worker thread since it was shown
	//to be a very expensive process, but we now won't be able to pick up newly connected
	//joysticks until the program is restarted.
	UINT maxJoystickCount = joyGetNumDevs();
	for(unsigned int i = 0; i < maxJoystickCount; ++i)
	{
		//Obtain info on this joystick if present
		JOYCAPS joystickCapabilities;
		MMRESULT joyGetDevCapsReturn = joyGetDevCaps(i, &joystickCapabilities, sizeof(joystickCapabilities));
		if(joyGetDevCapsReturn != JOYERR_NOERROR)
		{
			continue;
		}

		//Save information on this joystick
		connectedJoystickInfo[i] = joystickCapabilities;
	}

	//Start the joystick worker thread if required
	if(!connectedJoystickInfo.empty())
	{
		std::unique_lock<std::mutex> lock(joystickWorkerThreadMutex);
		joystickWorkerThreadActive = true;
		std::thread workerThread(std::bind(std::mem_fn(&ExodusInterface::JoystickInputWorkerThread), this));
		workerThread.detach();
	}

	return true;
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ExodusInterface::GetIGUIExtensionInterfaceVersion() const
{
	return ThisIGUIExtensionInterfaceVersion();
}

//----------------------------------------------------------------------------------------
//View manager functions
//----------------------------------------------------------------------------------------
IViewManager& ExodusInterface::GetViewManager() const
{
	return *viewManager;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
AssemblyHandle ExodusInterface::GetAssemblyHandle() const
{
	//##TODO## Pass the module handle into the constructor
	return GetModuleHandle(NULL);
}

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
	std::list<unsigned int> loadedModuleIDs = system->GetLoadedModuleIDs();
	for(std::list<unsigned int>::const_iterator i = loadedModuleIDs.begin(); i != loadedModuleIDs.end(); ++i)
	{
		LoadedModuleInfo moduleInfo;
		if(system->GetLoadedModuleInfo(*i, moduleInfo))
		{
			std::wstring moduleIdentifierString = moduleInfo.GetSystemClassName() + L"." + moduleInfo.GetModuleClassName() + L"." + moduleInfo.GetModuleInstanceName();
			moduleIdentifierStrings.push_back(moduleIdentifierString);
			if(moduleInfo.GetIsProgramModule())
			{
				loadedProgramModuleInstanceNames.push_back(moduleInfo.GetModuleInstanceName());
			}
		}
	}

	//Obtain a list of all current connector mapping identifier strings
	std::list<std::wstring> connectorMappingStrings;
	std::list<unsigned int> connectorIDs = system->GetConnectorIDs();
	for(std::list<unsigned int>::const_iterator i = connectorIDs.begin(); i != connectorIDs.end(); ++i)
	{
		ConnectorInfo connectorInfo;
		if(system->GetConnectorInfo(*i, connectorInfo))
		{
			//If this connector is used, add information about the connector mapping to
			//the list of connector mappings.
			if(connectorInfo.GetIsConnectorUsed())
			{
				//Load information on the module which exports this connector
				LoadedModuleInfo exportingModuleInfo;
				if(!system->GetLoadedModuleInfo(connectorInfo.GetExportingModuleID(), exportingModuleInfo))
				{
					continue;
				}

				//Load information on the module which imports this connector
				LoadedModuleInfo importingModuleInfo;
				if(!system->GetLoadedModuleInfo(connectorInfo.GetImportingModuleID(), importingModuleInfo))
				{
					continue;
				}

				//Build a string describing the connector mapping
				std::wstring connectorMappingString = L"(" + importingModuleInfo.GetSystemClassName() + L"." + importingModuleInfo.GetModuleClassName() + L"." + importingModuleInfo.GetModuleInstanceName() + L"." + connectorInfo.GetImportingModuleConnectorInstanceName() + L"@" + exportingModuleInfo.GetSystemClassName() + L"." + exportingModuleInfo.GetModuleClassName() + L"." + exportingModuleInfo.GetModuleInstanceName() + L"." + connectorInfo.GetExportingModuleConnectorInstanceName() + L")";

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

	//Build a hash of the system identifier string. We encode this into the filename to try and
	//ensure that only savestates compatible with the current system configuration are offered to
	//the user. Note that our original code used boost::hash. We've eliminated boost from our
	//library dependencies at this time, and implemented the same logic as the boost hashing
	//algorithm used below.
	size_t systemIdentifierHashRaw = 0;
	for(std::wstring::size_type i = 0; i < systemIdentifierString.size(); ++i)
	{
		systemIdentifierHashRaw ^= (size_t)systemIdentifierString[i] + 0x9E3779B9 + (systemIdentifierHashRaw << 6) + (systemIdentifierHashRaw >> 2);
	}
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
	bool running = system->SystemRunning();
	system->StopSystem();

	//Load a state file
	std::wstring selectedFilePath;
	if(SelectExistingFile(L"Compressed savestate files|exs;Uncompressed savestate files|xml", L"exs", L"", folder, true, selectedFilePath))
	{
		//Determine the type of state file being loaded
		std::wstring fileExtension = PathGetFileExtension(selectedFilePath);
		ISystemGUIInterface::FileType fileType = ISystemGUIInterface::FileType::ZIP;
		if(fileExtension == L"xml")
		{
			fileType = ISystemGUIInterface::FileType::XML;
		}

		//Perform the state load operation
		LoadStateFromFile(selectedFilePath, fileType, debuggerState);
	}

	if(running)
	{
		system->RunSystem();
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::LoadStateFromFile(const std::wstring& filePath, ISystemGUIInterface::FileType fileType, bool debuggerState)
{
	system->LoadState(filePath, fileType, debuggerState);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SaveState(const std::wstring& folder, bool debuggerState)
{
	bool running = system->SystemRunning();
	system->StopSystem();

	//Save a state file
	std::wstring selectedFilePath;
	if(SelectNewFile(L"Compressed savestate files|exs;Uncompressed savestate files|xml", L"exs", L"", folder, selectedFilePath))
	{
		//Determine the type of state file being saved
		std::wstring fileExtension = PathGetFileExtension(selectedFilePath);
		ISystemGUIInterface::FileType fileType = ISystemGUIInterface::FileType::ZIP;
		if(fileExtension == L"xml")
		{
			fileType = ISystemGUIInterface::FileType::XML;
		}

		//Perform the state save operation
		SaveStateToFile(selectedFilePath, fileType, debuggerState);
	}

	if(running)
	{
		system->RunSystem();
	}
}

//----------------------------------------------------------------------------------------
void ExodusInterface::SaveStateToFile(const std::wstring& filePath, ISystemGUIInterface::FileType fileType, bool debuggerState)
{
	system->SaveState(filePath, fileType, debuggerState);
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
	LoadStateFromFile(filePath, ISystemGUIInterface::FileType::ZIP, debuggerState);
	if(debuggerState && prefs.loadWorkspaceWithDebugState)
	{
		std::wstring workspaceFileName = GetSavestateAutoFileNamePrefix() + L" - " + filePostfix + L" - DebugWorkspace" + L".xml";
		std::wstring workspaceFilePath = PathCombinePaths(prefs.pathSavestates, workspaceFileName);

		//Spawn a worker thread to handle loading of the workspace. We do this in a
		//separate thread so that view requests are still processed during the operation,
		//which is important in order to make sure views can actually be loaded. Note that
		//we specifically take a copy of the path string, rather than pass it by
		//reference.
		std::thread workerThread(std::bind(std::mem_fn(&ExodusInterface::LoadWorkspaceFromFile), this, workspaceFilePath));
		workerThread.detach();
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
	SaveStateToFile(filePath, ISystemGUIInterface::FileType::ZIP, debuggerState);
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
//Workspace functions
//----------------------------------------------------------------------------------------
void ExodusInterface::LoadWorkspace(const std::wstring& folder)
{
	//Load a state file
	std::wstring selectedFilePath;
	if(SelectExistingFile(L"Workspace Files|xml", L"xml", L"", folder, true, selectedFilePath))
	{
		//Spawn a worker thread to handle loading of the workspace. We do this in a
		//separate thread so that view requests are still processed during the operation,
		//which is important in order to make sure views can actually be loaded. Note that
		//we specifically take a copy of the path string, rather than pass it by
		//reference, since it's going to go out of scope when we leave this function.
		std::thread workerThread(std::bind(std::mem_fn(&ExodusInterface::LoadWorkspaceFromFile), this, selectedFilePath));
		workerThread.detach();
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
	file.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
	file.ProcessByteOrderMark();
	HierarchicalStorageTree tree;
	if(!tree.LoadTree(file))
	{
		return false;
	}

	//Validate the root node type of the XML structure
	IHierarchicalStorageNode& rootNode = tree.GetRootNode();
	if(rootNode.GetName() != L"Workspace")
	{
		return false;
	}

	//Attempt to retrieve the required child entries in this workspace state
	IHierarchicalStorageNode* moduleRelationshipsNode = rootNode.GetChild(L"ModuleRelationships");
	IHierarchicalStorageNode* viewLayoutNode = rootNode.GetChild(L"ViewLayout");
	if((moduleRelationshipsNode == 0) || (viewLayoutNode == 0))
	{
		return false;
	}

	//Load the ModuleRelationships node
	ISystemGUIInterface::ModuleRelationshipMap relationshipMap;
	if(!system->LoadModuleRelationshipsNode(*moduleRelationshipsNode, relationshipMap))
	{
		return false;
	}

	//Load the view layout
	if(!viewManager->LoadViewLayout(*viewLayoutNode, relationshipMap))
	{
		return false;
	}

	//Return the result to the caller
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
	IHierarchicalStorageNode& moduleRelationshipsNode = rootNode.CreateChild(L"ModuleRelationships");
	system->SaveModuleRelationshipsNode(moduleRelationshipsNode);

	//Save the view layout
	IHierarchicalStorageNode& viewLayoutNode = rootNode.CreateChild(L"ViewLayout");
	viewManager->SaveViewLayout(viewLayoutNode);

	//Save the workspace XML tree to the target workspace file
	Stream::File file(Stream::IStream::TextEncoding::UTF8);
	if(!file.Open(filePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
	{
		return false;
	}
	file.InsertByteOrderMark();
	tree.SaveTree(file);

	//Return the result to the caller
	return true;
}

//----------------------------------------------------------------------------------------
//Module functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::CanModuleBeLoaded(const MarshalSupport::Marshal::In<std::wstring>& filePath) const
{
	//Read the connector info for the module
	ISystemGUIInterface::ConnectorImportList connectorsImported;
	ISystemGUIInterface::ConnectorExportList connectorsExported;
	std::wstring systemClassName;
	if(!system->ReadModuleConnectorInfo(filePath, systemClassName, connectorsImported, connectorsExported))
	{
		return false;
	}

	//Ensure that all connectors required by this module are available
	ISystemGUIInterface::ConnectorMappingList connectorMappings;
	std::list<unsigned int> loadedConnectorIDList = system->GetConnectorIDs();
	for(ISystemGUIInterface::ConnectorImportList::const_iterator i = connectorsImported.begin(); i != connectorsImported.end(); ++i)
	{
		//Build a list of all available, loaded connectors which match the desired type.
		std::list<ConnectorInfo> availableConnectors;
		std::list<unsigned int>::const_iterator loadedConnectorID = loadedConnectorIDList.begin();
		while(loadedConnectorID != loadedConnectorIDList.end())
		{
			ConnectorInfo connectorInfo;
			if(system->GetConnectorInfo(*loadedConnectorID, connectorInfo))
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
bool ExodusInterface::LoadModuleFromFile(const MarshalSupport::Marshal::In<std::wstring>& filePath)
{
	//Pause view event processing while the system load is in progress. We do this to
	//ensure any views which are flagged to be displayed in the system definition don't
	//appear until after the load is complete.
	viewManager->PauseEventProcessing();

	//Read the connector info for the module
	ISystemGUIInterface::ConnectorImportList connectorsImported;
	ISystemGUIInterface::ConnectorExportList connectorsExported;
	std::wstring systemClassName;
	if(!system->ReadModuleConnectorInfo(filePath, systemClassName, connectorsImported, connectorsExported))
	{
		std::wstring text = L"Could not read connector info for module.";
		std::wstring title = L"Error loading module!";
		SafeMessageBox(mainWindowHandle, text, title, MB_ICONEXCLAMATION);

		viewManager->ResumeEventProcessing();
		return false;
	}

	//Map all imported connectors to available connectors in the system
	//##TODO## Cache this information in our module database when the modules are scanned,
	//so that we only have to access the module definition file once for a module load.
	ISystemGUIInterface::ConnectorMappingList connectorMappings;
	std::list<unsigned int> loadedConnectorIDList = system->GetConnectorIDs();
	for(ISystemGUIInterface::ConnectorImportList::const_iterator i = connectorsImported.begin(); i != connectorsImported.end(); ++i)
	{
		//Build a list of all available, loaded connectors which match the desired type.
		std::list<ConnectorInfo> availableConnectors;
		std::list<unsigned int>::const_iterator loadedConnectorID = loadedConnectorIDList.begin();
		while(loadedConnectorID != loadedConnectorIDList.end())
		{
			ConnectorInfo connectorInfo;
			if(system->GetConnectorInfo(*loadedConnectorID, connectorInfo))
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

			viewManager->ResumeEventProcessing();
			return false;
		}

		//Map the connector
		if(availableConnectors.size() == 1)
		{
			//If only one connector was found that matches the required type, map it.
			ConnectorInfo connector = *availableConnectors.begin();
			ISystemGUIInterface::ConnectorMapping connectorMapping;
			connectorMapping.connectorID = connector.GetConnectorID();
			connectorMapping.importingModuleConnectorInstanceName = i->instanceName;
			connectorMappings.push_back(connectorMapping);
		}
		else
		{
			//If multiple compatible connectors are available, open a dialog allowing the
			//user to select a connector from the list of available connectors.
			MapConnectorDialogParams mapConnectorDialogParams;
			mapConnectorDialogParams.system = system;
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

				viewManager->ResumeEventProcessing();
				return false;
			}

			//Map the selected connector
			ISystemGUIInterface::ConnectorMapping connectorMapping;
			connectorMapping.connectorID = mapConnectorDialogParams.selectedConnector.GetConnectorID();
			connectorMapping.importingModuleConnectorInstanceName = i->instanceName;
			connectorMappings.push_back(connectorMapping);
		}
	}

	//Begin the module load process
	system->LoadModuleSynchronous(filePath, connectorMappings);

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
			std::wstring text = L"An error occurred while building the system, settings, or debug windows for the system->";
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

	//Now that the operation is complete, resume view event processing.
	viewManager->ResumeEventProcessing();

	//Since the loaded system configuration has now changed, update any display info which
	//is dependent on the loaded modules.
	UpdateSaveSlots();

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
	return system->SaveSystem(filePath);
}

//----------------------------------------------------------------------------------------
void ExodusInterface::UnloadModule(unsigned int moduleID)
{
	//Spawn a worker thread to perform the module unload
	moduleCommandComplete = false;
	std::thread workerThread(std::bind(std::mem_fn(&ExodusInterface::UnloadModuleThread), this, moduleID));
	workerThread.detach();

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
}

//----------------------------------------------------------------------------------------
void ExodusInterface::UnloadAllModules()
{
	if(systemLoaded)
	{
		//Spawn a worker thread to perform the system unload
		moduleCommandComplete = false;
		std::thread workerThread(std::bind(std::mem_fn(&ExodusInterface::UnloadSystemThread), this));
		workerThread.detach();

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
	if(!file.Open(filePath, Stream::File::OpenMode::ReadOnly, Stream::File::CreateMode::Open))
	{
		return false;
	}

	//Attempt to decode the XML contents of the file
	file.SetTextEncoding(Stream::IStream::TextEncoding::UTF8);
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
			SetGlobalPreferencePathModules((*i)->GetData());
		}
		else if((*i)->GetName() == L"SavestatesPath")
		{
			SetGlobalPreferencePathSavestates((*i)->GetData());
		}
		else if((*i)->GetName() == L"PersistentStatePath")
		{
			SetGlobalPreferencePathPersistentState((*i)->GetData());
		}
		else if((*i)->GetName() == L"WorkspacesPath")
		{
			SetGlobalPreferencePathWorkspaces((*i)->GetData());
		}
		else if((*i)->GetName() == L"CapturesPath")
		{
			SetGlobalPreferencePathCaptures((*i)->GetData());
		}
		else if((*i)->GetName() == L"AssembliesPath")
		{
			SetGlobalPreferencePathAssemblies((*i)->GetData());
		}
		else if((*i)->GetName() == L"DefaultSystem")
		{
			SetGlobalPreferenceInitialSystem((*i)->GetData());
		}
		else if((*i)->GetName() == L"DefaultWorkspace")
		{
			SetGlobalPreferenceInitialWorkspace((*i)->GetData());
		}
		else if((*i)->GetName() == L"EnableThrottling")
		{
			SetGlobalPreferenceEnableThrottling((*i)->ExtractData<bool>());
		}
		else if((*i)->GetName() == L"RunWhenProgramModuleLoaded")
		{
			SetGlobalPreferenceRunWhenProgramModuleLoaded((*i)->ExtractData<bool>());
		}
		else if((*i)->GetName() == L"EnablePersistentState")
		{
			SetGlobalPreferenceEnablePersistentState((*i)->ExtractData<bool>());
		}
		else if((*i)->GetName() == L"LoadWorkspaceWithDebugState")
		{
			SetGlobalPreferenceLoadWorkspaceWithDebugState((*i)->ExtractData<bool>());
		}
		else if((*i)->GetName() == L"ShowDebugConsole")
		{
			SetGlobalPreferenceShowDebugConsole((*i)->ExtractData<bool>());
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
	rootNode.CreateChild(L"ModulesPath").SetData(PathRemoveBasePath(preferenceDirectoryPath, prefs.pathModules));
	rootNode.CreateChild(L"SavestatesPath").SetData(PathRemoveBasePath(preferenceDirectoryPath, prefs.pathSavestates));
	rootNode.CreateChild(L"PersistentStatePath").SetData(PathRemoveBasePath(preferenceDirectoryPath, prefs.pathPersistentState));
	rootNode.CreateChild(L"WorkspacesPath").SetData(PathRemoveBasePath(preferenceDirectoryPath, prefs.pathWorkspaces));
	rootNode.CreateChild(L"CapturesPath").SetData(PathRemoveBasePath(preferenceDirectoryPath, prefs.pathCaptures));
	rootNode.CreateChild(L"AssembliesPath").SetData(PathRemoveBasePath(preferenceDirectoryPath, prefs.pathAssemblies));
	rootNode.CreateChild(L"DefaultSystem").SetData(PathRemoveBasePath(prefs.pathModules, prefs.loadSystem));
	rootNode.CreateChild(L"DefaultWorkspace").SetData(PathRemoveBasePath(prefs.pathWorkspaces, prefs.loadWorkspace));
	rootNode.CreateChild(L"EnableThrottling").SetData(prefs.enableThrottling);
	rootNode.CreateChild(L"RunWhenProgramModuleLoaded").SetData(prefs.runWhenProgramModuleLoaded);
	rootNode.CreateChild(L"EnablePersistentState").SetData(prefs.enablePersistentState);
	rootNode.CreateChild(L"LoadWorkspaceWithDebugState").SetData(prefs.loadWorkspaceWithDebugState);
	rootNode.CreateChild(L"ShowDebugConsole").SetData(prefs.showDebugConsole);

	Stream::File file(Stream::IStream::TextEncoding::UTF8);
	if(file.Open(filePath, Stream::File::OpenMode::ReadAndWrite, Stream::File::CreateMode::Create))
	{
		file.InsertByteOrderMark();
		tree.SaveTree(file);
	}
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExodusInterface::GetPreferenceDirectoryPath() const
{
	return preferenceDirectoryPath;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExodusInterface::GetGlobalPreferencePathModules() const
{
	return prefs.pathModules;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExodusInterface::GetGlobalPreferencePathSavestates() const
{
	return prefs.pathSavestates;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExodusInterface::GetGlobalPreferencePathPersistentState() const
{
	return prefs.pathPersistentState;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExodusInterface::GetGlobalPreferencePathWorkspaces() const
{
	return prefs.pathWorkspaces;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExodusInterface::GetGlobalPreferencePathCaptures() const
{
	return prefs.pathCaptures;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExodusInterface::GetGlobalPreferencePathAssemblies() const
{
	return prefs.pathAssemblies;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExodusInterface::GetGlobalPreferenceInitialSystem() const
{
	return prefs.loadSystem;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ExodusInterface::GetGlobalPreferenceInitialWorkspace() const
{
	return prefs.loadWorkspace;
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

//----------------------------------------------------------------------------------------
//Assembly functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::LoadAssembliesFromFolder(const std::wstring& folderPath)
{
	//Begin the plugin load process
	loadPluginsComplete = false;
	loadPluginsProgress = 0;
	loadPluginsAborted = false;
	std::thread workerThread(std::bind(std::mem_fn(&ExodusInterface::LoadAssembliesFromFolderSynchronous), this, std::ref(folderPath)));
	workerThread.detach();

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
		std::unique_lock<std::mutex> lock(loadPluginsMutex);
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
bool ExodusInterface::LoadAssembly(const MarshalSupport::Marshal::In<std::wstring>& filePath)
{
	//Attempt to load the target assembly and retrieve information on its plugin interface
	PluginInfo pluginInfo;
	if(!LoadAssemblyInfo(filePath, pluginInfo))
	{
		return false;
	}

	//Register each device in the assembly
	bool result = true;
	if(pluginInfo.GetDeviceEntry != 0)
	{
		unsigned int entryNo = 0;
		DeviceInfo entry;
		while(pluginInfo.GetDeviceEntry(entryNo, entry))
		{
			//##DEBUG##
			std::wcout << L"Registering device \"" << entry.GetDeviceImplementationName() << L"\"...";

			if(!system->RegisterDevice(entry, pluginInfo.assemblyHandle))
			{
				//##DEBUG##
				std::wcout << L"Failed! Check event log.\n";

				result = false;
				++entryNo;
				continue;
			}

			//##DEBUG##
			std::wcout << L"Done.\n";

			std::unique_lock<std::mutex> lock(registeredElementMutex);
			RegisteredDeviceInfo registeredDeviceInfo;
			registeredDeviceInfo.assemblyPath = filePath;
			registeredDeviceInfo.info = entry;
			registeredDevices.push_back(registeredDeviceInfo);

			++entryNo;
		}
	}

	//Register each extension in the assembly
	if(pluginInfo.GetExtensionEntry != 0)
	{
		unsigned int entryNo = 0;
		ExtensionInfo entry;
		while(pluginInfo.GetExtensionEntry(entryNo, entry))
		{
			//##DEBUG##
			std::wcout << L"Registering extension \"" << entry.GetExtensionImplementationName() << L"\"...";

			if(!system->RegisterExtension(entry, pluginInfo.assemblyHandle))
			{
				//##DEBUG##
				std::wcout << L"Failed! Check event log.\n";

				result = false;
				++entryNo;
				continue;
			}

			//##DEBUG##
			std::wcout << L"Done.\n";

			std::unique_lock<std::mutex> lock(registeredElementMutex);
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
bool ExodusInterface::LoadAssemblyInfo(const std::wstring& filePath, PluginInfo& pluginInfo)
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
	unsigned int interfaceVersion = GetInterfaceVersion();
	if(interfaceVersion < EXODUS_INTERFACEVERSION)
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
	bool (*GetSystemEntry)(unsigned int entryNo, ISystemInfo& entry);
	GetDeviceEntry = (bool (*)(unsigned int entryNo, IDeviceInfo& entry))GetProcAddress(dllHandle, "GetDeviceEntry");
	GetExtensionEntry = (bool (*)(unsigned int entryNo, IExtensionInfo& entry))GetProcAddress(dllHandle, "GetExtensionEntry");
	GetSystemEntry = (bool (*)(unsigned int entryNo, ISystemInfo& entry))GetProcAddress(dllHandle, "GetSystemEntry");
	if((GetDeviceEntry == 0) && (GetExtensionEntry == 0) && (GetSystemEntry == 0))
	{
		//##DEBUG##
		std::wcout << "Error loading assembly " << filePath << "!\n"
			<< "The assembly appears to be a plugin, but is missing required exports!\n";
		FreeLibrary(dllHandle);
		return false;
	}

	//Return information on this plugin to the caller
	pluginInfo.assemblyHandle = (AssemblyHandle)dllHandle;
	pluginInfo.interfaceVersion = interfaceVersion;
	pluginInfo.GetDeviceEntry = GetDeviceEntry;
	pluginInfo.GetExtensionEntry = GetExtensionEntry;
	pluginInfo.GetSystemEntry = GetSystemEntry;
	return true;
}

//----------------------------------------------------------------------------------------
//File selection functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::SelectExistingFile(const MarshalSupport::Marshal::In<std::wstring>& selectionTypeString, const MarshalSupport::Marshal::In<std::wstring>& defaultExtension, const MarshalSupport::Marshal::In<std::wstring>& initialFilePath, const MarshalSupport::Marshal::In<std::wstring>& initialDirectory, bool scanIntoArchives, const MarshalSupport::Marshal::Out<std::wstring>& selectedFilePath) const
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
	std::wstring selectedFilePathTemp;
	bool fileSelected = false;
	bool fileSelectionAborted = false;
	while(!fileSelected && !fileSelectionAborted)
	{
		//Select a target file, using the first element in the supplied path as the
		//initial target.
		if(!::SelectExistingFile(mainWindowHandle, fullSelectionTypes, defaultExtension, initialPathFirstElement, initialDirectory, selectedFilePathTemp))
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
			std::wstring selectedFileExtension = PathGetFileExtension(selectedFilePathTemp);
			if(selectedFileExtension == L"zip")
			{
				//Attempt to find a compatible file in this archive. Note that we pass the
				//original selection types here, not the modified selection type list,
				//since we only want to match types that were originally specified.
				std::wstring relativePathWithinArchive;
				if(!SelectExistingFileScanIntoArchive(selectionTypes, selectedFilePathTemp, relativePathWithinArchive))
				{
					continue;
				}

				//Build the full path to the selected file within the archive
				selectedFilePathTemp = selectedFilePathTemp + L'|' + relativePathWithinArchive;
			}
		}

		//Flag that a file has been selected
		fileSelected = true;
	}
	selectedFilePath = selectedFilePathTemp;

	return fileSelected;
}

//----------------------------------------------------------------------------------------
bool ExodusInterface::SelectExistingFileScanIntoArchive(const std::list<FileSelectionType>& selectionTypes, const std::wstring archivePath, std::wstring& selectedFilePath) const
{
	//Open the target archive file
	Stream::File archiveFile;
	if(!archiveFile.Open(archivePath, Stream::File::OpenMode::ReadOnly, Stream::File::CreateMode::Open))
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
bool ExodusInterface::SelectNewFile(const MarshalSupport::Marshal::In<std::wstring>& selectionTypeString, const MarshalSupport::Marshal::In<std::wstring>& defaultExtension, const MarshalSupport::Marshal::In<std::wstring>& initialFilePath, const MarshalSupport::Marshal::In<std::wstring>& initialDirectory, const MarshalSupport::Marshal::Out<std::wstring>& selectedFilePath) const
{
	//Extract the first component from our combined path
	std::vector<std::wstring> initialPathElements = PathSplitElements(initialFilePath);
	std::wstring initialPathFirstElement = initialPathElements[0];

	//Select a target file, using the first element in the supplied path as the initial
	//target.
	std::wstring selectedFilePathTemp;
	bool result = ::SelectNewFile(mainWindowHandle, selectionTypeString, defaultExtension, initialPathFirstElement, initialDirectory, selectedFilePathTemp);
	selectedFilePath = selectedFilePathTemp;
	return result;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::vector<std::wstring>> ExodusInterface::PathSplitElements(const MarshalSupport::Marshal::In<std::wstring>& path) const
{
	//##TODO## Format and comment this
	std::wstring pathTemp = path;
	const std::wstring elementSeparators = L"|";
	std::vector<std::wstring> pathElements;
	std::wstring::size_type currentPos = 0;
	while(currentPos != std::wstring::npos)
	{
		std::wstring::size_type separatorPos = pathTemp.find_first_of(elementSeparators, currentPos);
		std::wstring::size_type pathElementEndPos = (separatorPos != std::wstring::npos)? separatorPos - currentPos: std::wstring::npos;
		std::wstring pathElement = pathTemp.substr(currentPos, pathElementEndPos);
		pathElements.push_back(pathElement);
		currentPos = (separatorPos != std::wstring::npos)? (separatorPos + 1): std::wstring::npos;
	}
	return pathElements;
}

//----------------------------------------------------------------------------------------
Stream::IStream* ExodusInterface::OpenExistingFileForRead(const MarshalSupport::Marshal::In<std::wstring>& path) const
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
			if(!file->Open(pathElements[i], Stream::File::OpenMode::ReadOnly, Stream::File::CreateMode::Open))
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
void ExodusInterface::DeleteFileStream(Stream::IStream* stream) const
{
	delete stream;
}

//----------------------------------------------------------------------------------------
//Device functions
//----------------------------------------------------------------------------------------
std::list<ExodusInterface::RegisteredDeviceInfo> ExodusInterface::GetRegisteredDevices() const
{
	std::unique_lock<std::mutex> lock(registeredElementMutex);
	return registeredDevices;
}

//----------------------------------------------------------------------------------------
//Extension functions
//----------------------------------------------------------------------------------------
std::list<ExodusInterface::RegisteredExtensionInfo> ExodusInterface::GetRegisteredExtensions() const
{
	std::unique_lock<std::mutex> lock(registeredElementMutex);
	return registeredExtensions;
}

//----------------------------------------------------------------------------------------
//Menu functions
//----------------------------------------------------------------------------------------
bool ExodusInterface::BuildMenuRecursive(HMENU parentMenu, IMenuItem& amenuItem, unsigned int& nextMenuID, int& insertPos, bool& leadingMenuItemsPresent, bool& trailingSeparatorPresent, bool& insertLeadingSeparatorBeforeNextItem)
{
	bool result = true;

	if(amenuItem.GetType() == IMenuItem::Type::Segment)
	{
		IMenuSegment& menuItem = *((IMenuSegment*)(&amenuItem));
		if(!menuItem.NoMenuItemsExist())
		{
			if(menuItem.GetSurroundWithSeparators() && leadingMenuItemsPresent && !trailingSeparatorPresent)
			{
				insertLeadingSeparatorBeforeNextItem = true;
			}

			std::list<IMenuItem*> childMenuItems = menuItem.GetSortedMenuItems();
			for(std::list<IMenuItem*>::const_iterator childMenuItemsIterator = childMenuItems.begin(); childMenuItemsIterator != childMenuItems.end(); ++childMenuItemsIterator)
			{
				result &= BuildMenuRecursive(parentMenu, *(*childMenuItemsIterator), nextMenuID, insertPos, leadingMenuItemsPresent, trailingSeparatorPresent, insertLeadingSeparatorBeforeNextItem);
			}

			if(menuItem.GetSurroundWithSeparators() && leadingMenuItemsPresent && !trailingSeparatorPresent)
			{
				insertLeadingSeparatorBeforeNextItem = true;
			}
		}
	}
	else if(amenuItem.GetType() == IMenuItem::Type::SubMenu)
	{
		IMenuSubmenu& menuItem = *((IMenuSubmenu*)(&amenuItem));
		if(!menuItem.NoMenuItemsExist())
		{
			if(insertLeadingSeparatorBeforeNextItem)
			{
				result &= InsertMenuItemSeparator(parentMenu, insertPos);
				trailingSeparatorPresent = true;
				insertLeadingSeparatorBeforeNextItem = false;
			}

			leadingMenuItemsPresent = true;

			HMENU subMenu = CreatePopupMenu();
			if(subMenu == NULL)
			{
				result = false;
			}
			else
			{
				std::wstring menuItemTitle = menuItem.GetMenuTitle();
				MENUITEMINFO menuItemInfo;
				menuItemInfo.cbSize = sizeof(MENUITEMINFO);
				menuItemInfo.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU;
				menuItemInfo.fType = MF_STRING;
				menuItemInfo.hSubMenu = subMenu;
				menuItemInfo.dwTypeData = &menuItemTitle[0];
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
					bool childMenuItemLeadingMenuItemsPresent = false;
					bool childMenuItemTrailingSeparatorPresent = false;
					bool childMenuItemInsertLeadingSeparatorBeforeNextItem = false;
					std::list<IMenuItem*> childMenuItems = menuItem.GetMenuItems();
					for(std::list<IMenuItem*>::iterator childMenuItem = childMenuItems.begin(); childMenuItem != childMenuItems.end(); ++childMenuItem)
					{
						result &= BuildMenuRecursive(subMenu, *(*childMenuItem), nextMenuID, childMenuItemInsertPos, childMenuItemLeadingMenuItemsPresent, childMenuItemTrailingSeparatorPresent, childMenuItemInsertLeadingSeparatorBeforeNextItem);
					}
				}
			}
		}
	}
	else if(amenuItem.GetType() == IMenuItem::Type::SelectableOption)
	{
		if(insertLeadingSeparatorBeforeNextItem)
		{
			result &= InsertMenuItemSeparator(parentMenu, insertPos);
			trailingSeparatorPresent = true;
			insertLeadingSeparatorBeforeNextItem = false;
		}

		leadingMenuItemsPresent = true;

		MenuSelectableOption& menuItem = *((MenuSelectableOption*)(&amenuItem));
		std::wstring menuItemTitle = menuItem.GetMenuTitle();
		MENUITEMINFO menuItemInfo;
		menuItemInfo.cbSize = sizeof(MENUITEMINFO);
		menuItemInfo.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING;
		menuItemInfo.fType = MF_STRING;
		menuItemInfo.fState = (menuItem.GetCheckedState()? MFS_CHECKED: 0);
		menuItemInfo.wID = nextMenuID;
		menuItemInfo.dwTypeData = &menuItemTitle[0];
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
bool ExodusInterface::InsertMenuItemSeparator(HMENU parentMenu, int& insertPos)
{
	bool result = true;

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
	fileSubmenu->DeleteAllMenuItems();

	//Add menu items to the submenu
	system->BuildFileOpenMenu(*fileSubmenu);

	//Build the actual menu using the menu structure
	int insertPos = 0;
	bool leadingMenuItemsPresent = false;
	bool trailingSeparatorPresent = false;
	bool insertLeadingSeparatorBeforeNextItem = false;
	std::list<IMenuItem*> menuItems = fileSubmenu->GetMenuItems();
	for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		result &= BuildMenuRecursive(fileMenu, *(*i), nextFreeMenuID, insertPos, leadingMenuItemsPresent, trailingSeparatorPresent, insertLeadingSeparatorBeforeNextItem);
	}

	//Ensure a trailing separator is added to the menu between any dynamic menu items and
	//our static menu items
	if(leadingMenuItemsPresent)
	{
		InsertMenuItemSeparator(fileMenu, insertPos);
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
	systemSubmenu->DeleteAllMenuItems();

	//Add menu items to the submenu
	system->BuildSystemMenu(*systemSubmenu);

	//Build the actual menu using the menu structure
	int insertPos = -1;
	bool leadingMenuItemsPresent = true;
	bool trailingSeparatorPresent = false;
	bool insertLeadingSeparatorBeforeNextItem = true;
	std::list<IMenuItem*> menuItems = systemSubmenu->GetMenuItems();
	for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		result &= BuildMenuRecursive(systemMenu, *(*i), nextFreeMenuID, insertPos, leadingMenuItemsPresent, trailingSeparatorPresent, insertLeadingSeparatorBeforeNextItem);
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
	settingsSubmenu->DeleteAllMenuItems();

	//Add menu items to the submenu
	system->BuildSettingsMenu(*settingsSubmenu);

	//Build the actual menu using the menu structure
	int insertPos = -1;
	bool leadingMenuItemsPresent = true;
	bool trailingSeparatorPresent = false;
	bool insertLeadingSeparatorBeforeNextItem = true;
	std::list<IMenuItem*> menuItems = settingsSubmenu->GetMenuItems();
	for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		result &= BuildMenuRecursive(settingsMenu, *(*i), nextFreeMenuID, insertPos, leadingMenuItemsPresent, trailingSeparatorPresent, insertLeadingSeparatorBeforeNextItem);
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
	debugSubmenu->DeleteAllMenuItems();

	//Add menu items to the submenu
	system->BuildDebugMenu(*debugSubmenu);

	//Build the actual menu using the menu structure
	int insertPos = -1;
	bool leadingMenuItemsPresent = false;
	bool trailingSeparatorPresent = false;
	bool insertLeadingSeparatorBeforeNextItem = false;
	std::list<IMenuItem*> menuItems = debugSubmenu->GetMenuItems();
	for(std::list<IMenuItem*>::iterator i = menuItems.begin(); i != menuItems.end(); ++i)
	{
		result &= BuildMenuRecursive(debugMenu, *(*i), nextFreeMenuID, insertPos, leadingMenuItemsPresent, trailingSeparatorPresent, insertLeadingSeparatorBeforeNextItem);
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
	system->UnloadModule(moduleID);

	moduleCommandComplete = true;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::UnloadSystemThread()
{
	if(systemLoaded)
	{
		//Clear all previously loaded system info
		system->UnloadAllModules();
	}

	systemLoaded = false;
	moduleCommandComplete = true;
}

//----------------------------------------------------------------------------------------
void ExodusInterface::DestroySystemInterfaceThread()
{
	//Stop the system if it is currently running
	system->StopSystem();

	//Explicitly close all currently open views
	viewManager->CloseAllViews();

	//Unload the system, to ensure all device menus are cleaned up.
	UnloadAllModules();

	//Save settings.xml
	std::wstring preferenceFilePath = PathCombinePaths(preferenceDirectoryPath, L"settings.xml");
	SavePrefs(preferenceFilePath);

	//Send a message to the main window, requesting it to close.
	SendMessage(mainWindowHandle, WM_USER+2, 0, 0);
}

//----------------------------------------------------------------------------------------
//Window callbacks
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
				text << moduleInfo.GetModuleInstanceName() << L"." << i->GetExportingModuleConnectorInstanceName();
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
			std::unique_lock<std::mutex> lock(state->loadPluginsMutex);
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
		if(state->system->LoadModuleSynchronousComplete())
		{
			INT_PTR result = 0;
			if(state->system->LoadModuleSynchronousResult())
			{
				result = 1;
			}
			else if(state->system->LoadModuleSynchronousAborted())
			{
				result = -1;
			}
			KillTimer(hwnd, 1);
			EndDialog(hwnd, result);
		}
		else
		{
			std::wstring currentModuleName = state->system->LoadModuleSynchronousCurrentModuleName();
			if(!currentModuleName.empty())
			{
				UpdateDlgItemString(hwnd, IDC_LOADMODULE_MODULETEXT, currentModuleName);
			}
			float progress = state->system->LoadModuleSynchronousProgress();
			int progressInt = (int)((progress * 100) + 0.5);
			SendMessage(GetDlgItem(hwnd, IDC_LOADMODULE_PROGRESS), PBM_SETPOS, (WPARAM)progressInt, 0);
		}
		break;}
	case WM_CLOSE:
		state->system->LoadModuleSynchronousAbort();
		break;
	case WM_COMMAND:
		if(LOWORD(wParam) == IDC_LOADMODULE_CANCEL)
		{
			state->system->LoadModuleSynchronousAbort();
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
			std::wstring currentModuleName = state->system->UnloadModuleSynchronousCurrentModuleName();
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
			state->system->StopSystem();
			break;
		case ID_FILE_RUNSYSTEM:
			state->system->RunSystem();
			break;
		case ID_FILE_HARDRESET:
			state->system->FlagInitialize();
			break;
		case ID_SYSTEM_TOGGLETHROTTLE:
			state->system->SetThrottlingState(!state->system->GetThrottlingState());
			break;
		case ID_FILE_LOADMODULE:
			state->LoadModule(state->prefs.pathModules);
			break;
		case ID_FILE_SAVESYSTEM:
			state->SaveSystem(state->prefs.pathModules);
			break;
		case ID_FILE_MANAGEMODULES:
			state->menuHandler->HandleMenuItemSelect(MenuHandler::MENUITEM_MODULEMANAGER);
			break;
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
		case ID_SETTINGS_PLATFORMSETTINGS:
			state->menuHandler->HandleMenuItemSelect(MenuHandler::MENUITEM_SETTINGS);
			break;
		case ID_HELP_ABOUT:
			state->menuHandler->HandleMenuItemSelect(MenuHandler::MENUITEM_ABOUT);
			break;
		case ID_SELECTWINDOW_REVERSE:{
			if(IsWindowVisible(state->windowSelectHandle) != 0)
			{
				state->selectedWindow = (state->selectedWindow == 0)? (unsigned int)(state->topLevelWindowList.size() - 1): state->selectedWindow - 1;
				InvalidateRect(state->windowSelectHandle, NULL, FALSE);
			}
			else
			{
				//Only show the window selection screen if more than one active child
				//window exists
				std::list<HWND> windowList = state->viewManager->GetOpenFloatingWindows();
				state->topLevelWindowList = std::vector<HWND>(windowList.begin(), windowList.end());
				if(state->topLevelWindowList.size() > 1)
				{
					state->selectedWindow = (unsigned int)state->topLevelWindowList.size() - 1;
					ShowWindow(state->windowSelectHandle, SW_SHOW);
					UpdateWindow(state->windowSelectHandle);
				}
			}
			break;}
		case ID_SELECTWINDOW:{
			if(IsWindowVisible(state->windowSelectHandle) != 0)
			{
				state->selectedWindow = (state->selectedWindow + 1) % (unsigned int)state->topLevelWindowList.size();
				InvalidateRect(state->windowSelectHandle, NULL, FALSE);
			}
			else
			{
				//Only show the window selection screen if more than one active child
				//window exists
				std::list<HWND> openViewList = state->viewManager->GetOpenFloatingWindows();
				state->topLevelWindowList = std::vector<HWND>(openViewList.begin(), openViewList.end());
				if(state->topLevelWindowList.size() > 1)
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
				newMenuItem->second.menuItem.GetMenuHandler().HandleMenuItemSelect(newMenuItem->second.menuItem.GetMenuItemID());
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

		//Retrieve the new position of the main window
		RECT windowRect;
		GetWindowRect(hwnd, &windowRect);
		int newWindowPosX = (int)(windowRect.left);
		int newWindowPosY = (int)(windowRect.top);

		//If we had a previous position of the main window captured, move each floating
		//window to keep the same relative position to the main window.
		if(state->mainWindowPosCaptured)
		{
			int windowMoveX = newWindowPosX - state->mainWindowPosX;
			int windowMoveY = newWindowPosY - state->mainWindowPosY;
			state->viewManager->AdjustFloatingWindowPositions(windowMoveX, windowMoveY);
		}

		//Record the new position of the main window
		state->mainWindowPosX = newWindowPosX;
		state->mainWindowPosY = newWindowPosY;
		state->mainWindowPosCaptured = true;

		break;}
	case WM_SIZE:
		//Resize the child docking window to match the new main window size
		if(state != 0)
		{
			SetWindowPos(state->mainDockingWindowHandle, NULL, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOREPOSITION);
		}
		break;
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
				std::thread workerThread(std::bind(std::mem_fn(&ExodusInterface::DestroySystemInterfaceThread), state));
				workerThread.detach();
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
		ISystemGUIInterface::StateInfo stateInfo = exodusInterface->system->GetStateInfo(saveFilePath, ISystemGUIInterface::FileType::ZIP);
		ISystemGUIInterface::StateInfo debuggerStateInfo = exodusInterface->system->GetStateInfo(debuggerSaveFilePath, ISystemGUIInterface::FileType::ZIP);
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
			if(source.Open(filePath, Stream::File::OpenMode::ReadOnly, Stream::File::CreateMode::Open))
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
			HWND windowHandle = state->topLevelWindowList[state->selectedWindow];
			ShowWindow(windowHandle, SW_SHOW);
			SetActiveWindow(windowHandle);
			SetForegroundWindow(windowHandle);
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
			int lastCellLastColumn = (int)(state->topLevelWindowList.size() - 1) % (int)state->windowSelectEntriesPerColumn;
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
			unsigned int windowEntries = (unsigned int)state->topLevelWindowList.size();
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
		unsigned int windowEntries = (unsigned int)state->topLevelWindowList.size();
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

			//Retrieve the title of the target window
			HWND windowHandle = state->topLevelWindowList[i];
			int windowTitleLength = GetWindowTextLength(windowHandle);
			std::vector<wchar_t> windowTitleBuffer(windowTitleLength + 1);
			if(GetWindowText(windowHandle, &windowTitleBuffer[0], (int)windowTitleBuffer.size()) == 0)
			{
				windowTitleBuffer[0] = L'\0';
			}
			std::wstring windowTitle = &windowTitleBuffer[0];

			//Write the name of the window this cell corresponds to
			RECT textRect;
			textRect.left = cellPosX + fontOffset;
			textRect.right = cellPosX + cellWidth;
			textRect.top = cellPosY;
			textRect.bottom = cellPosY + cellHeight;
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

	//Initialize the button and axis state for each joystick
	std::vector<std::vector<bool>> buttonState;
	std::vector<std::vector<float>> axisState;
	buttonState.resize(connectedJoystickInfo.size());
	axisState.resize(connectedJoystickInfo.size());
	for(unsigned int i = 0; i < connectedJoystickInfo.size(); ++i)
	{
		buttonState[i].resize(maxButtonCount);
		axisState[i].resize(maxAxisCount);
	}

	//Process input state changes from joysticks until we're requested to stop
	while(joystickWorkerThreadActive)
	{
		//Latch new values from each joystick
		for(std::map<unsigned int, JOYCAPS>::const_iterator connectedJoystickInfoIterator = connectedJoystickInfo.begin(); connectedJoystickInfoIterator != connectedJoystickInfo.end(); ++connectedJoystickInfoIterator)
		{
			unsigned int joystickNo = connectedJoystickInfoIterator->first;
			const JOYCAPS& joystickCapabilities = connectedJoystickInfoIterator->second;

			//Obtain info on the current button state for this joystick
			JOYINFOEX joystickInfo;
			joystickInfo.dwSize = sizeof(joystickInfo);
			joystickInfo.dwFlags = JOY_RETURNALL;
			MMRESULT joyGetPosExResult = joyGetPosEx(joystickNo, &joystickInfo);
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
				if(buttonState[joystickNo][buttonNo] == buttonStateNew)
				{
					continue;
				}

				//Latch the new button state
				buttonState[joystickNo][buttonNo] = buttonStateNew;

				//Notify the system of the button state change
				ISystemGUIInterface::KeyCode keyCode;
				if(system->TranslateJoystickButton(joystickNo, buttonNo, keyCode))
				{
					if(buttonStateNew)
					{
						system->HandleInputKeyDown(keyCode);
					}
					else
					{
						system->HandleInputKeyUp(keyCode);
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
				if(axisState[joystickNo][axisNo] == newAxisState[axisNo])
				{
					continue;
				}

				//Latch the new axis state
				float axisStateNew = newAxisState[axisNo];
				float axisStateOld = axisState[joystickNo][axisNo];
				axisState[joystickNo][axisNo] = axisStateNew;

				//Notify the system of axis state changes
				ISystemDeviceInterface::AxisCode axisCode;
				if(system->TranslateJoystickAxis(joystickNo, axisNo, axisCode))
				{
					system->HandleInputAxisUpdate(axisCode, axisStateNew);
				}

				//Notify the system of button state changes linked to this axis
				if((axisStateOld >= 0.25f) && (axisStateNew < 0.25f))
				{
					ISystemGUIInterface::KeyCode keyCode;
					if(system->TranslateJoystickAxisAsButton(joystickNo, axisNo, true, keyCode))
					{
						system->HandleInputKeyUp(keyCode);
					}
				}
				if((axisStateOld <= -0.25f) && (axisStateNew > -0.25f))
				{
					ISystemGUIInterface::KeyCode keyCode;
					if(system->TranslateJoystickAxisAsButton(joystickNo, axisNo, false, keyCode))
					{
						system->HandleInputKeyUp(keyCode);
					}
				}
				if((axisStateOld < 0.25f) && (axisStateNew >= 0.25f))
				{
					ISystemGUIInterface::KeyCode keyCode;
					if(system->TranslateJoystickAxisAsButton(joystickNo, axisNo, true, keyCode))
					{
						system->HandleInputKeyDown(keyCode);
					}
				}
				if((axisStateOld > -0.25f) && (axisStateNew <= -0.25f))
				{
					ISystemGUIInterface::KeyCode keyCode;
					if(system->TranslateJoystickAxisAsButton(joystickNo, axisNo, false, keyCode))
					{
						system->HandleInputKeyDown(keyCode);
					}
				}
			}
		}

		//Delay until it's time to latch the joystick input state again
		Sleep(10);
	}

	//Since this thread is terminating, notify any waiting threads.
	std::unique_lock<std::mutex> lock(joystickWorkerThreadMutex);
	joystickWorkerThreadStopped.notify_all();
}
