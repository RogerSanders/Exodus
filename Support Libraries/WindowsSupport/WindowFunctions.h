#ifndef __WINDOWFUNCTIONS_H__
#define __WINDOWFUNCTIONS_H__
#define _WIN32_WINNT 0x0501
//Define NOMINMAX so we don't get the extremely problematic "min" and "max" macro
//definitions from windef.h
#define NOMINMAX
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <uxtheme.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "StreamInterface/StreamInterface.pkg"

//Ugly workarounds for a silly 64-bit portability warning problem in winuser.h. Issue
//acknowledged by Microsoft. Waiting for SDK update to fix problem as of September 2008.
#ifndef _WIN64
#undef GetWindowLongPtr
#undef SetWindowLongPtr
#ifdef UNICODE
#define GetWindowLongPtr(a, b)	(LONG_PTR)GetWindowLongPtrW(a, b)
#define SetWindowLongPtr(a, b, c)	SetWindowLongPtrW(a, b, (LONG)c)
#else
#define GetWindowLongPtr(a, b)	(LONG_PTR)GetWindowLongPtrA(a, b)
#define SetWindowLongPtr(a, b, c)	SetWindowLongPtrA(a, b, (LONG)c)
#endif
#endif

//Module helper functions
enum VersionInfoProperty
{
	VERSIONINFOPROPERTY_COMMENTS,
	VERSIONINFOPROPERTY_COMPANYNAME,
	VERSIONINFOPROPERTY_FILEDESCRIPTION,
	VERSIONINFOPROPERTY_FILEVERSION,
	VERSIONINFOPROPERTY_INTERNALNAME,
	VERSIONINFOPROPERTY_LEGALCOPYRIGHT,
	VERSIONINFOPROPERTY_LEGALTRADEMARKS,
	VERSIONINFOPROPERTY_ORIGINALFILENAME,
	VERSIONINFOPROPERTY_PRODUCTNAME,
	VERSIONINFOPROPERTY_PRODUCTVERSION,
	VERSIONINFOPROPERTY_PRIVATEBUILD,
	VERSIONINFOPROPERTY_SPECIALBUILD
};
bool GetModuleVersionInfoString(const std::wstring& modulePath, VersionInfoProperty targetProperty, std::wstring& content);
std::wstring GetModuleFilePath(HMODULE moduleHandle);

//DPI functions
bool DPIIsScalingActive();
void DPIGetScreenSettings(int& dpiX, int& dpiY);
void DPIGetScreenScaleFactors(float& dpiScaleX, float& dpiScaleY);
template<class T> T DPIScaleWidth(T pixelWidth);
template<class T> T DPIScaleHeight(T pixelHeight);
template<class T> T DPIReverseScaleWidth(T pixelWidth);
template<class T> T DPIReverseScaleHeight(T pixelHeight);

//BindStdHandlesToConsole function
void BindStdHandlesToConsole();

//Environment variable functions
std::wstring GetEnvironmentVariableString(std::wstring variableName);
void SetEnvironmentVariableString(std::wstring variableName, std::wstring value);

//Window creation helpers
HWND CreateWindowThread(HINSTANCE hInstance, const std::wstring& windowName, WNDPROC wndproc, unsigned int width, unsigned int height, LPVOID params);
HWND CreateWindowsWindow(HINSTANCE hInstance, const std::wstring& windowName, WNDPROC wndproc, unsigned int width, unsigned int height, LPVOID params);
HGLRC CreateOpenGLWindow(HWND hwnd);
void GetHiddenBorderDimensions(HWND hwnd, int& borderLeft, int& borderRight, int& borderTop, int& borderBottom);

//Internal implementation for CreateWindowThread
struct CreateWindowThreadParams
{
	HINSTANCE hInstance;
	std::wstring windowName;
	WNDPROC wndproc;
	unsigned int width;
	unsigned int height;
	LPVOID params;
	volatile bool* writtenHandle;
	volatile HWND* hwndPrivate;
};
CreateWindowThreadParams* AllocCreateWindowThreadParams();
void ReleaseCreateWindowThreadParams(CreateWindowThreadParams* object);
DWORD WINAPI CreateWindowThreadFunction(LPVOID aparams);
void WindowsMessageLoop(HWND hwnd);

