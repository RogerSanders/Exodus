#include "MemoryEditorViewModel.h"
#include "MemoryEditorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryEditorViewModel::MemoryEditorViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, MemoryRead* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* MemoryRead::MemoryEditorViewModel::CreateView()
{
	return new MemoryEditorView(device);
}

//----------------------------------------------------------------------------------------
void MemoryRead::MemoryEditorViewModel::DeleteView(IView* aview)
{
	delete aview;
}
