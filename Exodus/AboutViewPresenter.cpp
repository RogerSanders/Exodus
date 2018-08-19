#include "AboutViewPresenter.h"
#include "AboutView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
AboutViewPresenter::AboutViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusInterface& owner, ExodusInterface& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID), _owner(owner), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring AboutViewPresenter::GetUnqualifiedViewTitle()
{
	return L"About";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* AboutViewPresenter::CreateView(IUIManager& uiManager)
{
	return new AboutView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void AboutViewPresenter::DeleteView(IView* view)
{
	delete view;
}
