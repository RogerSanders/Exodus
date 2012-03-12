#include "ViewModelBase.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewModelBase::ViewModelBase(const std::wstring& amenuHandlerName, int aviewModelID, bool aownerIsSystem, bool aownerIsDevice, const std::wstring& adeviceInstanceName, unsigned int amoduleID)
:menuHandlerName(amenuHandlerName), viewModelID(aviewModelID), view(0), notifier(0), viewOpen(false), viewOwnerIsDevice(false), viewOwnerIsSystem(false)
{
	if(aownerIsSystem)
	{
		SetViewOwnerAsSystem();
	}
	else if(aownerIsDevice)
	{
		SetViewOwnerAsDevice(adeviceInstanceName, amoduleID);
	}
}

//----------------------------------------------------------------------------------------
//View management functions
//----------------------------------------------------------------------------------------
bool ViewModelBase::OpenView(IViewModelNotifier* anotifier, void* aparentWindow, int xpos, int ypos)
{
	//Ensure the view isn't already open
	if(viewOpen)
	{
		return false;
	}

	//Ensure a view object doesn't already exist. This should never actually be true here.
	if(view != 0)
	{
		return false;
	}

	//Create the view
	view = CreateView();
	if(view == 0)
	{
		return false;
	}

	//Save a reference to the notifier
	notifier = anotifier;

	//Open the view
	viewOpen = true;
	if(!view->OpenView(aparentWindow, this, xpos, ypos))
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
void ViewModelBase::CloseView()
{
	if(viewOpen)
	{
		view->CloseView();
	}
}

//----------------------------------------------------------------------------------------
void ViewModelBase::ShowView()
{
	if(viewOpen)
	{
		view->ShowView();
	}
}

//----------------------------------------------------------------------------------------
void ViewModelBase::HideView()
{
	if(viewOpen)
	{
		view->HideView();
	}
}

//----------------------------------------------------------------------------------------
void ViewModelBase::ActivateView()
{
	if(viewOpen)
	{
		view->ActivateView();
	}
}

//----------------------------------------------------------------------------------------
void ViewModelBase::NotifyViewClosed(IView* aview)
{
	if(view == aview)
	{
		NotifyViewClosed();
		notifier->NotifyViewClosed();
		DeleteView(view);
		view = 0;
		notifier = 0;
		viewOpen = false;
	}
}

//----------------------------------------------------------------------------------------
//Position and size functions
//----------------------------------------------------------------------------------------
bool ViewModelBase::CanResizeView() const
{
	if(viewOpen)
	{
		return view->CanResizeView();
	}
	return false;
}

//----------------------------------------------------------------------------------------
void ViewModelBase::GetViewPosition(int& xpos, int& ypos) const
{
	if(viewOpen)
	{
		view->GetViewPosition(xpos, ypos);
	}
}

//----------------------------------------------------------------------------------------
void ViewModelBase::GetViewSize(int& sizex, int& sizey) const
{
	if(viewOpen)
	{
		view->GetViewSize(sizex, sizey);
	}
}

//----------------------------------------------------------------------------------------
void ViewModelBase::SetViewPosition(int xpos, int ypos)
{
	if(viewOpen)
	{
		view->SetViewPosition(xpos, ypos);
	}
}

//----------------------------------------------------------------------------------------
void ViewModelBase::SetViewSize(int sizex, int sizey)
{
	if(viewOpen)
	{
		view->SetViewSize(sizex, sizey);
	}
}

//----------------------------------------------------------------------------------------
//Notifiers
//----------------------------------------------------------------------------------------
void ViewModelBase::NotifyViewOpened()
{}

//----------------------------------------------------------------------------------------
void ViewModelBase::NotifyViewClosed()
{}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
int ViewModelBase::GetViewID() const
{
	return viewModelID;
}

//----------------------------------------------------------------------------------------
bool ViewModelBase::SaveViewLayoutState(IHeirarchicalStorageNode& node) const
{
	if(viewOpen)
	{
		return view->SaveViewLayoutState(node);
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewModelBase::LoadViewLayoutState(IHeirarchicalStorageNode& node)
{
	if(viewOpen)
	{
		return view->LoadViewLayoutState(node);
	}
	return true;
}

//----------------------------------------------------------------------------------------
bool ViewModelBase::DoesWindowHandleMatchView(void* awindowHandle) const
{
	if(viewOpen)
	{
		return view->DoesWindowHandleMatchView(awindowHandle);
	}
	return false;
}

//----------------------------------------------------------------------------------------
//View owner functions
//----------------------------------------------------------------------------------------
bool ViewModelBase::IsViewOwnerDevice() const
{
	return viewOwnerIsDevice;
}

//----------------------------------------------------------------------------------------
bool ViewModelBase::IsViewOwnerSystem() const
{
	return viewOwnerIsSystem;
}

//----------------------------------------------------------------------------------------
void ViewModelBase::SetViewOwnerAsSystem()
{
	viewOwnerIsSystem = true;
	viewOwnerIsDevice = false;
}

//----------------------------------------------------------------------------------------
void ViewModelBase::SetViewOwnerAsDeviceInternal(const wchar_t* deviceInstanceName, unsigned int moduleID)
{
	viewOwnerIsDevice = true;
	viewOwnerIsSystem = false;
	viewOwnerDeviceInstanceName = deviceInstanceName;
	viewOwnerModuleID = moduleID;
}

//----------------------------------------------------------------------------------------
const wchar_t* ViewModelBase::GetViewOwnerDeviceInstanceNameInternal() const
{
	return viewOwnerDeviceInstanceName.c_str();
}

//----------------------------------------------------------------------------------------
unsigned int ViewModelBase::GetViewOwnerModuleID() const
{
	return viewOwnerModuleID;
}

//----------------------------------------------------------------------------------------
//Title functions
//----------------------------------------------------------------------------------------
const wchar_t* ViewModelBase::GetViewTitleInternal() const
{
	viewTitleCache.clear();
	if(viewOpen)
	{
		viewTitleCache = view->GetViewTitle();
	}
	return viewTitleCache.c_str();
}

//----------------------------------------------------------------------------------------
//State functions
//----------------------------------------------------------------------------------------
const wchar_t* ViewModelBase::GetMenuHandlerNameInternal() const
{
	return menuHandlerName.c_str();
}

//----------------------------------------------------------------------------------------
//View management functions
//----------------------------------------------------------------------------------------
IView* ViewModelBase::GetOpenView() const
{
	return view;
}
