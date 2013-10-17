#include "PortMonitorViewModel.h"
#include "PortMonitorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::PortMonitorViewModel::PortMonitorViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::PortMonitorViewModel::CreateView()
{
	return new PortMonitorView(device);
}

//----------------------------------------------------------------------------------------
void S315_5313::PortMonitorViewModel::DeleteView(IView* aview)
{
	delete aview;
}
