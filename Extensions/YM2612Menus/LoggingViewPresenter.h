#ifndef __LOGGINGVIEWPRESENTER_H__
#define __LOGGINGVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "YM2612Menus.h"
#include "YM2612/IYM2612.h"

class LoggingViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	LoggingViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, YM2612Menus& aowner, const IDevice& amodelInstanceKey, IYM2612& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Logging functions
	bool SelectLoggingPath(std::wstring& fileName) const;

private:
	YM2612Menus& owner;
	const IDevice& modelInstanceKey;
	IYM2612& model;
};

#endif
