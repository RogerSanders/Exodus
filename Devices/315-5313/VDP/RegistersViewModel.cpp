#include "RegistersViewModel.h"
#include "RegistersView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::RegistersViewModel::RegistersViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
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
