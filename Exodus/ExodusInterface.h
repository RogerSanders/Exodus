/*--------------------------------------------------------------------------------------*\
Things to do:
-Add a window management menu, where the list of loaded views can be enumerated and
managed.
-Add an auto-layout option for window control, which will use autopositioning to layout
all the existing views.
-Save the workspace client window size and maximize state in the workspace save file
-Add a "Size main window to fit dialogs" option, which will find the extents of the loaded
views, and fit the client frame around it.
-Add snapping when dragging windows, so that the windows will snap to corners and edges of
other loaded windows.
-Consider adding advanced logic when dragging client windows, so that when dragging a
window, it "moves" other windows out of the way, to allow it to be inserted between
windows. The user would see the windows move out of the way as the drag operation is being
performed, but the windows would return to their original position if the user continues
to move the window being dragged. Windows should "push" other windows as the window is
being dragged, so that if a window is being moved out of the way to the right, and there's
some space next to it, then another window to the right, the window being moved should
swallow up that space, then push the next window to the right across too, and so on. This
behaviour will make rearranging even very complex layouts much simpler.

Ideas for savestate selection window:
-The user should be able to click on a savestate slot to select it
-A rightclick context menu should be provided. The user should be able to delete
savestates from a slot through the menu, as well as access additional info about the
savestate.
-An interface needs to be provided which allows the "Comments" metadata to be viewed
and modified. I would suggest the user should be able to rightclick on a slot, and
select an option which will bring up a window where they can enter this information.
-Moving the mouse over the image in a slot which has a savestate loaded into it should
bring up a floating details window, which contains a larger version of the screenshot,
as well as the comments for the savestate, and any additional information we choose to
add later. This interface should be the same one which is displayed to edit the
comments.
-The user should be able to rearrange existing savestates by dragging them around in
the slots.
\*--------------------------------------------------------------------------------------*/
#ifndef __EXODUSINTERFACE_H__
#define __EXODUSINTERFACE_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include "SystemInterface/SystemInterface.pkg"
#include "Image/Image.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "IViewManagerNotifierInterface.h"
#include "ViewManager.h"
#include <set>
#include <map>
#include <list>
#include <mutex>
#include <condition_variable>
#include "MenuSubmenu.h"
#include "DeviceInfo.h"
#include "ExtensionInfo.h"
#include "SystemInfo.h"
class MenuHandler;

class ExodusInterface :public IGUIExtensionInterface
{
public:
	//Structures
	struct PluginInfo;
	struct RegisteredDeviceInfo;
	struct RegisteredExtensionInfo;

public:
	//Constructors
	//##TODO## Implement a destructor, which cleans up our views.
	ExodusInterface();
	virtual ~ExodusInterface();

	//System interface functions
	void BindToSystem(ISystemGUIInterface* asystem);
	void UnbindFromSystem();
	ISystemGUIInterface* GetSystemInterface() const;

	//Initialization functions
	HWND CreateMainInterface(HINSTANCE hinstance);
	bool InitializeSystem();

	//Interface version functions
	virtual unsigned int GetIGUIExtensionInterfaceVersion() const;

	//View manager functions
	virtual IViewManager& GetViewManager() const;
	HWND CreateDashboard(const std::wstring& dashboardTitle) const;

	//Window functions
	AssemblyHandle GetAssemblyHandle() const;
	virtual void* GetMainWindowHandle() const;
	HWND GetCurrentActiveDialogWindowHandle() const;

	//Savestate functions
	void LoadState(const std::wstring& folder, bool debuggerState);
	void LoadStateFromFile(const std::wstring& filePath, ISystemGUIInterface::FileType fileType, bool debuggerState);
	void SaveState(const std::wstring& folder, bool debuggerState);
	void SaveStateToFile(const std::wstring& filePath, ISystemGUIInterface::FileType fileType, bool debuggerState);

	//Savestate quick-select popup functions
	void QuickLoadState(bool debuggerState);
	void QuickSaveState(bool debuggerState);
	void IncrementSaveSlot();
	void DecrementSaveSlot();
	void SelectSaveSlot(unsigned int slotNo);

	//Workspace functions
	void LoadWorkspace(const std::wstring& folder);
	bool LoadWorkspaceFromFile(const std::wstring& filePath);
	void SaveWorkspace(const std::wstring& folder);
	bool SaveWorkspaceToFile(const std::wstring& filePath);

