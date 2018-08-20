#include "RegistersViewPresenter.h"
#include "RegistersView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RegistersViewPresenter::RegistersViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, S315_5313Menus& owner, const IDevice& modelInstanceKey, IS315_5313& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), owner(owner), modelInstanceKey(modelInstanceKey), _model(model)
{ }

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring RegistersViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Registers";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* RegistersViewPresenter::CreateView(IUIManager& uiManager)
{
	return new RegistersView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void RegistersViewPresenter::DeleteView(IView* view)
{
	delete view;
}
