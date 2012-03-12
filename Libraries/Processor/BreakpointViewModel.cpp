#include "BreakpointViewModel.h"
#include "BreakpointView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::BreakpointViewModel::BreakpointViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* Processor::BreakpointViewModel::CreateView()
{
	return new BreakpointView(device);
}

//----------------------------------------------------------------------------------------
void Processor::BreakpointViewModel::DeleteView(IView* aview)
{
	delete aview;
}
