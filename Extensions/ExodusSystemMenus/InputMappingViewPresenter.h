#ifndef __INPUTMAPPINGVIEWPRESENTER_H__
#define __INPUTMAPPINGVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusSystemMenus.h"

class InputMappingViewPresenter :public ViewPresenterBase
{
public:
	// Constructors
	InputMappingViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model);

	// View title functions
	static std::wstring GetUnqualifiedViewTitle();

	// View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

	// Window functions
	void OpenInputMappingDetailsView(IDevice* targetDevice);
	void CloseInputMappingDetailsView();

private:
	ExodusSystemMenus& _owner;
	ISystemGUIInterface& _model;
};

#endif
