#include "PropertiesViewPresenter.h"
#include "PropertiesView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PropertiesViewPresenter::PropertiesViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, SN76489Menus& aowner, const IDevice& amodelInstanceKey, ISN76489& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring PropertiesViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Properties";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* PropertiesViewPresenter::CreateView(IUIManager& uiManager)
{
	return new PropertiesView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void PropertiesViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
