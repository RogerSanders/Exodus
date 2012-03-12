#include "MemoryViewerOldViewModel.h"
#include "MemoryViewerOldView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryViewerOldViewModel::MemoryViewerOldViewModel(const std::wstring& amenuHandlerName, int aviewModelID, MemoryRead* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* MemoryRead::MemoryViewerOldViewModel::CreateView()
{
	return new MemoryViewerOldView(device);
}

//----------------------------------------------------------------------------------------
void MemoryRead::MemoryViewerOldViewModel::DeleteView(IView* aview)
{
	delete aview;
}
