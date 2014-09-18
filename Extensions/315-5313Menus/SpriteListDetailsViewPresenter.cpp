#include "SpriteListDetailsViewPresenter.h"
#include "SpriteListDetailsView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
SpriteListDetailsViewPresenter::SpriteListDetailsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel, unsigned int aspriteIndex)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{
	spriteIndex = aspriteIndex;
}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring SpriteListDetailsViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Sprite Details";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* SpriteListDetailsViewPresenter::CreateView(IUIManager& uiManager)
{
	return new SpriteListDetailsView(uiManager, *this, model, spriteIndex);
}

//----------------------------------------------------------------------------------------
void SpriteListDetailsViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Sprite index functions
//----------------------------------------------------------------------------------------
void SpriteListDetailsViewPresenter::SetSpriteIndex(unsigned int aspriteIndex)
{
	spriteIndex = aspriteIndex;
	IView* viewBase = GetOpenView();
	if(viewBase != 0)
	{
		SpriteListDetailsView* view = dynamic_cast<SpriteListDetailsView*>(viewBase);
		if(view != 0)
		{
			view->SetSpriteIndex(spriteIndex);
		}
	}
}
