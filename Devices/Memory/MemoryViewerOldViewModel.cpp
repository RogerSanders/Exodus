#include "MemoryViewerOldViewModel.h"
#include "MemoryViewerOldView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryViewerOldViewModel::MemoryViewerOldViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, MemoryRead* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
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
