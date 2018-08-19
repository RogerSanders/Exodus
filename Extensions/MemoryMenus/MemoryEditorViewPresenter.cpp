#include "MemoryEditorViewPresenter.h"
#include "MemoryEditorView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
MemoryEditorViewPresenter::MemoryEditorViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, MemoryMenus& owner, const IDevice& modelInstanceKey, IMemory& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------------------------------------
// View title functions
//----------------------------------------------------------------------------------------------------------------------
std::wstring MemoryEditorViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Memory Editor";
}

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* MemoryEditorViewPresenter::CreateView(IUIManager& uiManager)
{
	return new MemoryEditorView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------------------------------------
void MemoryEditorViewPresenter::DeleteView(IView* view)
{
	delete view;
}
