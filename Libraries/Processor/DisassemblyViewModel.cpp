#include "DisassemblyViewModel.h"
#include "DisassemblyView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::DisassemblyViewModel::DisassemblyViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
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
