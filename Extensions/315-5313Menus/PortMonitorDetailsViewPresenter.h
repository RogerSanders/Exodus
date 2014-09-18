#ifndef __PORTMONITORDETAILSVIEWPRESENTER_H__
#define __PORTMONITORDETAILSVIEWPRESENTER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "S315_5313Menus.h"
#include "315-5313/IS315_5313.h"

class PortMonitorDetailsViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	PortMonitorDetailsViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel, const IS315_5313::PortMonitorEntry& aentry);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Port monitor entry functions
	void SetPortMonitorEntry(const IS315_5313::PortMonitorEntry& aentry);

private:
	S315_5313Menus& owner;
	const IDevice& modelInstanceKey;
	IS315_5313& model;
	IS315_5313::PortMonitorEntry entry;
};

#endif
