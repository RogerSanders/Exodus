#include "RegistersViewPresenter.h"
#include "RegistersView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
RegistersViewPresenter::RegistersViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, YM2612Menus& owner, const IDevice& modelInstanceKey, IYM2612& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring RegistersViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Raw Registers";
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
