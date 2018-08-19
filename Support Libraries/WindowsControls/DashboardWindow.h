#ifndef __DASHBOARDWINDOW_H__
#define __DASHBOARDWINDOW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "Image/Image.pkg"
#include "Stream/Stream.pkg"
#include "IDockingWindow.h"
#include <list>
#include <set>
#include <map>

class DashboardWindow :public IDockingWindow
{
public:
	//Enumerations
	enum class WindowMessages :unsigned int;
	enum class InsertDirection;

	//Structures
	struct AddWindowParams;
	struct DividerListEntry;

	//Constants
	static const wchar_t* WindowClassName;

public:
	//Constructors
	DashboardWindow(HINSTANCE moduleHandle, HWND hwnd);
	virtual ~DashboardWindow();

	//Class registration
	static bool RegisterWindowClass(HINSTANCE moduleHandle);
	static bool UnregisterWindowClass(HINSTANCE moduleHandle);

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//Enumerations
	enum class DockTargetPos;

	//Structures
	struct DividerContentEntry;
	struct Divider;
	struct ContentRegion;
	struct DashboardWindowDropTargetInfo;
	struct DividerSplitPosition;
	struct DividerExtendButtonPosition;

	//Constants
	static const wchar_t* PlacementTargetWindowClassName;
	static const wchar_t* PlacementShadowWindowClassName;
	static const wchar_t* DividerHighlightWindowClassName;
	static const unsigned int DividerHighlightWindowCount = 4;

private:
	//Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZING(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_LBUTTONUP(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_LBUTTONDOWN(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSEMOVE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_MOUSELEAVE(WPARAM wParam, LPARAM lParam);
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
	LRESULT msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PRINTCLIENT(WPARAM wParam, LPARAM lParam);
	LRESULT msgDASHWIN_ADDCONTENTWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDASHWIN_REMOVECONTENTWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDASHWIN_GETREGIONCOUNT(WPARAM wParam, LPARAM lParam);
	LRESULT msgDASHWIN_GETREGIONWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDASHWIN_SETREGIONWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDASHWIN_REMOVEALLREGIONS(WPARAM wParam, LPARAM lParam);
	LRESULT msgDASHWIN_SAVELAYOUTTODIVIDERLIST(WPARAM wParam, LPARAM lParam);
	LRESULT msgDASHWIN_LOADLAYOUTFROMDIVIDERLIST(WPARAM wParam, LPARAM lParam);

