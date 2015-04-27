#ifndef __DISASSEMBLYVIEWPRESENTER_H__
#define __DISASSEMBLYVIEWPRESENTER_H__
#include "ProcessorMenus.h"
#include "DeviceInterface/DeviceInterface.pkg"
#include "Processor/Processor.pkg"

class DisassemblyViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	DisassemblyViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ProcessorMenus& aowner, const IDevice& amodelInstanceKey, IProcessor& amodel);

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
