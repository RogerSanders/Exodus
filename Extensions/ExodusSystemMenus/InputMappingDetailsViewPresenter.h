#ifndef __INPUTMAPPINGDETAILSVIEWPRESENTER_H__
#define __INPUTMAPPINGDETAILSVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusSystemMenus.h"

class InputMappingDetailsViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	InputMappingDetailsViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model, IDevice* targetDevice);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

	//Target device functions
	void SetTargetDevice(IDevice* targetDevice);

private:
	ExodusSystemMenus& _owner;
	ISystemGUIInterface& _model;
	IDevice* _targetDevice;
};

#endif
