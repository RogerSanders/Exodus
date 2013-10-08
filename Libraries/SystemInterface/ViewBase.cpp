#include "ViewBase.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewBase::ViewBase()
:hwndInternal(0), parentWindow(0), viewModel(0), assemblyHandle(0), dialogSettingsCaptured(false), windowSettingsCaptured(false)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ViewBase::GetIViewVersion() const
{
	return ThisIViewVersion();
}

//----------------------------------------------------------------------------------------
//Window settings
//----------------------------------------------------------------------------------------
void ViewBase::SetWindowSettings(const std::wstring& awindowTitle, DWORD awindowStyle, DWORD aextendedWindowStyle, unsigned int awidth, unsigned int aheight, bool aresizable)
{
	windowSettingsCaptured = true;
	windowTitle = awindowTitle;
	windowStyle = awindowStyle;
	extendedWindowStyle = aextendedWindowStyle;
	originalWindowWidth = awidth;
	originalWindowHeight = aheight;
	resizable = aresizable;
}

//----------------------------------------------------------------------------------------
void ViewBase::SetDialogTemplateSettings(const std::wstring& awindowTitle, void* aassemblyHandle, LPCWSTR atemplateName, bool aresizable)
{
	dialogSettingsCaptured = true;
	windowTitle = awindowTitle;
	assemblyHandle = aassemblyHandle;
	templateName = atemplateName;
	resizable = aresizable;
}

//----------------------------------------------------------------------------------------
//View management functions
//----------------------------------------------------------------------------------------
bool ViewBase::OpenView(void* aparentWindow, IViewModel* aviewModel, int xpos, int ypos)
{
	//Save a reference to the owning view model
	viewModel = aviewModel;

	//Open the window
	bool result = false;
	if(dialogSettingsCaptured)
	{
		result = OpenViewDialog(aparentWindow, xpos, ypos);
	}
	else if(windowSettingsCaptured)
	{
		result = OpenViewWindow(aparentWindow, xpos, ypos);
	}

	//Return the result
	return result;
}

