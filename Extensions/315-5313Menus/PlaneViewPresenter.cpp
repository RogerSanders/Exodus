#include "PlaneViewPresenter.h"
#include "PlaneView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PlaneViewPresenter::PlaneViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring PlaneViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Plane Viewer";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* PlaneViewPresenter::CreateView(IUIManager& uiManager)
{
	return new PlaneView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void PlaneViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//System interface functions
//----------------------------------------------------------------------------------------
ISystemExtensionInterface& PlaneViewPresenter::GetSystemInterface() const
{
	return owner.GetSystemInterface();
}
