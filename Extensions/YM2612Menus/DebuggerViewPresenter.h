#ifndef __DEBUGGERVIEWPRESENTER_H__
#define __DEBUGGERVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "YM2612Menus.h"
#include "YM2612/IYM2612.h"

class DebuggerViewPresenter :public ViewPresenterBase
{
public:
	// Constructors
	DebuggerViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, YM2612Menus& owner, const IDevice& modelInstanceKey, IYM2612& model);

	// View title functions
	static std::wstring GetUnqualifiedViewTitle();

	// View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

	// Window functions
	void OpenOperatorView(unsigned int channelNo, unsigned int operatorNo);

private:
	YM2612Menus& _owner;
	const IDevice& _modelInstanceKey;
	IYM2612& _model;
};

#endif
