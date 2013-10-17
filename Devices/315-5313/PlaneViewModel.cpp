#include "PlaneViewModel.h"
#include "PlaneView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::PlaneViewModel::PlaneViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::PlaneViewModel::CreateView()
{
	return new PlaneView(device);
}

//----------------------------------------------------------------------------------------
void S315_5313::PlaneViewModel::DeleteView(IView* aview)
{
	delete aview;
}
