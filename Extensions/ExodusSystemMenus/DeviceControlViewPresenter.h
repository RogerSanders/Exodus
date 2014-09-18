#ifndef __DEVICECONTROLVIEWPRESENTER_H__
#define __DEVICECONTROLVIEWPRESENTER_H__
#include "ExodusExtensionInterface/ExodusExtensionInterface.pkg"
#include "ExodusSystemMenus.h"
#include "SystemInterface/SystemInterface.pkg"

class DeviceControlViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	DeviceControlViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

private:
	ExodusSystemMenus& owner;
	ISystemGUIInterface& model;
};

#endif
