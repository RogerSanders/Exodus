#include "EmbeddedROMViewPresenter.h"
#include "EmbeddedROMView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
EmbeddedROMViewPresenter::EmbeddedROMViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID), _owner(owner), _model(model)
{ }

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring EmbeddedROMViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Embedded ROM Selection";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* EmbeddedROMViewPresenter::CreateView(IUIManager& uiManager)
{
	return new EmbeddedROMView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void EmbeddedROMViewPresenter::DeleteView(IView* view)
{
	delete view;
}

//----------------------------------------------------------------------------------------------------------------------
// Interface functions
//----------------------------------------------------------------------------------------------------------------------
IGUIExtensionInterface& EmbeddedROMViewPresenter::GetGUIInterface() const
{
	return _owner.GetGUIInterface();
}
