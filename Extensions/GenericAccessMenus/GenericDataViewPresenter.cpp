#include "GenericDataViewPresenter.h"
#include "GenericDataView.h"

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericDataViewPresenter::GenericDataViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, GenericAccessMenus& aowner, const IDevice& amodelInstanceKey, IGenericAccess& amodel, const IGenericAccessPage* apage)
:ViewPresenterBase(aowner.GetAssemblyHandle(), aviewGroupName, aviewName, aviewID, amodelInstanceKey.GetDeviceInstanceName(), amodelInstanceKey.GetDeviceModuleID(), amodelInstanceKey.GetModuleDisplayName()), owner(aowner), modelInstanceKey(amodelInstanceKey), model(amodel), page(apage)
{}

//----------------------------------------------------------------------------------------
//View creation and deletion
//----------------------------------------------------------------------------------------
IView* GenericDataViewPresenter::CreateView(IUIManager& uiManager)
{
	return new GenericDataView(uiManager, *this, model, page);
}

//----------------------------------------------------------------------------------------
void GenericDataViewPresenter::DeleteView(IView* aview)
{
	delete aview;
}

//----------------------------------------------------------------------------------------
//Path functions
//----------------------------------------------------------------------------------------
bool GenericDataViewPresenter::SelectFile(std::wstring& filePath, bool existingFile, const std::wstring& typeSelectionString, const std::wstring& defaultExtension, bool allowScanningIntoArchives) const
{
	if(existingFile)
	{
		return owner.GetGUIInterface().SelectExistingFile(typeSelectionString, defaultExtension, filePath, L"", allowScanningIntoArchives, filePath);
	}
	else
	{
		return owner.GetGUIInterface().SelectNewFile(typeSelectionString, defaultExtension, filePath, L"", filePath);
	}
}

//----------------------------------------------------------------------------------------
bool GenericDataViewPresenter::SelectFolder(std::wstring& folderPath, bool existingFolder) const
{
	//##TODO##
	return false;
}
