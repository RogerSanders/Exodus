#include "DisassemblyViewPresenter.h"
#include "DisassemblyView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DisassemblyViewPresenter::DisassemblyViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring DisassemblyViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Disassembly";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* DisassemblyViewPresenter::CreateView(IUIManager& uiManager)
{
	return new DisassemblyView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void DisassemblyViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
