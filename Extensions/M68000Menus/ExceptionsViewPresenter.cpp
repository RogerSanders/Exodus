#include "ExceptionsViewPresenter.h"
#include "ExceptionsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExceptionsViewPresenter::ExceptionsViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, M68000Menus& owner, const IDevice& modelInstanceKey, IM68000& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring ExceptionsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Exception Control";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* ExceptionsViewPresenter::CreateView(IUIManager& uiManager)
{
	return new ExceptionsView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------
void ExceptionsViewPresenter::DeleteView(IView* view)
{
	delete view;
}
