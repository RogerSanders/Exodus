#include "DebuggerViewPresenter.h"
#include "DebuggerView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
DebuggerViewPresenter::DebuggerViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, YM2612Menus& owner, const IDevice& modelInstanceKey, IYM2612& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring DebuggerViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Debugger";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* DebuggerViewPresenter::CreateView(IUIManager& uiManager)
{
	return new DebuggerView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void DebuggerViewPresenter::DeleteView(IView* view)
{
	delete view;
}

//----------------------------------------------------------------------------------------------------------------------
// Window functions
//----------------------------------------------------------------------------------------------------------------------
void DebuggerViewPresenter::OpenOperatorView(unsigned int channelNo, unsigned int operatorNo)
{
	_owner.OpenOperatorView(&_modelInstanceKey, channelNo, operatorNo);
}
