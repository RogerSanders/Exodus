#include "ImageViewModel.h"
#include "ImageView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::ImageViewModel::ImageViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::ImageViewModel::CreateView()
{
	return new ImageView(device);
}

//----------------------------------------------------------------------------------------
void S315_5313::ImageViewModel::DeleteView(IView* aview)
{
	delete aview;
}
