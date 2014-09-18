#ifndef __REGISTERSVIEWPRESENTER_H__
#define __REGISTERSVIEWPRESENTER_H__
#include "ExodusDeviceInterface/ExodusDeviceInterface.pkg"
#include "M68000Menus.h"
#include "M68000/IM68000.h"

class RegistersViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	RegistersViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, M68000Menus& aowner, const IDevice& amodelInstanceKey, IM68000& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

private:
	M68000Menus& owner;
	const IDevice& modelInstanceKey;
	IM68000& model;
};

#endif
