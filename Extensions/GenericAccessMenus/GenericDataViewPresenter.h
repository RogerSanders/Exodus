#ifndef __GENERICDATAVIEWPRESENTER_H__
#define __GENERICDATAVIEWPRESENTER_H__
#include "GenericAccess/GenericAccess.pkg"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "GenericAccessMenus.h"

class GenericDataViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	GenericDataViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, GenericAccessMenus& aowner, const IDevice& amodelInstanceKey, IGenericAccess& amodel, const IGenericAccessPage* apage);

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Path functions
	bool SelectFile(std::wstring& filePath, bool existingFile, const std::wstring& typeSelectionString, const std::wstring& defaultExtension, bool allowScanningIntoArchives) const;
	bool SelectFolder(std::wstring& folderPath, bool existingFolder) const;

private:
	GenericAccessMenus& owner;
	const IDevice& modelInstanceKey;
	IGenericAccess& model;
	const IGenericAccessPage* page;
};

#endif
