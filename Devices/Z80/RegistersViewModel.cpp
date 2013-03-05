#include "RegistersViewModel.h"
#include "RegistersView.h"
namespace Z80{

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
Z80::RegistersViewModel::RegistersViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, Z80* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* Z80::RegistersViewModel::CreateView()
{
	return new RegistersView(device);
}

//----------------------------------------------------------------------------------------
void Z80::RegistersViewModel::DeleteView(IView* aview)
{
	delete aview;
}

} //Close namespace Z80
