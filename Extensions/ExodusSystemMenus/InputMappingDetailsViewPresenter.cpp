#include "InputMappingDetailsViewPresenter.h"
#include "InputMappingDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
InputMappingDetailsViewPresenter::InputMappingDetailsViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model, IDevice* targetDevice)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID), _owner(owner), _model(model), _targetDevice(targetDevice)
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
	return new InputMappingDetailsView(uiManager, *this, _model, _targetDevice);
}

//----------------------------------------------------------------------------------------
void InputMappingDetailsViewPresenter::DeleteView(IView* view)
{
	delete view;
}

//----------------------------------------------------------------------------------------
//Target device functions
//----------------------------------------------------------------------------------------
void InputMappingDetailsViewPresenter::SetTargetDevice(IDevice* targetDevice)
{
	_targetDevice = targetDevice;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		InputMappingDetailsView* view = dynamic_cast<InputMappingDetailsView*>(viewBase);
		if(view != 0)
		{
			view->SetTargetDevice(_targetDevice);
		}
	}
}
