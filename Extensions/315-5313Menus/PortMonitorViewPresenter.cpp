#include "PortMonitorViewPresenter.h"
#include "PortMonitorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PortMonitorViewPresenter::PortMonitorViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, S315_5313Menus& owner, const IDevice& modelInstanceKey, IS315_5313& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring PortMonitorViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Port Monitor";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* PortMonitorViewPresenter::CreateView(IUIManager& uiManager)
{
	return new PortMonitorView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------
void PortMonitorViewPresenter::DeleteView(IView* view)
{
	delete view;
}
