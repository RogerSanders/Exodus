#include "SpriteListViewPresenter.h"
#include "SpriteListView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SpriteListViewPresenter::SpriteListViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
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
	return new SpriteListView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void SpriteListViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Window functions
//----------------------------------------------------------------------------------------
void SpriteListViewPresenter::OpenSpriteListDetailsView(unsigned int aspriteIndex)
{
	owner.OpenSpriteListDetailsView(&modelInstanceKey, aspriteIndex);
}
