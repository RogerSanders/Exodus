#ifndef __INPUTMAPPINGVIEWPRESENTER_H__
#define __INPUTMAPPINGVIEWPRESENTER_H__
#include "DeviceInterface/DeviceInterface.pkg"
#include "ExodusSystemMenus.h"

class InputMappingViewPresenter :public ViewPresenterBase
{
public:
	//Constructors
	InputMappingViewPresenter(const std::wstring& aviewGroupName, const std::wstring& aviewName, int aviewID, ExodusSystemMenus& aowner, ISystemGUIInterface& amodel);

	//View title functions
	static std::wstring GetUnqualifiedViewTitle();

	//View creation and deletion
	virtual IView* CreateView(IUIManager& uiManager);
	virtual void DeleteView(IView* aview);

	//Window functions
	void OpenInputMappingDetailsView(IDevice* targetDevice);
	void CloseInputMappingDetailsView();

private:
	ExodusSystemMenus& owner;
	ISystemGUIInterface& model;
};

#endif
