#ifndef __VIEWMANAGER_H__
#define __VIEWMANAGER_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include "WindowsControls/WindowsControls.pkg"
#include "IViewManagerNotifierInterface.h"
#include "ViewStateChangeNotifier.h"
#include "SystemInterface/SystemInterface.pkg"
#include <functional>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <list>

class ViewManager :public IUIManager, public IViewManagerNotifierInterface
{
public:
	//Constructors
	ViewManager(HINSTANCE aviewManagerAssemblyHandle, HWND amainWindow, HWND amainDockingWindow, ISystemGUIInterface& asystem);
	virtual ~ViewManager();

	//Interface version functions
	virtual unsigned int GetIViewManagerVersion() const;
	virtual unsigned int GetIUIManagerVersion() const;

	//Event processing functions
	bool IsEventProcessingPaused() const;
	void PauseEventProcessing();
	void ResumeEventProcessing();

	//View management functions
	virtual bool OpenView(IViewPresenter& aviewPresenter, bool waitToClose = true);
	virtual bool OpenView(IViewPresenter& aviewPresenter, IHierarchicalStorageNode& viewState, bool waitToClose = true);
	virtual void CloseView(IViewPresenter& aviewPresenter, bool waitToClose = true);
	virtual void ShowView(IViewPresenter& aviewPresenter);
	virtual void HideView(IViewPresenter& aviewPresenter);
	virtual void ActivateView(IViewPresenter& aviewPresenter);
	virtual bool WaitUntilViewOpened(IViewPresenter& aviewPresenter);
	virtual void WaitUntilViewClosed(IViewPresenter& aviewPresenter);
	virtual void NotifyViewClosed(IViewPresenter& aviewPresenter);

	//Main window functions
	virtual HWND GetMainWindow() const;

	//Native window creation functions
	virtual HWND CreateDialogWindow(IView& view, IViewPresenter& viewPresenter, HINSTANCE assemblyHandle, DLGPROC windowProcedure, LPCWSTR templateName);
	virtual HWND CreateNativeWindow(IView& view, IViewPresenter& viewPresenter, HINSTANCE assemblyHandle, WNDPROC windowProcedure, DWORD windowStyle, DWORD extendedWindowStyle);

	//Window management functions
	virtual bool ShowWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const MarshalSupport::Marshal::In<std::wstring>& windowTitle, IHierarchicalStorageNode* windowState = 0);
	virtual void CloseWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle);
	virtual void ShowWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle);
	virtual void HideWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle);
	virtual void ActivateWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle);
	virtual void NotifyWindowDestroyed(IView& view, IViewPresenter& viewPresenter, HWND windowHandle);

	//Window state functions
	virtual bool LoadWindowState(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, IHierarchicalStorageNode& windowState);
	virtual bool SaveWindowState(const IView& view, const IViewPresenter& viewPresenter, HWND windowHandle, IHierarchicalStorageNode& windowState) const;

	//Window size functions
	virtual void ResizeWindowToTargetClientSize(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, unsigned int windowClientWidth, unsigned int windowClientHeight);

	//Window title functions
	virtual void UpdateWindowTitle(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const MarshalSupport::Marshal::In<std::wstring>& windowTitle);

	//Dialog management functions
	virtual void NotifyDialogActivated(HWND dialogWindow);
	virtual void NotifyDialogDeactivated(HWND dialogWindow);
	HWND GetActiveDialogWindow() const;

	//View closing helper functions
	void CloseViewsForSystem();
	void CloseViewsForModule(unsigned int moduleID);
	void CloseViewsForDevice(unsigned int moduleID, const std::wstring& deviceInstanceName);
	void CloseAllViews();

	//Floating window methods
	void AdjustFloatingWindowPositions(int displacementX, int displacementY);

	//Layout functions
	bool LoadViewLayout(IHierarchicalStorageNode& viewLayout, const ISystemGUIInterface::ModuleRelationshipMap& relationshipMap);
	bool SaveViewLayout(IHierarchicalStorageNode& viewLayout) const;

	//Child window selection functions
	std::list<HWND> GetOpenFloatingWindows() const;

private:
	//Constants
	static const int defaultWindowPosStackDepth = 10;
	static const wchar_t* dialogFrameWindowClassName;

	//Enumerations
	enum class ViewOperationType;

	//Structures
	struct ViewInfo;
	struct ViewOperation;
	struct PlaceholderWindowInfo;
	struct OpenWindowInfo;
	struct InvokeUIParams;
	struct Region2D;
	struct DialogWindowFrameState;

