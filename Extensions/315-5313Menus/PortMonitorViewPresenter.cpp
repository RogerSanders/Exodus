#include "PortMonitorViewPresenter.h"
#include "PortMonitorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PortMonitorViewPresenter::PortMonitorViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring PortMonitorViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Port Monitor";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* PortMonitorViewPresenter::CreateView(IUIManager& uiManager)
{
	return new PortMonitorView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void PortMonitorViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
