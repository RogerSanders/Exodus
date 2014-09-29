#ifndef __DOCKINGWINDOW_H__
#define __DOCKINGWINDOW_H__
#include "IDockingWindow.h"
#include <list>
#include <vector>
#include <map>

//##TODO## Add an option to create a named docking window, which has a specific given
//title that doesn't change, and only lists a single tab when in autohide mode.
//##TODO## Make the tab control for content windows optional, and support a kind of grid
//layout as an alternative which allows each content window to be positioned next to each
//other, with all windows visible at once. This would be most useful for allowing content
//windows to be "stacked" vertically or horizontally, to make groups of docked windows
//aligned to the edge of the main window which show each window on the same level, rather
//than having them nested within each other. We may only allow windows to be stacked
//either horizontally or vertically, but ideally we can position them freely in either
//direction, having one content window span the full height next to three other windows
//stacked on each other, for example.
//##TODO## More on the above, we could create a "free-form" docking window mode, where
//there is no content region, only docking windows, and we use a different kind of layout
//method instead of the nested dockpanel control. In this mode, all hosted windows must be
//contained within docking windows, and the free form docking window must be named. It
//will also be perfectly valid to have such a named free form docking window without any
//hosted content, and windows can simply be dragged in or out as desired. With a free form
//docking window, the docking window being dragged can potentially be placed anywhere
//relative to the currently present docked windows. If there are no hosted windows, it
//will simply take up all the available space in the window. If there is one window
//present, it can be placed to any side of the window. If there are two windows present,
//it can be placed to any side of either window, and so forth.
//##TODO## Thinking more about the free form docking window mode, what that really
//involves is dock panels within dock panels. Each window that is docked has a dock panel
//wrapped around it. The first added window will create the first dock panel. From there,
//each panel that is docked will in fact be docked within a docking region target from the
//docking panel of the first docked window. We'll implement this by making docked windows
//aware that their docking parent is in free form docking mode, and changing the behaviour
//of the docking placement targets so that rather than docking the dragged window inside
//the docking window that's being targeted, it will instead dock into the dock panel
//around it. The inner dock placement targets will simply add to the end of the dock panel
//list for that dock window as normal. The outer dock placement targets will add to the
//start of the dock panel list for the root docked element in the free form docking
//window, allowing docked windows to be pushed to the very outside edges of the free from
//dock window by dragging the window over any hosted docked window.
//##NOTE## There seems to be precedent for the free-form docking concept. The term "anchor
//docking" has been seen to describe a solution to this kind of docking framework, as
//opposed to "align docking", which is what we have with a dockpanel. See the following
//article:
//http://wiki.freepascal.org/Anchor_Docking
//Note that this article also highlights some cases where a dockpanel cannot produce a
//layout that's possible with free-form docking.
//##NOTE## It's not clear how the anchor docking system is implemented, but it seems that
//the splitters between windows are the "active" layout elements in this system, with the
//windows themselves being placed in the free space blocked out by the parent window and
//the splitter positions. Splitters must run between either a window edge or a splitter.
//You could work out this layout system by tracking the intersection targets at each end
//of each splitter, and processing them in a well defined order to number the free spaces
//that appear in the split window region. Windows are then associated with a numbered
//region. When doing a layout pass, the position and size of each numbered region is
//calculated, with the contained windows resized and repositioned as necessary if the
//region changes position or size. There is still a considerable amount of layout work to
//do when a new splitter is introduced, but it's a flat layout algorithm, with no window
//reparenting or creation/deletion necessary.
//##TODO## Consider eliminating the use of the common controls tab control for the content
//window tabs, and drawing the tabs ourselves. This will allow us to stack the tabs on
//multiple rows rather than have them go into an overflow area, but without changing the
//order of the tabs when they're selected like they do on the common tab control. It would
//also allow us to clip the text of the window title for each tab if there's not enough
//space to hold the full title of each one, and collapse the size of each tab down
//progressively until we reach a minimum size, like is done in tab-based web browsers. The
//behaviour of Internet Explorer is particularly nice, with scroll buttons appearing at
//each edge of the tab area. I would make the currently selected tab display at its full
//width if possible though, with tabs being shifted to the right to accommodate it, but
//the tab list never being scrolled to the left automatically. We could also add a close
//button on each tab itself if desired, like in web browsers.
class DockingWindow :public IDockingWindow
{
public:
	//Constants
	static const wchar_t* windowClassName;

	//Enumerations
	enum class WindowMessages;
	enum class ContentWindowInfo :unsigned int;
	enum class DockedWindowInfo :unsigned int;

