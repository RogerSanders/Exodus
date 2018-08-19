#include "ViewBase.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewBase::ViewBase(IUIManager& uiManager, IViewPresenter& viewPresenter)
:_hwndInternal(NULL), _parentWindow(0), _uiManager(uiManager), _viewPresenter(viewPresenter), _assemblyHandle(0), _dialogSettingsCaptured(false), _windowSettingsCaptured(false), _initiallyDocked(false), _initiallyCollapsed(false), _windowShownForFirstTime(false), _viewType(ViewType::Dockable), _initialDialogPos(DialogPos::Default)
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
void ViewBase::SetWindowSettings(const std::wstring& initialWindowTitle, DWORD windowStyle, DWORD extendedWindowStyle, unsigned int width, unsigned int height)
{
	_windowSettingsCaptured = true;
	_initialWindowTitle = initialWindowTitle;
	_windowStyle = windowStyle;
	_extendedWindowStyle = extendedWindowStyle;
	_originalWindowWidth = width;
	_originalWindowHeight = height;
}

//----------------------------------------------------------------------------------------
void ViewBase::SetDialogTemplateSettings(const std::wstring& initialWindowTitle, void* assemblyHandle, LPCWSTR templateName)
{
	_dialogSettingsCaptured = true;
	_initialWindowTitle = initialWindowTitle;
	_assemblyHandle = assemblyHandle;
	_templateName = templateName;
}

//----------------------------------------------------------------------------------------
void ViewBase::SetDocumentViewType()
{
	_viewType = ViewType::Document;
}

//----------------------------------------------------------------------------------------
void ViewBase::SetDockableViewType(bool initiallyDocked, DockPos initialDockPos, bool initiallyCollapsed, const std::wstring& viewDockingGroup)
{
	_viewType = ViewType::Dockable;
	_viewDockingGroup = viewDockingGroup;
	_initiallyDocked = initiallyDocked;
	_initialDockPos = initialDockPos;
	_initiallyCollapsed = initiallyCollapsed;
}

//----------------------------------------------------------------------------------------
void ViewBase::SetDialogViewType(DialogMode dialogMode, bool dialogResizable, DialogPos initialDialogPos)
{
	_viewType = ViewType::Dialog;
	_dialogMode = dialogMode;
	_dialogResizable = dialogResizable;
	_initialDialogPos = initialDialogPos;
}

//----------------------------------------------------------------------------------------
//View management functions
//----------------------------------------------------------------------------------------
bool ViewBase::OpenView(IHierarchicalStorageNode* viewState)
{
	//Create the native window for this view. Note that the window will initially appear
	//hidden.
	_hwndInternal = NULL;
	if(_dialogSettingsCaptured)
	{
		_hwndInternal = _uiManager.CreateDialogWindow(*this, _viewPresenter, (HINSTANCE)_assemblyHandle, WndProcDialogInternal, _templateName);
	}
	else if(_windowSettingsCaptured)
	{
		_hwndInternal = _uiManager.CreateNativeWindow(*this, _viewPresenter, (HINSTANCE)_viewPresenter.GetAssemblyHandle(), WndProcWindowInternal, _windowStyle, _extendedWindowStyle);
	}
	if(_hwndInternal == NULL)
	{
		return false;
	}

	//If we're opening a native window rather than a dialog, resize the window to the
	//requested window size.
	if(!_dialogSettingsCaptured && _windowSettingsCaptured)
	{
		_uiManager.ResizeWindowToTargetClientSize(*this, _viewPresenter, _hwndInternal, _originalWindowWidth, _originalWindowHeight);
	}

	//If a view state has been defined to restore, load it now, otherwise show the window.
	if(viewState != 0)
	{
		//Attempt to load the state for this view. This will also show the view with the
		//saved state for the window.
		if(!LoadViewState(*viewState))
		{
			_uiManager.CloseWindow(*this, _viewPresenter, _hwndInternal);
			return false;
		}
	}
	else
	{
		//Request the UI manager to show this view for the first time
		if(!_uiManager.ShowWindowFirstTime(*this, _viewPresenter, _hwndInternal, _initialWindowTitle))
		{
			_uiManager.CloseWindow(*this, _viewPresenter, _hwndInternal);
			return false;
		}
		_windowShownForFirstTime = true;
	}

	//Return the result to the caller
	return true;
}

