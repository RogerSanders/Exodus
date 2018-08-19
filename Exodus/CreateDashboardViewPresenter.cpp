#include "CreateDashboardViewPresenter.h"
#include "CreateDashboardView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
CreateDashboardViewPresenter::CreateDashboardViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusInterface& owner, ExodusInterface& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID), _owner(owner), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring CreateDashboardViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Create Dashboard";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* CreateDashboardViewPresenter::CreateView(IUIManager& uiManager)
{
	return new CreateDashboardView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void CreateDashboardViewPresenter::DeleteView(IView* view)
{
	delete view;
}
