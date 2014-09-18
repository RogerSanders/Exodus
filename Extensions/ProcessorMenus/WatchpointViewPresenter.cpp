#include "WatchpointViewPresenter.h"
#include "WatchpointView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
WatchpointViewPresenter::WatchpointViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring WatchpointViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Watchpoints";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* WatchpointViewPresenter::CreateView(IUIManager& uiManager)
{
	return new WatchpointView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void WatchpointViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
