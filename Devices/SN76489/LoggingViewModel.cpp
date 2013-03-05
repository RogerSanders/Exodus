#include "LoggingViewModel.h"
#include "LoggingView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489::LoggingViewModel::LoggingViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, SN76489* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* SN76489::LoggingViewModel::CreateView()
{
	return new LoggingView(device);
}

//----------------------------------------------------------------------------------------
void SN76489::LoggingViewModel::DeleteView(IView* aview)
{
	delete aview;
}
