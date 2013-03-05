#include "BreakpointViewModel.h"
#include "BreakpointView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::BreakpointViewModel::BreakpointViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, Processor* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
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
