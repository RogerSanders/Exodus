#ifndef __BREAKPOINTVIEWPRESENTER_H__
#define __BREAKPOINTVIEWPRESENTER_H__
#include "ProcessorMenus.h"
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Processor/Processor.pkg"

class BreakpointViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	BreakpointViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel);

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
