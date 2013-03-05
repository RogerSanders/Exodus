#include "DisassemblyViewModel.h"
#include "DisassemblyView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::DisassemblyViewModel::DisassemblyViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, Processor* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* Processor::DisassemblyViewModel::CreateView()
{
	return new DisassemblyView(device);
}

//----------------------------------------------------------------------------------------
void Processor::DisassemblyViewModel::DeleteView(IView* aview)
{
	delete aview;
}
