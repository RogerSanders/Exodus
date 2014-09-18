#include "DebugSettingsViewPresenter.h"
#include "DebugSettingsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebugSettingsViewPresenter::DebugSettingsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring DebugSettingsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Debug Settings";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* DebugSettingsViewPresenter::CreateView(IUIManager& uiManager)
{
	return new DebugSettingsView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void DebugSettingsViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
