#include "ImageViewModel.h"
#include "ImageView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::ImageViewModel::ImageViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
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
