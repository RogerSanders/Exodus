#include "DisassemblyViewPresenter.h"
#include "DisassemblyView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
DisassemblyViewPresenter::DisassemblyViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring DisassemblyViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Disassembly";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* DisassemblyViewPresenter::CreateView(IUIManager& uiManager)
{
	return new DisassemblyView(uiManager, *this, _model);
}

//----------------------------------------------------------------------------------------
void DisassemblyViewPresenter::DeleteView(IView* view)
{
	delete view;
}
