#include "WindowFunctions.h"
#include "DataConversion/DataConversion.pkg"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <io.h>
#include <vector>
#include <dwmapi.h>

//----------------------------------------------------------------------------------------
//Module helper functions
//----------------------------------------------------------------------------------------
bool GetModuleVersionInfoString(const std::wstring& modulePath, VersionInfoProperty targetProperty, std::wstring& content)
{
	//Retrieve the size of the version info table in the target module
	DWORD nullValue;
	DWORD getFileVersionInfoSizeReturn = GetFileVersionInfoSize(modulePath.c_str(), &nullValue);
	if(getFileVersionInfoSizeReturn == 0)
	{
		return false;
	}
	DWORD fileVersionInfoTableSize = getFileVersionInfoSizeReturn;

	//Read the contents of the file version table in the target module
	std::vector<unsigned char> fileVersionInfoBuffer(fileVersionInfoTableSize);
	DWORD getFileVersionInfoReturn = GetFileVersionInfo(modulePath.c_str(), 0, fileVersionInfoTableSize, (LPVOID)&fileVersionInfoBuffer[0]);
	if(getFileVersionInfoReturn == 0)
	{
		return false;
	}

	//Obtain text string identifiers for the first language and code page combination
	//listed in the version info table
	LPVOID translationArrayDataLocation = 0;
	UINT translationArrayCharacterCount = 0;
	BOOL verQueryValueReturnForLanguage = VerQueryValue(&fileVersionInfoBuffer[0], L"\\VarFileInfo\\Translation", &translationArrayDataLocation, &translationArrayCharacterCount);
	if((verQueryValueReturnForLanguage == 0) || (translationArrayDataLocation == 0))
	{
		return false;
	}
	short languageCode = *((short*)translationArrayDataLocation);
	short codePageCode = *(((short*)translationArrayDataLocation) + 1);
	std::wstring languageCodeString;
	IntToStringBase16(languageCode, languageCodeString, 4, false);
	std::wstring codePageString;
	IntToStringBase16(codePageCode, codePageString, 4, false);

	//Build a string path to the target property within the file version table
	std::wstring propertyKey;
	switch(targetProperty)
	{
	case VERSIONINFOPROPERTY_COMMENTS:
		propertyKey = L"Comments";
		break;
	case VERSIONINFOPROPERTY_COMPANYNAME:
		propertyKey = L"CompanyName";
		break;
	case VERSIONINFOPROPERTY_FILEDESCRIPTION:
		propertyKey = L"FileDescription";
		break;
	case VERSIONINFOPROPERTY_FILEVERSION:
		propertyKey = L"FileVersion";
		break;
	case VERSIONINFOPROPERTY_INTERNALNAME:
		propertyKey = L"InternalName";
		break;
	case VERSIONINFOPROPERTY_LEGALCOPYRIGHT:
		propertyKey = L"LegalCopyright";
		break;
	case VERSIONINFOPROPERTY_LEGALTRADEMARKS:
		propertyKey = L"LegalTrademarks";
		break;
	case VERSIONINFOPROPERTY_ORIGINALFILENAME:
		propertyKey = L"OriginalFilename";
		break;
	case VERSIONINFOPROPERTY_PRODUCTNAME:
		propertyKey = L"ProductName";
		break;
	case VERSIONINFOPROPERTY_PRODUCTVERSION:
		propertyKey = L"ProductVersion";
		break;
	case VERSIONINFOPROPERTY_PRIVATEBUILD:
		propertyKey = L"PrivateBuild";
		break;
	case VERSIONINFOPROPERTY_SPECIALBUILD:
		propertyKey = L"SpecialBuild";
		break;
	default:
		return false;
	}
	std::wstring targetPropertyPath = L"\\StringFileInfo\\" + languageCodeString + codePageString + L"\\" + propertyKey;

	//Attempt to retrieve the target property from the version info table
	LPVOID targetDataLocation = 0;
	UINT targetCharacterCount = 0;
	BOOL verQueryValueReturn = VerQueryValue(&fileVersionInfoBuffer[0], targetPropertyPath.c_str(), &targetDataLocation, &targetCharacterCount);
	if((verQueryValueReturn == 0) || (targetDataLocation == 0))
	{
		return false;
	}
	content = (wchar_t*)targetDataLocation;

	return true;
}

//----------------------------------------------------------------------------------------
std::wstring GetModuleFilePath(HMODULE moduleHandle)
{
	std::wstring modulePath;

	//Loop around until we manage to extract the entire path to the target module
	DWORD bufferSize = MAX_PATH;
	bool aborted = false;
	bool fileNameRead = false;
	while(!aborted && !fileNameRead)
	{
		//Create a buffer to store the temporary module path output
		std::wstring stringBuffer;
		stringBuffer.resize(bufferSize);

		//Attempt to obtain the full path to the target module
		DWORD getModuleFileNameReturn = GetModuleFileName(moduleHandle, &stringBuffer[0], bufferSize);
		if(getModuleFileNameReturn == 0)
		{
			aborted = true;
			continue;
		}

		//If it appears that the path to the target module was truncated, increase the
		//buffer size, and attempt to read the path again.
		if(getModuleFileNameReturn == bufferSize)
		{
			bufferSize *= 2;
			continue;
		}

		//Save the path to the target module, and flag that we've successfully read the
		//module file path.
		modulePath = stringBuffer.c_str();
		fileNameRead = true;
	}

	return modulePath;
}

//----------------------------------------------------------------------------------------
//DPI functions
//----------------------------------------------------------------------------------------
bool DPIIsScalingActive()
{
	//Obtain the current screen DPI settings
	int dpiX;
	int dpiY;
	DPIGetScreenSettings(dpiX, dpiY);

	//Return true if the selected DPI mode is different from 96 DPI
	bool dpiScalingActive = (dpiX != 96) || (dpiY != 96);
	return dpiScalingActive;
}

//----------------------------------------------------------------------------------------
void DPIGetScreenSettings(int& dpiX, int& dpiY)
{
	//Obtain the current screen DPI settings
	dpiX = 96;
	dpiY = 96;
	HDC hdc = GetDC(NULL);
	if(hdc)
	{
		dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
		dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(NULL, hdc);
	}
}

//----------------------------------------------------------------------------------------
void DPIGetScreenScaleFactors(float& dpiScaleX, float& dpiScaleY)
{
	//Obtain the current screen DPI settings
	int dpiX;
	int dpiY;
	DPIGetScreenSettings(dpiX, dpiY);

	//Calculate a scale value for pixel values based on the current screen DPI settings
	dpiScaleX = (float)dpiX / 96.0f;
	dpiScaleY = (float)dpiY / 96.0f;
}

//----------------------------------------------------------------------------------------
//BindStdHandlesToConsole function
//----------------------------------------------------------------------------------------
void BindStdHandlesToConsole()
{
	//Redirect unbuffered STDIN to the console
	HANDLE stdInHandle = GetStdHandle(STD_INPUT_HANDLE);
	if(stdInHandle != INVALID_HANDLE_VALUE)
	{
		int fileDescriptor = _open_osfhandle((intptr_t)stdInHandle, _O_TEXT);
		if(fileDescriptor != -1)
		{
			FILE* file = _fdopen(fileDescriptor, "r");
			if(file != NULL)
			{
				*stdin = *file;
				setvbuf(stdin, NULL, _IONBF, 0);
			}
		}
	}

	//Redirect unbuffered STDOUT to the console
	HANDLE stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if(stdOutHandle != INVALID_HANDLE_VALUE)
	{
		int fileDescriptor = _open_osfhandle((intptr_t)stdOutHandle, _O_TEXT);
		if(fileDescriptor != -1)
		{
			FILE* file = _fdopen(fileDescriptor, "w");
			if(file != NULL)
			{
				*stdout = *file;
				setvbuf(stdout, NULL, _IONBF, 0);
			}
		}
	}

	//Redirect unbuffered STDERR to the console
	HANDLE stdErrHandle = GetStdHandle(STD_ERROR_HANDLE);
	if(stdErrHandle != INVALID_HANDLE_VALUE)
	{
		int fileDescriptor = _open_osfhandle((intptr_t)stdErrHandle, _O_TEXT);
		if(fileDescriptor != -1)
		{
			FILE* file = _fdopen(fileDescriptor, "w");
			if(file != NULL)
			{
				*stderr = *file;
				setvbuf(stderr, NULL, _IONBF, 0);
			}
		}
	}

	//Clear the error state for each of the C++ standard stream objects. We need to do this, as
	//attempts to access the standard streams before they refer to a valid target will cause the
	//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
	//to always occur during startup regardless of whether anything has been read from or written to
	//the console or not.
	std::wcin.clear();
	std::cin.clear();
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
}

