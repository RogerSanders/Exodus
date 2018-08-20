#include "InputMappingViewPresenter.h"
#include "InputMappingView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
InputMappingViewPresenter::InputMappingViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID), _owner(owner), _model(model)
{ }

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring InputMappingViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Input Mapping";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* InputMappingViewPresenter::CreateView(IUIManager& uiManager)
{
	return new InputMappingView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void InputMappingViewPresenter::DeleteView(IView* view)
{
	delete view;
}

//----------------------------------------------------------------------------------------------------------------------
// Window functions
//----------------------------------------------------------------------------------------------------------------------
void InputMappingViewPresenter::OpenInputMappingDetailsView(IDevice* targetDevice)
{
	_owner.OpenInputMappingDetailsView(targetDevice);
}

//----------------------------------------------------------------------------------------------------------------------
void InputMappingViewPresenter::CloseInputMappingDetailsView()
{
	_owner.CloseInputMappingDetailsView();
}
