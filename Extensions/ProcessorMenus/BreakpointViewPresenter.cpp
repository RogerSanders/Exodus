#include "BreakpointViewPresenter.h"
#include "BreakpointView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
BreakpointViewPresenter::BreakpointViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring BreakpointViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Breakpoints";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* BreakpointViewPresenter::CreateView(IUIManager& uiManager)
{
	return new BreakpointView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void BreakpointViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
