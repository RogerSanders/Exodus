#ifndef __DASHBOARDWINDOW_H__
#define __DASHBOARDWINDOW_H__
#include "WindowsSupport/WindowsSupport.pkg"

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
class DashboardWindow
{
public:
	//Constants
	static const wchar_t* windowClassName;

public:
	//Constructors
	DashboardWindow(HINSTANCE amoduleHandle, HWND ahwnd);
	virtual ~DashboardWindow();

	//Class registration
	static bool RegisterWindowClass(HINSTANCE amoduleHandle);
	static bool UnregisterWindowClass(HINSTANCE amoduleHandle);

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//Enumerations
	enum InsertDirection;

	//Structures
	struct DividerContentEntry;
	struct Divider;
	struct ContentRegion;

private:
	//Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETCURSOR(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_GETFONT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFONT(WPARAM wParam, LPARAM lParam);

	//Cached data
	void UpdateCachedLocations();
	void UpdateCachedDividerLocation(int regionPosX, int regionPosY, const ContentRegion& precedingRegion, Divider& divider);

	//Region insertion functions
	void InsertRegion(ContentRegion& startRegion, InsertDirection insertDirection, HWND hostedWindow);

private:
	//Window handles
	HINSTANCE moduleHandle;
	HWND hwnd;

	//Font info
	HFONT controlFont;
	int controlFontHeight;

	//Cursor info
	HCURSOR cursorSizeHorizontal;
	HCURSOR cursorSizeVertical;
	HCURSOR cursorSizeAll;

	//Window metrics
	unsigned int dividerSize;
	unsigned int controlWidth;
	unsigned int controlHeight;

	//Region and divider info
	std::list<DividerContentEntry> topLevelDividersFromTop;
	std::list<DividerContentEntry> topLevelDividersFromLeft;
	std::list<ContentRegion*> regions;
	std::list<Divider*> dividers;
};

#include "DashboardWindow.inl"
#endif
