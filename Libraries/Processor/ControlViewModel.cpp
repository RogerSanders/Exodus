#include "ControlViewModel.h"
#include "ControlView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::ControlViewModel::ControlViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* Processor::ControlViewModel::CreateView()
{
	return new ControlView(device);
}

//----------------------------------------------------------------------------------------
void Processor::ControlViewModel::DeleteView(IView* aview)
{
	delete aview;
}
