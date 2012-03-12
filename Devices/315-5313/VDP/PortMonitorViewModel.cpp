#include "PortMonitorViewModel.h"
#include "PortMonitorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::PortMonitorViewModel::PortMonitorViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::PortMonitorViewModel::CreateView()
{
	return new PortMonitorView(device);
}

//----------------------------------------------------------------------------------------
void VDP::PortMonitorViewModel::DeleteView(IView* aview)
{
	delete aview;
}
