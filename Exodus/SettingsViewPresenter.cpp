#include "SettingsViewPresenter.h"
#include "SettingsView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
SettingsViewPresenter::SettingsViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusInterface& owner, ExodusInterface& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID), _owner(owner), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring SettingsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Settings";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* SettingsViewPresenter::CreateView(IUIManager& uiManager)
{
	return new SettingsView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void SettingsViewPresenter::DeleteView(IView* view)
{
	delete view;
}
