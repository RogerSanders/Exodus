#include "LayerRemovalViewPresenter.h"
#include "LayerRemovalView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LayerRemovalViewPresenter::LayerRemovalViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring LayerRemovalViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Layer Removal";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* LayerRemovalViewPresenter::CreateView(IUIManager& uiManager)
{
	return new LayerRemovalView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void LayerRemovalViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
