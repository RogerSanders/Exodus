#include "RegistersViewModel.h"
#include "RegistersView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489::RegistersViewModel::RegistersViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, SN76489* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* SN76489::RegistersViewModel::CreateView()
{
	return new RegistersView(device);
}

//----------------------------------------------------------------------------------------
void SN76489::RegistersViewModel::DeleteView(IView* aview)
{
	delete aview;
}
