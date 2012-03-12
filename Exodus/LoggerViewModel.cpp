#include "LoggerViewModel.h"
#include "LoggerView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
System::LoggerViewModel::LoggerViewModel(const std::wstring& amenuHandlerName, int aviewModelID, System* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, true, false, L"", 0), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* System::LoggerViewModel::CreateView()
{
	return new LoggerView(device);
}

//----------------------------------------------------------------------------------------
void System::LoggerViewModel::DeleteView(IView* aview)
{
	delete aview;
}