//----------------------------------------------------------------------------------------
bool ViewBase::OpenViewDialog(void* aparentWindow, int xpos, int ypos)
{
	//Ensure that the dialog template settings have been supplied
	if(!dialogSettingsCaptured)
	{
		return false;
	}

	//Create the dialog
	hwndInternal = CreateDialogParam((HINSTANCE)assemblyHandle, templateName, (HWND)aparentWindow, WndProcDialogInternal, (LPARAM)this);
	if(hwndInternal == NULL)
	{
		return false;
	}

	//Set the title for the window
	SetWindowText(hwndInternal, windowTitle.c_str());

	//Position the window
	SetWindowPos(hwndInternal, NULL, xpos, ypos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	return true;
}

//----------------------------------------------------------------------------------------
bool ViewBase::OpenViewWindow(void* aparentWindow, int xpos, int ypos)
{
	//Create the window class
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProcWindowInternal;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = (HINSTANCE)assemblyHandle;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = windowTitle.c_str();
	wc.hIconSm       = NULL;
	if((RegisterClassEx(&wc) == 0) && (GetLastError() != ERROR_CLASS_ALREADY_EXISTS))
	{
		return false;
	}

	//Create the window
	extendedWindowStyle |= WS_EX_TOOLWINDOW;
	windowStyle |= WS_POPUP | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU;
	windowStyle |= WS_MINIMIZEBOX;
	unsigned int scaledWindowWidth = DPIScaleWidth(originalWindowWidth);
	unsigned int scaledWindowHeight = DPIScaleHeight(originalWindowHeight);
	hwndInternal = CreateWindowEx(extendedWindowStyle, windowTitle.c_str(), windowTitle.c_str(), windowStyle, 0, 0, scaledWindowWidth, scaledWindowHeight, (HWND)aparentWindow, NULL, (HINSTANCE)assemblyHandle, (LPVOID)this);
	if(hwndInternal == NULL)
	{
		return false;
	}

	//Calculate the total window size to use in order to achieve a client region of the
	//specified size.
	RECT clientRect;
	clientRect.left = 0;
	clientRect.top = 0;
	clientRect.right = scaledWindowWidth;
	clientRect.bottom = scaledWindowHeight;
	if(AdjustWindowRectEx(&clientRect, windowStyle, FALSE, extendedWindowStyle) != 0)
	{
		//Calculate the actual window width and height to use
		int adjustedWindowWidth = clientRect.right - clientRect.left;
		int adjustedWindowHeight = clientRect.bottom - clientRect.top;

		//Resize and position the window
		SetWindowPos(hwndInternal, NULL, xpos, ypos, adjustedWindowWidth, adjustedWindowHeight, SWP_NOZORDER);
	}

	return true;
}

//----------------------------------------------------------------------------------------
void ViewBase::CloseView()
{
	if(hwndInternal != 0)
	{
		//Close the window
		DestroyWindow(hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
void ViewBase::ShowView()
{
	if(hwndInternal != 0)
	{
		//Show the window
		ShowWindow(hwndInternal, SW_SHOWNORMAL);
		UpdateWindow(hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
void ViewBase::HideView()
{
	if(hwndInternal != 0)
	{
		//Hide the window
		ShowWindow(hwndInternal, SW_HIDE);
	}
}

//----------------------------------------------------------------------------------------
void ViewBase::ActivateView()
{
	if(hwndInternal != 0)
	{
		//Activate the window
		ShowWindow(hwndInternal, SW_RESTORE);
		SetActiveWindow(hwndInternal);
		SetForegroundWindow(hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
//Title functions
//----------------------------------------------------------------------------------------
std::wstring ViewBase::GetViewTitle() const
{
	return windowTitle;
}

//----------------------------------------------------------------------------------------
void ViewBase::GetViewTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetViewTitle());
}

//----------------------------------------------------------------------------------------
//Position and size functions
//----------------------------------------------------------------------------------------
bool ViewBase::CanResizeView() const
{
	return resizable;
}

//----------------------------------------------------------------------------------------
void ViewBase::GetViewPosition(int& xpos, int& ypos) const
{
	xpos = 0;
	ypos = 0;
	if(hwndInternal != 0)
	{
		int borderLeft;
		int borderRight;
		int borderTop;
		int borderBottom;
		GetHiddenBorderDimensions(hwndInternal, borderLeft, borderRight, borderTop, borderBottom);

		RECT windowRect;
		if(GetWindowRect(hwndInternal, &windowRect) != 0)
		{
			xpos = (int)windowRect.left - borderLeft;
			ypos = (int)windowRect.top - borderTop;
		}
	}
}

//----------------------------------------------------------------------------------------
void ViewBase::GetViewSize(int& sizex, int& sizey) const
{
	sizex = 0;
	sizey = 0;
	if(hwndInternal != 0)
	{
		int borderLeft;
		int borderRight;
		int borderTop;
		int borderBottom;
		GetHiddenBorderDimensions(hwndInternal, borderLeft, borderRight, borderTop, borderBottom);

		RECT windowRect;
		if(GetWindowRect(hwndInternal, &windowRect) != 0)
		{
			sizex = (int)((windowRect.right + borderRight) - (windowRect.left - borderLeft));
			sizey = (int)((windowRect.bottom + borderBottom) - (windowRect.top - borderTop));
		}
	}
}

//----------------------------------------------------------------------------------------
void ViewBase::SetViewPosition(int xpos, int ypos)
{
	if(hwndInternal != 0)
	{
		int borderLeft;
		int borderRight;
		int borderTop;
		int borderBottom;
		GetHiddenBorderDimensions(hwndInternal, borderLeft, borderRight, borderTop, borderBottom);

		UINT flags = SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE;
		SetWindowPos(hwndInternal, 0, xpos + borderLeft, ypos + borderTop, 0, 0, flags);
	}
}

//----------------------------------------------------------------------------------------
void ViewBase::SetViewSize(int sizex, int sizey)
{
	if(hwndInternal != 0)
	{
		if(resizable)
		{
			int borderLeft;
			int borderRight;
			int borderTop;
			int borderBottom;
			GetHiddenBorderDimensions(hwndInternal, borderLeft, borderRight, borderTop, borderBottom);

			UINT flags = SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOMOVE;
			SetWindowPos(hwndInternal, 0, 0, 0, sizex - (borderLeft + borderRight), sizey - (borderTop + borderBottom), flags);
		}
	}
}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
bool ViewBase::SaveViewLayoutState(IHeirarchicalStorageNode& node) const
{
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewBase::LoadViewLayoutState(IHeirarchicalStorageNode& node)
{
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewBase::DoesWindowHandleMatchView(void* awindowHandle) const
{
	if(hwndInternal != 0)
	{
		return ((HWND)awindowHandle == hwndInternal);
	}
	return false;
}

//----------------------------------------------------------------------------------------
//Helper functions
//----------------------------------------------------------------------------------------
std::wstring ViewBase::BuildWindowTitle(const std::wstring& moduleDisplayName, const std::wstring& deviceInstanceName, const std::wstring& windowTitle) const
{
	return moduleDisplayName + L" - " + deviceInstanceName + L" - " + windowTitle;
}

//----------------------------------------------------------------------------------------
std::wstring ViewBase::GetLockingSupportedMessage() const
{
	return L"Selected controls on this window support register locking. "
	       L"Register locking allows the current value of a register to "
	       L"be frozen, preventing changes from within the system. "
	       L"Attempts by any device to modify locked registers will be "
	       L"ignored. The current value of locked registers can still be "
	       L"modified through the debugger. To toggle locking for a "
	       L"register, hold the CTRL key while clicking on a control. "
	       L"Controls that support locking will change colour to indicate "
	       L"when they are locked.";
}

//----------------------------------------------------------------------------------------
//Member window procedure
//----------------------------------------------------------------------------------------
INT_PTR ViewBase::WndProcDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return 0;
}

//----------------------------------------------------------------------------------------
LRESULT ViewBase::WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return 0;
}

//----------------------------------------------------------------------------------------
//Window procedure helper functions
//----------------------------------------------------------------------------------------
void ViewBase::WndProcDialogImplementSaveFieldWhenLostFocus(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	//Make sure no textbox is selected on startup, and remove focus from textboxes when
	//the user clicks an unused area of the window.
	case WM_LBUTTONDOWN:
	case WM_SHOWWINDOW:
		SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(0, EN_SETFOCUS), NULL);
		SetFocus(NULL);
		break;
	}
}

//----------------------------------------------------------------------------------------
//Static window procedure
//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ViewBase::WndProcDialogInternal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	ViewBase* state = (ViewBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = (ViewBase*)lparam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcDialog(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_CLOSE:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			INT_PTR result = state->WndProcDialog(hwnd, msg, wparam, lparam);

			//By default, Windows does not destroy modeless dialogs in response to a
			//WM_CLOSE message. We change that default here, and destroy the dialog window
			//on a WM_CLOSE message, to make the behaviour consistent with a normal
			//window.
			if(result == FALSE)
			{
				DestroyWindow(hwnd);
			}

			//Since we've handled this message ourselves, return true. We also need to
			//abort right here regardless, as if the call to DestroyWindow has completed,
			//state is no longer valid, as the object may have been destroyed.
			return TRUE;
		}
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			INT_PTR result = state->WndProcDialog(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Notify the ViewModel that the view has closed
			state->viewModel->NotifyViewClosed(state);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = FALSE;
	if(state != 0)
	{
		result = state->WndProcDialog(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
LRESULT CALLBACK ViewBase::WndProcWindowInternal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	ViewBase* state = (ViewBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_CREATE:
		//Set the object pointer
		state = (ViewBase*)((CREATESTRUCT*)lparam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(state));

		//Pass this message on to the member window procedure function
		if(state != 0)
		{
			return state->WndProcWindow(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			LRESULT result = state->WndProcWindow(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Notify the ViewModel that the view has closed
			state->viewModel->NotifyViewClosed(state);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	if(state != 0)
	{
		return state->WndProcWindow(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
