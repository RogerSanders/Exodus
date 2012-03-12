#include "DisassemblyOldViewModel.h"
#include "DisassemblyOldView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Processor::DisassemblyOldViewModel::DisassemblyOldViewModel(const std::wstring& amenuHandlerName, int aviewModelID, Processor* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* Processor::DisassemblyOldViewModel::CreateView()
{
	return new DisassemblyOldView(device);
}

//----------------------------------------------------------------------------------------
void Processor::DisassemblyOldViewModel::DeleteView(IView* aview)
{
	delete aview;
}
