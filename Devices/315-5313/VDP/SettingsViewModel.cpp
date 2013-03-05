#include "SettingsViewModel.h"
#include "SettingsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::SettingsViewModel::SettingsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::SettingsViewModel::CreateView()
{
	return new SettingsView(device);
}

//----------------------------------------------------------------------------------------
void VDP::SettingsViewModel::DeleteView(IView* aview)
{
	delete aview;
}