	//Module functions
	virtual bool CanModuleBeLoaded(const MarshalSupport::Marshal::In<std::wstring>& filePath) const;
	bool LoadModule(const std::wstring& folder);
	virtual bool LoadModuleFromFile(const MarshalSupport::Marshal::In<std::wstring>& filePath);
	bool SaveSystem(const std::wstring& folder);
	bool SaveSystemToFile(const std::wstring& filePath);
	virtual void UnloadModule(unsigned int moduleID);
	virtual void UnloadAllModules();

	//Global preference functions
	bool LoadPrefs(const std::wstring& filePath);
	void SavePrefs(const std::wstring& filePath);
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetPreferenceDirectoryPath() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetGlobalPreferencePathModules() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetGlobalPreferencePathSavestates() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetGlobalPreferencePathPersistentState() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetGlobalPreferencePathWorkspaces() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetGlobalPreferencePathCaptures() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetGlobalPreferencePathAssemblies() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetGlobalPreferenceInitialSystem() const;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetGlobalPreferenceInitialWorkspace() const;
	virtual bool GetGlobalPreferenceEnableThrottling() const;
	virtual bool GetGlobalPreferenceRunWhenProgramModuleLoaded() const;
	virtual bool GetGlobalPreferenceEnablePersistentState() const;
	virtual bool GetGlobalPreferenceLoadWorkspaceWithDebugState() const;
	virtual bool GetGlobalPreferenceShowDebugConsole() const;
	void SetGlobalPreferencePathModules(const std::wstring& state);
	void SetGlobalPreferencePathSavestates(const std::wstring& state);
	void SetGlobalPreferencePathPersistentState(const std::wstring& state);
	void SetGlobalPreferencePathWorkspaces(const std::wstring& state);
	void SetGlobalPreferencePathCaptures(const std::wstring& state);
	void SetGlobalPreferencePathAssemblies(const std::wstring& state);
	void SetGlobalPreferenceInitialSystem(const std::wstring& state);
	void SetGlobalPreferenceInitialWorkspace(const std::wstring& state);
	void SetGlobalPreferenceEnableThrottling(bool state);
	void SetGlobalPreferenceRunWhenProgramModuleLoaded(bool state);
	void SetGlobalPreferenceEnablePersistentState(bool state);
	void SetGlobalPreferenceLoadWorkspaceWithDebugState(bool state);
	void SetGlobalPreferenceShowDebugConsole(bool state);

	//Assembly functions
	bool LoadAssembliesFromFolder(const std::wstring& folder);
	bool LoadAssembliesFromFolderSynchronous(const std::wstring& folder);
	virtual bool LoadAssembly(const MarshalSupport::Marshal::In<std::wstring>& filePath);
	bool LoadAssemblyInfo(const std::wstring& filePath, PluginInfo& pluginInfo);

	//File selection functions
	virtual bool SelectExistingFile(const MarshalSupport::Marshal::In<std::wstring>& selectionTypeString, const MarshalSupport::Marshal::In<std::wstring>& defaultExtension, const MarshalSupport::Marshal::In<std::wstring>& initialFilePath, const MarshalSupport::Marshal::In<std::wstring>& initialDirectory, bool scanIntoArchives, const MarshalSupport::Marshal::Out<std::wstring>& selectedFilePath) const;
	virtual bool SelectNewFile(const MarshalSupport::Marshal::In<std::wstring>& selectionTypeString, const MarshalSupport::Marshal::In<std::wstring>& defaultExtension, const MarshalSupport::Marshal::In<std::wstring>& initialFilePath, const MarshalSupport::Marshal::In<std::wstring>& initialDirectory, const MarshalSupport::Marshal::Out<std::wstring>& selectedFilePath) const;
	virtual MarshalSupport::Marshal::Ret<std::vector<std::wstring>> PathSplitElements(const MarshalSupport::Marshal::In<std::wstring>& path) const;
	virtual Stream::IStream* OpenExistingFileForRead(const MarshalSupport::Marshal::In<std::wstring>& path) const;
	virtual void DeleteFileStream(Stream::IStream* stream) const;

	//Device functions
	std::list<RegisteredDeviceInfo> GetRegisteredDevices() const;

	//Extension functions
	std::list<RegisteredExtensionInfo> GetRegisteredExtensions() const;

private:
	//Structures
	struct SystemPrefs
	{
		std::wstring pathModules;
		std::wstring pathSavestates;
		std::wstring pathPersistentState;
		std::wstring pathWorkspaces;
		std::wstring pathCaptures;
		std::wstring pathAssemblies;
		std::wstring loadSystem;
		std::wstring loadWorkspace;
		bool enableThrottling;
		bool runWhenProgramModuleLoaded;
		bool enablePersistentState;
		bool loadWorkspaceWithDebugState;
		bool showDebugConsole;
	};
	struct NewMenuItem;
	struct SavestateCellWindowState;
	struct MapConnectorDialogParams;
	struct SelectCompressedFileDialogParams;
	struct SelectCompressedFileDialogParamsFileEntry;

