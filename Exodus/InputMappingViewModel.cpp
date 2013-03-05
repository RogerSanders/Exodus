#include "InputMappingViewModel.h"
#include "InputMappingView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::InputMappingViewModel::InputMappingViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, System* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, true, false, L"", 0), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* System::InputMappingViewModel::CreateView()
{
	return new InputMappingView(device);
}

//----------------------------------------------------------------------------------------
void System::InputMappingViewModel::DeleteView(IView* aview)
{
	delete aview;
}
