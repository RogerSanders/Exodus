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
#include <WindowFunctions/WindowFunctions.pkg>
#include "SystemInterface/SystemInterface.pkg"
#include "System.h"
#include "Image/Image.pkg"
#include "ThreadLib/ThreadLib.pkg"
#include "IViewModelManager.h"
#include "ViewModelNotifier.h"
#include <set>
#include <map>
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "MenuSubmenu.h"

class ExodusInterface :public IViewModelManager
{
public:
	//Constructors
	//##TODO## Implement a destructor, which cleans up our views.
	ExodusInterface(ISystemExternal& asystem);
	HWND CreateMainInterface(HINSTANCE hinstance);
	bool InitializeSystem();

	//Savestate functions
	void LoadState(const std::wstring& folder, bool debuggerState);
	void LoadStateFromFile(const std::wstring& fileDir, const std::wstring& fileName, System::FileType fileType, bool debuggerState);
	void SaveState(const std::wstring& folder, bool debuggerState);
	void SaveStateToFile(const std::wstring& fileDir, const std::wstring& fileName, System::FileType fileType, bool debuggerState);

	//Savestate quick-select popup functions
	void QuickLoadState(bool debuggerState);
	void QuickSaveState(bool debuggerState);
	void IncrementSaveSlot();
	void DecrementSaveSlot();
	void SelectSaveSlot(unsigned int slotNo);

	//Workspace functions
	void LoadWorkspace(const std::wstring& folder);
	bool LoadWorkspaceFromFile(const std::wstring& fileName);
	void SaveWorkspace(const std::wstring& folder);
	bool SaveWorkspaceToFile(const std::wstring& fileName);

	//Module functions
	bool LoadModule(const std::wstring& folder);
	bool LoadModuleFromFile(const std::wstring& fileDir, const std::wstring& fileName);
	bool SaveSystem(const std::wstring& folder);
	bool SaveSystemToFile(const std::wstring& fileDir, const std::wstring& fileName);
	void UnloadModule(unsigned int moduleID);
	void UnloadAllModules();

	//Global preference functions
	void LoadPrefs(const std::wstring& filePath);
	void SavePrefs(const std::wstring& filePath);
	void ResolvePrefs();

	//Assembly functions
	bool LoadAssembliesFromFolder(const std::wstring& folder);
	bool LoadAssembly(const std::wstring& filePath);

	//View management functions
	virtual bool OpenViewModel(IViewModel* aviewModel, bool waitToClose = true, bool openHidden = false);
	virtual void CloseViewModel(IViewModel* aviewModel, bool waitToClose = true);
	virtual void ShowViewModel(IViewModel* aviewModel);
	virtual void HideViewModel(IViewModel* aviewModel);
	virtual void ActivateViewModel(IViewModel* aviewModel);
	virtual bool WaitUntilViewModelOpened(IViewModel* aviewModel);
	virtual void WaitUntilViewModelClosed(IViewModel* aviewModel);
	virtual void NotifyModelViewClosed(IViewModel* aviewModel);

private:
	//Enumerations
	enum ViewOperationType;

	//Structures
	struct SystemPrefs
	{
		std::wstring pathModules;
		std::wstring pathModulesRaw;
		std::wstring pathSavestates;
		std::wstring pathSavestatesRaw;
		std::wstring pathDebugSessions;
		std::wstring pathDebugSessionsRaw;
		std::wstring pathWorkspaces;
		std::wstring pathWorkspacesRaw;
		std::wstring pathCaptures;
		std::wstring pathCapturesRaw;
		std::wstring pathAssemblies;
		std::wstring pathAssembliesRaw;
		std::wstring loadSystem;
		std::wstring loadSystemRaw;
		std::wstring loadWorkspace;
		std::wstring loadWorkspaceRaw;
		bool enableThrottling;
		bool runWhenProgramModuleLoaded;
	};
	struct NewMenuItem;
	struct ViewInfo;
	struct Region2D;
	struct SavestateCellWindowState;
	struct ViewOperation;
	struct WorkspaceViewEntryDetails;
	struct MapConnectorDialogParams;

