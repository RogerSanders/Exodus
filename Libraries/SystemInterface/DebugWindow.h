/*--------------------------------------------------------------------------------------*\
Things to do:
-Phase out DebugWindow and DebugWindowParams. Switch to the MVVM pattern, and create a
base IViewModel class for devices to use in order to create view models.
\*--------------------------------------------------------------------------------------*/
#ifndef __DEBUGWINDOW_H__
#define __DEBUGWINDOW_H__
#include "WindowFunctions/WindowFunctions.pkg"
#include <list>
class DebugWindowParams;

class DebugWindow
{
public:
	//Enumerations
	enum Type;

	//Constructors
	inline DebugWindow();
	inline DebugWindow(const std::wstring& aname, WNDPROC awndproc, DebugWindowParams* aparams, unsigned int awidth, unsigned int aheight, DWORD awindowStyle = 0, DWORD aextendedWindowStyle = 0, bool ashowDefault = false, bool aresizable = false);
	inline DebugWindow(const std::wstring& aname, DLGPROC adlgproc, DebugWindowParams* aparams, LPCTSTR atemplateName, bool ashowDefault = false, bool aresizable = false);
	inline DebugWindow(Type atype, const std::wstring& aname = L"");
	inline ~DebugWindow();
	inline DebugWindow(const DebugWindow& source);

public:
	HWND hwnd;
	bool showDefault;
	bool resizable;

	DebugWindowParams* params;
	std::wstring windowName;
	std::wstring menuName;
	unsigned int width;
	unsigned int height;
	Type type;
	union
	{
		WNDPROC wndproc;
		DLGPROC dlgproc;
	};
	LPCTSTR templateName;
	DWORD windowStyle;
	DWORD extendedWindowStyle;
	std::list<DebugWindow> children;
};

#include "DebugWindow.inl"
#endif
