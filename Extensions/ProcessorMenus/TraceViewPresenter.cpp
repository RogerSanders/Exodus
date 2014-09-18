#include "TraceViewPresenter.h"
#include "TraceView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
TraceViewPresenter::TraceViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring TraceViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Trace";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* TraceViewPresenter::CreateView(IUIManager& uiManager)
{
	return new TraceView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void TraceViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
