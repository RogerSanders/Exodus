#ifndef __IDOCKINGWINDOW_H__
#define __IDOCKINGWINDOW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "InteropSupport/InteropSupport.pkg"
#include "WC_DockPanel.h"

//##TODO## Make this base interface common for docking windows and dashboard windows, so
//they can be docked within each other. This is achievable without too much effort.
class IDockingWindow
{
public:
	//Constructors
	virtual ~IDockingWindow() = 0 {}

	//Window handle methods
	virtual HWND GetWindowHandle() const = 0;
	//##FIX## Eliminate this method, as it isn't compatible with a dashboard parent.
	virtual HWND GetDockPanelWindowHandle() const = 0;

	//Placement content methods
	virtual void HidePlacementTargets() = 0;
	virtual void ShowPlacementTargets(bool allowCenterDocking) = 0;
	virtual void UpdatePlacementShadow(int cursorPosX, int cursorPosY, int dockWindowWidth, int dockWindowHeight) = 0;
	//##FIX## This method isn't very extensible or compatible with a dashboard parent.
	//Consider an alternate method and use of this method which will allow a dashboard
	//window to return information on how and where to dock the target window when it is
	//dropped at the given location. Note that as part of this, the target window will
	//need to completely decide what happens to the dropped window when the event is
	//processed.
	virtual bool HitTestPlacementTargets(int cursorPosX, int cursorPosY, bool& dockLocationIsContentRegion, WC_DockPanel::DockLocation& dockLocation, bool& forceTop) const = 0;

	//Tab control update methods
	virtual void AllDockTabsHitTest(int cursorPosX, int cursorPosY) = 0;

	//Child container methods
	//##FIX## Replace this method with something else which doesn't require the container
	//being docked to be aware of how the new parent docking window performs docking. This
	//only requires us to replace the HitTestPlacementTargets method above with something
	//else, provide a method for a child to request its parent container explicitly to
	//toggle auto-hide mode rather than undocking and redocking it, and the
	//DOCKWIN_ADDDOCKEDWINDOW message to get the target window to add the child window,
	//rather than the child request it be added to the parent.
	virtual void AddChildContainer(IDockingWindow* childContainer, WC_DockPanel::DockLocation dockLocation, bool autoHide, bool forceTop) = 0;
	virtual void RemoveChildContainer(IDockingWindow* childContainer) = 0;
	virtual void UpdateAutoHideChildContainerContent(IDockingWindow* childContainer) = 0;
	inline std::list<IDockingWindow*> GetNestedChildDockingWindowList() const;

	//Hosted content methods
	//##FIX## Remove this method. This only requires changes to HitTestPlacementTargets.
	inline void AddHostedContent(HWND contentWindow, const std::wstring& contentTitle);
	//##FIX## Remove this method. This won't require any logic changes.
	virtual void RemoveHostedContent(HWND contentWindow) = 0;
	virtual unsigned int GetHostedContentCount() const = 0;
	virtual unsigned int GetSortedContentEntryNo(unsigned int sortedContentEntryIndex) const = 0;
	virtual bool GetHostedContentIndexFromWindow(HWND contentWindow, unsigned int& contentEntryNo) const = 0;
	virtual void SetActiveContent(unsigned int contentEntryNo) = 0;
	inline std::wstring GetHostedContentTitle(unsigned int contentEntryNo) const;
	virtual HWND GetHostedContentWindow(unsigned int contentEntryNo) const = 0;

	//Parent docking window methods
	virtual IDockingWindow* GetParentDockingWindow() const = 0;
	//##FIX## Remove this method. This will only require changes to the
	//WindowMessages::DOCKWIN_ADDDOCKEDWINDOW message as above.
	virtual void DockToNewParent(IDockingWindow* parentContainer, WC_DockPanel::DockLocation dockLocation, bool autoHide, bool forceTop) = 0;
	//##FIX## Remove this method. Won't require any additional work.
	virtual void UndockFromParent() = 0;
	//##FIX## Remove this method. Will only require a parent docking window to retrieve
	//the known child dock location itself, rather than asking the child container for it.
	virtual WC_DockPanel::DockLocation GetDockLocation() const = 0;
	//##FIX## Remove this method. Only requires that we remember the auto hide state for
	//our own child windows.
	virtual bool IsAutoHideEnabled() const = 0;
	virtual void NotifyParentDestroyed() = 0;

protected:
	//Hosted content methods
	virtual void AddHostedContentInternal(HWND contentWindow, const InteropSupport::ISTLObjectSource<std::wstring>& contentTitleMarshaller) = 0;
	virtual void GetHostedContentTitleInternal(unsigned int contentEntryNo, const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const = 0;

	//Child container methods
	virtual void GetNestedChildDockingWindowListInternal(const InteropSupport::ISTLObjectTarget<std::list<IDockingWindow*>>& marshaller) const = 0;
};

#include "IDockingWindow.inl"
#endif
