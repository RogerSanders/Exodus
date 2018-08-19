#ifndef __MODULEMANAGERVIEWPRESENTER_H__
#define __MODULEMANAGERVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusInterface.h"

class ModuleManagerViewPresenter :public ViewPresenterBase
{
public:
	// Constructors
	ModuleManagerViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusInterface& owner, ExodusInterface& model);

	// View title functions
	static std::wstring GetUnqualifiedViewTitle();

	// View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

private:
	ExodusInterface& _owner;
	ExodusInterface& _model;
};

#endif
