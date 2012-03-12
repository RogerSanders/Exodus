#include "DebuggerViewModel.h"
#include "DebuggerView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
YM2612::DebuggerViewModel::DebuggerViewModel(const std::wstring& amenuHandlerName, int aviewModelID, YM2612* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* YM2612::DebuggerViewModel::CreateView()
{
	return new DebuggerView(device);
}

//----------------------------------------------------------------------------------------
void YM2612::DebuggerViewModel::DeleteView(IView* aview)
{
	delete aview;
}
