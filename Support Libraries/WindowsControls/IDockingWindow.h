#ifndef __IDOCKINGWINDOW_H__
#define __IDOCKINGWINDOW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "MarshalSupport/MarshalSupport.pkg"
#include "IDockingWindowDropTargetInfo.h"

class IDockingWindow
{
public:
	//Enumerations
	enum class WindowEdge;

public:
	//Constructors
	virtual ~IDockingWindow() = 0 {}

	//Window handle methods
	virtual HWND GetWindowHandle() const = 0;

	//Placement content methods
	virtual void HideDropTargets(IDockingWindow* callingDockingWindow) = 0;
	virtual void ShowDropTargets(IDockingWindow* callingDockingWindow, int dockWindowWidth, int dockWindowHeight, int cursorPosX, int cursorPosY) = 0;
	virtual bool HitTestDropTargets(IDockingWindow* callingDockingWindow, int cursorPosX, int cursorPosY, IDockingWindowDropTargetInfo*& dropTargetInfo) const = 0;

	//Child container methods
	virtual void AddChildContainer(IDockingWindow* childContainer, const IDockingWindowDropTargetInfo* dropTargetInfo) = 0;
	virtual void RemoveChildContainer(IDockingWindow* childContainer) = 0;
	virtual void NotifyChildContainerContentChanged(IDockingWindow* childContainer) = 0;
	virtual bool HasNestedChildDockingWindows() const = 0;
	virtual MarshalSupport::Marshal::Ret<std::list<IDockingWindow*>> GetNestedChildDockingWindowList() const = 0;
	virtual bool CanResizeChildContainerWindowEdge(IDockingWindow* childContainer, WindowEdge windowEdge) const = 0;
	virtual void UpdateDesiredChildContainerSize(IDockingWindow* childContainer, int desiredWidth, int desiredHeight) = 0;
	virtual HCURSOR ParentOverrideCursorForChildContainer(IDockingWindow* childContainer, int cursorPosX, int cursorPosY) const = 0;
	virtual bool ParentBorderClickForChildContainer(IDockingWindow* childContainer, int cursorPosX, int cursorPosY) = 0;

	//Hosted content methods
	//##TODO## Replace these functions with a simpler set of more generic functions, whose
	//purpose is to expose the idea of panes within a docking window, and allow them to be
	//enumerated and activated.
	virtual unsigned int GetHostedContentCount() const = 0;
	virtual unsigned int GetSortedContentEntryNo(unsigned int sortedContentEntryIndex) const = 0;
	virtual bool GetHostedContentIndexFromWindow(HWND contentWindow, unsigned int& contentEntryNo) const = 0;
	virtual void SetActiveContent(unsigned int contentEntryNo) = 0;
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetHostedContentTitle(unsigned int contentEntryNo) const = 0;
	virtual HWND GetHostedContentWindow(unsigned int contentEntryNo) const = 0;

	//Parent docking window methods
	virtual IDockingWindow* GetParentDockingWindow() const = 0;
	virtual void NotifyAddedToParent(IDockingWindow* newParentDockingWindow) = 0;
	virtual void NotifyRemovedFromParent() = 0;
};

#include "IDockingWindow.inl"
#endif
