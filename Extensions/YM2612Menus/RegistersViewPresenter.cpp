#include "RegistersViewPresenter.h"
#include "RegistersView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
RegistersViewPresenter::RegistersViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, YM2612Menus& aowner, const IDevice& amodelInstanceKey, IYM2612& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring RegistersViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Raw Registers";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* RegistersViewPresenter::CreateView(IUIManager& uiManager)
{
	return new RegistersView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void RegistersViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
