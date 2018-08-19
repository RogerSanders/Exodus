#ifndef __CALLSTACKVIEWPRESENTER_H__
#define __CALLSTACKVIEWPRESENTER_H__
#include "ProcessorMenus.h"
#include "DeviceInterface/DeviceInterface.pkg"
#include "Processor/Processor.pkg"

class CallStackViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	CallStackViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, ProcessorMenus& owner, const IDevice& modelInstanceKey, IProcessor& model);

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
