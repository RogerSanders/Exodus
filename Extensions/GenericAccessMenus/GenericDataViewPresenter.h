#ifndef __GENERICDATAVIEWPRESENTER_H__
#define __GENERICDATAVIEWPRESENTER_H__
#include "GenericAccess/GenericAccess.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "GenericAccessMenus.h"

class GenericDataViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	GenericDataViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, GenericAccessMenus& owner, const IDevice& modelInstanceKey, IGenericAccess& model, const IGenericAccessPage* page);

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

	//Path functions
	bool SelectFile(std::wstring& filePath, bool existingFile, const std::wstring& typeSelectionString, const std::wstring& defaultExtension, bool allowScanningIntoArchives) const;
	bool SelectFolder(std::wstring& folderPath, bool existingFolder) const;

private:
	GenericAccessMenus& _owner;
	const IDevice& _modelInstanceKey;
	IGenericAccess& _model;
	const IGenericAccessPage* _page;
};

#endif
