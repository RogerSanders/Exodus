#ifndef __MEMORYEDITORVIEWPRESENTER_H__
#define __MEMORYEDITORVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "MemoryMenus.h"
#include "Memory/MemoryRead.h"

class MemoryEditorViewPresenter :public ViewPresenterBase
{
public:
	// Constructors
	MemoryEditorViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, MemoryMenus& owner, const IDevice& modelInstanceKey, IMemory& model);

	// View title functions
	static std::wstring GetUnqualifiedViewTitle();

	// View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

private:
	MemoryMenus& _owner;
	const IDevice& _modelInstanceKey;
	IMemory& _model;
};

#endif
