#include "AboutViewPresenter.h"
#include "AboutView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
AboutViewPresenter::AboutViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusInterface& aowner, ExodusInterface& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring AboutViewPresenter::GetUnqualifiedViewTitle()
{
	return L"About";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* AboutViewPresenter::CreateView(IUIManager& uiManager)
{
	return new AboutView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void AboutViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
