#ifndef __IDOCKINGWINDOWDROPTARGETINFO_H__
#define __IDOCKINGWINDOWDROPTARGETINFO_H__
#include "WindowsSupport/WindowsSupport.pkg"

class IDockingWindowDropTargetInfo
{
public:
	//Constructors
	virtual ~IDockingWindowDropTargetInfo() = 0 {}
	virtual void Destroy() = 0;

	//Info functions
	virtual HWND GetOwningDockingWindow() = 0;
};

#endif
