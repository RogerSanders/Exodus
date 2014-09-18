#include "ExceptionsViewPresenter.h"
#include "ExceptionsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ExceptionsViewPresenter::ExceptionsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, M68000Menus& aowner, const IDevice& amodelInstanceKey, IM68000& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring ExceptionsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Exception Control";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* ExceptionsViewPresenter::CreateView(IUIManager& uiManager)
{
	return new ExceptionsView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void ExceptionsViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
