#include "VRAMViewModel.h"
#include "VRAMView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::VRAMViewModel::VRAMViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::VRAMViewModel::CreateView()
{
	return new VRAMView(device);
}

//----------------------------------------------------------------------------------------
void VDP::VRAMViewModel::DeleteView(IView* aview)
{
	delete aview;
}
