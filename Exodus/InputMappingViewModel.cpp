#include "InputMappingViewModel.h"
#include "InputMappingView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::InputMappingViewModel::InputMappingViewModel(const std::wstring& amenuHandlerName, int aviewModelID, System* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, true, false, L"", 0), device(adevice)
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
