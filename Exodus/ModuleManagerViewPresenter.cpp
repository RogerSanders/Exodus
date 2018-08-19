#include "ModuleManagerViewPresenter.h"
#include "ModuleManagerView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
ModuleManagerViewPresenter::ModuleManagerViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusInterface& owner, ExodusInterface& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID), _owner(owner), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring ModuleManagerViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Module Manager";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* ModuleManagerViewPresenter::CreateView(IUIManager& uiManager)
{
	return new ModuleManagerView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void ModuleManagerViewPresenter::DeleteView(IView* view)
{
	delete view;
}
