#ifndef __PROPERTIESVIEWPRESENTER_H__
#define __PROPERTIESVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "SN76489Menus.h"
#include "SN76489/ISN76489.h"

class PropertiesViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	PropertiesViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, SN76489Menus& aowner, const IDevice& amodelInstanceKey, ISN76489& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

private:
	SN76489Menus& owner;
	const IDevice& modelInstanceKey;
	ISN76489& model;
};

#endif
