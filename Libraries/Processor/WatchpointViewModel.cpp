#include "WatchpointViewModel.h"
#include "WatchpointView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::WatchpointViewModel::WatchpointViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* Processor::WatchpointViewModel::CreateView()
{
	return new WatchpointView(device);
}

//----------------------------------------------------------------------------------------
void Processor::WatchpointViewModel::DeleteView(IView* aview)
{
	delete aview;
}
