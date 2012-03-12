#include "LayerRemovalViewModel.h"
#include "LayerRemovalView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::LayerRemovalViewModel::LayerRemovalViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::LayerRemovalViewModel::CreateView()
{
	return new LayerRemovalView(device);
}

//----------------------------------------------------------------------------------------
void VDP::LayerRemovalViewModel::DeleteView(IView* aview)
{
	delete aview;
}
