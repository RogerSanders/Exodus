#include "ActiveDisassemblyViewModel.h"
#include "ActiveDisassemblyView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::ActiveDisassemblyViewModel::ActiveDisassemblyViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, Processor* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* Processor::ActiveDisassemblyViewModel::CreateView()
{
	return new ActiveDisassemblyView(device);
}

//----------------------------------------------------------------------------------------
void Processor::ActiveDisassemblyViewModel::DeleteView(IView* aview)
{
	delete aview;
}
