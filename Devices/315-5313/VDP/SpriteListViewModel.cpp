#include "SpriteListViewModel.h"
#include "SpriteListView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::SpriteListViewModel::SpriteListViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::SpriteListViewModel::CreateView()
{
	return new SpriteListView(device);
}

//----------------------------------------------------------------------------------------
void VDP::SpriteListViewModel::DeleteView(IView* aview)
{
	delete aview;
}
