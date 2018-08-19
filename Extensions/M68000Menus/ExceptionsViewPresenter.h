#ifndef __EXCEPTIONSVIEWPRESENTER_H__
#define __EXCEPTIONSVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "M68000Menus.h"
#include "M68000/IM68000.h"

class ExceptionsViewPresenter :public ViewPresenterBase
{
public:
	// Constructors
	ExceptionsViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, M68000Menus& owner, const IDevice& modelInstanceKey, IM68000& model);

	// View title functions
	static std::wstring GetUnqualifiedViewTitle();

	// View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

private:
	M68000Menus& _owner;
	const IDevice& _modelInstanceKey;
	IM68000& _model;
};

#endif
