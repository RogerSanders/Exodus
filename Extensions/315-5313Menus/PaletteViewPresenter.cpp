#include "PaletteViewPresenter.h"
#include "PaletteView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
PaletteViewPresenter::PaletteViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, S315_5313Menus& owner, const IDevice& modelInstanceKey, IS315_5313& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring PaletteViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Palette";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* PaletteViewPresenter::CreateView(IUIManager& uiManager)
{
	return new PaletteView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void PaletteViewPresenter::DeleteView(IView* view)
{
	delete view;
}
