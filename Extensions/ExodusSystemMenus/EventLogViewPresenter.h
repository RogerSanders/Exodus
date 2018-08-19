#ifndef __EVENTLOGVIEWPRESENTER_H__
#define __EVENTLOGVIEWPRESENTER_H__
#include "SystemInterface/SystemInterface.pkg"
#include "ExodusSystemMenus.h"

class EventLogViewPresenter :public ViewPresenterBase
{
public:
	// Constructors
	EventLogViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ExodusSystemMenus& owner, ISystemGUIInterface& model);

	// View title functions
	static std::wstring GetUnqualifiedViewTitle();

	// View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

	// Log functions
	void SaveEventLog(const std::vector<ISystemGUIInterface::SystemLogEntry>& eventLog, const std::wstring& filePath);

private:
	ExodusSystemMenus& _owner;
	ISystemGUIInterface& _model;
};

#endif
