#ifndef __ABOUTVIEWPRESENTER_H__
#define __ABOUTVIEWPRESENTER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "ExodusInterface.h"

class AboutViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	AboutViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusInterface& aowner, ExodusInterface& amodel);

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