	//Typedefs
	typedef std::map<unsigned int, NewMenuItem> NewMenuList;
	typedef std::pair<unsigned int, NewMenuItem> NewMenuListEntry;

private:
	//Savestate functions
	std::wstring GetSavestateAutoFileNamePrefix() const;

	//Savestate quick-select popup functions
	//##FIX## Replace this function with a loaded module callback notification, since
	//requests to load modules may not come through the GUI.
	void UpdateSaveSlots();

	//File selection functions
	bool SelectExistingFileScanIntoArchive(const std::list<FileSelectionType>& selectionTypes, const std::wstring archivePath, std::wstring& selectedFilePath) const;

	//Menu functions
	bool BuildMenuRecursive(HMENU parentMenu, IMenuItem& amenuItem, unsigned int& nextMenuID, int& insertPos, bool& leadingMenuItemsPresent, bool& trailingSeparatorPresent, bool& insertLeadingSeparatorBeforeNextItem);
	bool InsertMenuItemSeparator(HMENU parentMenu, int& insertPos);
	bool BuildFileMenu();
	bool BuildSystemMenu();
	bool BuildSettingsMenu();
	bool BuildDebugMenu();

	//Thread handlers
	void UnloadModuleThread(unsigned int moduleID);
	void UnloadSystemThread();
	void DestroySystemInterfaceThread();

	//Window callbacks
	static INT_PTR CALLBACK MapConnectorProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK LoadPluginProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK LoadModuleProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK UnloadModuleProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK SelectCompressedFileProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndSavestateCellProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndSavestateProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndWindowSelectProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//Joystick functions
	void JoystickInputWorkerThread();

private:
	ISystemGUIInterface* system;
	HMENU fileMenu;
	int fileMenuNonDynamicMenuItemCount;
	HMENU systemMenu;
	int systemMenuFirstItemIndex;
	HMENU settingsMenu;
	int settingsMenuFirstItemIndex;
	HMENU debugMenu;
	int debugMenuFirstItemIndex;
	MenuSubmenu* fileSubmenu;
	MenuSubmenu* systemSubmenu;
	MenuSubmenu* settingsSubmenu;
	MenuSubmenu* debugSubmenu;
	NewMenuList newMenuList;
	unsigned int nextFreeMenuID;
	SystemPrefs prefs;
	std::wstring preferenceDirectoryPath;
	volatile bool moduleCommandComplete;
	bool systemLoaded;
	bool systemDestructionInProgress;
	bool debugConsoleOpen;

	mutable std::mutex joystickWorkerThreadMutex;
	volatile bool joystickWorkerThreadActive;
	std::condition_variable joystickWorkerThreadStopped;
	std::map<unsigned int, JOYCAPS> connectedJoystickInfo;

	ViewManager* viewManager;

	mutable std::mutex registeredElementMutex;
	std::list<RegisteredDeviceInfo> registeredDevices;
	std::list<RegisteredExtensionInfo> registeredExtensions;

	HWND mainWindowHandle;
	HWND mainDockingWindowHandle;
	HFONT hfont;
	bool mainWindowPosCaptured;
	int mainWindowPosX;
	int mainWindowPosY;

	static const unsigned int cellCount = 10;
	std::vector<SavestateCellWindowState> cell;
	HWND savestatePopup;
	unsigned int selectedSaveCell;
	unsigned int savestatePopupTimeout;
	int savestateMonitorPosX;
	int savestateMonitorPosY;
	int savestateMonitorWidth;
	int savestateMonitorHeight;

	HWND windowSelectHandle;
	std::vector<HWND> topLevelWindowList;
	unsigned int selectedWindow;
	unsigned int windowSelectColumns;
	unsigned int windowSelectEntriesPerColumn;

	//Plugin load progress
	mutable std::mutex loadPluginsMutex;
	volatile float loadPluginsProgress;
	volatile bool loadPluginsComplete;
	volatile bool loadPluginsResult;
	volatile bool loadPluginsAborted;
	std::wstring loadPluginsCurrentPluginName;

	//Menu handling
	MenuHandler* menuHandler;
};

#include "ExodusInterface.inl"
#endif
