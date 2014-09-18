#include "InputMappingViewPresenter.h"
#include "InputMappingView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
InputMappingViewPresenter::InputMappingViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring InputMappingViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Input Mapping";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* InputMappingViewPresenter::CreateView(IUIManager& uiManager)
{
	return new InputMappingView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void InputMappingViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void InputMappingViewPresenter::OpenInputMappingDetailsView(IDevice* targetDevice)
{
	owner.OpenInputMappingDetailsView(targetDevice);
}

//----------------------------------------------------------------------------------------
void InputMappingViewPresenter::CloseInputMappingDetailsView()
{
	owner.CloseInputMappingDetailsView();
}
