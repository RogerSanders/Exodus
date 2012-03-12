#include "RegistersViewModel.h"
#include "RegistersView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612::RegistersViewModel::RegistersViewModel(const std::wstring& amenuHandlerName, int aviewModelID, YM2612* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* YM2612::RegistersViewModel::CreateView()
{
	return new RegistersView(device);
}

//----------------------------------------------------------------------------------------
void YM2612::RegistersViewModel::DeleteView(IView* aview)
{
	delete aview;
}
