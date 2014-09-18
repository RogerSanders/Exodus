#include "DebuggerViewPresenter.h"
#include "DebuggerView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DebuggerViewPresenter::DebuggerViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, YM2612Menus& aowner, const IDevice& amodelInstanceKey, IYM2612& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring DebuggerViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Debugger";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* DebuggerViewPresenter::CreateView(IUIManager& uiManager)
{
	return new DebuggerView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void DebuggerViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void DebuggerViewPresenter::OpenOperatorView(unsigned int achannelNo, unsigned int aoperatorNo)
{
	owner.OpenOperatorView(&modelInstanceKey, achannelNo, aoperatorNo);
}
