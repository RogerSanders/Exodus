#include "PortMonitorDetailsViewModel.h"
#include "PortMonitorDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
VDP::PortMonitorDetailsViewModel::PortMonitorDetailsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, VDP* adevice, const PortMonitorEntry& aentry)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice), entry(aentry)
{}

//----------------------------------------------------------------------------------------
//Port monitor entry functions
//----------------------------------------------------------------------------------------
void VDP::PortMonitorDetailsViewModel::SetPortMonitorEntry(const PortMonitorEntry& aentry)
{
	entry = aentry;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		PortMonitorDetailsView* view = dynamic_cast<PortMonitorDetailsView*>(viewBase);
		if(view != 0)
		{
			view->SetPortMonitorEntry(entry);
		}
	}
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* VDP::PortMonitorDetailsViewModel::CreateView()
{
	return new PortMonitorDetailsView(device, entry);
}

//----------------------------------------------------------------------------------------
void VDP::PortMonitorDetailsViewModel::DeleteView(IView* aview)
{
	delete aview;
}
