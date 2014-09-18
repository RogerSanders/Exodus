#ifndef __PLANEVIEWPRESENTER_H__
#define __PLANEVIEWPRESENTER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "S315_5313Menus.h"
#include "315-5313/IS315_5313.h"

class PlaneViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	PlaneViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, S315_5313Menus& aowner, const IDevice& amodelInstanceKey, IS315_5313& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//System interface functions
	ISystemExtensionInterface& GetSystemInterface() const;

private:
	S315_5313Menus& owner;
	const IDevice& modelInstanceKey;
	IS315_5313& model;
};

#endif
