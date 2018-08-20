#ifndef __VIEWBASE_H__
#define __VIEWBASE_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "IView.h"
#include "IViewPresenter.h"
#include "IUIManager.h"
#include <string>
#include <functional>

class ViewBase :public IView
{
public:
	// Constructors
	ViewBase(IUIManager& uiManager, IViewPresenter& viewPresenter);

	// Interface version functions
	virtual unsigned int GetIViewVersion() const;

	// Window settings
	void SetWindowSettings(const std::wstring& initialWindowTitle, DWORD windowStyle, DWORD extendedWindowStyle, unsigned int width, unsigned int height);
	void SetDialogTemplateSettings(const std::wstring& initialWindowTitle, void* assemblyHandle, LPCWSTR templateName);
	void SetDocumentViewType();
	void SetDockableViewType(bool initiallyDocked = false, DockPos initialDockPos = DockPos::Left, bool initiallyCollapsed = false, const std::wstring& viewDockingGroup = L"");
	void SetDialogViewType(DialogMode dialogMode = DialogMode::Modeless, bool dialogResizable = false, DialogPos initialDialogPos = DialogPos::Default);

	// View management functions
	virtual bool OpenView(IHierarchicalStorageNode* viewState);
	virtual void CloseView();
	virtual void ShowView();
	virtual void HideView();
	virtual void ActivateView();

	// State functions
	virtual bool LoadViewState(IHierarchicalStorageNode& viewState);
	virtual bool SaveViewState(IHierarchicalStorageNode& viewState) const;

	// New window state
	virtual Marshal::Ret<std::wstring> GetViewDockingGroup() const;
	virtual bool IsViewInitiallyDocked() const;
	virtual bool IsViewInitiallyCollapsed() const;
	virtual DockPos GetViewInitialDockPosition() const;
	virtual ViewType GetViewType() const;
	virtual DialogMode GetViewDialogMode() const;
	virtual DialogPos GetViewInitialDialogPosition() const;
	virtual bool CanResizeDialog() const;

protected:
	// Helper functions
	HINSTANCE GetAssemblyHandle() const;

	// Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// Child window functions
	HWND CreateChildDialog(HWND parentWindow, void* assemblyHandle, LPCWSTR templateName, const std::function<INT_PTR(HWND, UINT, WPARAM, LPARAM)>& dlgProcHandler);
	HWND CreateChildWindow(DWORD windowStyle, DWORD extendedWindowStyle, unsigned int posX, unsigned int posY, unsigned int width, unsigned int height, HWND parentWindow, const std::function<INT_PTR(HWND, UINT, WPARAM, LPARAM)>& wndProcHandler);
	static INT_PTR CALLBACK WndProcChildDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WndProcChildWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	// Window procedure helper functions
	void WndProcDialogImplementSaveFieldWhenLostFocus(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void WndProcDialogImplementGiveFocusToChildWindowOnClick(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	// Structures
	struct ChildDialogState
	{
		ViewBase* view;
		std::function<INT_PTR(HWND, UINT, WPARAM, LPARAM)> windowProcedureMethod;
	};

private:
	// Static window procedure
	static INT_PTR CALLBACK WndProcDialogInternal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WndProcWindowInternal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	HWND _hwndInternal;
	IUIManager& _uiManager;
	IViewPresenter& _viewPresenter;
	bool _windowShownForFirstTime;

	// Window settings
	bool _dialogSettingsCaptured;
	bool _windowSettingsCaptured;
	std::wstring _initialWindowTitle;
	void* _assemblyHandle;
	LPCWSTR _templateName;
	DWORD _windowStyle;
	DWORD _extendedWindowStyle;
	unsigned int _originalWindowWidth;
	unsigned int _originalWindowHeight;

	// View settings
	std::wstring _viewDockingGroup;
	bool _initiallyDocked;
	bool _initiallyCollapsed;
	DockPos _initialDockPos;
	ViewType _viewType;
	DialogMode _dialogMode;
	DialogPos _initialDialogPos;
	bool _dialogResizable;
};

#endif
