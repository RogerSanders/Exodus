#ifndef __CREATEDASHBOARDVIEWPRESENTER_H__
#define __CREATEDASHBOARDVIEWPRESENTER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "ExodusInterface.h"

class CreateDashboardViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	CreateDashboardViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusInterface& aowner, ExodusInterface& amodel);

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
