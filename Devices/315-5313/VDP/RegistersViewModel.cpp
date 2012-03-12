#include "RegistersViewModel.h"
#include "RegistersView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::RegistersViewModel::RegistersViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::RegistersViewModel::CreateView()
{
	return new RegistersView(device);
}

//----------------------------------------------------------------------------------------
void VDP::RegistersViewModel::DeleteView(IView* aview)
{
	delete aview;
}
