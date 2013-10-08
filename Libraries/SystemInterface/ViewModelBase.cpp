#include "ViewModelBase.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ViewModelBase::ViewModelBase(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, bool aownerIsSystem, bool aownerIsDevice, const std::wstring& adeviceInstanceName, unsigned int amoduleID)
:viewModelGroupName(aviewModelGroupName), viewModelName(aviewModelName), viewModelID(aviewModelID), view(0), notifier(0), viewOpen(false), viewOwnerIsDevice(false), viewOwnerIsSystem(false)
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
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int ViewModelBase::GetIViewModelVersion() const
{
	return ThisIViewModelVersion();
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
IView* ViewModelBase::GetOpenView() const
{
	return view;
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
std::wstring ViewModelBase::GetViewOwnerDeviceInstanceName() const
{
	return viewOwnerDeviceInstanceName;
}

//----------------------------------------------------------------------------------------
void ViewModelBase::SetViewOwnerAsDevice(const std::wstring& deviceInstanceName, unsigned int moduleID)
{
	viewOwnerIsDevice = true;
	viewOwnerIsSystem = false;
	viewOwnerDeviceInstanceName = deviceInstanceName;
	viewOwnerModuleID = moduleID;
}

//----------------------------------------------------------------------------------------
void ViewModelBase::SetViewOwnerAsDeviceInternal(const InteropSupport::ISTLObjectSource<std::wstring>& deviceInstanceNameMarshaller, unsigned int moduleID)
{
	SetViewOwnerAsDevice(deviceInstanceNameMarshaller.MarshalTo(), moduleID);
}

//----------------------------------------------------------------------------------------
void ViewModelBase::GetViewOwnerDeviceInstanceNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetViewOwnerDeviceInstanceName());
}

//----------------------------------------------------------------------------------------
void ViewModelBase::SetViewOwnerAsSystem()
{
	viewOwnerIsSystem = true;
	viewOwnerIsDevice = false;
}

//----------------------------------------------------------------------------------------
unsigned int ViewModelBase::GetViewOwnerModuleID() const
{
	return viewOwnerModuleID;
}

//----------------------------------------------------------------------------------------
//Title functions
//----------------------------------------------------------------------------------------
std::wstring ViewModelBase::GetViewTitle() const
{
	std::wstring viewTitle;
	if(viewOpen)
	{
		viewTitle = view->GetViewTitle();
	}
	return viewTitle;
}

//----------------------------------------------------------------------------------------
void ViewModelBase::GetViewTitleInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetViewTitle());
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
std::wstring ViewModelBase::GetViewModelGroupName() const
{
	return viewModelGroupName;
}

//----------------------------------------------------------------------------------------
std::wstring ViewModelBase::GetViewModelName() const
{
	return viewModelName;
}

//----------------------------------------------------------------------------------------
void ViewModelBase::GetViewModelGroupNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetViewModelGroupName());
}

//----------------------------------------------------------------------------------------
void ViewModelBase::GetViewModelNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const
{
	marshaller.MarshalFrom(GetViewModelName());
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
