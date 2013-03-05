#include "CallStackViewModel.h"
#include "CallStackView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::CallStackViewModel::CallStackViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, Processor* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
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
