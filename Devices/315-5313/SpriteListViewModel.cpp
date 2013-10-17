#include "SpriteListViewModel.h"
#include "SpriteListView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::SpriteListViewModel::SpriteListViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
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
