#include "ControlViewPresenter.h"
#include "ControlView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ControlViewPresenter::ControlViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring ControlViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Processor Control";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* ControlViewPresenter::CreateView(IUIManager& uiManager)
{
	return new ControlView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------
void ControlViewPresenter::DeleteView(IView* view)
{
	delete view;
}
