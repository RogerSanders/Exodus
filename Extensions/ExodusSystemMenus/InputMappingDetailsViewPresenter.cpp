#include "InputMappingDetailsViewPresenter.h"
#include "InputMappingDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
InputMappingDetailsViewPresenter::InputMappingDetailsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel, IDevice* atargetDevice)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel), targetDevice(atargetDevice)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring InputMappingDetailsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Device Input Mapping";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* InputMappingDetailsViewPresenter::CreateView(IUIManager& uiManager)
{
	return new InputMappingDetailsView(uiManager, *this, model, targetDevice);
}

//----------------------------------------------------------------------------------------
void InputMappingDetailsViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Target device functions
//----------------------------------------------------------------------------------------
void InputMappingDetailsViewPresenter::SetTargetDevice(IDevice* atargetDevice)
{
	targetDevice = atargetDevice;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		InputMappingDetailsView* view = dynamic_cast<InputMappingDetailsView*>(viewBase);
		if(view != 0)
		{
			view->SetTargetDevice(targetDevice);
		}
	}
}
