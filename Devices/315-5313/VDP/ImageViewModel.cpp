#include "ImageViewModel.h"
#include "ImageView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::ImageViewModel::ImageViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::ImageViewModel::CreateView()
{
	return new ImageView(device);
}

//----------------------------------------------------------------------------------------
void VDP::ImageViewModel::DeleteView(IView* aview)
{
	delete aview;
}
