#ifndef __WC_STACKPANEL_H__
#define __WC_STACKPANEL_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <map>
#include <map>

class WC_StackPanel
{
public:
	//Constants
	static const wchar_t* windowClassName;

	//Enumerations
	enum class WindowMessages :unsigned int;
	enum class StackDirection;
	enum class VerticalAlignment;
	enum class HorizontalAlignment;
	enum class SizeMode;

	//Structures
	struct SetAlignmentParams;

public:
	//Constructors
	WC_StackPanel(HINSTANCE amoduleHandle, HWND ahwnd);

	//Class registration
	static bool RegisterWindowClass(HINSTANCE amoduleHandle);
	static bool UnregisterWindowClass(HINSTANCE amoduleHandle);

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//Structures
	struct HostedWindowInfo;
	struct RowInfo;

private:
	//Message handlers
	LRESULT WndProcPrivate(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_CREATE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_ERASEBKGND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NCHITTEST(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_NOTIFY(WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_BOUNCE(WPARAM wParam, LPARAM lParam);
	LRESULT msgSTACK_ADDWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgSTACK_INSERTWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgSTACK_REMOVEWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgSTACK_SETDIRECTION(WPARAM wParam, LPARAM lParam);
	LRESULT msgSTACK_SETWRAPPINGENABLED(WPARAM wParam, LPARAM lParam);
	LRESULT msgSTACK_SETALIGNMENT(WPARAM wParam, LPARAM lParam);
	LRESULT msgSTACK_SETSIZEMODE(WPARAM wParam, LPARAM lParam);
	LRESULT msgSTACK_SETVERTICALPADDING(WPARAM wParam, LPARAM lParam);
	LRESULT msgSTACK_SETHORIZONTALPADDING(WPARAM wParam, LPARAM lParam);

	//Hosted window methods
	bool InsertWindow(const HostedWindowInfo& windowInfo, unsigned int insertPos);
	void RemoveWindow(HWND windowHandle);

	//Sizing methods
	void HandleSizeChanged(int newWidth, int newHeight);
	void UpdateChildWindowPositions();

private:
	HINSTANCE moduleHandle;
	HWND hwnd;
	int currentControlWidth;
	int currentControlHeight;
	StackDirection stackDirection;
	double verticalContentAlignmentDisplacement;
	double horizontalContentAlignmentDisplacement;
	SizeMode sizeMode;
	bool wrappingEnabled;
	bool updatingControlSizeInternally;
	unsigned int verticalPadding;
	unsigned int horizontalPadding;
	std::map<HWND, unsigned int> hostedWindowsSet;
	std::vector<HostedWindowInfo> hostedWindows;
};

#include "WC_StackPanel.inl"
#endif