	//Structures
	struct AddContentWindowParams;
	struct ModifyContentWindowParams;
	struct AddDockedWindowParams;
	struct GetContentWindowInfo;
	struct GetDockedWindowInfo;

public:
	//Constructors
	DockingWindow(HINSTANCE amoduleHandle, HWND ahwnd);
	virtual ~DockingWindow();

	//Class registration
	static bool RegisterWindowClass(HINSTANCE amoduleHandle);
	static bool UnregisterWindowClass(HINSTANCE amoduleHandle);

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	//Hosted content methods
	virtual void AddHostedContentInternal(HWND contentWindow, const InteropSupport::ISTLObjectSource<std::wstring>& contentTitleMarshaller);
	virtual void GetHostedContentTitleInternal(unsigned int contentEntryNo, const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Child container methods
	virtual void GetNestedChildDockingWindowListInternal(const InteropSupport::ISTLObjectTarget<std::list<IDockingWindow*>>& marshaller) const;

private:
	//Constants
	static const wchar_t* tabTrayWindowClassName;
	static const wchar_t* placementTargetWindowClassName;
	static const wchar_t* placementShadowWindowClassName;

	//Structures
	struct ChildContainerEntry;
	struct ContentEntry;
	struct AutoHideDockTab;
	struct AutoHideDockTabGroup;
	struct AutoHideDockInfo;
	struct TabRenderInfo;

private:
	//Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZING(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOVING(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ENTERSIZEMOVE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_EXITSIZEMOVE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_WINDOWPOSCHANGING(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NCHITTEST(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NCLBUTTONUP(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETCURSOR(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_GETFONT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFONT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_BOUNCE(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_ADDCONTENTWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_REMOVECONTENTWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_MODIFYCONTENTWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_ADDDOCKEDWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_REMOVEDOCKEDWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_GETCONTENTWINDOWINDEXFROMHANDLE(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_GETDOCKEDWINDOWINDEXFROMHANDLE(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_GETCONTENTWINDOWCOUNT(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_GETCONTENTWINDOWINFO(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_GETDOCKEDWINDOWCOUNT(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_GETDOCKEDWINDOWINFO(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_GETACTIVECONTENTWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCKWIN_SETACTIVECONTENTWINDOW(WPARAM wParam, LPARAM lParam);

	//Tab tray message handlers
	static LRESULT CALLBACK TabTrayWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT TabTrayWndProcPrivate(HWND tabTrayHwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgTabTrayWM_SIZE(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgTabTrayWM_TIMER(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgTabTrayWM_MOUSEMOVE(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgTabTrayWM_PAINT(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgTabTrayWM_PRINTCLIENT(HWND tabTrayHwnd, WPARAM wParam, LPARAM lParam);

	//Placement target message handlers
	static LRESULT CALLBACK PlacementTargetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT PlacementTargetWndProcPrivate(HWND placementTargetHwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgPlacementTargetWM_PAINT(HWND placementTargetHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgPlacementTargetWM_PRINTCLIENT(HWND placementTargetHwnd, WPARAM wParam, LPARAM lParam);

	//Placement shadow message handlers
	static LRESULT CALLBACK PlacementShadowWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT PlacementShadowWndProcPrivate(HWND placementShadowHwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgPlacementShadowWM_PAINT(HWND placementShadowHwnd, WPARAM wParam, LPARAM lParam);

	//Helper methods
	static IDockingWindow* GetDockingWindowFromHWND(HWND hwnd);
	static BOOL CALLBACK EnumDockingWindowsProc(HWND hwnd, LPARAM lParam);
	static void AddDockingWindowToWindowList(HWND hwnd, std::list<IDockingWindow*>& dockingWindowList);
	static HCURSOR LoadCursorFile(const unsigned char* cursorData, size_t cursorDataSize);

	//Window handle methods
	virtual HWND GetWindowHandle() const;
	virtual HWND GetDockPanelWindowHandle() const;

	//Placement content methods
	virtual void HidePlacementTargets();
	virtual void ShowPlacementTargets(bool allowCenterDocking);
	virtual void UpdatePlacementShadow(int cursorPosX, int cursorPosY, int dockWindowWidth, int dockWindowHeight);
	virtual bool HitTestPlacementTargets(int cursorPosX, int cursorPosY, bool& dockLocationIsContentRegion, WC_DockPanel::DockLocation& dockLocation, bool& forceTop) const;

	//Tab control update methods
	void HandleContentTabChanged(int newTabIndex);
	void TabHitTest(HWND tabTrayHwnd, int cursorPosX, int cursorPosY);
	virtual void AllDockTabsHitTest(int cursorPosX, int cursorPosY);
	void UpdateAutoHideWindowTabSizes();

	//Child container methods
	virtual void AddChildContainer(IDockingWindow* childContainer, WC_DockPanel::DockLocation dockLocation, bool autoHide, bool forceTop);
	virtual void RemoveChildContainer(IDockingWindow* childContainer);
	virtual void UpdateAutoHideChildContainerContent(IDockingWindow* childContainer);
	std::list<IDockingWindow*> GetNestedChildDockingWindowList() const;
	void ExpandHiddenDockPanel(IDockingWindow* targetDockedWindow, HWND targetContentWindow, int newWindowPosX, int newWindowPosY, int newWindowWidth, int newWindowHeight);
	void CollapseHiddenDockPanel();

	//Hosted content methods
	void AddHostedContent(HWND contentWindow, const std::wstring& contentTitle);
	virtual void RemoveHostedContent(HWND contentWindow);
	virtual unsigned int GetHostedContentCount() const;
	virtual unsigned int GetSortedContentEntryNo(unsigned int sortedContentEntryIndex) const;
	virtual bool GetHostedContentIndexFromWindow(HWND contentWindow, unsigned int& contentEntryNo) const;
	unsigned int GetActiveContent() const;
	virtual void SetActiveContent(unsigned int contentEntryNo);
	std::wstring GetHostedContentTitle(unsigned int contentEntryNo) const;
	void SetHostedContentTitle(unsigned int contentEntryNo, const std::wstring& newContentTitle);
	virtual HWND GetHostedContentWindow(unsigned int contentEntryNo) const;
	void SetHostedContentWindow(unsigned int contentEntryNo, HWND newContentWindow);

	//Parent docking window methods
	virtual IDockingWindow* GetParentDockingWindow() const;
	virtual void DockToNewParent(IDockingWindow* parentContainer, WC_DockPanel::DockLocation dockLocation, bool autoHide, bool forceTop);
	virtual void UndockFromParent();
	virtual WC_DockPanel::DockLocation GetDockLocation() const;
	virtual bool IsAutoHideEnabled() const;
	virtual void NotifyParentDestroyed();

	//Sizing methods
	void HandleSizeChanged(int newWidth, int newHeight);

private:
	//Window handles
	HINSTANCE moduleHandle;
	HWND hwnd;
	HWND tabDockPanel;
	HWND dockPanel;

	//Font info
	HFONT controlFont;
	HFONT controlFontVertical;
	int controlFontHeight;
	int controlFontDescent;

	//Parent docking window info
	IDockingWindow* parentDockingWindow;
	bool autoHideDockedWindow;
	WC_DockPanel::DockLocation currentDockLocation;

	//Child content info
	std::list<ChildContainerEntry> childContainers;
	std::vector<ContentEntry> hostedContent;
	std::map<int, size_t> tabIndexToHostedContentNo;
	HWND contentTabControl;
	int currentSelectedTabIndex;
	int dockingPanelTabMarginSize;
	int dockingPanelTabGroupSeparatorSize;

	//Autohide child containers
	std::map<WC_DockPanel::DockLocation, AutoHideDockInfo> autoHideDocks;
	std::map<WC_DockPanel::DockLocation, HCURSOR> enableAutoHideCursor;
	std::map<WC_DockPanel::DockLocation, HCURSOR> disableAutoHideCursor;
	bool autoHidePanelVisible;
	IDockingWindow* currentAutoHidePanel;
	HWND currentAutoHidePanelContent;
	int tabExtendedHitRegionX;
	int tabExtendedHitRegionY;
	int tabExtendedHitRegionWidth;
	int tabExtendedHitRegionHeight;

	//Drag-info
	bool windowDragInProgress;
	IDockingWindow* dockingWindowUnderDragPos;
	RECT windowSizeMoveInitialPos;
	int lastDragCursorPosX;
	int lastDragCursorPosY;
	bool leftMouseButtonDown;
	bool ignoreNextSizeAndMove;
	bool tabDragActive;
	bool tabDragIgnoreTabIndex;
	int tabDragIndexToIgnore;
	int dragTabIndex;
	int tabWithDeadZoneForDrag;
	int tabDeadZoneStartX;
	int tabDeadZoneEndX;

	//Placement target info
	std::map<WC_DockPanel::DockLocation, HWND> placementTargets;
	std::map<WC_DockPanel::DockLocation, HWND> placementTargetsForceTop;
	bool placementTargetCenterEnabled;
	HWND placementTargetCenter;
	bool placementShadowVisible;
	HWND placementShadow;
};

#include "DockingWindow.inl"
#endif
