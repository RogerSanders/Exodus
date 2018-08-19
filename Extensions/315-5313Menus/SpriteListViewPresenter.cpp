#include "SpriteListViewPresenter.h"
#include "SpriteListView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SpriteListViewPresenter::SpriteListViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, S315_5313Menus& owner, const IDevice& modelInstanceKey, IS315_5313& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring SpriteListViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Sprite List";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* SpriteListViewPresenter::CreateView(IUIManager& uiManager)
{
	return new SpriteListView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------
void SpriteListViewPresenter::DeleteView(IView* view)
{
	delete view;
}
