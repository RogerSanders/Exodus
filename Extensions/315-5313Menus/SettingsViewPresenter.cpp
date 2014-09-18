#include "SettingsViewPresenter.h"
#include "SettingsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SettingsViewPresenter::SettingsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring SettingsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Settings";
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