//----------------------------------------------------------------------------------------
void ViewBase::CloseView()
{
	if(_hwndInternal != NULL)
	{
		_uiManager.CloseWindow(*this, _viewPresenter, _hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
void ViewBase::ShowView()
{
	if(_hwndInternal != NULL)
	{
		_uiManager.ShowWindow(*this, _viewPresenter, _hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
void ViewBase::HideView()
{
	if(_hwndInternal != NULL)
	{
		_uiManager.HideWindow(*this, _viewPresenter, _hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
void ViewBase::ActivateView()
{
	if(_hwndInternal != NULL)
	{
		_uiManager.ActivateWindow(*this, _viewPresenter, _hwndInternal);
	}
}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
bool ViewBase::LoadViewState(IHierarchicalStorageNode& viewState)
{
	//Ensure the window has been created
	if(_hwndInternal == NULL)
	{
		return false;
	}

	//Load the window state for this view if present
	IHierarchicalStorageNode* windowState = viewState.GetChild(L"WindowState");
	if(!_windowShownForFirstTime)
	{
		if(!_uiManager.ShowWindowFirstTime(*this, _viewPresenter, _hwndInternal, _initialWindowTitle, windowState))
		{
			return false;
		}
		_windowShownForFirstTime = true;
	}
	else
	{
		if(windowState != 0)
		{
			if(!_uiManager.LoadWindowState(*this, _viewPresenter, _hwndInternal, *windowState))
			{
				return false;
			}
		}
	}

	//Return the result to the caller
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewBase::SaveViewState(IHierarchicalStorageNode& viewState) const
{
	//Ensure the window has been created
	if(_hwndInternal == NULL)
	{
		return false;
	}

	//Save the window state for this view
	return _uiManager.SaveWindowState(*this, _viewPresenter, _hwndInternal, viewState.CreateChild(L"WindowState"));
}

//----------------------------------------------------------------------------------------
//New window state
//----------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ViewBase::GetViewDockingGroup() const
{
	return _viewDockingGroup;
}

//----------------------------------------------------------------------------------------
bool ViewBase::IsViewInitiallyDocked() const
{
	return _initiallyDocked;
}

//----------------------------------------------------------------------------------------
bool ViewBase::IsViewInitiallyCollapsed() const
{
	return _initiallyCollapsed;
}

//----------------------------------------------------------------------------------------
ViewBase::DockPos ViewBase::GetViewInitialDockPosition() const
{
	return _initialDockPos;
}

//----------------------------------------------------------------------------------------
ViewBase::ViewType ViewBase::GetViewType() const
{
	return _viewType;
}

//----------------------------------------------------------------------------------------
ViewBase::DialogMode ViewBase::GetViewDialogMode() const
{
	return _dialogMode;
}

//----------------------------------------------------------------------------------------
ViewBase::DialogPos ViewBase::GetViewInitialDialogPosition() const
{
	return _initialDialogPos;
}

//----------------------------------------------------------------------------------------
bool ViewBase::CanResizeDialog() const
{
	return _dialogResizable;
}

//----------------------------------------------------------------------------------------
//Helper functions
//----------------------------------------------------------------------------------------
HINSTANCE ViewBase::GetAssemblyHandle() const
{
	return (HINSTANCE)_viewPresenter.GetAssemblyHandle();
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
void ViewBase::WndProcDialogImplementGiveFocusToChildWindowOnClick(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_PARENTNOTIFY:
		switch(LOWORD(wparam))
		{
		case WM_LBUTTONDOWN:{
			//If the user has clicked on a child window within our window region, ensure that
			//the child window gets focus.
			POINT mousePos;
			mousePos.x = (short)LOWORD(lparam);
			mousePos.y = (short)HIWORD(lparam);
			HWND targetWindow = ChildWindowFromPoint(hwnd, mousePos);
			if((targetWindow != NULL) && (targetWindow != hwnd))
			{
				SetFocus(targetWindow);
			}
			break;}
		}
		break;
	}
}

//----------------------------------------------------------------------------------------
//Child window functions
//----------------------------------------------------------------------------------------
HWND ViewBase::CreateChildDialog(HWND parentWindow, void* assemblyHandle, LPCWSTR templateName, const std::function<INT_PTR(HWND, UINT, WPARAM, LPARAM)>& dlgProcHandler)
{
	ChildDialogState state;
	state.view = this;
	state.windowProcedureMethod = dlgProcHandler;
	return CreateDialogParam((HINSTANCE)assemblyHandle, templateName, parentWindow, WndProcChildDialog, (LPARAM)&state);
}

//----------------------------------------------------------------------------------------
HWND ViewBase::CreateChildWindow(DWORD windowStyle, DWORD extendedWindowStyle, unsigned int posX, unsigned int posY, unsigned int width, unsigned int height, HWND parentWindow, const std::function<INT_PTR(HWND, UINT, WPARAM, LPARAM)>& wndProcHandler)
{
	//Register the child window class
	static const std::wstring childWindowClassName = L"ViewBaseChildWindow";
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProcChildWindow;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetAssemblyHandle();
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = childWindowClassName.c_str();
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);

	//Create the child window
	return CreateWindowEx(extendedWindowStyle, childWindowClassName.c_str(), L"", windowStyle, posX, posY, width, height, parentWindow, NULL, GetAssemblyHandle(), (LPVOID)&wndProcHandler);
}

//----------------------------------------------------------------------------------------
INT_PTR CALLBACK ViewBase::WndProcChildDialog(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the state object
	ChildDialogState* state = (ChildDialogState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_INITDIALOG:
		//Set the object pointer
		state = new ChildDialogState(*(ChildDialogState*)lparam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)state);

		//Pass this message on to the member window procedure function
		return state->windowProcedureMethod(hwnd, msg, wparam, lparam);
	case WM_DESTROY:
		if(state != 0)
		{
			//Pass this message on to the member window procedure function
			INT_PTR result = state->windowProcedureMethod(hwnd, msg, wparam, lparam);

			//Destroy the state object
			delete state;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Return the result from processing the message
			return result;
		}
		break;
	case WM_ACTIVATE:{
		//Notify the UI manager of changes to the activation state of this dialog, so that
		//special dialog messages such as tab key presses can be correctly processed.
		if(state != 0)
		{
			if(LOWORD(wparam) == WA_ACTIVE)
			{
				state->view->_uiManager.NotifyDialogActivated(hwnd);
			}
			else if(LOWORD(wparam) == WA_INACTIVE)
			{
				state->view->_uiManager.NotifyDialogDeactivated(hwnd);
			}
		}
		break;}
	}

	//Pass this message on to the member window procedure function
	INT_PTR result = FALSE;
	if(state != 0)
	{
		result = state->windowProcedureMethod(hwnd, msg, wparam, lparam);
	}
	return result;
}

//----------------------------------------------------------------------------------------
LRESULT CALLBACK ViewBase::WndProcChildWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Obtain the object pointer
	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>* windowProcedureMethod = (std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	//Process the message
	switch(msg)
	{
	case WM_CREATE:
		//Set the object pointer
		windowProcedureMethod = new std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>(*(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>*)((CREATESTRUCT*)lparam)->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(windowProcedureMethod));

		//Pass this message on to the member window procedure function
		if(windowProcedureMethod != 0)
		{
			return (*windowProcedureMethod)(hwnd, msg, wparam, lparam);
		}
		break;
	case WM_DESTROY:
		if(windowProcedureMethod != 0)
		{
			//Pass this message on to the member window procedure function
			LRESULT result = (*windowProcedureMethod)(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			delete windowProcedureMethod;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Return the result from processing the message
			return result;
		}
		break;
	}

	//Pass this message on to the member window procedure function
	if(windowProcedureMethod != 0)
	{
		return (*windowProcedureMethod)(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

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
			//window. Note that we must always destroy any window which was created for us
			//by the UI manager by calling the CloseWindow method, so that it is able to
			//correctly remove this window from the interface, and perform any necessary
			//cleanup.
			if(result == FALSE)
			{
				state->_uiManager.CloseWindow(*state, state->_viewPresenter, hwnd);
			}

			//Since we've handled this message ourselves, return true. We also need to
			//abort right here regardless, as if the call to DestroyWindow has succeeded,
			//state is no longer valid, as the object would have been destroyed.
			return TRUE;
		}
		break;
	case WM_DESTROY:
		if(state != 0)
		{
			//Notify the UI manager that the window has been destroyed. Note that we need
			//to do this for any window that the UI manager created for us.
			state->_uiManager.NotifyWindowDestroyed(*state, state->_viewPresenter, hwnd);

			//Pass this message on to the member window procedure function
			INT_PTR result = state->WndProcDialog(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Notify the view presenter that the view has closed
			state->_viewPresenter.NotifyViewClosed(state);

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
			//Notify the UI manager that the window has been destroyed. Note that we need
			//to do this for any window that the UI manager created for us.
			state->_uiManager.NotifyWindowDestroyed(*state, state->_viewPresenter, hwnd);

			//Pass this message on to the member window procedure function
			LRESULT result = state->WndProcWindow(hwnd, msg, wparam, lparam);

			//Discard the object pointer
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)0);

			//Notify the view presenter that the view has closed
			state->_viewPresenter.NotifyViewClosed(state);

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
