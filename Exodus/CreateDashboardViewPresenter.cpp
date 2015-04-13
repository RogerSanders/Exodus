#include "CreateDashboardViewPresenter.h"
#include "CreateDashboardView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
CreateDashboardViewPresenter::CreateDashboardViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusInterface& aowner, ExodusInterface& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring CreateDashboardViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Create Dashboard";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* CreateDashboardViewPresenter::CreateView(IUIManager& uiManager)
{
	return new CreateDashboardView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void CreateDashboardViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
