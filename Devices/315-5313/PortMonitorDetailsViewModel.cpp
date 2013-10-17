#include "PortMonitorDetailsViewModel.h"
#include "PortMonitorDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
S315_5313::PortMonitorDetailsViewModel::PortMonitorDetailsViewModel(const std::wstring& aviewModelGroupName, const std::wstring& aviewModelName, int aviewModelID, S315_5313* adevice, const PortMonitorEntry& aentry)
:ViewModelBase(aviewModelGroupName, aviewModelName, aviewModelID, false, true, adevice->GetDeviceInstanceName(), adevice->GetDeviceModuleID()), device(adevice), entry(aentry)
{}

//----------------------------------------------------------------------------------------
//Port monitor entry functions
//----------------------------------------------------------------------------------------
void S315_5313::PortMonitorDetailsViewModel::SetPortMonitorEntry(const PortMonitorEntry& aentry)
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
IView* S315_5313::PortMonitorDetailsViewModel::CreateView()
{
	return new PortMonitorDetailsView(device, entry);
}

//----------------------------------------------------------------------------------------
void S315_5313::PortMonitorDetailsViewModel::DeleteView(IView* aview)
{
	delete aview;
}
