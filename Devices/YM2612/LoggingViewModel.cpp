#include "LoggingViewModel.h"
#include "LoggingView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612::LoggingViewModel::LoggingViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, YM2612* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* YM2612::LoggingViewModel::CreateView()
{
	return new LoggingView(device);
}

//----------------------------------------------------------------------------------------
void YM2612::LoggingViewModel::DeleteView(IView* aview)
{
	delete aview;
}
