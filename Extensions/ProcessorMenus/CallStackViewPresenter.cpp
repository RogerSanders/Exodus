#include "CallStackViewPresenter.h"
#include "CallStackView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
CallStackViewPresenter::CallStackViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring CallStackViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Call Stack";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* CallStackViewPresenter::CreateView(IUIManager& uiManager)
{
	return new CallStackView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void CallStackViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
