#include "DeviceControlViewPresenter.h"
#include "DeviceControlView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DeviceControlViewPresenter::DeviceControlViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring DeviceControlViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Device Control";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* DeviceControlViewPresenter::CreateView(IUIManager& uiManager)
{
	return new DeviceControlView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void DeviceControlViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
