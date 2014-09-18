#ifndef __INPUTMAPPINGDETAILSVIEWPRESENTER_H__
#define __INPUTMAPPINGDETAILSVIEWPRESENTER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "ExodusSystemMenus.h"

class InputMappingDetailsViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	InputMappingDetailsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel, IDevice* atargetDevice);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Target device functions
	void SetTargetDevice(IDevice* atargetDevice);

private:
	ExodusSystemMenus& owner;
	ISystemGUIInterface& model;
	IDevice* targetDevice;
};

#endif
