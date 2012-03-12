#include "MemoryEditorViewModel.h"
#include "MemoryEditorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryEditorViewModel::MemoryEditorViewModel(const std::wstring& amenuHandlerName, int aviewModelID, MemoryRead* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
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
