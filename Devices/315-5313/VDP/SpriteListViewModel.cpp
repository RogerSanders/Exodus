#include "SpriteListViewModel.h"
#include "SpriteListView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::SpriteListViewModel::SpriteListViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
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
