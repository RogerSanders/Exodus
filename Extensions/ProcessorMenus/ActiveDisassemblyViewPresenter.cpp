#include "ActiveDisassemblyViewPresenter.h"
#include "ActiveDisassemblyView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ActiveDisassemblyViewPresenter::ActiveDisassemblyViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{ }

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring ActiveDisassemblyViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Active Disassembly";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* ActiveDisassemblyViewPresenter::CreateView(IUIManager& uiManager)
{
	return new ActiveDisassemblyView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void ActiveDisassemblyViewPresenter::DeleteView(IView* view)
{
	delete view;
}
