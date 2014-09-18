#ifndef __REGISTERSVIEWPRESENTER_H__
#define __REGISTERSVIEWPRESENTER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "Z80Menus.h"
#include "Z80/IZ80.h"

class RegistersViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	RegistersViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, Z80Menus& aowner, const IDevice& amodelInstanceKey, IZ80& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

private:
	Z80Menus& owner;
	const IDevice& modelInstanceKey;
	IZ80& model;
};

#endif
