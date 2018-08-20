#include "ViewPresenterBase.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ViewPresenterBase::ViewPresenterBase(AssemblyHandle assemblyHandle, const std::wstring& viewGroupName, const std::wstring& viewName, int viewID)
:_assemblyHandle(assemblyHandle), _viewGroupName(viewGroupName), _viewName(viewName), _viewID(viewID), _view(0), _notifier(0), _viewOpen(false), _viewTarget(ViewTarget::System), _viewTargetGlobalExtension(false)
{ }

//----------------------------------------------------------------------------------------------------------------------
ViewPresenterBase::ViewPresenterBase(AssemblyHandle assemblyHandle, const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, unsigned int moduleID, const std::wstring& moduleDisplayName)
:_assemblyHandle(assemblyHandle), _viewGroupName(viewGroupName), _viewName(viewName), _viewID(viewID), _view(0), _notifier(0), _viewOpen(false), _viewTarget(ViewTarget::Module), _viewTargetModuleID(moduleID), _viewTargetModuleDisplayName(moduleDisplayName), _viewTargetGlobalExtension(false)
{ }

//----------------------------------------------------------------------------------------------------------------------
ViewPresenterBase::ViewPresenterBase(AssemblyHandle assemblyHandle, const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, const std::wstring& deviceInstanceName, unsigned int moduleID, const std::wstring& moduleDisplayName)
:_assemblyHandle(assemblyHandle), _viewGroupName(viewGroupName), _viewName(viewName), _viewID(viewID), _view(0), _notifier(0), _viewOpen(false), _viewTarget(ViewTarget::Device), _viewTargetDeviceInstanceName(deviceInstanceName), _viewTargetModuleID(moduleID), _viewTargetModuleDisplayName(moduleDisplayName), _viewTargetGlobalExtension(false)
{ }

//----------------------------------------------------------------------------------------------------------------------
ViewPresenterBase::ViewPresenterBase(AssemblyHandle assemblyHandle, const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, const std::wstring& extensionInstanceName, bool globalExtension, unsigned int moduleID, const std::wstring& moduleDisplayName)
:_assemblyHandle(assemblyHandle), _viewGroupName(viewGroupName), _viewName(viewName), _viewID(viewID), _view(0), _notifier(0), _viewOpen(false), _viewTarget(ViewTarget::Device), _viewTargetExtensionInstanceName(extensionInstanceName), _viewTargetModuleID(moduleID), _viewTargetModuleDisplayName(moduleDisplayName), _viewTargetGlobalExtension(globalExtension)
{ }

//----------------------------------------------------------------------------------------------------------------------
// Interface version functions
//----------------------------------------------------------------------------------------------------------------------
unsigned int ViewPresenterBase::GetIViewPresenterVersion() const
{
	return ThisIViewPresenterVersion();
}

//----------------------------------------------------------------------------------------------------------------------
// Assembly handle functions
//----------------------------------------------------------------------------------------------------------------------
AssemblyHandle ViewPresenterBase::GetAssemblyHandle() const
{
	return _assemblyHandle;
}

//----------------------------------------------------------------------------------------------------------------------
// View management functions
//----------------------------------------------------------------------------------------------------------------------
bool ViewPresenterBase::OpenView(IUIManager& uiManager, IViewStateChangeNotifier* notifier, IHierarchicalStorageNode* viewState)
{
	// Ensure the view isn't already open
	if (_viewOpen)
	{
		return false;
	}

	// Create the view
	_view = CreateView(uiManager);
	if (_view == 0)
	{
		return false;
	}

	// Save a reference to the notifier
	_notifier = notifier;

	// Open the view
	_viewOpen = true;
	IHierarchicalStorageNode* containedViewState = 0;
	if (viewState != 0)
	{
		containedViewState = viewState->GetChild(L"ViewState");
	}
	if (!_view->OpenView(containedViewState))
	{
		DeleteView(_view);
		_view = 0;
		_notifier = 0;
		_viewOpen = false;
		return false;
	}

	// Fire a notification that the view has been opened
	NotifyViewOpened();

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ViewPresenterBase::CloseView()
{
	if (_viewOpen)
	{
		_view->CloseView();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ViewPresenterBase::ShowView()
{
	if (_viewOpen)
	{
		_view->ShowView();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ViewPresenterBase::HideView()
{
	if (_viewOpen)
	{
		_view->HideView();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ViewPresenterBase::ActivateView()
{
	if (_viewOpen)
	{
		_view->ActivateView();
	}
}

//----------------------------------------------------------------------------------------------------------------------
void ViewPresenterBase::NotifyViewClosed(IView* view)
{
	if (_view == view)
	{
		//##TODO## Comment this
		NotifyViewClosed();
		_notifier->NotifyViewClosed();
		DeleteView(_view);
		_view = 0;
		_notifier = 0;
		_viewOpen = false;
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool ViewPresenterBase::IsViewOpen() const
{
	return _viewOpen;
}

//----------------------------------------------------------------------------------------------------------------------
IView* ViewPresenterBase::GetOpenView() const
{
	return _view;
}

//----------------------------------------------------------------------------------------------------------------------
// View target functions
//----------------------------------------------------------------------------------------------------------------------
ViewPresenterBase::ViewTarget ViewPresenterBase::GetViewTarget() const
{
	return _viewTarget;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ViewPresenterBase::GetViewTargetDeviceInstanceName() const
{
	return _viewTargetDeviceInstanceName;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ViewPresenterBase::GetViewTargetExtensionInstanceName() const
{
	return _viewTargetExtensionInstanceName;
}

//----------------------------------------------------------------------------------------------------------------------
bool ViewPresenterBase::GetViewTargetGlobalExtension() const
{
	return _viewTargetGlobalExtension;
}

//----------------------------------------------------------------------------------------------------------------------
unsigned int ViewPresenterBase::GetViewTargetModuleID() const
{
	return _viewTargetModuleID;
}

//----------------------------------------------------------------------------------------------------------------------
// Notifiers
//----------------------------------------------------------------------------------------------------------------------
void ViewPresenterBase::NotifyViewOpened()
{ }

//----------------------------------------------------------------------------------------------------------------------
void ViewPresenterBase::NotifyViewClosed()
{ }

//----------------------------------------------------------------------------------------------------------------------
// State functions
//----------------------------------------------------------------------------------------------------------------------
int ViewPresenterBase::GetViewID() const
{
	return _viewID;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ViewPresenterBase::GetViewGroupName() const
{
	return _viewGroupName;
}

//----------------------------------------------------------------------------------------------------------------------
Marshal::Ret<std::wstring> ViewPresenterBase::GetViewName() const
{
	return _viewName;
}

//----------------------------------------------------------------------------------------------------------------------
bool ViewPresenterBase::LoadViewState(IHierarchicalStorageNode& viewState)
{
	// Ensure this view is currently open
	if (!_viewOpen)
	{
		return false;
	}

	// Load the state for this view if present
	IHierarchicalStorageNode* containedViewState = viewState.GetChild(L"ViewState");
	if (containedViewState != 0)
	{
		if (!_view->LoadViewState(*containedViewState))
		{
			return false;
		}
	}

	// Return the result to the caller
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ViewPresenterBase::SaveViewState(IHierarchicalStorageNode& viewState) const
{
	// Ensure this view is currently open
	if (!_viewOpen)
	{
		return false;
	}

	// Save the state for the view
	return _view->SaveViewState(viewState.CreateChild(L"ViewState"));
}
