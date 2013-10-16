#ifndef __VIEWBASE_H__
#define __VIEWBASE_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "IView.h"
#include "IViewModel.h"
#include <string>

class ViewBase :public IView
{
public:
	//Constructors
	ViewBase();

	//Interface version functions
	virtual unsigned int GetIViewVersion() const;

	//Window settings
	void SetWindowSettings(const std::wstring& awindowTitle, DWORD awindowStyle, DWORD aextendedWindowStyle, unsigned int awidth, unsigned int aheight, bool aresizable = false);
	void SetDialogTemplateSettings(const std::wstring& awindowTitle, void* aassemblyHandle, LPCWSTR atemplateName, bool aresizable = false);

	//View management functions
	virtual bool OpenView(void* aparentWindow, IViewModel* aviewModel, int xpos, int ypos);
	virtual void CloseView();
	virtual void ShowView();
	virtual void HideView();
	virtual void ActivateView();

	//Title functions
	std::wstring GetViewTitle() const;

	//Position and size functions
	virtual bool CanResizeView() const;
	virtual void GetViewPosition(int& xpos, int& ypos) const;
	virtual void GetViewSize(int& sizex, int& sizey) const;
	virtual void SetViewPosition(int xpos, int ypos);
	virtual void SetViewSize(int sizex, int sizey);

	//State functions
	virtual bool SaveViewLayoutState(IHierarchicalStorageNode& node) const;
	virtual bool LoadViewLayoutState(IHierarchicalStorageNode& node);
	virtual bool DoesWindowHandleMatchView(void* awindowHandle) const;

protected:
	//Title functions
	virtual void GetViewTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;

	//Helper functions
	std::wstring BuildWindowTitle(const std::wstring& moduleDisplayName, const std::wstring& deviceInstanceName, const std::wstring& windowTitle) const;
	std::wstring GetLockingSupportedMessage() const;

	//Member window procedure
	virtual INT_PTR WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Window procedure helper functions
	void WndProcDialogImplementSaveFieldWhenLostFocus(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

private:
	//View management functions
	bool OpenViewDialog(void* aparentWindow, int xpos, int ypos);
	bool OpenViewWindow(void* aparentWindow, int xpos, int ypos);

	//Static window procedure
	static INT_PTR CALLBACK WndProcDialogInternal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WndProcWindowInternal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	HWND hwndInternal;
	HWND parentWindow;
	IViewModel* viewModel;

	//Window settings
	bool dialogSettingsCaptured;
	bool windowSettingsCaptured;
	void* assemblyHandle;
	LPCWSTR templateName;
	bool resizable;
	std::wstring windowTitle;
	DWORD windowStyle;
	DWORD extendedWindowStyle;
	unsigned int originalWindowWidth;
	unsigned int originalWindowHeight;
};

#endif
