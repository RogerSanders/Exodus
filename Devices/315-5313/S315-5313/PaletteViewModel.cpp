#include "PaletteViewModel.h"
#include "PaletteView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::PaletteViewModel::PaletteViewModel(const std::wstring& amenuHandlerName, int aviewModelID, S315_5313* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* S315_5313::PaletteViewModel::CreateView()
{
	return new PaletteView(device);
}

//----------------------------------------------------------------------------------------
void S315_5313::PaletteViewModel::DeleteView(IView* aview)
{
	delete aview;
}