//Parent and owner window functions
void SetWindowParent(HWND targetWindow, HWND newParent);
HWND GetFirstOwnerWindow(HWND targetWindow);
HWND GetFirstOwnerWindowOrTopLevelParent(HWND targetWindow);
void SetOwnerWindow(HWND targetWindow, HWND newOwnerWindow);

//General window helper functions
std::wstring GetClassName(HWND targetWindow);

//Control text helper functions
void UpdateDlgItemBin(HWND hwnd, int controlID, unsigned int data);
unsigned int GetDlgItemBin(HWND hwnd, int controlID);
void UpdateDlgItemHex(HWND hwnd, int controlID, unsigned int width, unsigned int data);
unsigned int GetDlgItemHex(HWND hwnd, int controlID);
void UpdateDlgItemFloat(HWND hwnd, int controlID, float data);
float GetDlgItemFloat(HWND hwnd, int controlID);
void UpdateDlgItemDouble(HWND hwnd, int controlID, double data);
double GetDlgItemDouble(HWND hwnd, int controlID);
void UpdateDlgItemString(HWND hwnd, int controlID, const std::wstring& data);
std::wstring GetDlgItemString(HWND hwnd, int controlID);

//Modal window functions
std::list<HWND> DisableAllEnabledDialogWindows(HWND ownerWindow);
void EnableDialogWindows(const std::list<HWND>& windowList);
int SafeMessageBox(HWND hwnd, const std::wstring& message, const std::wstring& title, UINT type);
INT_PTR SafeDialogBox(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc);
INT_PTR SafeDialogBoxParam(HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
INT_PTR SafeDialogBoxIndirect(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc);
INT_PTR SafeDialogBoxIndirectParam(HINSTANCE hInstance, LPCDLGTEMPLATE hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);

//Window enumeration
std::list<HWND> GetOwnedDialogWindows(HWND ownerWindow);
std::list<HWND> GetDescendantWindows(HWND targetWindow);

//Tooltip functions
HWND CreateTooltipControl(HINSTANCE moduleHandle, HWND hwndParent, unsigned int maxWidth = 300);
BOOL AddTooltip(HINSTANCE moduleHandle, HWND hwndTooltip, HWND hwndParent, int targetControlID, const std::wstring& text, bool createAnchor = false, const std::wstring& anchorLink = L"");

//Child window message bounce back
#define WM_BOUNCE WM_APP + 0x800
struct BounceMessage
{
	BounceMessage(HWND ahwnd, UINT auMsg, WPARAM awParam, LPARAM alParam)
	:hwnd(ahwnd), uMsg(auMsg), wParam(awParam), lParam(alParam), result(0), caught(false)
	{}

	void SetResult(LRESULT aresult)
	{
		caught = true;
		result = aresult;
	}

	HWND hwnd;
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
	LRESULT result;
	bool caught;
};
LRESULT CALLBACK BounceBackSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

//Edit control extensions
LRESULT CALLBACK EditBoxFocusFixSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

//Static control extensions
LRESULT CALLBACK ResizableStaticControlSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

//Highlight extensions
void PaintCheckboxHighlight(HWND hwnd);

//Icon helper functions
unsigned int GetIconFileEntryCount(Stream::IStream& iconFileStream);
bool ConvertIconFileToIconResource(Stream::IStream& iconFileStream, Stream::IStream& iconDirectoryData, const std::map<int, Stream::IStream*>& iconResourceData);

//WinColor struct
struct WinColor
{
	WinColor()
	{}
	WinColor(unsigned char ar, unsigned char ag, unsigned char ab)
	:r(ar), g(ag), b(ab)
	{}

	COLORREF GetColorREF() const
	{
		return RGB(r, g, b);
	}
	void Invert()
	{
		r = ~r;
		g = ~g;
		b = ~b;
	}

	unsigned char r;
	unsigned char g;
	unsigned char b;
};

//DLGTEMPLATEEX struct, based on information provided by Microsoft. This structure is not
//defined in any public header files.
struct DLGTEMPLATEEX
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
};

#include "WindowFunctions.inl"
#endif
