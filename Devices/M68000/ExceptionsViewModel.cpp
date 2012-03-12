#include "ExceptionsViewModel.h"
#include "ExceptionsView.h"
namespace M68000{

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
M68000::ExceptionsViewModel::ExceptionsViewModel(const std::wstring& amenuHandlerName, int aviewModelID, M68000* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* M68000::ExceptionsViewModel::CreateView()
{
	return new ExceptionsView(device);
}

//----------------------------------------------------------------------------------------
void M68000::ExceptionsViewModel::DeleteView(IView* aview)
{
	delete aview;
}

} //Close namespace M68000
