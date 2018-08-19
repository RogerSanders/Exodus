#ifndef __ACTIVEDISASSEMBLYVIEWPRESENTER_H__
#define __ACTIVEDISASSEMBLYVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ProcessorMenus.h"
#include "Processor/Processor.pkg"

class ActiveDisassemblyViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	ActiveDisassemblyViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

private:
	ProcessorMenus& _owner;
	const IDevice& _modelInstanceKey;
	IProcessor& _model;
};

#endif
