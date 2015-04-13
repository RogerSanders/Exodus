#include "ModuleManagerViewPresenter.h"
#include "ModuleManagerView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
ModuleManagerViewPresenter::ModuleManagerViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusInterface& aowner, ExodusInterface& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring ModuleManagerViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Module Manager";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* ModuleManagerViewPresenter::CreateView(IUIManager& uiManager)
{
	return new ModuleManagerView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void ModuleManagerViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
