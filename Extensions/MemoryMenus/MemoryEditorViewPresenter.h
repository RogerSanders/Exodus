#ifndef __MEMORYEDITORVIEWPRESENTER_H__
#define __MEMORYEDITORVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "MemoryMenus.h"
#include "Memory/MemoryRead.h"

class MemoryEditorViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	MemoryEditorViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, MemoryMenus& aowner, const IDevice& amodelInstanceKey, IMemory& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

private:
	MemoryMenus& owner;
	const IDevice& modelInstanceKey;
	IMemory& model;
};

#endif
