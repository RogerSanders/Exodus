#include "ControlViewPresenter.h"
#include "ControlView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ControlViewPresenter::ControlViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring ControlViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Processor Control";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* ControlViewPresenter::CreateView(IUIManager& uiManager)
{
	return new ControlView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void ControlViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
