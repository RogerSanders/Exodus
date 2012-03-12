#include "PaletteViewModel.h"
#include "PaletteView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::PaletteViewModel::PaletteViewModel(const std::wstring& amenuHandlerName, int aviewModelID, VDP* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::PaletteViewModel::CreateView()
{
	return new PaletteView(device);
}

//----------------------------------------------------------------------------------------
void VDP::PaletteViewModel::DeleteView(IView* aview)
{
	delete aview;
}
