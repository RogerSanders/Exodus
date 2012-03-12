#include "CallStackViewModel.h"
#include "CallStackView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::CallStackViewModel::CallStackViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* Processor::CallStackViewModel::CreateView()
{
	return new CallStackView(device);
}

//----------------------------------------------------------------------------------------
void Processor::CallStackViewModel::DeleteView(IView* aview)
{
	delete aview;
}
