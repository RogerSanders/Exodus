#include "DeviceControlViewPresenter.h"
#include "DeviceControlView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
DeviceControlViewPresenter::DeviceControlViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID), _owner(owner), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring DeviceControlViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Device Control";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* DeviceControlViewPresenter::CreateView(IUIManager& uiManager)
{
	return new DeviceControlView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void DeviceControlViewPresenter::DeleteView(IView* view)
{
	delete view;
}
