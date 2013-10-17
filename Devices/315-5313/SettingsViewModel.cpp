#include "SettingsViewModel.h"
#include "SettingsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::SettingsViewModel::SettingsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::SettingsViewModel::CreateView()
{
	return new SettingsView(device);
}

//----------------------------------------------------------------------------------------
void S315_5313::SettingsViewModel::DeleteView(IView* aview)
{
	delete aview;
}