	//Placement target message handlers
	static LRESULT CALLBACK PlacementTargetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT PlacementTargetWndProcPrivate(HWND placementTargetHwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgPlacementTargetWM_PAINT(HWND placementTargetHwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgPlacementTargetWM_PRINTCLIENT(HWND placementTargetHwnd, WPARAM wParam, LPARAM lParam);

	//Placement shadow message handlers
	static LRESULT CALLBACK PlacementShadowWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT PlacementShadowWndProcPrivate(HWND placementShadowHwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgPlacementShadowWM_PAINT(HWND placementShadowHwnd, WPARAM wParam, LPARAM lParam);

	//Divider highlight message handlers
	static LRESULT CALLBACK DividerHighlightWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DividerHighlightWndProcPrivate(HWND placementShadowHwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgDividerHighlightWM_PAINT(HWND placementShadowHwnd, WPARAM wParam, LPARAM lParam);

	//Helper methods
	static IDockingWindow* GetDockingWindowFromHWND(HWND hwnd);
	static BOOL CALLBACK EnumDockingWindowsProc(HWND hwnd, LPARAM lParam);
	static void AddDockingWindowToWindowList(HWND hwnd, std::list<IDockingWindow*>& dockingWindowList);

	//Window handle methods
	virtual HWND GetWindowHandle() const;

	//Placement content methods
	virtual void HideDropTargets(IDockingWindow* callingDockingWindow);
	void HideDropTargets(IDockingWindow* callingDockingWindow, bool hideChildWindowDropTargets);
	virtual void ShowDropTargets(IDockingWindow* callingDockingWindow, int dockWindowWidth, int dockWindowHeight, int cursorPosX, int cursorPosY);
	virtual bool HitTestDropTargets(IDockingWindow* callingDockingWindow, int cursorPosX, int cursorPosY, IDockingWindowDropTargetInfo*& dropTargetInfo) const;

	//Child container methods
	virtual void AddChildContainer(IDockingWindow* childContainer, const IDockingWindowDropTargetInfo* dropTargetInfo);
	virtual void RemoveChildContainer(IDockingWindow* childContainer);
	virtual void NotifyChildContainerContentChanged(IDockingWindow* childContainer);
	virtual bool HasNestedChildDockingWindows() const;
	virtual Marshal::Ret<std::list<IDockingWindow*>> GetNestedChildDockingWindowList() const;
	virtual bool CanResizeChildContainerWindowEdge(IDockingWindow* childContainer, WindowEdge windowEdge) const;
	virtual void UpdateDesiredChildContainerSize(IDockingWindow* childContainer, int desiredWidth, int desiredHeight);
	virtual HCURSOR ParentOverrideCursorForChildContainer(IDockingWindow* childContainer, int cursorPosX, int cursorPosY) const;
	virtual bool ParentBorderClickForChildContainer(IDockingWindow* childContainer, int cursorPosX, int cursorPosY);

	//Hosted content methods
	//##FIX##
	virtual void RemoveHostedContent(HWND contentWindow) { }
	virtual unsigned int GetHostedContentCount() const { return 1; }
	virtual unsigned int GetSortedContentEntryNo(unsigned int sortedContentEntryIndex) const { return 0; }
	virtual bool GetHostedContentIndexFromWindow(HWND contentWindow, unsigned int& contentEntryNo) const { contentEntryNo = 0; return true; }
	virtual void SetActiveContent(unsigned int contentEntryNo) { }
	virtual Marshal::Ret<std::wstring> GetHostedContentTitle(unsigned int contentEntryNo) const { return GetWindowText(_hwnd); }
	virtual HWND GetHostedContentWindow(unsigned int contentEntryNo) const { return _hwnd; }

	//Parent docking window methods
	virtual IDockingWindow* GetParentDockingWindow() const;
	virtual void NotifyAddedToParent(IDockingWindow* newParentDockingWindow);
	virtual void NotifyRemovedFromParent();

	//Cached data
	void UpdateCachedLocations(bool deferWindowUpdate = false);
	void ApplyCachedLocations();
	void UpdateCachedDividerLocation(int regionPosX, int regionPosY, const ContentRegion& precedingRegion, Divider& divider);

	//Region functions
	void InsertRegion(ContentRegion& existingRegion, InsertDirection insertDirection, HWND hostedWindow, double regionProportion);
	Divider* InsertRegion(ContentRegion& existingRegion, InsertDirection insertDirection, HWND hostedWindow, int distanceAlongRegion);
	void RemoveRegion(ContentRegion& existingRegion);
	void RemoveAllRegions();
	void UpdateRegionContent(ContentRegion& existingRegion, HWND newHostedWindow);
	ContentRegion* GetRegionAtPosition(int posX, int posY) const;
	void BuildSortedRegionList(std::list<ContentRegion*>& sortedRegionList) const;
	void BuildSortedRegionList(const std::list<DividerContentEntry>& targetContentList, std::set<ContentRegion*>& processedRegions, std::list<ContentRegion*>& sortedRegionList) const;

	//Divider functions
	std::set<Divider*> GetMergeCandidates(Divider* targetDivider, bool getPartialOverlappingMatches) const;
	std::set<Divider*> GetAllMergeCandidates(Divider* targetDivider) const;
	void GetAllMergeCandidatesInternal(Divider* targetDivider, std::set<Divider*>& resultSet) const;
	std::list<DividerSplitPosition> GetSplitPositionsForDivider(Divider* targetDivider) const;
	std::list<DividerExtendButtonPosition> GetExtendButtonPositionsForDivider(Divider* targetDivider) const;
	Divider* SwapContinuingDividerAtJunction(Divider* firstDividerToMerge, Divider* secondDividerToMerge, Divider* dividerToSplit);
	void ExtendDivider(Divider* targetDivider, bool extendFromStart, bool extendPrecedingContent);

	//Sizing methods
	void HandleSizeChanged(int newWidth, int newHeight);
	std::list<Divider*> GetDividersAtPosition(int posX, int posY) const;

	//Layout functions
	void SaveLayoutInfo(std::list<DividerListEntry>& dividerList) const;
	void LoadLayoutInfo(const std::list<DividerListEntry>& dividerList);

	//Canvas helper functions
	//##TODO## Move these into a proper canvas class once we've implemented it, and modify
	//our code to use that class.
	static void DrawImageLine(IImage& image, unsigned int startPosX, unsigned int startPosY, unsigned int endPosX, unsigned int endPosY, const std::map<int, unsigned char>& penColorComponents);
	static void DrawImageSquare(IImage& image, unsigned int startPosX, unsigned int startPosY, unsigned int width, unsigned int height, const std::map<int, unsigned char>& penColorComponents);
	//##TODO## Consider shifting this into the Image class, once the future of the image
	//load routines and the image class itself is determined.
	static HBITMAP ImageToLoadedDIB(IImage& image, HDC deviceContext);

private:
	//Window handles
	HINSTANCE _moduleHandle;
	HWND _hwnd;

	//Font info
	HFONT _controlFont;
	int _controlFontHeight;

	//Cursor info
	HCURSOR _cursorSizeHorizontal;
	HCURSOR _cursorSizeVertical;
	HCURSOR _cursorSizeAll;

	//Window metrics
	int _dividerSizeX;
	int _dividerSizeY;
	int _controlWidth;
	int _controlHeight;

	//Parent docking window info
	IDockingWindow* _parentDockingWindow;

	//Drag-info
	bool _windowDragInProgress;
	IDockingWindow* _dockingWindowUnderDragPos;
	RECT _windowSizeMoveInitialPos;
	int _lastDragCursorPosX;
	int _lastDragCursorPosY;
	bool _leftMouseButtonDown;
	bool _ignoreNextSizeAndMove;

	//Region and divider info
	std::list<DividerContentEntry> _topLevelDividersFromTop;
	std::list<DividerContentEntry> _topLevelDividersFromLeft;
	std::list<ContentRegion*> _regions;
	std::list<Divider*> _dividers;

	//Divider drag info
	bool _dividerDragActive;
	int _dragLastPosX;
	int _dragLastPosY;
	int _dragPosOverLimitX;
	int _dragPosOverLimitY;
	int _dragPosSnapDisplacementX;
	int _dragPosSnapDisplacementY;
	std::set<Divider*> _verticalDividersBeingDragged;
	std::set<Divider*> _horizontalDividersBeingDragged;

	//Drop target info
	std::map<DockTargetPos, HWND> _dropTargets;
	IDockingWindow* _dropTargetsChildDockingWindow;
	bool _dropTargetsVisible;
	ContentRegion* _currentRegionDropTarget;
	bool _dropShadowVisible;
	DockTargetPos _dropShadowCurrentPos;
	HWND _dropShadow;

	//Divider highlight info
	unsigned int _dividerHighlightVisibleWindowCount;
	HWND _dividerHighlightWindows[DividerHighlightWindowCount];

	//Render info
	WinColor _dividerBackgroundColor;
	Image _grabberImage;
	Image _arrowImageTopLeft;
	Image _arrowImageTopRight;
	Image _arrowImageBottomLeft;
	Image _arrowImageBottomRight;
};

#include "DashboardWindow.inl"
#endif
