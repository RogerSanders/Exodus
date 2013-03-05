#include "InputMappingDetailsViewModel.h"
#include "InputMappingDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::InputMappingDetailsViewModel::InputMappingDetailsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, System* adevice, IDevice* atargetDevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, true, false, L"", 0), device(adevice), targetDevice(atargetDevice)
{}

//----------------------------------------------------------------------------------------
//Target device functions
//----------------------------------------------------------------------------------------
void System::InputMappingDetailsViewModel::SetTargetDevice(IDevice* atargetDevice)
{
	targetDevice = atargetDevice;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		InputMappingDetailsView* view = dynamic_cast<InputMappingDetailsView*>(viewBase);
		if(view != 0)
		{
			view->SetTargetDevice(targetDevice);
		}
	}
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* System::InputMappingDetailsViewModel::CreateView()
{
	return new InputMappingDetailsView(device, targetDevice);
}

//----------------------------------------------------------------------------------------
void System::InputMappingDetailsViewModel::DeleteView(IView* aview)
{
	delete aview;
}
