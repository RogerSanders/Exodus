#include "WatchpointViewPresenter.h"
#include "WatchpointView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
WatchpointViewPresenter::WatchpointViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring WatchpointViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Watchpoints";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* WatchpointViewPresenter::CreateView(IUIManager& uiManager)
{
	return new WatchpointView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void WatchpointViewPresenter::DeleteView(IView* view)
{
	delete view;
}
