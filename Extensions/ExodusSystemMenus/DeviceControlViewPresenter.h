#ifndef __DEVICECONTROLVIEWPRESENTER_H__
#define __DEVICECONTROLVIEWPRESENTER_H__
#include "ExtensionInterface/ExtensionInterface.pkg"
#include "ExodusSystemMenus.h"
#include "SystemInterface/SystemInterface.pkg"

class DeviceControlViewPresenter :public ViewPresenterBase
{
public:
	// Constructors
	DeviceControlViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model);

	// View title functions
	static std::wstring GetUnqualifiedViewTitle();

	// View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

private:
	ExodusSystemMenus& _owner;
	ISystemGUIInterface& _model;
};

#endif
