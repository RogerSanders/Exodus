#include "GenericDataViewPresenter.h"
#include "GenericDataView.h"

//----------------------------------------------------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------------------------------------------------
GenericDataViewPresenter::GenericDataViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, GenericAccessMenus& owner, const IDevice& modelInstanceKey, IGenericAccess& model, const IGenericAccessPage* page)
:ViewPresenterBase(owner.GetAssemblyHandle(), viewGroupName, viewName, viewID, modelInstanceKey.GetDeviceInstanceName(), modelInstanceKey.GetDeviceModuleID(), modelInstanceKey.GetModuleDisplayName()), _owner(owner), _modelInstanceKey(modelInstanceKey), _model(model), _page(page)
{ }

//----------------------------------------------------------------------------------------------------------------------
// View creation and deletion
//----------------------------------------------------------------------------------------------------------------------
IView* GenericDataViewPresenter::CreateView(IUIManager& uiManager)
{
	return new GenericDataView(uiManager, *this, _model, _page);
}

//----------------------------------------------------------------------------------------------------------------------
void GenericDataViewPresenter::DeleteView(IView* view)
{
	delete view;
}

//----------------------------------------------------------------------------------------------------------------------
// Path functions
//----------------------------------------------------------------------------------------------------------------------
bool GenericDataViewPresenter::SelectFile(std::wstring& filePath, bool existingFile, const std::wstring& typeSelectionString, const std::wstring& defaultExtension, bool allowScanningIntoArchives) const
{
	if (existingFile)
	{
		return _owner.GetGUIInterface().SelectExistingFile(typeSelectionString, defaultExtension, filePath, L"", allowScanningIntoArchives, filePath);
	}
	else
	{
		return _owner.GetGUIInterface().SelectNewFile(typeSelectionString, defaultExtension, filePath, L"", filePath);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool GenericDataViewPresenter::SelectFolder(std::wstring& folderPath, bool existingFolder) const
{
	//##TODO##
	return false;
}
