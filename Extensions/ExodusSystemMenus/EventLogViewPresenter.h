#ifndef __EVENTLOGVIEWPRESENTER_H__
#define __EVENTLOGVIEWPRESENTER_H__
#include "SystemInterface/SystemInterface.pkg"
#include "ExodusSystemMenus.h"

class EventLogViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	EventLogViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Log functions
	void SaveEventLog(const std::vector<ISystemGUIInterface::SystemLogEntry>& eventLog, const std::wstring& filePath);

private:
	ExodusSystemMenus& owner;
	ISystemGUIInterface& model;
};

#endif
