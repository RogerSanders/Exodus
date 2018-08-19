#ifndef __WC_DOCKPANEL_H__
#define __WC_DOCKPANEL_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <list>

class WC_DockPanel
{
public:
	// Enumerations
	enum class WindowMessages :unsigned int;
	enum class DockLocation;

	// Structures
	struct CalculateNewDockedWindowRectParams;

	// Constants
	static const wchar_t* WindowClassName;

public:
	// Constructors
	WC_DockPanel(HINSTANCE moduleHandle, HWND hwnd);

	// Class registration
	static bool RegisterWindowClass(HINSTANCE moduleHandle);
	static bool UnregisterWindowClass(HINSTANCE moduleHandle);

	// Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	// Structures
	struct DockedWindowEntry;

private:
	// Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NCHITTEST(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NOTIFY(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_BOUNCE(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_ADDCONTENTWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_REMOVECONTENTWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_ADDDOCKEDWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_ADDDOCKEDWINDOWTOFRONT(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_REMOVEDOCKEDWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_GETDOCKEDWINDOWDESIREDWIDTH(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_SETDOCKEDWINDOWDESIREDWIDTH(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_GETDOCKEDWINDOWDESIREDHEIGHT(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_SETDOCKEDWINDOWDESIREDHEIGHT(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_GETCONTENTRECT(WPARAM wParam, LPARAM lParam);
	LRESULT msgDOCK_CALCULATENEWDOCKEDWINDOWRECT(WPARAM wParam, LPARAM lParam);

	// Docked window methods
	void AddDockedWindow(HWND dockedWindow, DockLocation dockLocation, bool pushToFront);
	void RemoveDockedWindow(HWND dockedWindow);
	int GetDockedWindowDesiredWidth(HWND dockedWindow);
	void SetDockedWindowDesiredWidth(HWND dockedWindow, int desiredWidth);
	int GetDockedWindowDesiredHeight(HWND dockedWindow);
	void SetDockedWindowDesiredHeight(HWND dockedWindow, int desiredHeight);

	// Hosted content methods
	void AddHostedContent(HWND hostedContent);
	void RemoveHostedContent(HWND hostedContent);

	// Sizing methods
	void HandleSizeChanged(int newWidth, int newHeight);
	void UpdateChildWindowSizes();
	void CalculateNewDockedWindowPositionAndSize(DockLocation dockLocation, bool addToFront, int preferredWidth, int preferredHeight, int& windowPosX, int& windowPosY, int& windowWidth, int& windowHeight);
	static void UpdateDockedWindowPositionAndSize(DockedWindowEntry& entry, bool& updatedWindowSize, bool& updatedWindowPosition, int& currentClientPosX, int& currentClientPosY, int& remainingClientWidth, int& remainingClientHeight);

private:
	HINSTANCE _moduleHandle;
	HWND _hwnd;
	int _currentControlWidth;
	int _currentControlHeight;

	std::list<DockedWindowEntry> _dockedWindows;
	std::list<HWND> _hostedContent;
	int _hostedContentWindowPosX;
	int _hostedContentWindowPosY;
	int _hostedContentWindowWidth;
	int _hostedContentWindowHeight;
};

#include "WC_DockPanel.inl"
#endif
