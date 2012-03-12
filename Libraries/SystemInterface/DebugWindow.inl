#include "DebugWindowParams.h"
#include "IDevice.h"

//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum DebugWindow::Type
{
	TYPE_WINDOW,
	TYPE_DIALOG,
	TYPE_LINE,
	TYPE_MENU
};

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugWindow::DebugWindow()
:params(0)
{}

//----------------------------------------------------------------------------------------
DebugWindow::DebugWindow(const std::wstring& aname, WNDPROC awndproc, DebugWindowParams* aparams, unsigned int awidth, unsigned int aheight, DWORD awindowStyle, DWORD aextendedWindowStyle, bool ashowDefault, bool aresizable)
:params(aparams), width(awidth), height(aheight), wndproc(awndproc), type(TYPE_WINDOW), windowStyle(awindowStyle), extendedWindowStyle(aextendedWindowStyle), showDefault(ashowDefault), resizable(aresizable)
{
	menuName = aname;
	if(params->baseDevice != 0)
	{
		windowName = params->baseDevice->GetDeviceInstanceName();
		windowName += L" - " + aname;
	}
	else
	{
		windowName = aname;
	}
}

//----------------------------------------------------------------------------------------
DebugWindow::DebugWindow(const std::wstring& aname, DLGPROC adlgproc, DebugWindowParams* aparams, LPCTSTR atemplateName, bool ashowDefault, bool aresizable)
:params(aparams), dlgproc(adlgproc), templateName(atemplateName), type(TYPE_DIALOG), showDefault(ashowDefault), resizable(aresizable)
{
	menuName = aname;
	if(params->baseDevice != 0)
	{
		windowName = params->baseDevice->GetDeviceInstanceName();
		windowName += L" - " + aname;
	}
	else
	{
		windowName = aname;
	}
}

//----------------------------------------------------------------------------------------
DebugWindow::DebugWindow(Type atype, const std::wstring& aname)
:params(0), type(atype)
{
	menuName = aname;
	windowName = aname;
}

//----------------------------------------------------------------------------------------
DebugWindow::~DebugWindow()
{
	delete params;
}

//----------------------------------------------------------------------------------------
DebugWindow::DebugWindow(const DebugWindow& source)
{
	*this = source;
	params = 0;
	if(source.params != 0)
	{
		params = source.params->Clone();
	}
}
