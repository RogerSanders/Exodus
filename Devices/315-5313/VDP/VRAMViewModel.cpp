#include "VRAMViewModel.h"
#include "VRAMView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::VRAMViewModel::VRAMViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::VRAMViewModel::CreateView()
{
	return new VRAMView(device);
}

//----------------------------------------------------------------------------------------
void VDP::VRAMViewModel::DeleteView(IView* aview)
{
	delete aview;
}
