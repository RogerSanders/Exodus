#include "EmbeddedROMViewModel.h"
#include "EmbeddedROMView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::EmbeddedROMViewModel::EmbeddedROMViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, System* adevice)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, true, false, L"", 0), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* System::EmbeddedROMViewModel::CreateView()
{
	return new EmbeddedROMView(device);
}

//----------------------------------------------------------------------------------------
void System::EmbeddedROMViewModel::DeleteView(IView* aview)
{
	delete aview;
}
