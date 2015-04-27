#ifndef __MODULEMANAGERVIEWPRESENTER_H__
#define __MODULEMANAGERVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusInterface.h"

class ModuleManagerViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	ModuleManagerViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusInterface& aowner, ExodusInterface& amodel);

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
