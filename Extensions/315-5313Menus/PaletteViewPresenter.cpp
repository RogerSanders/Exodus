#include "PaletteViewPresenter.h"
#include "PaletteView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
PaletteViewPresenter::PaletteViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring PaletteViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Palette";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* PaletteViewPresenter::CreateView(IUIManager& uiManager)
{
	return new PaletteView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void PaletteViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
