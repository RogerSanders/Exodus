#ifndef __IUIMANAGER_H__
#define __IUIMANAGER_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "InteropSupport/InteropSupport.pkg"
#include "HierarchicalStorageInterface/HierarchicalStorageInterface.pkg"
#include "IViewPresenter.h"

class IUIManager
{
public:
	//Enumerations
	enum class WindowType;
	enum class InitialDockingLocation;

public:
	//Constructors
	virtual ~IUIManager() = 0 {}

	//Interface version functions
	static inline unsigned int ThisIUIManagerVersion();
	virtual unsigned int GetIUIManagerVersion() const = 0;

	//Main window functions
	virtual HWND GetMainWindow() const = 0;

	//Native window creation functions
	virtual HWND CreateDialogWindow(IView& view, IViewPresenter& viewPresenter, HINSTANCE assemblyHandle, DLGPROC windowProcedure, LPCWSTR templateName) = 0;
	virtual HWND CreateNativeWindow(IView& view, IViewPresenter& viewPresenter, HINSTANCE assemblyHandle, WNDPROC windowProcedure, DWORD windowStyle, DWORD extendedWindowStyle) = 0;

	//Window management functions
	inline bool ShowWindowFirstTime(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle, IHierarchicalStorageNode* windowState = 0);
	virtual void CloseWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle) = 0;
	virtual void ShowWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle) = 0;
	virtual void HideWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle) = 0;
	virtual void ActivateWindow(IView& view, IViewPresenter& viewPresenter, HWND windowHandle) = 0;
	virtual void NotifyWindowDestroyed(IView& view, IViewPresenter& viewPresenter, HWND windowHandle) = 0;

	//Window state functions
	virtual bool LoadWindowState(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, IHierarchicalStorageNode& windowState) = 0;
	virtual bool SaveWindowState(const IView& view, const IViewPresenter& viewPresenter, HWND windowHandle, IHierarchicalStorageNode& windowState) const = 0;

	//Window size functions
	virtual void ResizeWindowToTargetClientSize(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, unsigned int windowClientWidth, unsigned int windowClientHeight) = 0;

	//Window title functions
	inline void UpdateWindowTitle(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const std::wstring& windowTitle);

protected:
	//Window management functions
	virtual bool ShowWindowFirstTimeInternal(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const InteropSupport::ISTLObjectSource<std::wstring>& windowTitleMarshaller, IHierarchicalStorageNode* windowState) = 0;

	//Window title functions
	virtual void UpdateWindowTitleInternal(IView& view, IViewPresenter& viewPresenter, HWND windowHandle, const InteropSupport::ISTLObjectSource<std::wstring>& windowTitleMarshaller) = 0;
};

#include "IUIManager.inl"
#endif
