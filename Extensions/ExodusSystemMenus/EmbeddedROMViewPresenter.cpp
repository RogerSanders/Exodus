#include "EmbeddedROMViewPresenter.h"
#include "EmbeddedROMView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
EmbeddedROMViewPresenter::EmbeddedROMViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID), owner(aowner), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring EmbeddedROMViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Embedded ROM Selection";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* EmbeddedROMViewPresenter::CreateView(IUIManager& uiManager)
{
	return new EmbeddedROMView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void EmbeddedROMViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Interface functions
//----------------------------------------------------------------------------------------
IGUIExtensionInterface& EmbeddedROMViewPresenter::GetGUIInterface() const
{
	return owner.GetGUIInterface();
}
