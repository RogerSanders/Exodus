#include "ActiveDisassemblyViewPresenter.h"
#include "ActiveDisassemblyView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ActiveDisassemblyViewPresenter::ActiveDisassemblyViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring ActiveDisassemblyViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Active Disassembly";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* ActiveDisassemblyViewPresenter::CreateView(IUIManager& uiManager)
{
	return new ActiveDisassemblyView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void ActiveDisassemblyViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
