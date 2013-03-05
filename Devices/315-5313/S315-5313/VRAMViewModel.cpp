#include "VRAMViewModel.h"
#include "VRAMView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::VRAMViewModel::VRAMViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::VRAMViewModel::CreateView()
{
	return new VRAMView(device);
}

//----------------------------------------------------------------------------------------
void S315_5313::VRAMViewModel::DeleteView(IView* aview)
{
	delete aview;
}
