#include "ViewPresenterBase.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewPresenterBase::ViewPresenterBase(AssemblyHandle aassemblyHandle, const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID)
:assemblyHandle(aassemblyHandle), viewGroupName(aviewGroupName), viewName(aviewName), viewID(aviewID), view(0), notifier(0), viewOpen(false), viewTarget(ViewTarget::System), viewTargetGlobalExtension(false)
{}

//----------------------------------------------------------------------------------------
ViewPresenterBase::ViewPresenterBase(AssemblyHandle aassemblyHandle, const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, unsigned int amoduleID, const std::wstring& amoduleDisplayName)
:assemblyHandle(aassemblyHandle), viewGroupName(aviewGroupName), viewName(aviewName), viewID(aviewID), view(0), notifier(0), viewOpen(false), viewTarget(ViewTarget::Module), viewTargetModuleID(amoduleID), viewTargetModuleDisplayName(amoduleDisplayName), viewTargetGlobalExtension(false)
{}

//----------------------------------------------------------------------------------------
ViewPresenterBase::ViewPresenterBase(AssemblyHandle aassemblyHandle, const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, const std::wstring& adeviceInstanceName, unsigned int amoduleID, const std::wstring& amoduleDisplayName)
:assemblyHandle(aassemblyHandle), viewGroupName(aviewGroupName), viewName(aviewName), viewID(aviewID), view(0), notifier(0), viewOpen(false), viewTarget(ViewTarget::Device), viewTargetDeviceInstanceName(adeviceInstanceName), viewTargetModuleID(amoduleID), viewTargetModuleDisplayName(amoduleDisplayName), viewTargetGlobalExtension(false)
{}

//----------------------------------------------------------------------------------------
ViewPresenterBase::ViewPresenterBase(AssemblyHandle aassemblyHandle, const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, const std::wstring& aextensionInstanceName, bool aglobalExtension, unsigned int amoduleID, const std::wstring& amoduleDisplayName)
:assemblyHandle(aassemblyHandle), viewGroupName(aviewGroupName), viewName(aviewName), viewID(aviewID), view(0), notifier(0), viewOpen(false), viewTarget(ViewTarget::Device), viewTargetExtensionInstanceName(aextensionInstanceName), viewTargetModuleID(amoduleID), viewTargetModuleDisplayName(amoduleDisplayName), viewTargetGlobalExtension(aglobalExtension)
{}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ViewPresenterBase::GetIViewPresenterVersion() const
{
	return ThisIViewPresenterVersion();
}

//----------------------------------------------------------------------------------------
//Assembly handle functions
//----------------------------------------------------------------------------------------
AssemblyHandle ViewPresenterBase::GetAssemblyHandle() const
{
	return assemblyHandle;
}

//----------------------------------------------------------------------------------------
//View management functions
//----------------------------------------------------------------------------------------
bool ViewPresenterBase::OpenView(IUIManager& uiManager, IViewStateChangeNotifier* anotifier, IHierarchicalStorageNode* viewState)
{
	//Ensure the view isn't already open
	if(viewOpen)
	{
		return false;
	}

	//Create the view
	view = CreateView(uiManager);
	if(view == 0)
	{
		return false;
	}

	//Save a reference to the notifier
	notifier = anotifier;

	//Open the view
	viewOpen = true;
	IHierarchicalStorageNode* containedViewState = 0;
	if(viewState != 0)
	{
		containedViewState = viewState->GetChild(L"ViewState");
	}
	if(!view->OpenView(containedViewState))
	{
		DeleteView(view);
		view = 0;
		notifier = 0;
		viewOpen = false;
		return false;
	}

	//Fire a notification that the view has been opened
	NotifyViewOpened();

	return true;
}

//----------------------------------------------------------------------------------------
void ViewPresenterBase::CloseView()
{
	if(viewOpen)
	{
		view->CloseView();
	}
}

//----------------------------------------------------------------------------------------
void ViewPresenterBase::ShowView()
{
	if(viewOpen)
	{
		view->ShowView();
	}
}

//----------------------------------------------------------------------------------------
void ViewPresenterBase::HideView()
{
	if(viewOpen)
	{
		view->HideView();
	}
}

//----------------------------------------------------------------------------------------
void ViewPresenterBase::ActivateView()
{
	if(viewOpen)
	{
		view->ActivateView();
	}
}

//----------------------------------------------------------------------------------------
void ViewPresenterBase::NotifyViewClosed(IView* aview)
{
	if(view == aview)
	{
		//##TODO## Comment this
		NotifyViewClosed();
		notifier->NotifyViewClosed();
		DeleteView(view);
		view = 0;
		notifier = 0;
		viewOpen = false;
	}
}

//----------------------------------------------------------------------------------------
bool ViewPresenterBase::IsViewOpen() const
{
	return viewOpen;
}

//----------------------------------------------------------------------------------------
IView* ViewPresenterBase::GetOpenView() const
{
	return view;
}

//----------------------------------------------------------------------------------------
//View target functions
//----------------------------------------------------------------------------------------
ViewPresenterBase::ViewTarget ViewPresenterBase::GetViewTarget() const
{
	return viewTarget;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ViewPresenterBase::GetViewTargetDeviceInstanceName() const
{
	return viewTargetDeviceInstanceName;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ViewPresenterBase::GetViewTargetExtensionInstanceName() const
{
	return viewTargetExtensionInstanceName;
}

//----------------------------------------------------------------------------------------
bool ViewPresenterBase::GetViewTargetGlobalExtension() const
{
	return viewTargetGlobalExtension;
}

//----------------------------------------------------------------------------------------
unsigned int ViewPresenterBase::GetViewTargetModuleID() const
{
	return viewTargetModuleID;
}

//----------------------------------------------------------------------------------------
//Notifiers
//----------------------------------------------------------------------------------------
void ViewPresenterBase::NotifyViewOpened()
{}

//----------------------------------------------------------------------------------------
void ViewPresenterBase::NotifyViewClosed()
{}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
int ViewPresenterBase::GetViewID() const
{
	return viewID;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ViewPresenterBase::GetViewGroupName() const
{
	return viewGroupName;
}

//----------------------------------------------------------------------------------------
MarshalSupport::Marshal::Ret<std::wstring> ViewPresenterBase::GetViewName() const
{
	return viewName;
}

//----------------------------------------------------------------------------------------
bool ViewPresenterBase::LoadViewState(IHierarchicalStorageNode& viewState)
{
	//Ensure this view is currently open
	if(!viewOpen)
	{
		return false;
	}

	//Load the state for this view if present
	IHierarchicalStorageNode* containedViewState = viewState.GetChild(L"ViewState");
	if(containedViewState != 0)
	{
		if(!view->LoadViewState(*containedViewState))
		{
			return false;
		}
	}

	//Return the result to the caller
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewPresenterBase::SaveViewState(IHierarchicalStorageNode& viewState) const
{
	//Ensure this view is currently open
	if(!viewOpen)
	{
		return false;
	}

	//Save the state for the view
	return view->SaveViewState(viewState.CreateChild(L"ViewState"));
}
