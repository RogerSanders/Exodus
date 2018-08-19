#include "VRAMViewPresenter.h"
#include "VRAMView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
VRAMViewPresenter::VRAMViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, S315_5313Menus& owner, const IDevice& modelInstanceKey, IS315_5313& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring VRAMViewPresenter::GetUnqualifiedViewTitle()
{
	return L"VRAM Pattern Viewer";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* VRAMViewPresenter::CreateView(IUIManager& uiManager)
{
	return new VRAMView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void VRAMViewPresenter::DeleteView(IView* view)
{
	delete view;
}
