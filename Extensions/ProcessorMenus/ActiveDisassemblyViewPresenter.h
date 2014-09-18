#ifndef __ACTIVEDISASSEMBLYVIEWPRESENTER_H__
#define __ACTIVEDISASSEMBLYVIEWPRESENTER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "ProcessorMenus.h"
#include "Processor/Processor.pkg"

class ActiveDisassemblyViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	ActiveDisassemblyViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel);

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
