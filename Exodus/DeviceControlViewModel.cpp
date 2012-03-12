#include "DeviceControlViewModel.h"
#include "DeviceControlView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::DeviceControlViewModel::DeviceControlViewModel(const std::wstring& amenuHandlerName, int aviewModelID, System* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, true, false, L"", 0), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* System::DeviceControlViewModel::CreateView()
{
	return new DeviceControlView(device);
}

//----------------------------------------------------------------------------------------
void System::DeviceControlViewModel::DeleteView(IView* aview)
{
	delete aview;
}
