#include "SpriteListViewModel.h"
#include "SpriteListView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::SpriteListViewModel::SpriteListViewModel(const std::wstring& amenuHandlerName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::SpriteListViewModel::CreateView()
{
	return new SpriteListView(device);
}

//----------------------------------------------------------------------------------------
void S315_5313::SpriteListViewModel::DeleteView(IView* aview)
{
	delete aview;
}
