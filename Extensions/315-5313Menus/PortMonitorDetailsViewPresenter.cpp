#include "PortMonitorDetailsViewPresenter.h"
#include "PortMonitorDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PortMonitorDetailsViewPresenter::PortMonitorDetailsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel, const IS315_5313::PortMonitorEntry& aentry)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel), entry(aentry)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring PortMonitorDetailsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Port Monitor Entry Details";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* PortMonitorDetailsViewPresenter::CreateView(IUIManager& uiManager)
{
	return new PortMonitorDetailsView(uiManager, *this, model, entry);
}

//----------------------------------------------------------------------------------------
void PortMonitorDetailsViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Port monitor entry functions
//----------------------------------------------------------------------------------------
void PortMonitorDetailsViewPresenter::SetPortMonitorEntry(const IS315_5313::PortMonitorEntry& aentry)
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
