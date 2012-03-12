#include "TraceViewModel.h"
#include "TraceView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::TraceViewModel::TraceViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* Processor::TraceViewModel::CreateView()
{
	return new TraceView(device);
}

//----------------------------------------------------------------------------------------
void Processor::TraceViewModel::DeleteView(IView* aview)
{
	delete aview;
}
