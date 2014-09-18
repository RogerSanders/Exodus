#include "MemoryEditorViewPresenter.h"
#include "MemoryEditorView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
MemoryEditorViewPresenter::MemoryEditorViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, MemoryMenus& aowner, const IDevice& amodelInstanceKey, IMemory& amodel)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel)
{}

//----------------------------------------------------------------------------------------
//View title functions
//----------------------------------------------------------------------------------------
std::wstring MemoryEditorViewPresenter::GetUnqualifiedViewTitle()
{
	return L"Memory Editor";
}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* MemoryEditorViewPresenter::CreateView(IUIManager& uiManager)
{
	return new MemoryEditorView(uiManager, *this, model);
}

//----------------------------------------------------------------------------------------
void MemoryEditorViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}
