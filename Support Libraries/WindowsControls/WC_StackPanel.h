#ifndef __WC_STACKPANEL_H__
#define __WC_STACKPANEL_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <map>
#include <map>

class WC_StackPanel
{
public:
	//Enumerations
	enum class WindowMessages :unsigned int;
	enum class StackDirection;
	enum class VerticalAlignment;
	enum class HorizontalAlignment;
	enum class SizeMode;

	//Structures
	struct SetAlignmentParams;

	//Constants
	static const wchar_t* WindowClassName;

public:
	//Constructors
	WC_StackPanel(HINSTANCE moduleHandle, HWND hwnd);

	//Class registration
	static bool RegisterWindowClass(HINSTANCE moduleHandle);
	static bool UnregisterWindowClass(HINSTANCE moduleHandle);

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
	HINSTANCE _moduleHandle;
	HWND _hwnd;
	int _currentControlWidth;
	int _currentControlHeight;
	StackDirection _stackDirection;
	double _verticalContentAlignmentDisplacement;
	double _horizontalContentAlignmentDisplacement;
	SizeMode _sizeMode;
	bool _wrappingEnabled;
	bool _updatingControlSizeInternally;
	unsigned int _verticalPadding;
	unsigned int _horizontalPadding;
	std::map<HWND, unsigned int> _hostedWindowsSet;
	std::vector<HostedWindowInfo> _hostedWindows;
};

#include "WC_StackPanel.inl"
#endif
