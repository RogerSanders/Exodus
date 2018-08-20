#include "PlaneViewPresenter.h"
#include "PlaneView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
PlaneViewPresenter::PlaneViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, S315_5313Menus& owner, const IDevice& modelInstanceKey, IS315_5313& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{ }

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring PlaneViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Plane Viewer";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* PlaneViewPresenter::CreateView(IUIManager& uiManager)
{
	return new PlaneView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void PlaneViewPresenter::DeleteView(IView* view)
{
	delete view;
}

//----------------------------------------------------------------------------------------------------------------------
// System interface functions
//----------------------------------------------------------------------------------------------------------------------
ISystemExtensionInterface& PlaneViewPresenter::GetSystemInterface() const
{
	return _owner.GetSystemInterface();
}