	//Typedefs
	typedef std::map<unsigned int, NewMenuItem> NewMenuList;
	typedef std::pair<unsigned int, NewMenuItem> NewMenuListEntry;
	typedef std::map<IViewModel*, ViewInfo*> ViewInfoSet;
	typedef std::pair<IViewModel*, ViewInfo*> ViewInfoSetEntry;
	typedef std::map<IViewModel*, ViewInfo*> ViewModels;
	typedef std::pair<IViewModel*, ViewInfo*> ViewModelsEntry;
	typedef std::list<ViewOperation> ViewOperationQueue;

private:
	//Savestate functions
	std::wstring GetSavestateAutoFileNamePrefix() const;

	//Savestate quick-select popup functions
	void UpdateSaveSlots();

	//Module functions
	void UpdateModuleDisplayInfo() const;

	//View management functions
	void FlagProcessPendingEvents();
	void ProcessPendingEvents();
	void ProcessOpenView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessCloseView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessDeleteView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessActivateView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessShowView(IViewModel* viewModel, ViewInfo* viewInfo);
	void ProcessHideView(IViewModel* viewModel, ViewInfo* viewInfo);

	//View closing helper functions
	void CloseViewsForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName);
	void CloseViewsForModule(unsigned int moduleID, bool closeDependentModuleViews);
	void CloseViewsForSystem();
	void CloseAllViews();

	//Child window selection functions
	void BuildActiveWindowList();

	//Menu functions
	bool BuildMenuRecursive(HWND parentWindow, HMENU parentMenu, IMenuItem& amenuItem, unsigned int& nextMenuID);
	bool BuildDebugMenu(HMENU debugMenu);

	//Thread handlers
	void UnloadModuleThread(unsigned int moduleID);
	void UnloadSystemThread();
	void DestroySystemInterfaceThread();

	//Window auto-position functions
	void GetNewWindowPosition(int newWindowWidth, int newWindowHeight, int& newWindowPosX, int& newWindowPosY);
	bool IntersectRegion(const Region2D& existingRegion, const Region2D& regionToIntersect, std::list<Region2D>& newRegionsToCreate) const;
	bool PointWithinRegion(int posx, int posy, const Region2D& region) const;
	bool RegionIntersectsHorizontalLine(int posx, int posy, int width, const Region2D& region) const;
	bool RegionIntersectsVerticalLine(int posx, int posy, int height, const Region2D& region) const;

	//Window callbacks
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
	static int CALLBACK SetSHBrowseForFolderInitialDir(HWND hwnd, UINT umsg, LPARAM lparam, LPARAM lpData);
	static INT_PTR CALLBACK MapConnectorProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK LoadModuleProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK UnloadModuleProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK ModuleManagerProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK AboutProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK SettingsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndSavestateCellProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndSavestateProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndWindowSelectProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	ISystemExternal& system;
	HMENU debugMenu;
	HWND moduleManagerDialog;
	MenuSubmenu* debugSubmenu;
	NewMenuList newMenuList;
	unsigned int nextDebugMenuID;
	SystemPrefs prefs;
	std::wstring originalWorkingDir;
	volatile bool moduleCommandComplete;
	bool systemLoaded;
	bool systemDestructionInProgress;
	bool viewEventProcessingPaused;

	HWND mainWindowHandle;
	int mainWindowPosX;
	int mainWindowPosY;
	unsigned int childWindowXPosDefault;
	unsigned int childWindowYPosDefault;
	static const unsigned int childWindowXPosIncrement = 18;
	static const unsigned int childWindowYPosIncrement = 18;

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
	std::vector<IViewModel*> activeWindowList;
	unsigned int selectedWindow;
	unsigned int windowSelectColumns;
	unsigned int windowSelectEntriesPerColumn;

	mutable boost::mutex viewMutex;
	bool viewOperationPending;
	ViewOperationQueue viewOperationQueue;
	//##TODO## Unify these two containers. Using a simple boolean flag to indicate whether
	//the view is open would allow us to use a single container.
	ViewInfoSet viewInfoSet;
	ViewModels viewModels;
};

#include "ExodusInterface.inl"
#endif