//----------------------------------------------------------------------------------------
//Environment variable functions
//----------------------------------------------------------------------------------------
std::wstring GetEnvironmentVariableString(std::wstring variableName)
{
	std::wstring minidumpPath;
	DWORD getEnvironmentVariableReturn;
	getEnvironmentVariableReturn = GetEnvironmentVariable(variableName.c_str(), 0, 0);
	if(getEnvironmentVariableReturn != 0)
	{
		std::vector<wchar_t> buffer(getEnvironmentVariableReturn);
		getEnvironmentVariableReturn = GetEnvironmentVariable(variableName.c_str(), &buffer[0], (DWORD)buffer.size());
		if(getEnvironmentVariableReturn != 0)
		{
			minidumpPath = &buffer[0];
		}
	}
	return minidumpPath;
}

//----------------------------------------------------------------------------------------
void SetEnvironmentVariableString(std::wstring variableName, std::wstring value)
{
	SetEnvironmentVariable(&variableName[0], &value[0]);
}

//----------------------------------------------------------------------------------------
//Window creation helpers
//----------------------------------------------------------------------------------------
HWND CreateWindowThread(HINSTANCE hInstance, const std::wstring& windowName, WNDPROC wndproc, unsigned int width, unsigned int height, LPVOID params)
{
	volatile bool writtenHandle = false;
	volatile HWND hwnd = NULL;
	CreateWindowThreadParams* threadParams = AllocCreateWindowThreadParams();
	threadParams->hInstance = hInstance;
	threadParams->windowName = windowName;
	threadParams->wndproc = wndproc;
	threadParams->width = width;
	threadParams->height = height;
	threadParams->params = params;
	threadParams->writtenHandle = &writtenHandle;
	threadParams->hwndPrivate = &hwnd;

	HANDLE createThreadReturn;
	createThreadReturn = CreateThread(NULL, 0, CreateWindowThreadFunction, (LPVOID)threadParams, 0, NULL);
	if(createThreadReturn != NULL)
	{
		while(!writtenHandle)
		{
			Sleep(0);
		}
	}
	else
	{
		ReleaseCreateWindowThreadParams(threadParams);
	}

	return hwnd;
}

//----------------------------------------------------------------------------------------
HWND CreateWindowsWindow(HINSTANCE hInstance, const std::wstring& windowName, WNDPROC wndproc, unsigned int width, unsigned int height, LPVOID params)
{
	WNDCLASSEX wc;
	HWND hwnd = NULL;

	width += GetSystemMetrics(SM_CXBORDER) * 2;
	width += GetSystemMetrics(SM_CXSIZEFRAME) * 2;
	height += GetSystemMetrics(SM_CYCAPTION);
	height += GetSystemMetrics(SM_CYBORDER) * 2;
	height += GetSystemMetrics(SM_CYSIZEFRAME) * 2;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = wndproc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = windowName.c_str();
	wc.hIconSm       = NULL;
	if((RegisterClassEx(&wc) != 0) || (GetLastError() == ERROR_CLASS_ALREADY_EXISTS))
	{
		hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, windowName.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, (LPVOID)params);
	}

	return hwnd;
}

//----------------------------------------------------------------------------------------
HGLRC CreateOpenGLWindow(HWND hwnd)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 0;
	pfd.cAccumBits = 0;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 0;
	pfd.cAuxBuffers = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;

	HDC deviceContext = GetDC(hwnd);
	if(deviceContext == NULL)
	{
		return NULL;
	}

	GLuint pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
	if(pixelFormat == 0)
	{
		ReleaseDC(hwnd, deviceContext);
		return NULL;
	}

	if(SetPixelFormat(deviceContext,pixelFormat, &pfd) == FALSE)
	{
		ReleaseDC(hwnd, deviceContext);
		return NULL;
	}

	HGLRC renderingContext = wglCreateContext(deviceContext);
	if(renderingContext == NULL)
	{
		ReleaseDC(hwnd, deviceContext);
		return NULL;
	}

	if(wglMakeCurrent(deviceContext, renderingContext) == FALSE)
	{
		wglDeleteContext(renderingContext);
		ReleaseDC(hwnd, deviceContext);
		return NULL;
	}

	ReleaseDC(hwnd, deviceContext);
	return renderingContext;
}

