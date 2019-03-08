#include "TraceViewPresenter.h"
#include "TraceView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
TraceViewPresenter::TraceViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{ }

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring TraceViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Trace";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* TraceViewPresenter::CreateView(IUIManager& uiManager)
{
	return new TraceView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void TraceViewPresenter::DeleteView(IView* view)
{
	delete view;
}

//----------------------------------------------------------------------------------------------------------------------
// Interface functions
//----------------------------------------------------------------------------------------------------------------------
IGUIExtensionInterface& TraceViewPresenter::GetGUIInterface() const
{
	return _owner.GetGUIInterface();
}
