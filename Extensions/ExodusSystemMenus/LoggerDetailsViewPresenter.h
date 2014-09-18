#ifndef __LOGGERDETAILSVIEWPRESENTER_H__
#define __LOGGERDETAILSVIEWPRESENTER_H__
#include "SystemInterface/SystemInterface.pkg"
#include "ExodusSystemMenus.h"

class LoggerDetailsViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	LoggerDetailsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel, const ISystemGUIInterface::SystemLogEntry& alogEntry);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Log entry functions
	void SetLogEntry(const ISystemGUIInterface::SystemLogEntry& alogEntry);

private:
	ExodusSystemMenus& owner;
	ISystemGUIInterface& model;
	ISystemGUIInterface::SystemLogEntry logEntry;
};

#endif
