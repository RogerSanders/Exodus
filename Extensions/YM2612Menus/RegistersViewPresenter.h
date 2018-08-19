#ifndef __REGISTERSVIEWPRESENTER_H__
#define __REGISTERSVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "YM2612Menus.h"
#include "YM2612/IYM2612.h"

class RegistersViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	RegistersViewPresenter(const std::wstring& viewGroupName, const std::wstring& viewName, int viewID, YM2612Menus& owner, const IDevice& modelInstanceKey, IYM2612& model);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* view);

private:
	YM2612Menus& _owner;
	const IDevice& _modelInstanceKey;
	IYM2612& _model;
};

#endif
