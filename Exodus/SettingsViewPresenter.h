#ifndef __SETTINGSVIEWPRESENTER_H__
#define __SETTINGSVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusInterface.h"

class SettingsViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	SettingsViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusInterface& owner, ExodusInterface& model);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

private:
	ExodusInterface& _owner;
	ExodusInterface& _model;
};

#endif
