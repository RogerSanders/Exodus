#ifndef __WC_DOCKPANEL_H__
#define __WC_DOCKPANEL_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <list>

class WC_DockPanel
{
public:
	//Constants
	static const wchar_t* windowClassName;

	//Enumerations
	enum DockLocation;
	enum WindowMessages;

	//Structures
	struct CalculateNewDockedWindowRectParams;

public:
	//Constructors
	WC_DockPanel(HINSTANCE amoduleHandle, HWND ahwnd);

	//Class registration
	static bool RegisterWindowClass(HINSTANCE amoduleHandle);
	static bool UnregisterWindowClass(HINSTANCE amoduleHandle);

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//Structures
	struct DockedWindowEntry;

private:
	//Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
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

	//Docked window methods
	void AddDockedWindow(HWND dockedWindow, DockLocation dockLocation, bool pushToFront);
	void RemoveDockedWindow(HWND dockedWindow);
	int GetDockedWindowDesiredWidth(HWND dockedWindow);
	void SetDockedWindowDesiredWidth(HWND dockedWindow, int desiredWidth);
	int GetDockedWindowDesiredHeight(HWND dockedWindow);
	void SetDockedWindowDesiredHeight(HWND dockedWindow, int desiredHeight);

	//Hosted content methods
	void AddHostedContent(HWND ahostedContent);
	void RemoveHostedContent(HWND ahostedContent);

	//Sizing methods
	void HandleSizeChanged(int newWidth, int newHeight);
	void UpdateChildWindowSizes();
	void CalculateNewDockedWindowPositionAndSize(DockLocation dockLocation, bool addToFront, int preferredWidth, int preferredHeight, int& windowPosX, int& windowPosY, int& windowWidth, int& windowHeight);
	static void UpdateDockedWindowPositionAndSize(DockedWindowEntry& entry, bool& updatedWindowSize, bool& updatedWindowPosition, int& currentClientPosX, int& currentClientPosY, int& remainingClientWidth, int& remainingClientHeight);

private:
	HINSTANCE moduleHandle;
	HWND hwnd;
	int currentControlWidth;
	int currentControlHeight;

	std::list<DockedWindowEntry> dockedWindows;
	std::list<HWND> hostedContent;
	int hostedContentWindowPosX;
	int hostedContentWindowPosY;
	int hostedContentWindowWidth;
	int hostedContentWindowHeight;
};

#include "WC_DockPanel.inl"
#endif
