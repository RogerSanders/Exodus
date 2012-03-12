#include "PropertiesViewModel.h"
#include "PropertiesView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SN76489::PropertiesViewModel::PropertiesViewModel(const std::wstring& amenuHandlerName, int aviewModelID, SN76489* adevice)
:ViewModelBase(amenuHandlerName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* SN76489::PropertiesViewModel::CreateView()
{
	return new PropertiesView(device);
}

//----------------------------------------------------------------------------------------
void SN76489::PropertiesViewModel::DeleteView(IView* aview)
{
	delete aview;
}
