#include "DebugSettingsViewModel.h"
#include "DebugSettingsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::DebugSettingsViewModel::DebugSettingsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::DebugSettingsViewModel::CreateView()
{
	return new DebugSettingsView(device);
}

//----------------------------------------------------------------------------------------
void S315_5313::DebugSettingsViewModel::DeleteView(IView* aview)
{
	delete aview;
}