private:
	//Class registration
	static bool RegisterDialogFrameWindowClass(HINSTANCE amoduleHandle);
	static bool UnregisterDialogFrameWindowClass(HINSTANCE amoduleHandle);

	//Worker thread functions
	void WorkerThread();

	//View management functions
	bool OpenViewInternal(IViewPresenter& aviewPresenter, IHierarchicalStorageNode* viewState, bool waitToClose);
	void ProcessPendingEvents();
	void ProcessOpenView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo, IHierarchicalStorageNode* viewState);
	void ProcessCloseView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo);
	void ProcessDeleteView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo);
	void ProcessActivateView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo);
	void ProcessShowView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo);
	void ProcessHideView(IViewPresenter& aviewPresenter, ViewInfo* viewInfo);

	//Window management functions
	bool ShowDialogWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle);
	bool ShowDockingWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle);
	bool ShowDocumentWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle);
	HWND GetParentDockingWindow(HWND windowHandle) const;
	HWND GetParentDialogWindowFrame(HWND windowHandle) const;
	static bool IsDockingWindow(HWND windowHandle);
	static bool IsDashboardWindow(HWND windowHandle);
	static bool IsDialogFrame(HWND windowHandle);

	//Window state functions
	bool LoadWindowState(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, IHierarchicalStorageNode& windowState, bool showingForFirstTime, const std::wstring& initialWindowTitle = L"");

	//Window title functions
	std::wstring BuildQualifiedWindowTitle(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle) const;

	//Layout functions
	void LoadMainWindowStateFromViewLayout(IHierarchicalStorageNode& mainWindowState, std::map<unsigned int, PlaceholderWindowInfo>& placeholderWindows);
	HWND LoadDialogWindowFrameFromViewLayout(IHierarchicalStorageNode& dialogWindowState, std::map<unsigned int, PlaceholderWindowInfo>& placeholderWindows) const;
	HWND LoadDockingWindowFrameFromViewLayout(IHierarchicalStorageNode& dockingWindowState) const;
	HWND LoadDashboardWindowFrameFromViewLayout(IHierarchicalStorageNode& dashboardWindowState) const;
	void BindLoadedWindowFrameWithNoParent(HWND loadedWindow, IHierarchicalStorageNode& windowState) const;
	void BindLoadedWindowFrameWithDockingParent(HWND loadedWindow, IHierarchicalStorageNode& windowState, HWND parentDockingWindow) const;
	void BindLoadedWindowFrameWithDashboardParent(HWND loadedWindow, IHierarchicalStorageNode& windowState, HWND parentDockingWindow) const;
	void CreateDockingWindowChildrenFromViewLayout(HWND dockingWindow, IHierarchicalStorageNode& dockingWindowState, std::map<unsigned int, PlaceholderWindowInfo>& placeholderWindows) const;
	void CreateDashboardWindowChildrenFromViewLayout(HWND dashboardWindow, IHierarchicalStorageNode& dashboardWindowState, std::map<unsigned int, PlaceholderWindowInfo>& placeholderWindows) const;
	void SaveDialogWindowFrameStateToViewLayout(HWND dialogWindow, IHierarchicalStorageNode& dialogWindowState, std::map<HWND, unsigned int>& windowHandleToID, unsigned int& nextWindowID) const;
	void SaveDockingWindowFrameStateToViewLayout(HWND dockingWindow, IHierarchicalStorageNode& dockedWindowState, std::map<HWND, unsigned int>& windowHandleToID, unsigned int& nextWindowID) const;
	void SaveDashboardWindowFrameStateToViewLayout(HWND dashboardWindow, IHierarchicalStorageNode& dashboardWindowState, std::map<HWND, unsigned int>& windowHandleToID, unsigned int& nextWindowID) const;
	void BuildExistingWindowsToCloseList(std::list<HWND>& existingWindowsToClose) const;
	void CloseWindows(const std::list<HWND>& existingWindowsToClose) const;
	void DestroyUnusedPlaceholderWindows(const std::map<unsigned int, PlaceholderWindowInfo>& placeholderWindowInfo) const;
	void BuildCurrentlyOpenDashboardWindowList(std::list<HWND>& dashboardWindowList) const;
	static IDockingWindow::WindowEdge StringToDockingWindowEdge(const std::wstring& dockLocationAsString);
	static std::wstring DockingWindowEdgeToString(IDockingWindow::WindowEdge dockLocation);
	static IDockingWindow::WindowEdge ViewDockLocationToDockingWindowEdge(IView::DockPos viewDockLocation);
	static IView::ViewType StringToViewType(const std::wstring& viewTypeAsString);
	static std::wstring ViewTypeToString(IView::ViewType viewType);

	//Window auto-position functions
	bool FindBestWindowPosition(int newWindowWidth, int newWindowHeight, int& newWindowPosX, int& newWindowPosY) const;
	static bool IntersectRegion(const Region2D& existingRegion, const Region2D& regionToIntersect, std::list<Region2D>& newRegionsToCreate);
	static bool PointWithinRegion(int posx, int posy, const Region2D& region);
	static bool RegionIntersectsHorizontalLine(int posx, int posy, int width, const Region2D& region);
	static bool RegionIntersectsVerticalLine(int posx, int posy, int height, const Region2D& region);

	//Child window selection functions
	std::list<HWND> GetOpenFloatingDockingWindows() const;
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

	//Window procedures
	static LRESULT CALLBACK WndProcMessageWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcDialogWindowFrame(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//UI thread invocation
	void InvokeOnUIThread(const std::function<void()>& callback);

private:
	//Object interfaces
	ISystemGUIInterface& system;

	//Window handles
	HINSTANCE viewManagerAssemblyHandle;
	HWND mainWindow;
	HWND mainDockingWindow;
	HWND messageWindow;
	HWND activeDialogWindow;

	//UI thread invocation
	mutable std::mutex invokeMutex;
	unsigned long uithreadID;
	volatile bool pendingUIThreadInvoke;
	std::list<InvokeUIParams> pendingInvokeUIRequests;

	//View info
	mutable std::mutex viewMutex;
	bool eventProcessingPaused;
	std::list<ViewOperation> viewOperationQueue;
	std::map<IViewPresenter*, ViewInfo*> viewInfoSet;

	//Window info
	int defaultWindowPosX;
	int defaultWindowPosY;
	int defaultWindowPosIncrement;
	std::map<unsigned int, PlaceholderWindowInfo> placeholderWindowsForViewLayout;
	mutable std::map<HWND, unsigned int> windowHandleToIDForViewLayout;
	std::map<HWND, OpenWindowInfo> windowInfoSet;
};

#include "ViewManager.inl"
#endif
