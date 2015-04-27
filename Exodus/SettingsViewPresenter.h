#ifndef __SETTINGSVIEWPRESENTER_H__
#define __SETTINGSVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusInterface.h"

class SettingsViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	SettingsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusInterface& aowner, ExodusInterface& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

private:
	ExodusInterface& owner;
	ExodusInterface& model;
};

#endif
