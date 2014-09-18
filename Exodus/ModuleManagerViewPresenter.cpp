#include "SettingsViewPresenter.h"
#include "SettingsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SettingsViewPresenter::SettingsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusInterface& aowner, ExodusInterface& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring SettingsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"About";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* SettingsViewPresenter::CreateView(IUIManager& uiManager)
{
	return new SettingsView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void SettingsViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
