#include "CallStackViewPresenter.h"
#include "CallStackView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
CallStackViewPresenter::CallStackViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring CallStackViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Call Stack";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* CallStackViewPresenter::CreateView(IUIManager& uiManager)
{
	return new CallStackView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------
void CallStackViewPresenter::DeleteView(IView* view)
{
	delete view;
}
