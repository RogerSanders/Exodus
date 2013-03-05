#include "MemoryEditorOldViewModel.h"
#include "MemoryEditorOldView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryRead::MemoryEditorOldViewModel::MemoryEditorOldViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, MemoryRead* adevice, unsigned int atargetMemoryAddress)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{
	targetMemoryAddress = atargetMemoryAddress;
}

//----------------------------------------------------------------------------------------
//Memory address functions
//----------------------------------------------------------------------------------------
void MemoryRead::MemoryEditorOldViewModel::SetTargetMemoryAddress(unsigned int atargetMemoryAddress)
{
	targetMemoryAddress = atargetMemoryAddress;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		MemoryEditorOldView* view = dynamic_cast<MemoryEditorOldView*>(viewBase);
		if(view != 0)
		{
			view->SetTargetMemoryAddress(targetMemoryAddress);
		}
	}
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* MemoryRead::MemoryEditorOldViewModel::CreateView()
{
	return new MemoryEditorOldView(device, targetMemoryAddress);
}

//----------------------------------------------------------------------------------------
void MemoryRead::MemoryEditorOldViewModel::DeleteView(IView* aview)
{
	delete aview;
}
