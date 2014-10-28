#ifndef __VIEWBASE_H__
#define __VIEWBASE_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "IView.h"
#include "IViewPresenter.h"
#include "IUIManager.h"
#include <string>

class ViewBase :public IView
{
public:
	//Constructors
	ViewBase(IUIManager& auiManager, IViewPresenter& aviewPresenter);

	//Interface version functions
	virtual unsigned int GetIViewVersion() const;

	//Window settings
	void SetWindowSettings(const std::wstring& ainitialWindowTitle, DWORD awindowStyle, DWORD aextendedWindowStyle, unsigned int awidth, unsigned int aheight);
	void SetDialogTemplateSettings(const std::wstring& ainitialWindowTitle, void* aassemblyHandle, LPCWSTR atemplateName);
	void SetDocumentViewType();
	void SetDockableViewType(bool ainitiallyDocked = false, DockPos ainitialDockPos = DockPos::Left, bool ainitiallyCollapsed = false, const std::wstring& aviewDockingGroup = L"");
	void SetDialogViewType(DialogMode adialogMode = DialogMode::Modeless, bool adialogResizable = false, DialogPos ainitialDialogPos = DialogPos::Default);

	//View management functions
	virtual bool OpenView(IHierarchicalStorageNode* viewState);
	virtual void CloseView();
	virtual void ShowView();
	virtual void HideView();
	virtual void ActivateView();

	//State functions
	virtual bool LoadViewState(IHierarchicalStorageNode& viewState);
	virtual bool SaveViewState(IHierarchicalStorageNode& viewState) const;

	//New window state
	virtual MarshalSupport::Marshal::Ret<std::wstring> GetViewDockingGroup() const;
	virtual bool IsViewInitiallyDocked() const;
	virtual bool IsViewInitiallyCollapsed() const;
	virtual DockPos GetViewInitialDockPosition() const;
	virtual ViewType GetViewType() const;
	virtual DialogMode GetViewDialogMode() const;
	virtual DialogPos GetViewInitialDialogPosition() const;
	virtual bool CanResizeDialog() const;

protected:
	//Helper functions
	HINSTANCE GetAssemblyHandle() const;
	//##TODO## Remove this
	std::wstring GetLockingSupportedMessage() const;

	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Window procedure helper functions
	void WndProcDialogImplementSaveFieldWhenLostFocus(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

private:
	//Static window procedure
	static INT_PTR CALLBACK WndProcDialogInternal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WndProcWindowInternal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	HWND hwndInternal;
	HWND parentWindow;
	IUIManager& uiManager;
	IViewPresenter& viewPresenter;
	bool windowShownForFirstTime;

	//Window settings
	bool dialogSettingsCaptured;
	bool windowSettingsCaptured;
	std::wstring initialWindowTitle;
	void* assemblyHandle;
	LPCWSTR templateName;
	DWORD windowStyle;
	DWORD extendedWindowStyle;
	unsigned int originalWindowWidth;
	unsigned int originalWindowHeight;

	//View settings
	std::wstring viewDockingGroup;
	bool initiallyDocked;
	bool initiallyCollapsed;
	DockPos initialDockPos;
	ViewType viewType;
	DialogMode dialogMode;
	DialogPos initialDialogPos;
	bool dialogResizable;
};

#endif
