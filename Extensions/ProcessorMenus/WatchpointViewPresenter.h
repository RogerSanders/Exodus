#ifndef __WATCHPOINTVIEWPRESENTER_H__
#define __WATCHPOINTVIEWPRESENTER_H__
#include "ProcessorMenus.h"
#include "DeviceInterface/DeviceInterface.pkg"
#include "Processor/Processor.pkg"

class WatchpointViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	WatchpointViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

private:
	ProcessorMenus& owner;
	const IDevice& modelInstanceKey;
	IProcessor& model;
};

#endif
