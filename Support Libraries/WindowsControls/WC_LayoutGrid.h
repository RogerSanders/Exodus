#ifndef __WC_LAYOUTGRID_H__
#define __WC_LAYOUTGRID_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include <map>

class WC_LayoutGrid
{
public:
	//Constants
	static const wchar_t* windowClassName;

	//Enumerations
	enum class WindowMessages :unsigned int;
	enum class SizeMode;
	enum class WindowSizeMode;
	enum class VerticalAlignment;
	enum class HorizontalAlignment;

	//Structures
	struct AddWindowParams;
	struct AddRowParams;
	struct AddColumnParams;
	struct UpdateWindowSizesParams;

public:
	//Constructors
	WC_LayoutGrid(HINSTANCE amoduleHandle, HWND ahwnd);

	//Class registration
	static bool RegisterWindowClass(HINSTANCE amoduleHandle);
	static bool UnregisterWindowClass(HINSTANCE amoduleHandle);

	//Message handlers
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	//Structures
	struct RowInfo;
	struct ColumnInfo;
	struct HostedWindowInfo;

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
	LRESULT msgGRID_ADDROW(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_ADDCOLUMN(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_ADDWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_REMOVEWINDOW(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_UPDATEWINDOWSIZES(WPARAM wParam, LPARAM lParam);
	LRESULT msgGRID_RECALCULATESIZES(WPARAM wParam, LPARAM lParam);

	//Cell methods
	void AddRow(int minHeight, int maxHeight, SizeMode sizeMode, double proportionalRatio);
	void AddColumn(int minWidth, int maxWidth, SizeMode sizeMode, double proportionalRatio);

	//Hosted window methods
	void AddWindow(const HostedWindowInfo& windowInfo);
	void RemoveWindow(HWND windowHandle);
	void UpdateWindowSizes(const UpdateWindowSizesParams& params);

	//Sizing methods
	void HandleSizeChanged(int newWidth, int newHeight);
	void UpdateChildWindowSizes();

private:
	HINSTANCE moduleHandle;
	HWND hwnd;
	int currentControlWidth;
	int currentControlHeight;
	std::vector<RowInfo> rows;
	std::vector<ColumnInfo> columns;
	std::map<HWND, HostedWindowInfo> hostedWindows;
};

#include "WC_LayoutGrid.inl"
#endif