//----------------------------------------------------------------------------------------
void GetHiddenBorderDimensions(HWND hwnd, int& borderLeft, int& borderRight, int& borderTop, int& borderBottom)
{
	//Set all the border sizes to zero to start with. If we manage to obtain information
	//about hidden borders, we'll calculate the proper values for these.
	borderLeft = 0;
	borderRight = 0;
	borderTop = 0;
	borderBottom = 0;

	//When running under the Aero theme on Windows Vista and up, the GetWindowRect()
	//function returns incorrect dimensions. The returned dimensions of the window are as
	//if Aero was not running, and the "Basic" theme was enabled. Reportedly, if our
	//application is compiled to target Windows version 6 (Vista), GetWindowRect() returns
	//the true dimensions, but this would cut out XP support from our app, which is
	//undesirable at this time. To work around this, we need to call the
	//DwmGetWindowAttribute() function, which explicitly returns the true window
	//dimensions.
	if(hwnd != 0)
	{
		RECT extendedWindowRect;
		HRESULT dwmGetWindowAttributeResult = DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, (LPVOID)&extendedWindowRect, sizeof(extendedWindowRect));
		if(dwmGetWindowAttributeResult == S_OK)
		{
			RECT windowRect;
			if(GetWindowRect(hwnd, &windowRect) != 0)
			{
				borderLeft = windowRect.left - extendedWindowRect.left;
				borderRight = extendedWindowRect.right - windowRect.right;
				borderTop = windowRect.top - extendedWindowRect.top;
				borderBottom = extendedWindowRect.bottom - windowRect.bottom;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//Internal implementation for CreateWindowThread
//----------------------------------------------------------------------------------------
CreateWindowThreadParams* AllocCreateWindowThreadParams()
{
	return new CreateWindowThreadParams();
}

//----------------------------------------------------------------------------------------
void ReleaseCreateWindowThreadParams(CreateWindowThreadParams* object)
{
	delete object;
}

//----------------------------------------------------------------------------------------
DWORD WINAPI CreateWindowThreadFunction(LPVOID aparams)
{
	CreateWindowThreadParams* params = (CreateWindowThreadParams*)aparams;

	HWND hwnd;
	hwnd = CreateWindowsWindow(params->hInstance, params->windowName, params->wndproc, params->width, params->height, params->params);
	*params->hwndPrivate = hwnd;
	*params->writtenHandle = true;
	if(hwnd != NULL)
	{
		WindowsMessageLoop(hwnd);
	}

	ReleaseCreateWindowThreadParams(params);
	return 0;
}

//----------------------------------------------------------------------------------------
void WindowsMessageLoop(HWND hwnd)
{
	if(hwnd != NULL)
	{
		ShowWindow(hwnd, SW_SHOWNORMAL);
		UpdateWindow(hwnd);

		MSG message;
		while(GetMessage(&message, hwnd, 0, 0) != 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	return;
}

//----------------------------------------------------------------------------------------
//Parent and owner window functions
//----------------------------------------------------------------------------------------
HWND SetWindowParent(HWND targetWindow, HWND newParent)
{
	//Check if we have an existing parent window. Note that we need to use the GetAncestor
	//function here rather than GetParent, as GetParent returns the owner window instead
	//of the parent window if it has been defined. Also note that GetAncestor, unlike
	//GetParent, returns the desktop window as a root parent window for all top-level
	//windows, where GetParent returns NULL in this case, so we need to test if the
	//returned parent window is the desktop window to identify top-level windows.
	HWND existingParent = GetAncestor(targetWindow, GA_PARENT);
	HWND desktopWindow = GetDesktopWindow();
	bool hasExistingParent = ((existingParent != NULL) && (existingParent != desktopWindow));

	//If the target window is currently a top-level window and is about to become a child
	//window, alter any state which needs to be modified.
	if(!hasExistingParent && (newParent != NULL))
	{
		//Remove any current owner window bound to the target window. We need to do this,
		//because it's apparently invalid for a window to have both an owner and a parent,
		//but if a window already has an owner and no parent, and then SetParent is called
		//to assign a parent, the owner window is not removed, and this causes the
		//GetParent function to actually return the owner window, not the new parent
		//window. Also note that despite the naming of the GWL_HWNDPARENT flag, this flag
		//will change the owner window if the target window doesn't currently have a
		//parent window, otherwise it will change the parent window. Since we've confirmed
		//there's no parent window currently, we can safely clear it here to remove the
		//owner.
		SetWindowLongPtr(targetWindow, GWLP_HWNDPARENT, NULL);

		//Clear the WS_POPUP window style, and set the WS_CHILD window style. MSDN docs
		//state this should be done before calling SetParent when a previously top-level
		//window is being assigned as a child window.
		LONG_PTR currentWindowStyle = GetWindowLongPtr(targetWindow, GWL_STYLE);
		LONG_PTR newWindowStyle = (currentWindowStyle & ~WS_POPUP) | WS_CHILD;
		SetWindowLongPtr(targetWindow, GWL_STYLE, newWindowStyle);

		//Since we've changed window style data, trigger a notification of the change to
		//flush any cached state. This may not be required for these changes, but
		//documentation isn't clear, and warns that some changes made by SetWindowLongPtr
		//won't take effect until we trigger this update, so we do it here for safety.
		SetWindowPos(targetWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	//Set the new parent window for the target control
	SetParent(targetWindow, newParent);

	//If the target window is was a child window and has just become a top-level window,
	//alter any state which needs to be modified.
	if(hasExistingParent && (newParent == NULL))
	{
		//Clear the WS_CHILD window style, and set the WS_POPUP window style. MSDN docs
		//state this should be done after calling SetParent when what was previously a
		//child window is becoming a top-level window.
		LONG_PTR currentWindowStyle = GetWindowLongPtr(targetWindow, GWL_STYLE);
		LONG_PTR newWindowStyle = (currentWindowStyle & ~WS_CHILD) | WS_POPUP;
		SetWindowLongPtr(targetWindow, GWL_STYLE, newWindowStyle);

		//Since we've changed window style data, trigger a notification of the change to
		//flush any cached state. This may not be required for these changes, but
		//documentation isn't clear, and warns that some changes made by SetWindowLongPtr
		//won't take effect until we trigger this update, so we do it here for safety.
		SetWindowPos(targetWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	//##FIX## According to MSDN docs under the SetParent function:
	//"When you change the parent of a window, you should synchronize the UISTATE of both
	// windows. For more information, see WM_CHANGEUISTATE and WM_UPDATEUISTATE."

	//Return the existing parent of the target window
	return (hasExistingParent)? existingParent: NULL;
}

//----------------------------------------------------------------------------------------
HWND GetFirstOwnerWindow(HWND targetWindow)
{
	//Return the owner of the target window, if present.
	HWND ownerWindow = GetWindow(targetWindow, GW_OWNER);
	if(ownerWindow != NULL)
	{
		return ownerWindow;
	}

	//Check if we have an existing parent window. Note that we need to use the GetAncestor
	//function here rather than GetParent, as GetParent returns the owner window instead
	//of the parent window if it has been defined. Also note that GetAncestor, unlike
	//GetParent, returns the desktop window as a root parent window for all top-level
	//windows, where GetParent returns NULL in this case, so we need to test if the
	//returned parent window is the desktop window to identify top-level windows.
	HWND existingParent = GetAncestor(targetWindow, GA_PARENT);
	HWND desktopWindow = GetDesktopWindow();
	bool hasExistingParent = ((existingParent != NULL) && (existingParent != desktopWindow));

	//Return the owner of the parent window, if present.
	if(hasExistingParent)
	{
		return GetFirstOwnerWindow(existingParent);
	}

	//Since no owner window could be found, return NULL;
	return NULL;
}

//----------------------------------------------------------------------------------------
HWND GetFirstOwnerWindowOrTopLevelParent(HWND targetWindow)
{
	//Return the first owner window at the top of the chain of parent windows for the
	//target window, if present.
	HWND firstOwnerWindow = GetFirstOwnerWindow(targetWindow);
	if(firstOwnerWindow != NULL)
	{
		return firstOwnerWindow;
	}

	//If the target window has no owner window at the top of the parent chain, return the
	//top level parent window of the target window, if present.
	//##TODO## Confirm if this method returns the target window if there are no parent
	//windows
	HWND topLevelParentWindow = GetAncestor(targetWindow, GA_ROOT);
	if(topLevelParentWindow != NULL)
	{
		return topLevelParentWindow;
	}

	//If the target window has no parent or owner windows, return the target window.
	return targetWindow;
}

//----------------------------------------------------------------------------------------
void SetOwnerWindow(HWND targetWindow, HWND newOwnerWindow)
{
	//Check if we have an existing parent window. Note that we need to use the GetAncestor
	//function here rather than GetParent, as GetParent returns the owner window instead
	//of the parent window if it has been defined. Also note that GetAncestor, unlike
	//GetParent, returns the desktop window as a root parent window for all top-level
	//windows, where GetParent returns NULL in this case, so we need to test if the
	//returned parent window is the desktop window to identify top-level windows.
	HWND existingParent = GetAncestor(targetWindow, GA_PARENT);
	HWND desktopWindow = GetDesktopWindow();
	bool hasExistingParent = ((existingParent != NULL) && (existingParent != desktopWindow));

	//If the target window doesn't have a parent window, set the owner window for the
	//target window. We ensure no parent exists first because it's invalid for a window to
	//have both an owner and a parent, and problems arise when querying parents or testing
	//if windows are children if a child window ends up with an owner set. Note that
	//despite the naming of the GWL_HWNDPARENT flag, this flag will change the owner
	//window if the target window doesn't currently have a parent window, otherwise it
	//will change the parent window. Since we've confirmed there's no parent window
	//currently, we can safely set it here to assign the new owner window.
	if(!hasExistingParent)
	{
		SetWindowLongPtr(targetWindow, GWLP_HWNDPARENT, (LONG_PTR)newOwnerWindow);
	}
}

//----------------------------------------------------------------------------------------
//General window helper functions
//----------------------------------------------------------------------------------------
std::wstring GetClassName(HWND targetWindow)
{
	//Retrieve the class name of the target window. Note that according to MSDN, the
	//maximum supported length of a window class name is 256. Since the GetClassName
	//function doesn't give us an easy way to determine the actual length of the string,
	//we use a buffer of this maximum size in order to retrieve the window class name
	//without truncation.
	static const unsigned int classNameBufferSize = 256+1;
	wchar_t classNameBuffer[classNameBufferSize];
	int getClassNameReturn = GetClassName(targetWindow, &classNameBuffer[0], (int)classNameBufferSize);
	if(getClassNameReturn == 0)
	{
		return L"";
	}

	//Return the class name as a string
	return classNameBuffer;
}

//----------------------------------------------------------------------------------------
//Control text helper functions
//----------------------------------------------------------------------------------------
void UpdateDlgItemBin(HWND hwnd, int controlID, unsigned int data)
{
	const unsigned int maxTextLength = 1024;
	wchar_t currentTextTemp[maxTextLength];
	if(GetDlgItemText(hwnd, controlID, currentTextTemp, maxTextLength) == 0)
	{
		currentTextTemp[0] = L'\0';
	}
	std::wstring currentText = currentTextTemp;
	std::wstringstream text;
	text << data;
	if(text.str() != currentText)
	{
		SetDlgItemText(hwnd, controlID, text.str().c_str());
	}
}

//----------------------------------------------------------------------------------------
unsigned int GetDlgItemBin(HWND hwnd, int controlID)
{
	unsigned int value = 0;

	const unsigned int maxTextLength = 1024;
	wchar_t currentTextTemp[maxTextLength];
	if(GetDlgItemText(hwnd, controlID, currentTextTemp, maxTextLength) == 0)
	{
		currentTextTemp[0] = L'\0';
	}
	std::wstringstream buffer;
	buffer << currentTextTemp;
	buffer >> value;

	return value;
}

//----------------------------------------------------------------------------------------
void UpdateDlgItemHex(HWND hwnd, int controlID, unsigned int width, unsigned int data)
{
	const unsigned int maxTextLength = 1024;
	wchar_t currentTextTemp[maxTextLength];
	if(GetDlgItemText(hwnd, controlID, currentTextTemp, maxTextLength) == 0)
	{
		currentTextTemp[0] = L'\0';
	}
	std::wstring currentText = currentTextTemp;
	std::wstringstream text;
	text << std::setw(width) << std::setfill(L'0') << std::hex << std::uppercase;
	text << data;
	if(text.str() != currentText)
	{
		SetDlgItemText(hwnd, controlID, text.str().c_str());
	}
}

//----------------------------------------------------------------------------------------
unsigned int GetDlgItemHex(HWND hwnd, int controlID)
{
	unsigned int value = 0;

	const unsigned int maxTextLength = 1024;
	wchar_t currentTextTemp[maxTextLength];
	if(GetDlgItemText(hwnd, controlID, currentTextTemp, maxTextLength) == 0)
	{
		currentTextTemp[0] = L'\0';
	}
	std::wstringstream buffer;
	buffer << std::hex << currentTextTemp;
	buffer >> value;

	return value;
}

//----------------------------------------------------------------------------------------
void UpdateDlgItemFloat(HWND hwnd, int controlID, float data)
{
	const unsigned int maxTextLength = 1024;
	wchar_t currentTextTemp[maxTextLength];
	if(GetDlgItemText(hwnd, controlID, currentTextTemp, maxTextLength) == 0)
	{
		currentTextTemp[0] = L'\0';
	}
	std::wstring currentText = currentTextTemp;
	std::wstringstream text;
//	text << std::fixed << std::setprecision(10);
	text << std::setprecision(10);
	text << data;
	if(text.str() != currentText)
	{
		SetDlgItemText(hwnd, controlID, text.str().c_str());
	}
}

//----------------------------------------------------------------------------------------
float GetDlgItemFloat(HWND hwnd, int controlID)
{
	float value = 0;

	const unsigned int maxTextLength = 1024;
	wchar_t currentTextTemp[maxTextLength];
	if(GetDlgItemText(hwnd, controlID, currentTextTemp, maxTextLength) == 0)
	{
		currentTextTemp[0] = L'\0';
	}
	std::wstringstream buffer;
	buffer << currentTextTemp;
	buffer >> value;

	return value;
}

//----------------------------------------------------------------------------------------
void UpdateDlgItemDouble(HWND hwnd, int controlID, double data)
{
	const unsigned int maxTextLength = 1024;
	wchar_t currentTextTemp[maxTextLength];
	if(GetDlgItemText(hwnd, controlID, currentTextTemp, maxTextLength) == 0)
	{
		currentTextTemp[0] = L'\0';
	}
	std::wstring currentText = currentTextTemp;
	std::wstringstream text;
	//	text << std::fixed << std::setprecision(10);
	text << std::setprecision(10);
	text << data;
	if(text.str() != currentText)
	{
		SetDlgItemText(hwnd, controlID, text.str().c_str());
	}
}

//----------------------------------------------------------------------------------------
double GetDlgItemDouble(HWND hwnd, int controlID)
{
	double value = 0;

	const unsigned int maxTextLength = 1024;
	wchar_t currentTextTemp[maxTextLength];
	if(GetDlgItemText(hwnd, controlID, currentTextTemp, maxTextLength) == 0)
	{
		currentTextTemp[0] = L'\0';
	}
	std::wstringstream buffer;
	buffer << currentTextTemp;
	buffer >> value;

	return value;
}

//----------------------------------------------------------------------------------------
void UpdateDlgItemString(HWND hwnd, int controlID, const std::wstring& data)
{
	SetDlgItemText(hwnd, controlID, data.c_str());
}

//----------------------------------------------------------------------------------------
std::wstring GetDlgItemString(HWND hwnd, int controlID)
{
	std::wstring result;

	const unsigned int maxTextLength = 1024;
	wchar_t currentTextTemp[maxTextLength];
	if(GetDlgItemText(hwnd, controlID, currentTextTemp, maxTextLength) == 0)
	{
		currentTextTemp[0] = L'\0';
	}
	result = currentTextTemp;

	return result;
}

//----------------------------------------------------------------------------------------
//Modal window functions
//----------------------------------------------------------------------------------------
std::list<HWND> DisableAllEnabledDialogWindows(HWND ownerWindow)
{
	//Retrieve the current list of all popup and dialog windows owned by the target window
	std::list<HWND> ownedWindowList = GetOwnedDialogWindows(ownerWindow);

	//Disable any owned popup or dialog windows which are currently enabled, and build a
	//list of all the windows that were actually disabled.
	std::list<HWND> disabledWindows;
	for(std::list<HWND>::const_iterator i = ownedWindowList.begin(); i != ownedWindowList.end(); ++i)
	{
		HWND targetWindow = *i;
		if(EnableWindow(targetWindow, FALSE) == 0)
		{
			disabledWindows.push_back(targetWindow);
		}
	}

	//Return the list of windows which were disabled to the caller
	return disabledWindows;
}

//----------------------------------------------------------------------------------------
void EnableDialogWindows(const std::list<HWND>& windowList)
{
	//Enable all windows in the supplied window list
	for(std::list<HWND>::const_iterator i = windowList.begin(); i != windowList.end(); ++i)
	{
		HWND targetWindow = *i;
		EnableWindow(targetWindow, TRUE);
	}
}

//----------------------------------------------------------------------------------------
int SafeMessageBox(HWND hwnd, const std::wstring& message, const std::wstring& title, UINT type)
{
	//Disable all other popup or dialog windows for the target parent window that are not
	//currently disabled. We need to do this, because although the modal dialog box will
	//disable the parent window, it will not disable other popup windows of the parent,
	//and the user will still be able to interact with them while our dialog is open.
	std::list<HWND> disabledWindows = DisableAllEnabledDialogWindows(hwnd);

	//Display the message box to the user
	int messageBoxReturn = MessageBox(hwnd, message.c_str(), title.c_str(), type);

	//Re-enable all other popup or dialog windows which we disabled
	EnableDialogWindows(disabledWindows);

	//Return the result of the message box to the caller
	return messageBoxReturn;
}

//----------------------------------------------------------------------------------------
INT_PTR SafeDialogBox(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc)
{
	//Disable all other popup or dialog windows for the target parent window that are not
	//currently disabled. We need to do this, because although the modal dialog box will
	//disable the parent window, it will not disable other popup windows of the parent,
	//and the user will still be able to interact with them while our dialog is open.
	std::list<HWND> disabledWindows = DisableAllEnabledDialogWindows(hWndParent);

	//Display the modal dialog to the user
	INT_PTR dialogBoxReturn = DialogBox(hInstance, lpTemplate, hWndParent, lpDialogFunc);

	//Re-enable all other popup or dialog windows which we disabled
	EnableDialogWindows(disabledWindows);

	//Return the result of the dialog to the caller
	return dialogBoxReturn;
}

//----------------------------------------------------------------------------------------
INT_PTR SafeDialogBoxParam(HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	//Disable all other popup or dialog windows for the target parent window that are not
	//currently disabled. We need to do this, because although the modal dialog box will
	//disable the parent window, it will not disable other popup windows of the parent,
	//and the user will still be able to interact with them while our dialog is open.
	std::list<HWND> disabledWindows = DisableAllEnabledDialogWindows(hWndParent);

	//Display the modal dialog to the user
	INT_PTR dialogBoxReturn = DialogBoxParam(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);

	//Re-enable all other popup or dialog windows which we disabled
	EnableDialogWindows(disabledWindows);

	//Return the result of the dialog to the caller
	return dialogBoxReturn;
}

//----------------------------------------------------------------------------------------
INT_PTR SafeDialogBoxIndirect(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc)
{
	//Disable all other popup or dialog windows for the target parent window that are not
	//currently disabled. We need to do this, because although the modal dialog box will
	//disable the parent window, it will not disable other popup windows of the parent,
	//and the user will still be able to interact with them while our dialog is open.
	std::list<HWND> disabledWindows = DisableAllEnabledDialogWindows(hWndParent);

	//Display the modal dialog to the user
	INT_PTR dialogBoxReturn = DialogBoxIndirect(hInstance, lpTemplate, hWndParent, lpDialogFunc);

	//Re-enable all other popup or dialog windows which we disabled
	EnableDialogWindows(disabledWindows);

	//Return the result of the dialog to the caller
	return dialogBoxReturn;
}

//----------------------------------------------------------------------------------------
INT_PTR SafeDialogBoxIndirectParam(HINSTANCE hInstance, LPCDLGTEMPLATE hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	//Disable all other popup or dialog windows for the target parent window that are not
	//currently disabled. We need to do this, because although the modal dialog box will
	//disable the parent window, it will not disable other popup windows of the parent,
	//and the user will still be able to interact with them while our dialog is open.
	std::list<HWND> disabledWindows = DisableAllEnabledDialogWindows(hWndParent);

	//Display the modal dialog to the user
	INT_PTR dialogBoxReturn = DialogBoxIndirectParam(hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam);

	//Re-enable all other popup or dialog windows which we disabled
	EnableDialogWindows(disabledWindows);

	//Return the result of the dialog to the caller
	return dialogBoxReturn;
}

//----------------------------------------------------------------------------------------
//Window enumeration
//----------------------------------------------------------------------------------------
struct EnumThreadWindowsCallbackParams
{
	std::list<HWND> windowList;
	HWND ownerWindow;
};

//----------------------------------------------------------------------------------------
BOOL CALLBACK EnumThreadWindowsCallback(HWND hwnd, LPARAM lParam)
{
	//If this window has our target window as its owner, add it to the window list.
	EnumThreadWindowsCallbackParams& params = *((EnumThreadWindowsCallbackParams*)lParam);
	HWND ownerWindow = GetWindow(hwnd, GW_OWNER);
	if(ownerWindow == params.ownerWindow)
	{
		params.windowList.push_back(hwnd);
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------
BOOL CALLBACK EnumDescendantWindowsCallback(HWND hwnd, LPARAM lParam)
{
	std::list<HWND>& descendantWindows = *((std::list<HWND>*)lParam);
	descendantWindows.push_back(hwnd);
	return TRUE;
}

//----------------------------------------------------------------------------------------
std::list<HWND> GetOwnedDialogWindows(HWND ownerWindow)
{
	//Return the list of all windows which are owned by the target window
	EnumThreadWindowsCallbackParams params;
	params.ownerWindow = ownerWindow;
	DWORD ownerWindowThreadID = GetWindowThreadProcessId(ownerWindow, NULL);
	EnumThreadWindows(ownerWindowThreadID, EnumThreadWindowsCallback, (LPARAM)&params);
	return params.windowList;
}

//----------------------------------------------------------------------------------------
std::list<HWND> GetDescendantWindows(HWND targetWindow)
{
	//Return the list of all descendant windows under the target window
	std::list<HWND> descendantWindows;
	EnumChildWindows(targetWindow, EnumDescendantWindowsCallback, (LPARAM)&descendantWindows);
	return descendantWindows;
}

//----------------------------------------------------------------------------------------
//Tooltip functions
//----------------------------------------------------------------------------------------
HWND CreateTooltipControl(HINSTANCE moduleHandle, HWND hwndParent, unsigned int maxWidth)
{
	//Initialize common controls
	INITCOMMONCONTROLSEX iccex;
	iccex.dwICC = ICC_BAR_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	if(InitCommonControlsEx(&iccex) == FALSE)
	{
		return NULL;
	}

	//Create a tooltip window
	HWND hwndTooltip;
	hwndTooltip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOFADE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndParent, NULL, moduleHandle, NULL);
	if(hwndTooltip == NULL)
	{
		return NULL;
	}

	//This little hackaround here allows tooltips to work properly when used around OpenGL
	//and Direct3D windows. Tooltip windows all have the WS_EX_LAYERED window style
	//manually set by the tooltip window procedure after creation. This is done to enable
	//support for fading tooltips, but unfortunately, the window style is always added,
	//regardless of whether fading is enabled for the tooltip control or not. Layered
	//windows do not work correctly when they overlap a surface rendered using methods
	//which bypass the normal Windows GDI interface, in particular, they are fundamentally
	//incompatible with surfaces which render an image by writing directly to the
	//framebuffer, as is the case with hardware accelerated interfaces. What happens when
	//a tooltip window overlaps such as surface is that the tooltip control appears in the
	//overlapped region for a very brief period, but the next time the surface below
	//redraws, the region of the tooltip which overlaps the surface is cleared, and the
	//contents of the surface are displayed in that region instead. To work around this
	//problem, we have set the TTS_NOFADE window style for the tooltip control, which
	//removes the need for the WS_EX_LAYERED extended window style. We then read the real
	//extended window style of the tooltip control after it has been created, and
	//explicitly clear the WS_EX_LAYERED style. The new extended window style is then
	//re-applied, and all cached data regarding the old extended window style is cleared
	//after the SetWindowPos call below. This solution has been tested on Windows XP SP3
	//and Windows Vista SP1, and solves the problems with tooltips and OpenGL surfaces,
	//with no apparent side-effects.
	LONG_PTR extendedWindowStyle = GetWindowLongPtr(hwndTooltip, GWL_EXSTYLE);
	if(extendedWindowStyle != 0)
	{
		extendedWindowStyle &= ~((LONG_PTR)WS_EX_LAYERED);
		SetWindowLongPtr(hwndTooltip, GWL_EXSTYLE, extendedWindowStyle);
	}

	//Set the tooltip window as topmost
	SetWindowPos(hwndTooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	//Set the maximum width for the tooltip window
	SendMessage(hwndTooltip, TTM_SETMAXTIPWIDTH, 0, (LPARAM)maxWidth);

	//Set the time-out periods for the tooltip window
	SendMessage(hwndTooltip, TTM_SETDELAYTIME, (WPARAM)TTDT_AUTOPOP, (LPARAM)MAKELONG(0x7FFF, 0));
	SendMessage(hwndTooltip, TTM_SETDELAYTIME, (WPARAM)TTDT_INITIAL, (LPARAM)0);
	SendMessage(hwndTooltip, TTM_SETDELAYTIME, (WPARAM)TTDT_RESHOW, (LPARAM)0);

	return hwndTooltip;
}

//----------------------------------------------------------------------------------------
BOOL AddTooltip(HINSTANCE moduleHandle, HWND hwndTooltip, HWND hwndParent, int targetControlID, const std::wstring& text, bool createAnchor, const std::wstring& anchorLink)
{
	//Get the handle of the target control
	HWND hwndControl = GetDlgItem(hwndParent, targetControlID);
	if(hwndControl == NULL)
	{
		return FALSE;
	}

	if(createAnchor)
	{
		//Obtain the client position of the parent window
		POINT clientPos;
		clientPos.x = 0;
		clientPos.y = 0;
		if(ClientToScreen(hwndParent, &clientPos) == 0)
		{
			return FALSE;
		}

		//Calculate the dimensions of the target control in client coordinates
		RECT rect;
		if(GetWindowRect(hwndControl, &rect) == 0)
		{
			return FALSE;
		}
		rect.top -= clientPos.y;
		rect.bottom -= clientPos.y;
		rect.left -= clientPos.x;
		rect.right -= clientPos.x;

		//Calculate the initial position and size for the anchor control based on the
		//dimensions of the target control
		int anchorPosX = (int)rect.left;
		int anchorPosY = (int)rect.top;
		int anchorWidth = (int)(rect.right - rect.left);
		int anchorHeight = (int)(rect.bottom - rect.top);

		//Initialize the SysLink common control
		INITCOMMONCONTROLSEX iccex;
		iccex.dwICC = ICC_LINK_CLASS;
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		if(InitCommonControlsEx(&iccex) == FALSE)
		{
			return FALSE;
		}

		//Create a SysLink control as an anchor for the tooltip over the target control
		std::wstring sysLinkTarget = L"<a href=\"" + anchorLink + L"\">[?]</a>";
		hwndControl = CreateWindowEx(0, WC_LINK, &sysLinkTarget[0], WS_CHILD | WS_TABSTOP, anchorPosX, anchorPosY, anchorWidth, anchorHeight, hwndParent, NULL, moduleHandle, NULL);
		if(hwndControl == NULL)
		{
			return FALSE;
		}

		//Propagate the currently selected font from the parent window to the SysLink
		//control. Without this step, the text in the anchor will use the default raster
		//font, which not only looks ugly, it doesn't scale based on the screen DPI
		//settings.
		HFONT parentWindowFontHandle = (HFONT)SendMessage(hwndParent, WM_GETFONT, 0, 0);
		SendMessage(hwndControl, WM_SETFONT, (WPARAM)parentWindowFontHandle, (LPARAM)FALSE);

		//Adjust the anchor position and dimensions to ensure the full text is visible
		int preferredHeight = (int)SendMessage(hwndControl, LM_GETIDEALHEIGHT, 0, 0);
		if(anchorHeight < preferredHeight)
		{
			anchorPosY -= (preferredHeight - anchorHeight) / 2;
			anchorHeight = preferredHeight;
		}
		if(anchorWidth < anchorHeight)
		{
			//The horizontal adjustment here is... well... a bit of a guess really. The
			//LM_GETIDEALHEIGHT message above allows us to determine the correct height
			//for the control, but there's no assistance to calculate an ideal width for
			//a given string. We aim for a square dimension to the control here, which
			//works perfectly for the default visual settings on all versions of Windows
			//tested up to and including Vista, but the proper width for the control is
			//dependent on the font settings. This may require adjustment in the future.
			anchorWidth = anchorHeight;
		}
		if(MoveWindow(hwndControl, anchorPosX, anchorPosY, anchorWidth, anchorHeight, FALSE) == 0)
		{
			return FALSE;
		}

		//Now that the anchor has been positioned and sized correctly, make the control
		//visible.
		ShowWindow(hwndControl, SW_SHOWNORMAL);
	}

	//Define the properties of the tooltip
	TOOLINFO toolInfo;
	toolInfo.cbSize = sizeof(TOOLINFO);
	toolInfo.lpReserved = 0;
	toolInfo.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
	toolInfo.hwnd = hwndParent;
	toolInfo.hinst = moduleHandle;
	toolInfo.uId = (UINT_PTR)hwndControl;
	toolInfo.lpszText = (LPTSTR)text.c_str();

	//Register the tooltip with the tooltip control
	BOOL addToolResult = (BOOL)SendMessage(hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	return addToolResult;
}

//----------------------------------------------------------------------------------------
//Child window message bounce back
//----------------------------------------------------------------------------------------
LRESULT CALLBACK BounceBackSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	BounceMessage params(hwnd, uMsg, wParam, lParam);
	SendMessage(GetParent(hwnd), WM_BOUNCE, 0, (LPARAM)&params);
	if(params.caught)
	{
		return params.result;
	}
	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

//----------------------------------------------------------------------------------------
//Edit control extensions
//----------------------------------------------------------------------------------------
LRESULT CALLBACK EditBoxFocusFixSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	//##HACK##
	//There is a bug in the Windows edit control, which is acknowledged by Microsoft and
	//documented under KB230587. This bug is described in the following article:
	//http://support.microsoft.com/kb/230587/en-us
	//Unfortunately this bug will never be fixed, and we need to apply a workaround for it
	//here. The problem is that due to poor implementation, if an edit control has an
	//ancestor parent window that is a child window (has the WS_CHILD window style), and
	//has a caption bar (has the WS_CAPTION window style), the edit control ignores mouse
	//input events targeted at it. We work around this problem by detecting messages which
	//would be affected, and temporarily removing the WS_CAPTION window style from any
	//parent windows which are themselves child windows, and restoring the style after the
	//message is processed by the edit control, with some extra effort to prevent control
	//redraw artifacts as much as we can.
	//
	//This is an imperfect solution, and OpenGL child windows will still exhibit
	//flickering on redraw. It is possible that visual artifacts or flickering could occur
	//on different versions of Windows or with certain child windows. Ideally the root of
	//the problem would be fixed, and Microsoft would patch their code to only examine the
	//enabled state of non-child captioned parent windows, but that's never going to
	//happen. This bug has been known for well over a decade.
	//
	//For future reference, some more detail about this problem is as follows:
	//-The offending code is contained within comctl32.dll
	//-The offending function is named Edit_IsAncestorActive, according to the debug
	// symbols file provided by Microsoft.
	//-The Edit_IsAncestorActive function is called in two places, both from the
	// Edit_WndProc function, one from the WM_CONTEXTMENU handler, and another from the
	// WM_LBUTTONDOWN handler.
	//-The approximate code for the offending function from a disassembly analysis is as
	// follows:
	//	bool Edit_IsAncestorActive(HWND targetWindow)
	//	{
	//		bool result = true;
	//		HWND ancestorWindow = targetWindow;
	//		while(ancestorWindow != NULL)
	//		{
	//			//Undocumented value of -1 used as index, unknown internal data structure
	//			//returned.
	//			unsigned char* unknownData = (unsigned char*)GetWindowLongPtr(v1, -1);
	//			if((*((unsigned int*)(unknownData + 4)) & 0x200) == 0)
	//			{
	//				break;
	//			}
	//			if((*((unsigned int*)(unknownData + 12)) & WS_CHILD) == 0)
	//			{
	//				break;
	//			}
	//			if((*unknownData & 8) != 0) //If the window has a caption?
	//			{
	//				result = (((*unknownData >> 6) & 0x01) != 0); //Return the enabled state of the caption?
	//				break;
	//			}
	//			ancestorWindow = GetParent(ancestorWindow);
	//		}
	//		return result;
	//	}

	//If this message is one of the messages affected by the disabled captioned parent
	//window bug, process the message and implement our workaround.
	if((uMsg == WM_CONTEXTMENU) || (uMsg == WM_LBUTTONDOWN))
	{
		//Build a list of parent windows which have the caption window style, and are
		//child windows themselves.
		std::list<HWND> parentChildWindowsWithCaption;
		HWND parentWindow = GetAncestor(hwnd, GA_PARENT);
		while(parentWindow != NULL)
		{
			DWORD parentWindowStyle = (DWORD)GetWindowLongPtr(parentWindow, GWL_STYLE);
			if(((parentWindowStyle & WS_CAPTION) != 0) && ((parentWindowStyle & WS_CHILD) != 0))
			{
				parentChildWindowsWithCaption.push_back(parentWindow);
			}
			parentWindow = GetAncestor(parentWindow, GA_PARENT);
		}

		//If the window which has focus before processing this mouse event is different to
		//the target window, and it is an edit control too, flag it for redrawing.
		HWND previousFocusWindow = GetFocus();
		bool redrawPreviousFocusWindow = GetClassName(previousFocusWindow) == WC_EDIT;//((previousFocusWindow != hwnd) && (GetClassName(previousFocusWindow) == WC_EDIT));

		//Remove the WS_CAPTION window style from any parent windows which contain it, to
		//allow the edit control to process this mouse message properly.
		for(std::list<HWND>::const_iterator i = parentChildWindowsWithCaption.begin(); i != parentChildWindowsWithCaption.end(); ++i)
		{
			//Disable redrawing on the parent caption window. We do this because we're
			//about to change the border style, which will prompt a redraw of the client
			//and non-client region of the window. We want to disable redrawing to prevent
			//noticeable flicker as the style changes, since we're about to change it back
			//again almost immediately.
			HWND parentCaptionWindow = *i;
			SendMessage(parentCaptionWindow, WM_SETREDRAW, FALSE, NULL);

			//Remove the caption window style. This allows edit controls to process mouse
			//events properly.
			LONG_PTR currentWindowStyle = GetWindowLongPtr(parentCaptionWindow, GWL_STYLE);
			LONG_PTR newWindowStyle = (currentWindowStyle & ~WS_CAPTION);
			SetWindowLongPtr(parentCaptionWindow, GWL_STYLE, newWindowStyle);
		}

		//Allow the default window procedure to process this message
		LRESULT result = DefSubclassProc(hwnd, uMsg, wParam, lParam);

		//Restore the window style for any affected parent windows
		for(std::list<HWND>::const_iterator i = parentChildWindowsWithCaption.begin(); i != parentChildWindowsWithCaption.end(); ++i)
		{
			//Restore the caption window style
			HWND parentCaptionWindow = *i;
			LONG_PTR currentWindowStyle = GetWindowLongPtr(parentCaptionWindow, GWL_STYLE);
			LONG_PTR newWindowStyle = (currentWindowStyle | WS_CAPTION);
			SetWindowLongPtr(parentCaptionWindow, GWL_STYLE, newWindowStyle);

			//Now that we've changed the border style, call SetWindowPos to ensure the
			//change is applied. If the change wasn't inadvertently triggered during the
			//edit control message processing, this should have no real effect, since the
			//frame has been restored to what it was before, but if the frame change has
			//been applied, we need to ensure it is restored correctly.
			SetWindowPos(parentCaptionWindow, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);

			//Re-enable redrawing on the parent caption window
			SendMessage(parentCaptionWindow, WM_SETREDRAW, TRUE, NULL);
		}

		//Redraw the previous window to have focus, if requested.
		if(redrawPreviousFocusWindow)
		{
			RedrawWindow(previousFocusWindow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}

		//Return the result returned by the default window procedure
		return result;
	}

	//If this message is unaffected by the bug, process it normally.
	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

//----------------------------------------------------------------------------------------
//Static control extensions
//----------------------------------------------------------------------------------------
LRESULT CALLBACK ResizableStaticControlSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	//As a bit of an oversight, the win32 static text control doesn't invalidate its
	//content region when word wrapping is active, even when the text wrapping position
	//changes as a result of a window size change. To resolve this problem, after
	//processing a WM_SIZE event, we invalidate the client region to force the entire
	//client area to redraw.
	LRESULT result = DefSubclassProc(hwnd, uMsg, wParam, lParam);
	if(uMsg == WM_SIZE)
	{
		//Note that the static control doesn't use a full bitmask for its style, but
		//instead has a set of mutually exclusive "base" styles that use the lower 5 bits
		//of the window style. We test for each of these styles that applies word
		//wrapping, and invalidate the content region if one of them is active.
		unsigned int windowStyle = (unsigned int)GetWindowLongPtr(hwnd, GWL_STYLE);
		unsigned int baseStaticControlStyle = (windowStyle & 0x1F);
		if((baseStaticControlStyle == SS_LEFT) || (baseStaticControlStyle == SS_RIGHT) || (baseStaticControlStyle == SS_CENTER))
		{
			InvalidateRect(hwnd, NULL, FALSE);
		}
	}
	return result;
}

//----------------------------------------------------------------------------------------
//Highlight extensions
//----------------------------------------------------------------------------------------
void PaintCheckboxHighlight(HWND hwnd)
{
	//Read the dimensions of the control
	RECT rect;
	if(GetClientRect(hwnd, &rect) == 0)
	{
		return;
	}
	int controlWidth = rect.right;
	int controlHeight = rect.bottom;

	//Calculate the position and dimensions of the checkbox within the control
	int checkBoxBorderX = GetSystemMetrics(SM_CXBORDER);
	int checkBoxBorderY = GetSystemMetrics(SM_CYBORDER);
	int checkBoxContentSizeX = DPIScaleWidth(11);  //These values are constant and cannot be calculated
	int checkBoxContentSizeY = DPIScaleHeight(11); //or modified through any known means in Windows XP.
	int checkBoxTotalSizeX = checkBoxContentSizeX + (2 * checkBoxBorderX);
	int checkBoxTotalSizeY = checkBoxContentSizeY + (2 * checkBoxBorderY);
	int checkPosStartX = 0;
	int checkPosStartY = (controlHeight - checkBoxTotalSizeY) / 2;
	int checkBoxEndX = checkPosStartX + checkBoxTotalSizeX;
	int checkBoxEndY = checkPosStartY + checkBoxTotalSizeY;

	//Create a bitmap we can render the control onto
	HDC hdcControl = GetDC(hwnd);
	if(hdcControl == NULL)
	{
		return;
	}
	HDC hdcBitmap = CreateCompatibleDC(hdcControl);
	if(hdcBitmap == NULL)
	{
		ReleaseDC(hwnd, hdcControl);
		return;
	}
	HBITMAP hbitmap = CreateCompatibleBitmap(hdcControl, controlWidth, controlHeight);
	if(hbitmap == NULL)
	{
		DeleteDC(hdcBitmap);
		ReleaseDC(hwnd, hdcControl);
		return;
	}
	HBITMAP hbitmapOriginal = (HBITMAP)SelectObject(hdcBitmap, hbitmap);
	if((hbitmapOriginal == NULL) || (hbitmapOriginal == HGDI_ERROR))
	{
		DeleteObject(hbitmap);
		DeleteDC(hdcBitmap);
		ReleaseDC(hwnd, hdcControl);
		return;
	}

	//Send a WM_PRINTCLIENT message to the native control and get it to render into
	//the bitmap.
	SendMessage(hwnd, WM_PRINTCLIENT, (WPARAM)hdcBitmap, PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);

	//Calculate the colour of the checkmark in the checkbox. We assume that when the
	//control is checked, the pixel in the exact centre will be marked, and that all
	//pixels in the marked region are the same colour.
//	bool checked = (SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED);
//	COLORREF checkColor = GetPixel(hdcBitmap, checkPosStartX + (checkBoxTotalSizeX / 2), checkPosStartY + (checkBoxTotalSizeY / 2));
	//##NOTE## We disabled the checkmark exclusion code, because like we suspected, the
	//checkmark is not a solid colour on Windows Vista. It doesn't look too bad when we
	//affect the checkmark anyway, and it's reliable.

	//Examine each pixel within the checkbox region of the control, and calculate the
	//final colour value.
	for(int x = checkPosStartX + checkBoxBorderX; x < (checkBoxEndX - checkBoxBorderX); ++x)
	{
		for(int y = checkPosStartY + checkBoxBorderY; y < (checkBoxEndY - checkBoxBorderY); ++y)
		{
			//Read the current colour value from the pixel
			COLORREF currentColor = GetPixel(hdcBitmap, x, y);

			if(currentColor != CLR_INVALID)
			{
				//Only modify the pixel if it isn't part of a checkmark
	//			if(!checked || (currentColor != checkColor))
	//			{
					//Halve the value in the green and blue channels, and save the
					//updated pixel back into the bitmap.
					COLORREF newColor = RGB(GetRValue(currentColor), GetGValue(currentColor)/2, GetBValue(currentColor)/2);
					SetPixel(hdcBitmap, x, y, newColor);
	//			}
			}
		}
	}

	//Transfer the final bitmap for the control into the screen buffer
	BitBlt(hdcControl, 0, 0, controlWidth, controlHeight, hdcBitmap, 0, 0, SRCCOPY);

	//Validate the entire client area of the control. It's REALLY important that we
	//call this here, otherwise Windows is going to send WM_PAINT messages over and
	//over again waiting for the region to be validated.
	ValidateRect(hwnd, NULL);

	//Clean up the allocated handles
	SelectObject(hdcBitmap, hbitmapOriginal);
	DeleteObject(hbitmap);
	DeleteDC(hdcBitmap);
	ReleaseDC(hwnd, hdcControl);
}

//----------------------------------------------------------------------------------------
//Icon helper functions
//-The following structures and types are sourced mainly from the following MSDN article:
// http://msdn.microsoft.com/en-us/library/ms997538.aspx
//-Some additions and alterations have been made, in particular the addition of the
// RTCURSORDATA structure, which is key to allow cursor files to be mapped to cursor
// resources. The layout of this structure is completely undocumented and unmentioned in
// the public domain as far as has been observed. The layout of this structure was
// determined by disassembling the CreateIconFromResourceEx function within user32.dll.
//----------------------------------------------------------------------------------------
enum ICONIMAGETYPE
{
	ICONIMAGETYPE_ICON = 1,
	ICONIMAGETYPE_CURSOR = 2
};

//----------------------------------------------------------------------------------------
struct ICONDIRENTRY
{
	BYTE        bWidth;          // Width, in pixels, of the image
	BYTE        bHeight;         // Height, in pixels, of the image
	BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
	BYTE        bReserved;       // Reserved ( must be 0)
	union
	{
		WORD   wPlanes;              // Color Plane count for icons
		WORD   wHotSpotX;            // Hotspot X coordinate for cursors
	};
	union
	{
		WORD   wBitCount;            // Bits per pixel for icons
		WORD   wHotSpotY;            // Hotspot Y coordinate for cursors
	};
	DWORD       dwBytesInRes;    // How many bytes in this resource?
	DWORD       dwImageOffset;   // Where in the file is this image?
};

//----------------------------------------------------------------------------------------
struct ICONDIR
{
	WORD           idReserved;   // Reserved (must be 0)
	WORD           idType;       // Resource Type (1 for icons)
	WORD           idCount;      // How many images?
	ICONDIRENTRY   idEntries[1]; // An entry for each image (idCount of 'em)
};

//----------------------------------------------------------------------------------------
struct ICONIMAGE
{
	BITMAPINFOHEADER   icHeader;      // DIB header
	RGBQUAD         icColors[1];   // Color table
	BYTE            icXOR[1];      // DIB bits for XOR mask
	BYTE            icAND[1];      // DIB bits for AND mask
};

//----------------------------------------------------------------------------------------
struct GRPICONDIRENTRY //(RT_GROUP_ICON and RT_GROUP_CURSOR resource data)
{
	BYTE   bWidth;               // Width, in pixels, of the image
	BYTE   bHeight;              // Height, in pixels, of the image
	BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE   bReserved;            // Reserved
	union
	{
		WORD   wPlanes;              // Color Plane count for icons
		WORD   wHotSpotX;            // Hotspot X coordinate for cursors
	};
	union
	{
		WORD   wBitCount;            // Bits per pixel for icons
		WORD   wHotSpotY;            // Hotspot Y coordinate for cursors
	};
	DWORD   dwBytesInRes;         // how many bytes in this resource?
	WORD   nID;                  // the ID
};

//----------------------------------------------------------------------------------------
struct GRPICONDIR
{
	WORD            idReserved;   // Reserved (must be 0)
	WORD            idType;       // Resource type (1 for icons)
	WORD            idCount;      // How many images?
	GRPICONDIRENTRY   idEntries[1]; // The entries for each image
};

//----------------------------------------------------------------------------------------
struct RTICONDATA //(RT_ICON resource data)
{
	ICONIMAGE iconImage;
};

//----------------------------------------------------------------------------------------
struct RTCURSORDATA //(RT_CURSOR resource data)
{
	WORD wHotSpotX;
	WORD wHotSpotY;
	ICONIMAGE iconImage;
};

//----------------------------------------------------------------------------------------
unsigned int GetIconFileEntryCount(Stream::IStream& iconFileStream)
{
	//Save the original stream seek position
	Stream::IStream::SizeType originalSeekPos = iconFileStream.GetStreamPos();

	//Read the icon file header
	bool result = true;
	ICONDIR iconDir;
	result &= iconFileStream.ReadDataLittleEndian(iconDir.idReserved);
	result &= iconFileStream.ReadDataLittleEndian(iconDir.idType);
	result &= iconFileStream.ReadDataLittleEndian(iconDir.idCount);
	if(!result)
	{
		iconDir.idCount = 0;
	}

	//Restore the original stream seek position
	iconFileStream.SetStreamPos(originalSeekPos);

	//Return the number of directory entries in this icon file
	return (unsigned int)iconDir.idCount;
}

//----------------------------------------------------------------------------------------
bool ConvertIconFileToIconResource(Stream::IStream& iconFileStream, Stream::IStream& iconDirectoryData, const std::map<int, Stream::IStream*>& iconResourceData)
{
	//Read the icon file header
	bool result = true;
	ICONDIR iconDir;
	result &= iconFileStream.ReadDataLittleEndian(iconDir.idReserved);
	result &= iconFileStream.ReadDataLittleEndian(iconDir.idType);
	result &= iconFileStream.ReadDataLittleEndian(iconDir.idCount);
	if(!result)
	{
		return false;
	}

	//Convert the icon file header to an icon resource header
	GRPICONDIR resourceIconDir;
	resourceIconDir.idReserved = iconDir.idReserved;
	resourceIconDir.idType = iconDir.idType;
	resourceIconDir.idCount = iconDir.idCount;

	//Write the icon resource header to our icon directory resource stream
	result &= iconDirectoryData.WriteDataLittleEndian(iconDir.idReserved);
	result &= iconDirectoryData.WriteDataLittleEndian(iconDir.idType);
	result &= iconDirectoryData.WriteDataLittleEndian(iconDir.idCount);
	if(!result)
	{
		return false;
	}

	//Convert each icon entry in the supplied icon file
	for(unsigned int i = 0; i < (unsigned int)iconDir.idCount; ++i)
	{
		//Read the next icon file directory entry
		ICONDIRENTRY iconDirEntry;
		result &= iconFileStream.ReadDataLittleEndian(iconDirEntry.bWidth);
		result &= iconFileStream.ReadDataLittleEndian(iconDirEntry.bHeight);
		result &= iconFileStream.ReadDataLittleEndian(iconDirEntry.bColorCount);
		result &= iconFileStream.ReadDataLittleEndian(iconDirEntry.bReserved);
		result &= iconFileStream.ReadDataLittleEndian(iconDirEntry.wPlanes);
		result &= iconFileStream.ReadDataLittleEndian(iconDirEntry.wBitCount);
		result &= iconFileStream.ReadDataLittleEndian(iconDirEntry.dwBytesInRes);
		result &= iconFileStream.ReadDataLittleEndian(iconDirEntry.dwImageOffset);
		if(!result)
		{
			return false;
		}

		//Convert this icon file directory entry structure into an icon resource directory
		//entry structure
		GRPICONDIRENTRY resourceIconDirEntry;
		resourceIconDirEntry.bWidth = iconDirEntry.bWidth;
		resourceIconDirEntry.bHeight = iconDirEntry.bHeight;
		resourceIconDirEntry.bColorCount = iconDirEntry.bColorCount;
		resourceIconDirEntry.bReserved = iconDirEntry.bReserved;
		resourceIconDirEntry.wPlanes = iconDirEntry.wPlanes;
		resourceIconDirEntry.wBitCount = iconDirEntry.wBitCount;
		resourceIconDirEntry.dwBytesInRes = iconDirEntry.dwBytesInRes;
		resourceIconDirEntry.nID = (WORD)i;

		//Load this icon file directory entry structure into the icon resource directory
		result &= iconDirectoryData.WriteDataLittleEndian(resourceIconDirEntry.bWidth);
		result &= iconDirectoryData.WriteDataLittleEndian(resourceIconDirEntry.bHeight);
		result &= iconDirectoryData.WriteDataLittleEndian(resourceIconDirEntry.bColorCount);
		result &= iconDirectoryData.WriteDataLittleEndian(resourceIconDirEntry.bReserved);
		result &= iconDirectoryData.WriteDataLittleEndian(resourceIconDirEntry.wPlanes);
		result &= iconDirectoryData.WriteDataLittleEndian(resourceIconDirEntry.wBitCount);
		result &= iconDirectoryData.WriteDataLittleEndian(resourceIconDirEntry.dwBytesInRes);
		result &= iconDirectoryData.WriteDataLittleEndian(resourceIconDirEntry.nID);
		if(!result)
		{
			return false;
		}

		//Ensure this icon entry has a valid location and size specified within the
		//supplied icon file
		if(((size_t)iconDirEntry.dwImageOffset + (size_t)iconDirEntry.dwBytesInRes) > (size_t)iconFileStream.Size())
		{
			return false;
		}

		//Seek to the location of the data for this icon entry
		Stream::IStream::SizeType currentSeekLocation = iconFileStream.GetStreamPos();
		iconFileStream.SetStreamPos(iconDirEntry.dwImageOffset);

		//Retrieve the provided resource stream for this icon entry
		std::map<int, Stream::IStream*>::const_iterator iconResourceDataIterator = iconResourceData.find((int)i);
		if(iconResourceDataIterator == iconResourceData.end())
		{
			return false;
		}
		Stream::IStream& iconResourceDataStream = *(iconResourceDataIterator->second);

		//Transfer the additional cursor data from a file structure into a resource
		//structure. Cursor resources have the cursor hotspot X and Y coordinates first,
		//followed by the ICONIMAGE structure.
		if(iconDir.idType == ICONIMAGETYPE_CURSOR)
		{
			//Build the RTCURSORDATA data structure
			RTCURSORDATA cursorData;
			cursorData.wHotSpotX = iconDirEntry.wHotSpotX;
			cursorData.wHotSpotY = iconDirEntry.wHotSpotY;

			//Write the RTCURSORDATA data structure to the icon image data
			result &= iconResourceDataStream.WriteData(cursorData.wHotSpotX);
			result &= iconResourceDataStream.WriteData(cursorData.wHotSpotY);
		}

		//Transfer the ICONIMAGE icon data from a file structure into a resource
		//structure. This structure is the only data present for an RT_ICON resource type.
		//Since the two structures are equivalent in this case, we just copy the data.
		std::vector<unsigned char> iconResourceDataRaw;
		result &= iconFileStream.ReadData(iconResourceDataRaw, iconDirEntry.dwBytesInRes);
		result &= iconResourceDataStream.WriteData(iconResourceDataRaw);
		iconResourceDataStream.SetStreamPos(0);

		//Restore the seek location back to the end of the icon entry we just processed
		iconFileStream.SetStreamPos(currentSeekLocation);
	}

	return result;
}
